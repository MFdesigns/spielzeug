clang `
    -target x86_64-unknown-windows `
    -ffreestanding `
    -mno-red-zone `
    -c -o "./bin/main.o" "./src/efi_main.c"

clang `
    -target x86_64-unknown-windows `
    -nostdlib `
    "-Wl,-entry:efi_main" `
    "-Wl,-subsystem:efi_application" `
    -fuse-ld=lld-link `
    -o "./bin/BOOTX64.EFI" "./bin/main.o"
