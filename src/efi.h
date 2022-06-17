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

typedef void* EFI_EVENT;

typedef void (*EFI_EVENT_NOTIFY) (EFI_EVENT Event, void* Context);
typedef EFI_STATUS (*EFI_CREATE_EVENT) (u32 Type, EFI_TPL NotifyTpl, EFI_EVENT_NOTIFY NotifyFunction, void* NotifyContext, EFI_EVENT *Event); 

enum EFI_TIMER_DELAY {
    TimerCancel,
    TimerPeriodic,
    TimerRelative
};

typedef EFI_STATUS (*EFI_SET_TIMER) (EFI_EVENT Event, enum EFI_TIMER_DELAY Type, u64 TriggerTime);
typedef EFI_STATUS (*EFI_WAIT_FOR_EVENT) (u64 NumberOfEvents, EFI_EVENT *Event, u64 *Index);
typedef EFI_STATUS (*EFI_SIGNAL_EVENT) (EFI_EVENT Event);
typedef EFI_STATUS (*EFI_CLOSE_EVENT) (EFI_EVENT Event);
typedef EFI_STATUS (*EFI_CHECK_EVENT) (EFI_EVENT Event);

struct EFI_GUID {
    u32 Data1;
    u16 Data2;
    u16 Data3;
    u8  Data4[8];
};

enum EFI_INTERFACE_TYPE {
    EFI_NATIVE_INTERFACE
};

typedef EFI_STATUS (*EFI_INSTALL_PROTOCOL_INTERFACE) (
    void*                   Handle, 
    struct EFI_GUID*        Protocol, 
    enum EFI_INTERFACE_TYPE InterfaceType, 
    void*                   Interface
);
typedef EFI_STATUS (*EFI_REINSTALL_PROTOCOL_INTERFACE) (
    void*               Handle,
    struct EFI_GUID*    Protocol,
    void*               OldInterface,
    void*               NewInterface
);
typedef EFI_STATUS (*EFI_UNINSTALL_PROTOCOL_INTERFACE) (
    void*               Handle,
    struct EFI_GUID*    Protocol,
    void*               Interface
);
typedef EFI_STATUS (*EFI_HANDLE_PROTOCOL) (
    void*               Handle,
    struct EFI_GUID*    Protocol,
    void**              Interface
);
typedef EFI_STATUS (*EFI_REGISTER_PROTOCOL_NOTIFY) (
    struct EFI_GUID*    Protocol,
    EFI_EVENT           Event,
    void**              Registration
);

enum EFI_LOCATE_SEARCH_TYPE {
    AllHandles,
    ByRegisterNotify,
    ByProtocol
};

typedef EFI_STATUS (*EFI_LOCATE_HANDLE) (
    enum EFI_LOCATE_SEARCH_TYPE SearchType,
    struct EFI_GUID*            Protocol,
    void*                       SearchKey,
    u64*                        BufferSize,
    void**                      Buffer
);

struct EFI_DEVICE_PATH_PROTOCOL {
    u8 Type;
    u8 SubType;
    u8 Length[2];
};

typedef EFI_STATUS (*EFI_LOCATE_DEVICE_PATH) (
    struct EFI_GUID*                    Protocol,
    struct EFI_DEVICE_PATH_PROTOCOL**   DevicePath,
    void**                              Device
);
typedef EFI_STATUS (*EFI_INSTALL_CONFIGURATION_TABLE) (
    struct EFI_GUID*    Guid,
    void*               Table
);

typedef EFI_STATUS (*EFI_IMAGE_LOAD) (
    bool                                BootPolicy,
    void*                               ParentImageHandle,
    struct EFI_DEVICE_PATH_PROTOCOL*    DevicePath,
    void*                               SourceBuffer,
    u64                                 SourceSize,
    void**                              ImageHandle
);
typedef EFI_STATUS (*EFI_IMAGE_START) (
    void*       ImageHandle,
    u64*        ExitDataSize,
    char16**    ExitData
);
typedef EFI_STATUS (*EFI_EXIT) (
    void*       ImageHandle,
    EFI_STATUS  ExitStatus,
    u64         ExitDataSize,
    char16*     ExitData
);
typedef EFI_STATUS (*EFI_IMAGE_UNLOAD) (
    void* ImageHandle
);
typedef EFI_STATUS (*EFI_EXIT_BOOT_SERVICES) (
    void*   ImageHandle,
    u64     MapKey
);

typedef EFI_STATUS (*EFI_GET_NEXT_MONOTONIC_COUNT) (
    u64* Count
);
typedef EFI_STATUS (*EFI_STALL) (
    u64 Microseconds
);
typedef EFI_STATUS (*EFI_SET_WATCHDOG_TIMER) (
    u64     Timeout,
    u64     WatchdogCode,
    u64     DataSize,
    char16* WatchdogData
);
typedef EFI_STATUS (*EFI_CONNECT_CONTROLLER) (
    void*                            ControllerHandle,
    void**                           DriverImageHandle,
    struct EFI_DEVICE_PATH_PROTOCOL* RemainingDevicePath,
    bool                             Recursive
);
typedef EFI_STATUS (*EFI_DISCONNECT_CONTROLLER) (
    void* ControllerHandle,
    void* DriverImageHandle,
    void* ChildHandle
);

typedef EFI_STATUS (*EFI_OPEN_PROTOCOL) (
    void*            Handle,
    struct EFI_GUID* Protocol,
    void**           Interface,
    void*            AgentHandle,
    void*            ControllerHandle,
    u32              Attributes
);
typedef EFI_STATUS (*EFI_CLOSE_PROTOCOL) (
    void*            Handle,
    struct EFI_GUID* Protocol,
    void*            AgentHandle,
    void*            ControllerHandle
);

struct EFI_OPEN_PROTOCOL_INFORMATION_ENTRY {
    void* AgentHandle;
    void* ControllerHandle;
    u32   Attributes;
    u32   OpenCount;
};

typedef EFI_STATUS (*EFI_OPEN_PROTOCOL_INFORMATION) (
    void*                                        Handle,
    struct EFI_GUID*                             Protocol,
    struct EFI_OPEN_PROTOCOL_INFORMATION_ENTRY** EntryBuffer,
    u64*                                         EntryCount
);

typedef EFI_STATUS (*EFI_PROTOCOLS_PER_HANDLE) (
    void*              Handle,
    struct EFI_GUID*** ProtocolBuffer,
    u64*               ProtocolBufferCount
);
typedef EFI_STATUS (*EFI_LOCATE_HANDLE_BUFFER) (
    enum EFI_LOCATE_SEARCH_TYPE SearchType,
    struct EFI_GUID*            Protocol,
    void*                       SearchKey,
    u64*                        NoHandles,
    void***                     Buffer
);
typedef EFI_STATUS (*EFI_LOCATE_PROTOCOL) (
    struct EFI_GUID* Protocol,
    void*            Registration,
    void**           Interface
);
typedef EFI_STATUS (*EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES) (
    void** Handle,
    ...
);
typedef EFI_STATUS (*EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES) (
    void* Handle,
    ...
);

typedef EFI_STATUS (*EFI_CALCULATE_CRC32) (
    void* Data,
    u64   DataSize,
    u32*  Crc32
);
typedef void (*EFI_COPY_MEM) (
    void* Destination,
    void* Source,
    u64   Length
);
typedef void (*EFI_SET_MEM) (
    void* Buffer,
    u64   Size,
    u8    Value
);
typedef EFI_STATUS (*EFI_CREATE_EVENT_EX) (
    u32                    Type,
    EFI_TPL                NotifyTpl,
    EFI_EVENT_NOTIFY       NotifyFunction,
    const void*            NotifyContext,
    const struct EFI_GUID* EventGroup,
    EFI_EVENT*             Event
);

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
    void* Reserved; // EFI 1.0+
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

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
    {0x9042a9de, 0x23dc, 0x4a38,\
    {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}}

enum EFI_GRAPHICS_PIXEL_FORMAT {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
};

struct EFI_GRAPHICS_OUTPUT_PROTOCOL;

struct EFI_PIXEL_BITMASK {
    u32 RedMask;
    u32 GreenMask;
    u32 BlueMask;
    u32 ReservedMask;
};

struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION {
    u32                            Version;
    u32                            HorizontalResolution;
    u32                            VerticalResolution;
    enum EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    struct EFI_PIXEL_BITMASK       PixelInformation;
    u32                            PixelsPerScanLine;
};

typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE) (
    struct EFI_GRAPHICS_OUTPUT_PROTOCOL*          This,
    u32                                           ModeNumber,
    u64*                                          SizeOfInfo,
    struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION** Info
);

typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE) (
    struct EFI_GRAPHICS_OUTPUT_PROTOCOL* This,
    u32                                  ModeNumber
);

struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL {
    u8 Blue;
    u8 Green;
    u8 Red;
    u8 Reserved;
};

enum EFI_GRAPHICS_OUTPUT_BLT_OPERATION {
    EfiBltVideoFill,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo,
    EfiGraphicsOutputBltOperationMax
};

typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT) (
    struct EFI_GRAPHICS_OUTPUT_PROTOCOL*   This,
    struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL*  BltBuffer,
    enum EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation,
    u64                                    SourceX,
    u64                                    SourceY,
    u64                                    DestinationX,
    u64                                    DestinationY,
    u64                                    Width,
    u64                                    Height,
    u64                                    Delta
);

struct EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE {
    u32 MaxMode;
    u32 Mode;
    struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* Info;
    u64 SizeOfInfo;
    EFI_PHYSICAL_ADDRESS FrameBufferBase;
    u64 FrameBufferSize;
};

struct EFI_GRAPHICS_OUTPUT_PROTOCOL {
    EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE   QueryMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE     SetMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT          Blt;
    struct EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* Mode;
};
