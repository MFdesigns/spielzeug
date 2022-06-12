#pragma once
#include "types.h"

typedef u64 EFI_STATUS;
#define EFI_SUCCESS     0ull

struct EFI_TABLE_HEADER {
    u64 Signature;
    u32 Revision;
    u32 HeaderSize;
    u32 CRC32;
    u32 Reserved;
};

struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

struct EFI_INPUT_KEY {
    u16 ScanCode;
    char16 UnicodeChar;
};

typedef EFI_STATUS (*EFI_INPUT_RESET) (struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL* This, bool ExtendedVerification);
typedef EFI_STATUS (*EFI_INPUT_READ_KEY) (struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL* This, struct EFI_INPUT_KEY* Key);

struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_INPUT_RESET     Reset;
    EFI_INPUT_READ_KEY  ReadKeyStroke;
    void*               WaitForKey;
};


struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef EFI_STATUS (*EFI_TEXT_RESET) (struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, bool ExtendedVerification);
typedef EFI_STATUS (*EFI_TEXT_STRING) (struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, char16* String);
typedef EFI_STATUS (*EFI_TEXT_TEST_STRING) (struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, char16* String);
typedef EFI_STATUS (*EFI_TEXT_QUERY_MODE) (struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, u64 ModeNumber, u64* Columns, u64* Rows);
typedef EFI_STATUS (*EFI_TEXT_SET_MODE) (struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, u64 ModeNumber);
typedef EFI_STATUS (*EFI_TEXT_SET_ATTRIBUTE) (struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, u64 Attribute);
typedef EFI_STATUS (*EFI_TEXT_CLEAR_SCREEN) (struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This);
typedef EFI_STATUS (*EFI_TEXT_SET_CURSOR_POSITION) (struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, u64 Column, u64 Row);
typedef EFI_STATUS (*EFI_TEXT_ENABLE_CURSOR) (struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, bool Visible);

struct SIMPLE_TEXT_OUTPUT_MODE {
    s32 MaxMode;
    s32 Mode;
    s32 Attribute;
    s32 CursorColumn;
    s32 CursorRow;
    bool CursorVisible;
};

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_TEXT_RESET                  Reset;
    EFI_TEXT_STRING                 OutputString;
    EFI_TEXT_TEST_STRING            TestString;
    EFI_TEXT_QUERY_MODE             QueryMode;
    EFI_TEXT_SET_MODE               SetMode;
    EFI_TEXT_SET_ATTRIBUTE          SetAttribute;
    EFI_TEXT_CLEAR_SCREEN           ClearScreen;
    EFI_TEXT_SET_CURSOR_POSITION    SetCursorPosition;
    EFI_TEXT_ENABLE_CURSOR          EnableCursor;
    struct SIMPLE_TEXT_OUTPUT_MODE* Mode;
};

struct EFI_SYSTEM_TABLE {
    struct EFI_TABLE_HEADER                 Hdr;
    char16*                                 FirmwareVendor;
    u32                                     FirmwareRevision;
    void*                                   ConsoleInHandle;
    struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL*  ConIn;
    void*                                   ConsoleOutHandle;
    struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* ConOut;
    void*                                   StandardErrorHandle;
#if 0
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*    StdErr;
    EFI_RUNTIME_SERVICES*               RuntimeServices;
    EFI_BOOT_SERVICES*                  BootServices;
    u64                                 NumberOfTableEntries;
    EFI_CONFIGURATION_TABLE*            ConfigurationTable;
#endif
};

typedef EFI_STATUS (*EFI_IMAGE_ENTRY_POINT) (void* ImageHandle, struct EFI_SYSTEM_TABLE* SystemTable);
