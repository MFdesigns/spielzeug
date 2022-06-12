#include "types.h"
#include "efi.h"

EFI_STATUS efi_main(void* ImageHandle, struct EFI_SYSTEM_TABLE* SystemTable) {
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (char16*)u"Starting...\r\n");

    while(true) {};
    return EFI_SUCCESS;
}
