mkdir -p .iso/efi/boot
mkdir .iso/limine
mkdir -p .tmp/limine-binary

curl -L -o .tmp/limine-binary.tar.gz https://github.com/Limine-Bootloader/Limine/releases/latest/download/limine-binary.tar.gz
tar -xvf .tmp/limine-binary.tar.gz -C .tmp

mv .tmp/limine-binary/BOOTX64.EFI .iso/efi/boot/BOOTX64.EFI
mv .tmp/limine-binary/limine-bios-cd.bin .iso/limine/limine-bios-cd.bin
mv .tmp/limine-binary/limine-uefi-cd.bin .iso/limine/limine-uefi-cd.bin

rm -r .tmp