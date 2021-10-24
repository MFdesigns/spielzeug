#pragma once
#include "types.h"

typedef struct EFI_GUID {
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint8_t data4[8];
} EFI_GUID;

typedef uint64_t EFI_STATUS;
typedef void* EFI_HANDLE;
typedef void* EFI_EVENT;

typedef struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef struct EFI_INPUT_KEY EFI_INPUT_KEY;
typedef struct SIMPLE_TEXT_OUTPUT_MODE SIMPLE_TEXT_OUTPUT_MODE;
typedef struct EFI_TABLE_HEADER EFI_TABLE_HEADER;
typedef struct EFI_GUID EFI_GUID;
typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef struct EFI_RUNTIME_SERVICES EFI_RUNTIME_SERVICES;

typedef EFI_STATUS (*EFI_INPUT_RESET) (EFI_SIMPLE_TEXT_INPUT_PROTOCOL* this, bool extendedVerification);
typedef EFI_STATUS (*EFI_INPUT_READ_KEY) (EFI_SIMPLE_TEXT_INPUT_PROTOCOL* this, EFI_INPUT_KEY* key);
typedef EFI_STATUS (*EFI_TEXT_RESET) (EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* this, bool extendedVerification);
typedef EFI_STATUS (*EFI_TEXT_STRING) (EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* this, char16_t* string);
typedef EFI_STATUS (*EFI_TEXT_TEST_STRING) (EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* this, char16_t* string);
typedef EFI_STATUS (*EFI_TEXT_QUERY_MODE) (EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* this, uint64_t modeNumber, uint64_t* columns, uint64_t* rows);
typedef EFI_STATUS (*EFI_TEXT_SET_MODE) (EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* this, uint64_t modeNumber);
typedef EFI_STATUS (*EFI_TEXT_SET_ATTRIBUTE) (EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* this, uint64_t attribute);
typedef EFI_STATUS (*EFI_TEXT_CLEAR_SCREEN) (EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* this);
typedef EFI_STATUS (*EFI_TEXT_SET_CURSOR_POSITION) (EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* this, uint64_t column, uint64_t row);
typedef EFI_STATUS (*EFI_TEXT_ENABLE_CURSOR) (EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* this, bool visible);

const EFI_STATUS EFI_SUCCESS = 0;

struct EFI_INPUT_KEY {
    uint16_t scanCode;
    char16_t unicodeChar;
};

struct EFI_TABLE_HEADER {
    uint64_t signature;
    uint32_t revision;
    uint32_t headerSize;
    uint32_t crc32;
    uint32_t reserved;
};

struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_INPUT_RESET reset;
    EFI_INPUT_READ_KEY readKeyStroke;
    EFI_EVENT waitForKey;
};

struct SIMPLE_TEXT_OUTPUT_MODE {
    int32_t maxMode;
    int32_t mode;
    int32_t attribute;
    int32_t cursorColumn;
    int32_t cursorRow;
    bool cursorVisible;
};

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_TEXT_RESET reset;
    EFI_TEXT_STRING outputString;
    EFI_TEXT_TEST_STRING testString;
    EFI_TEXT_QUERY_MODE queryMode;
    EFI_TEXT_SET_MODE setMode;
    EFI_TEXT_SET_ATTRIBUTE setAttribute;
    EFI_TEXT_CLEAR_SCREEN clearScreen;
    EFI_TEXT_SET_CURSOR_POSITION setCursorPosition;
    EFI_TEXT_ENABLE_CURSOR enableCursor;
    SIMPLE_TEXT_OUTPUT_MODE* mode;
};

struct EFI_RUNTIME_SERVICES {
    EFI_TABLE_HEADER hdr;
#if 0
    EFI_GET_TIME getTime;
    EFI_SET_TIME setTime;
    EFI_GET_WAKUP_TIME getWakeupTime;
    EFI_SET_WAKUP_TIME setWakeupTime;
    EFI_SET_VIRTUAL_ADDRESS_MAP setVirualAddressMap;
    EFI_CONVERT_POINTER convertPointer;
    EFI_GET_VARIABLE getVariable;
    EFI_GET_NEXT_VARIABLE_NAME getNextVariableName;
    EFI_SET_VARIABEL setVariable;
    EFI_GET_NEXT_HIGH_MONO_COUNT getNextHighMonotonicCount;
    EFI_RESET_SYSTEM resetSystem;
    EFI_UPDATE_CAPSULE updateCapsule;
    EFI_QUERY_CAPSULE_CAPABILITIES queryCapsuleCapabilities;
    EFI_QUERY_VARIABLE_INFO queryVariableInfo;
#endif
};

typedef struct EFI_BOOT_SERVICES {
    EFI_TABLE_HEADER hdr;
#if 0
    // Task priority services
    EFI_RAISE_TPL raiseTpl;
    EFI_RESTORE_TPL restoreTpl;
    // Memory services
    EFI_ALLOCATE_PAGES allocatePages;
    EFI_FREE_PAGES freePages;
    EFI_GET_MEMORY_MAP getMemoryMap;
    EFI_ALLOCATE_POOL allocatePool;
    EFI_FREE_POOL freePool;
    // Event and timer services
    EFI_CREATE_EVENT createEvent;
    EFI_SET_TIMER setTimer;
    EFI_WAIT_FOR_EVENT waitForEvent;
    EFI_SIGNAL_EVENT signalEvent;
    EFI_CLOSE_EVENT closeEvent;
    EFI_CHECK_EVENT checkEvent;
    // Protocol handler services
    EFI_INSTALL_PROTOCOL_INTERFACE installProtocolInterface;
    EFI_REINSTALL_PROTOCOL_INTERFACE reinstallProtocolInterface;
    EFI_UNINSTALL_PROTOCOL_INTERFACE uninstallProtocolInterface;
    EFI_HANDLE_PROTOCOL handleProtocol;
    void* reserved;
    EFI_REGISTER_PROTOCOL_NOTIFY registerProtocolNotify;
    EFI_LOCATE_HANDLE locateHandle;
    EFI_LOCATE_DEVICE_PATH localteDevicePath;
    EFI_INSTALL_CONFIGURATION_TABLE installConfigurationTable;
    // Image services
    EFI_IMAGE_LOAD loadImage;
    EFI_IMAGE_START startImage;
    EFI_EXIT exit;
    EFI_IMAGE_UNLOAD unloadImage;
    EFI_EXIT_BOOT_SERVICES exitBootServices;
    // Miscellaneous services
    EFI_GET_NEXT_MONOTONIC_COUNT getNextMonotonicCount;
    EFI_STALL stall;
    EFI_SET_WATCHDOG_TIMER setWatchdogTimer;
    // EFI 1.1+
    // Driver support services
    EFI_CONNECT_CONTROLLER connectController;
    EFI_DISCONNECT_CONTROLLER disconnectController;
    // Open and close protocol services
    EFI_OPEN_PROTOCOL openProtocol;
    EFI_CLOSE_PROTOCOL closeProtocol;
    EFI_OPEN_PROTOCOL_INFORMATION openProtocolInformation;
    // Library services
    EFI_PROTOCOLS_PER_HANLE protocolsPerHandle;
    EFI_LOCATE_HANDLE_BUFFER locateHandleBuffer;
    EFI_LOCATE_PROTOCOL locateProtocol;
    EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES installMultipleProtocolInterfaces;
    EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES uninstallMultipleProtocolInterfaces;
    // 32-bit crc services
    EFI_CALCULATE_CRC32 calculateCrc32;
    // Miscellaneous services
    EFI_COPY_MEM copyMem;
    EFI_SET_MEM setMem;
    // UEFI 2.0+
    EFI_CREATE_EVENT_EX createEventEx;
#endif
} EFI_BOOT_SERVICES;

typedef struct EFI_CONFIGURATION_TABLE {
    EFI_GUID vendorGuid;
    void* vendorTable;
} EFI_CONFIGURATION_TABLE;

typedef struct EFI_SYSTEM_TABLE {
    EFI_TABLE_HEADER hdr;
    char16_t* firmwareVendor;
    uint32_t firmwareRevision;
    EFI_HANDLE consoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL* conIn;
    EFI_HANDLE consoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* conOut;
    EFI_HANDLE standardErrorHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* stdErr;
    EFI_RUNTIME_SERVICES* runtimeServices;
    EFI_BOOT_SERVICES* bootServices;
    uint64_t numberOfTableEntries;
    EFI_CONFIGURATION_TABLE* configurationTable;
} EFI_SYSTEM_TABLE;
