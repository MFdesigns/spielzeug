#pragma once
#include "types.h"

typedef u64 EFI_STATUS;
#define EFI_SUCCESS             0ull

#define EFI_ERROR_CODES         0x8000000000000000
#define EFI_BUFFER_TOO_SMALL    (EFI_ERROR_CODES | 5ull)

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

typedef u64 EFI_TPL;
#define TPL_APPLICATION 4
#define TPL_CALLBACK    8
#define TPL_NOTIFY      16
#define TPL_HIGH_LEVEL  31

typedef EFI_TPL (*EFI_RAISE_TPL) (EFI_TPL NewTpl);
typedef void (*EFI_RESTORE_TPL) (EFI_TPL OldTpl);

enum EFI_ALLOCATE_TYPE {
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress,
    MaxAllocateType
};

enum EFI_MEMORY_TYPE {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiUnacceptedMemoryType,
    EfiMaxMemoryType
};

typedef u64 EFI_PHYSICAL_ADDRESS;
typedef u64 EFI_VIRTUAL_ADDRESS;

struct EFI_MEMORY_DESCRIPTOR {
    u32                     Type;
    EFI_PHYSICAL_ADDRESS    PhysicalStart;
    EFI_VIRTUAL_ADDRESS     VirtualStart;
    u64                     NumberOfPages;
    u64                     Attribute;
};

typedef EFI_STATUS (*EFI_ALLOCATE_PAGES) (enum EFI_ALLOCATE_TYPE Type, enum EFI_MEMORY_TYPE MemoryType, u64 Pages, EFI_PHYSICAL_ADDRESS* Memory);
typedef EFI_STATUS (*EFI_FREE_PAGES) (EFI_PHYSICAL_ADDRESS Memory, u64 Pages);
typedef EFI_STATUS (*EFI_GET_MEMORY_MAP) (u64 *MemoryMapSize, struct EFI_MEMORY_DESCRIPTOR* MemoryMap, u64* MapKey, u64* DescriptorSize, u32* DescriptorVersion);
typedef EFI_STATUS (*EFI_ALLOCATE_POOL) (enum EFI_MEMORY_TYPE PoolType, u64 Size, void** Buffer);
typedef EFI_STATUS (*EFI_FREE_POOL) (void* Buffer);

struct EFI_BOOT_SERVICES {
    struct EFI_TABLE_HEADER Hdr;
    //
    // Task Priority Services
    //
    EFI_RAISE_TPL RaiseTPL; // EFI 1.0+
    EFI_RESTORE_TPL RestoreTPL; // EFI 1.0+
    //
    // Memory Services
    //
    EFI_ALLOCATE_PAGES AllocatePages; // EFI 1.0+
    EFI_FREE_PAGES FreePages; // EFI 1.0+
    EFI_GET_MEMORY_MAP GetMemoryMap; // EFI 1.0+
    EFI_ALLOCATE_POOL AllocatePool; // EFI 1.0+
    EFI_FREE_POOL FreePool; // EFI 1.0+
#if 0
    //
    // Event & Timer Services
    //
    EFI_CREATE_EVENT CreateEvent; // EFI 1.0+
    EFI_SET_TIMER SetTimer; // EFI 1.0+
    EFI_WAIT_FOR_EVENT WaitForEvent; // EFI 1.0+
    EFI_SIGNAL_EVENT SignalEvent; // EFI 1.0+
    EFI_CLOSE_EVENT CloseEvent; // EFI 1.0+
    EFI_CHECK_EVENT CheckEvent; // EFI 1.0+
    //
    // Protocol Handler Services
    //
    EFI_INSTALL_PROTOCOL_INTERFACE InstallProtocolInterface; // EFI 1.0+
    EFI_REINSTALL_PROTOCOL_INTERFACE ReinstallProtocolInterface; // EFI 1.0+
    EFI_UNINSTALL_PROTOCOL_INTERFACE UninstallProtocolInterface; // EFI 1.0+
    EFI_HANDLE_PROTOCOL HandleProtocol; // EFI 1.0+
    VOID* Reserved; // EFI 1.0+
    EFI_REGISTER_PROTOCOL_NOTIFY RegisterProtocolNotify; // EFI 1.0+
    EFI_LOCATE_HANDLE LocateHandle; // EFI 1.0+
    EFI_LOCATE_DEVICE_PATH LocateDevicePath; // EFI 1.0+
    EFI_INSTALL_CONFIGURATION_TABLE InstallConfigurationTable; // EFI 1.0+
    //
    // Image Services
    //
    EFI_IMAGE_LOAD LoadImage; // EFI 1.0+
    EFI_IMAGE_START StartImage; // EFI 1.0+
    EFI_EXIT Exit; // EFI 1.0+
    EFI_IMAGE_UNLOAD UnloadImage; // EFI 1.0+
    EFI_EXIT_BOOT_SERVICES ExitBootServices; // EFI 1.0+
    //
    // Miscellaneous Services
    //
    EFI_GET_NEXT_MONOTONIC_COUNT GetNextMonotonicCount; // EFI 1.0+
    EFI_STALL Stall; // EFI 1.0+
    EFI_SET_WATCHDOG_TIMER SetWatchdogTimer; // EFI 1.0+
    //
    // DriverSupport Services
    //
    EFI_CONNECT_CONTROLLER ConnectController; // EFI 1.1
    EFI_DISCONNECT_CONTROLLER DisconnectController;// EFI 1.1+
    //
    // Open and Close Protocol Services
    //
    EFI_OPEN_PROTOCOL OpenProtocol; // EFI 1.1+
    EFI_CLOSE_PROTOCOL CloseProtocol; // EFI 1.1+
    EFI_OPEN_PROTOCOL_INFORMATION OpenProtocolInformation; // EFI 1.1+
    //
    // Library Services
    //
    EFI_PROTOCOLS_PER_HANDLE ProtocolsPerHandle; // EFI 1.1+
    EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer; // EFI 1.1+
    EFI_LOCATE_PROTOCOL LocateProtocol; // EFI 1.1+
    EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES
    InstallMultipleProtocolInterfaces; // EFI 1.1+
    EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES
    UninstallMultipleProtocolInterfaces; // EFI 1.1+
    //
    // 32-bit CRC Services
    //
    EFI_CALCULATE_CRC32 CalculateCrc32; // EFI 1.1+
    //
    // Miscellaneous Services
    //
    EFI_COPY_MEM CopyMem; // EFI 1.1+
    EFI_SET_MEM SetMem; // EFI 1.1+
    EFI_CREATE_EVENT_EX CreateEventEx; // UEFI 2.0+
#endif
};

// TODO: implement me
#define EFI_RUNTIME_SERVICES void

struct EFI_SYSTEM_TABLE {
    struct EFI_TABLE_HEADER                 Hdr;
    char16*                                 FirmwareVendor;
    u32                                     FirmwareRevision;
    void*                                   ConsoleInHandle;
    struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL*  ConIn;
    void*                                   ConsoleOutHandle;
    struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* ConOut;
    void*                                   StandardErrorHandle;
    struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* StdErr;
    EFI_RUNTIME_SERVICES*                   RuntimeServices;
    struct EFI_BOOT_SERVICES*               BootServices;
#if 0
    u64                                 NumberOfTableEntries;
    EFI_CONFIGURATION_TABLE*            ConfigurationTable;
#endif
};

typedef EFI_STATUS (*EFI_IMAGE_ENTRY_POINT) (void* ImageHandle, struct EFI_SYSTEM_TABLE* SystemTable);
