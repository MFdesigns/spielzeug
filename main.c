#include <stdbool.h>
#include <stdint.h>
#include "efi.h"

EFI_STATUS efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable) {
    systemTable->conOut->outputString(systemTable->conOut, L"Hello World\r\n");
    
    while(true) {};
    return EFI_SUCCESS;
}
