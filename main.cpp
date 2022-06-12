#include "types.hpp"
#include "efi.hpp"

EFI_STATUS efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable) {
    systemTable->conOut->clearScreen(systemTable->conOut);
    systemTable->conOut->outputString(systemTable->conOut, (char16_t*)u"Hello World\r\n");

    uint64_t memMapSize = 1;
    EFI_MEMORY_DESCRIPTOR memoryMap[1];
    uint64_t mapKey = 0;
    uint64_t descriptorSize = 0;
    uint32_t descriptorVersion = 0;

    // systemTable->efiBootServices->getMemoryMap(&memMapSize, memoryMap, &mapKey, &descriptorSize, &descriptorVersion);
    
    while(true) {};
    return EFI_SUCCESS;
}
