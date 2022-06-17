#include "types.h"
#include "efi.h"
#include "print.h"

#define PAGE_SIZE 4096

EFI_STATUS efi_main(void* ImageHandle, struct EFI_SYSTEM_TABLE* SystemTable) {
    struct EFI_BOOT_SERVICES* bootServices = SystemTable->BootServices;

    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (char16*)u"Starting...\r\n");

    // UEFI Revision
    u16 efiMajor = SystemTable->Hdr.Revision >> 16;
    u16 efiMinor = SystemTable->Hdr.Revision & 0xFF;

    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"UEFI Revision ");
    char16 buffer[255];
    uintToString(buffer, (u64)efiMajor);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u".");

    uintToString(buffer, (u64)efiMinor);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

    // UEFI Firmware
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Firmware Vendor ");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, SystemTable->FirmwareVendor);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Firmware Revision ");
    uintToString(buffer, (u64)SystemTable->FirmwareRevision);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

    // Graphics stuff

    struct EFI_GUID graphicsProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    struct EFI_GRAPHICS_OUTPUT_PROTOCOL* graphicsInterface = 0;
    EFI_STATUS res = bootServices->LocateProtocol(
        &graphicsProtocolGuid,
        0,
        (void**)&graphicsInterface
    );

    if (res != EFI_SUCCESS) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, u"ERROR: could not get protocol interface for GraphicsOutput\r\n");
    }

    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Max Mode ");
    uintToString(buffer, (u64)graphicsInterface->Mode->MaxMode);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");
    
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Mode ");
    uintToString(buffer, (u64)graphicsInterface->Mode->Mode);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Size of Info ");
    uintToHexString(buffer, (u64)graphicsInterface->Mode->SizeOfInfo);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Frame Buffer Base 0x");
    uintToHexString(buffer, (u64)graphicsInterface->Mode->FrameBufferBase);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Frame Buffer Size ");
    uintToString(buffer, graphicsInterface->Mode->FrameBufferSize);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Horizontal Resolution ");
    uintToString(buffer, graphicsInterface->Mode->Info->HorizontalResolution);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Vertical Resolution ");
    uintToString(buffer, graphicsInterface->Mode->Info->VerticalResolution);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Pixel Format 0x");
    uintToHexString(buffer, graphicsInterface->Mode->Info->PixelFormat);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Pixel Information\r\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"R 0x");
    uintToHexString(buffer, graphicsInterface->Mode->Info->PixelInformation.RedMask);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u" G 0x");
    uintToHexString(buffer, graphicsInterface->Mode->Info->PixelInformation.GreenMask);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u" B 0x");
    uintToHexString(buffer, graphicsInterface->Mode->Info->PixelInformation.BlueMask);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u" RESERVED 0x");
    uintToHexString(buffer, graphicsInterface->Mode->Info->PixelInformation.ReservedMask);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"Pixels per Scanline ");
    uintToString(buffer, graphicsInterface->Mode->Info->PixelsPerScanLine);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, buffer);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"\r\n");

    if (graphicsInterface->Mode->Info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, u"ERROR: unsupported pixel format\r\n");
    }

    s64 timer = 0;
    s64 direction = 1;

    while (true) {
        timer += direction;
        if (timer >= 255) {
            direction = -1;
        } else if (timer <= 0) {
            direction = 1;
        }

        struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL pixel = {
            .Blue = (u8)(timer),
            .Green = 0,
            .Red = 0,
            .Reserved = 0,
        };

        graphicsInterface->Blt(
            graphicsInterface,
            &pixel,
            EfiBltVideoFill,
            0, 0,
            0, 0,
            graphicsInterface->Mode->Info->HorizontalResolution,
            graphicsInterface->Mode->Info->VerticalResolution,
            0
        );
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

    SystemTable->ConOut->OutputString(SystemTable->ConOut, (char16*)u"Bye...\r\n");

    while(true) {};
    return EFI_SUCCESS;
}
