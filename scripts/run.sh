# QEMU run script

LOCAL_OVMF_CODE_PATH=".ovmf/OVMF_CODE.fd"
REMOTE_OVMF_CODE_PATH="/usr/share/edk2/x64/OVMF_CODE.4m.fd"
ALLOCATED_MEMORY=512M

QEMU='\033[0;35m'
OVMF='\033[0;32m'
RST='\033[0m'

if [ -f "$LOCAL_OVMF_CODE_PATH" ]; then
    echo -e "[${OVMF}ovmf${RST}]: OVMF detected."
else
    echo -e "[${OVMF}ovmf${RST}]: OVMF not found."
    read -p "[${OVMF}ovmf${RST}]: Do you wish to move automatically $REMOTE_OVMF_CODE_PATH to the '.ovmf' directory? (y/n): " yn
    case $yn in
        [Yy]* ) mkdir -p .ovmf && cp $REMOTE_OVMF_CODE_PATH $LOCAL_OVMF_CODE_PATH; 
                echo -e "[${OVMF}ovmf${RST}]: Moved successfully.";;
        * ) echo -e "[${OVMF}ovmf${RST}]: Cancelled."; exit;;
    esac
fi

echo -e "[${QEMU}qemu${RST}]: Launching qemu..."

case $1 in
    iso )   qemu-system-x86_64 \
                -m $ALLOCATED_MEMORY \
                -drive if=pflash,format=raw,readonly=on,file=$LOCAL_OVMF_CODE_PATH \
                -cdrom build/protOS.iso \
                -boot d \
                -vga std \
                -enable-kvm \
                -cpu host \
                -serial stdio;;
    * )     qemu-system-x86_64 \
                -m $ALLOCATED_MEMORY \
                -drive if=pflash,format=raw,readonly=on,file=$LOCAL_OVMF_CODE_PATH \
                -drive if=ide,format=raw,file=fat:rw:.iso \
                -vga std \
                -enable-kvm \
                -cpu host \
                -serial stdio;;
esac
