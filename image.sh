dd if=/dev/zero of=bin/uefi.img bs=512 count=93750

parted bin/uefi.img -s -a minimal mklabel gpt
parted bin/uefi.img -s -a minimal mkpart EFI FAT16 2048s 93716s
parted bin/uefi.img -s -a minimal toggle 1 boot

dd if=/dev/zero of=bin/part.img bs=512 count=91669
mformat -i bin/part.img -h 32 -t 32 -n 64 -c 1

mmd -i bin/part.img ::/EFI
mmd -i bin/part.img ::/EFI/BOOT
mcopy -i bin/part.img bin/BOOTX64.EFI ::/EFI/BOOT

dd if=bin/part.img of=bin/uefi.img bs=512 count=91669 seek=2048 conv=notrunc
