#include "types.hpp"
#include "efi.hpp"

EFI_STATUS efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable) {
    systemTable->conOut->outputString(systemTable->conOut, (char16_t*)u"Hello World\r\n");
    
    while(true) {};
    return EFI_SUCCESS;
}
