./bin/image.exe ./bin/

qemu-system-x86_64.exe -cpu qemu64 -bios C:/Michel/bin/OVMF/OVMF-pure-efi.fd -drive file=./bin/spielzeug.iso,if=ide
