#include "types.h"
#include "efi.h"
#include "print.h"
#include "bmp_font.h"

#define PAGE_SIZE 4096

struct Terminal {
    u32 screenWidth;
    u32 screenHeight;
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL* frameBuffer;
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL* font;
    u32 cursorX;
    u32 cursorY;
    u32 rowCount;
    u32 columnCount;
};

void terminalPutChar(struct Terminal* term, char c) {
    if (c == '\r') {
        return;
    }

    if (c == '\n') {
        if (term->cursorY + 1 >= term->rowCount) {
            term->cursorY = 0;
        } else {
            term->cursorY++;
        }
        term->cursorX = 0;
        return;
    }

    u32 glyphIndex = CHAR_MAP[c];
    u32 glyphSize = CHAR_WIDTH * CHAR_HEIGHT;
    u8* glyph = &CHAR_BMP[glyphIndex * glyphSize];

    u32 originX = term->cursorX * CHAR_WIDTH;
    u32 originY = term->cursorY * CHAR_HEIGHT;
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL* origin = 
        &term->frameBuffer[originY * term->screenWidth + originX];
    for (u32 i = 0; i < glyphSize; i++) {
        u8 color = glyph[i];

        u32 x = i % CHAR_WIDTH;
        u32 y = i / CHAR_WIDTH;
        struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL* pixel = &origin[y * term->screenWidth + x];
        pixel->Red = color;
        pixel->Green = color;
        pixel->Blue = color;
        pixel->Reserved = 0;
    }

    term->cursorX++;
    if (term->cursorX >= term->columnCount) {
        term->cursorX = 0;
        term->cursorY++;
    }

    if (term->cursorY >= term->rowCount) {
        term->cursorY = 0;
    }
}

void putChar(void* instance, char c) {
    struct EFI_SYSTEM_TABLE* systemTable = instance;
    char16 tempChar[2] = { (char16)c, '\0' };
    systemTable->ConOut->OutputString(systemTable->ConOut, tempChar);
}

EFI_STATUS efi_main(void* imageHandle, struct EFI_SYSTEM_TABLE* systemTable) {
    struct EFI_BOOT_SERVICES* bootServices = systemTable->BootServices;
    formatInit(systemTable, putChar);

    systemTable->ConOut->ClearScreen(systemTable->ConOut);
    systemTable->ConOut->OutputString(systemTable->ConOut, (char16*)u"Starting...\r\n");

    // UEFI Revision
    u16 efiMajor = systemTable->Hdr.Revision >> 16;
    u16 efiMinor = systemTable->Hdr.Revision & 0xFF;

    format("UEFI Revision {}.{}\r\n", fmt(efiMajor), fmt(efiMinor));
    format("Firmware Vendor {}\r\n", fmt(systemTable->FirmwareVendor));
    format("Firmware Revision {}\r\n", fmt(systemTable->FirmwareRevision));

    // Graphics stuff

    struct EFI_GUID graphicsProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    struct EFI_GRAPHICS_OUTPUT_PROTOCOL* graphicsInterface = 0;
    EFI_STATUS res = bootServices->LocateProtocol(
        &graphicsProtocolGuid,
        0,
        (void**)&graphicsInterface
    );

    if (res != EFI_SUCCESS) {
        systemTable->ConOut->OutputString(systemTable->ConOut, u"ERROR: could not get protocol interface for GraphicsOutput\r\n");
    }

    u32 screenWidth = graphicsInterface->Mode->Info->HorizontalResolution;
    u32 screenHeight = graphicsInterface->Mode->Info->VerticalResolution;

    u32 pageCount = (CHAR_COUNT * CHAR_WIDTH * CHAR_HEIGHT * 4) / PAGE_SIZE + 1;
    EFI_PHYSICAL_ADDRESS glyphBuffer;
    EFI_STATUS allocRes = bootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, pageCount, &glyphBuffer); 
    if (allocRes != EFI_SUCCESS) {
        systemTable->ConOut->OutputString(systemTable->ConOut, (char16*)u"ERROR: could not allocate pages\r\n");
    }

    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL* glyphTexture = 
        (struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)glyphBuffer;

    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL* frameBuffer = 
        (struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)graphicsInterface->Mode->FrameBufferBase;

    for (u32 i = 0; i < CHAR_WIDTH * CHAR_HEIGHT * CHAR_COUNT; i++) {
        u32 bmp = CHAR_BMP[i];

        struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL* pixel = &glyphTexture[i];
        pixel->Red = bmp;
        pixel->Green = bmp;
        pixel->Blue = bmp;
        pixel->Reserved = 0;
    }
    struct Terminal term = {

        .screenWidth = screenWidth,
        .screenHeight = screenHeight,
        .frameBuffer = frameBuffer,
        .font = glyphTexture,
        .cursorX = 0,
        .cursorY = 0,
        .rowCount = screenHeight / CHAR_HEIGHT,
        .columnCount = screenWidth / CHAR_WIDTH,
    };
    formatInit(&term, (PutCharFunc)terminalPutChar);

    format("Max Mode {}\r\n", fmt(graphicsInterface->Mode->MaxMode));
    format("Mode {}\r\n", fmt(graphicsInterface->Mode->Mode));
    format("Size of Info 0x{X}\r\n", fmt(graphicsInterface->Mode->SizeOfInfo));
    format("Frame Buffer Base 0x{X}\r\n", fmt(graphicsInterface->Mode->FrameBufferBase));
    format("Frame Buffer Size {}\r\n", fmt(graphicsInterface->Mode->FrameBufferSize));

    format("Resolution {}x{}\r\n", fmt(screenWidth), fmt(screenHeight));

    format("Pixel Format 0x{X}\r\n", fmt((u64)graphicsInterface->Mode->Info->PixelFormat));
    format("Pixel Mask R 0x{X} G 0x{X} B 0x{X} RESERVED 0x{X}\r\n",
           fmt(graphicsInterface->Mode->Info->PixelInformation.RedMask),
           fmt(graphicsInterface->Mode->Info->PixelInformation.GreenMask),
           fmt(graphicsInterface->Mode->Info->PixelInformation.BlueMask),
           fmt(graphicsInterface->Mode->Info->PixelInformation.ReservedMask)
    );

    format("Pixels per Scanline {}\r\n", fmt(graphicsInterface->Mode->Info->PixelsPerScanLine));

    if (graphicsInterface->Mode->Info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
        systemTable->ConOut->OutputString(systemTable->ConOut, u"ERROR: unsupported pixel format\r\n");
    }

#if 0
    // Memory map
    u32 pageCount = 2;
    EFI_PHYSICAL_ADDRESS pages;
    EFI_STATUS allocRes = bootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, pageCount, &pages); 
    if (allocRes != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, (char16*)u"ERROR: could not allocate pages\r\n");
    }

    u64 memoryMapSize = pageCount * PAGE_SIZE;
    struct EFI_MEMORY_DESCRIPTOR* memoryMap = (struct EFI_MEMORY_DESCRIPTOR*)pages;
    u64 mapKey;
    u64 descriptorSize;
    u32 descriptorVersion;

    EFI_STATUS memMapRes = bootServices->GetMemoryMap(&memoryMapSize, memoryMap, &mapKey, &descriptorSize, &descriptorVersion);

    if (memMapRes == EFI_BUFFER_TOO_SMALL) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, (char16*)u"ERROR: buffer for memory map is too small\r\n");

        SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Needed buffer size 0x");
        uintToHexString(buffer, (u64)memoryMapSize);
        SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
        SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");
    } else if (memMapRes != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, (char16*)u"ERROR: could not get memory map\r\n");
    }

    u32 descriptorCount = memoryMapSize / descriptorSize;
    struct EFI_MEMORY_DESCRIPTOR* memoryDescriptor = memoryMap;
    for (u32 i = 0; i < descriptorCount; i++) {
        char16 tmpBuffer[255];
        uintToHexString(tmpBuffer, memoryDescriptor->Type);
        SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Type: 0x");
        SystemTable->ConOut->OutputString(SystemTable->ConOut, tmpBuffer);
        SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

        memoryDescriptor = (struct EFI_MEMORY_DESCRIPTOR*)((u8*)memoryDescriptor + descriptorSize);
    }

    EFI_STATUS deallocRes = bootServices->FreePages(pages, pageCount);
    if (deallocRes != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, (char16*)u"ERROR: could not deallocate pages\r\n");
    }
#endif

    systemTable->ConOut->OutputString(systemTable->ConOut, (char16*)u"Bye...\r\n");

    while(true) {};
    return EFI_SUCCESS;
}
