CFLAGS='-target x86_64-unknown-windows
        -ffreestanding
        -mno-red-zone'
LDFLAGS='-target x86_64-unknown-windows
         -nostdlib
         -Wl,-entry:efi_main
         -Wl,-subsystem:efi_application
         -fuse-ld=lld-link'
 
clang $CFLAGS -c -o main.o main.c
clang $LDFLAGS -o BOOTX64.EFI main.o
