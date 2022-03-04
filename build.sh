CFLAGS='-target x86_64-unknown-windows
        -ffreestanding
        -mno-red-zone'
LDFLAGS='-target x86_64-unknown-windows
         -nostdlib
         -Wl,-entry:efi_main
         -Wl,-subsystem:efi_application
         -fuse-ld=lld-link'
 
clang++ $CFLAGS -c -o bin/main.o main.cpp
clang++ $LDFLAGS -o bin/BOOTX64.EFI bin/main.o
