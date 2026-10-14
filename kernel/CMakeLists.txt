add_executable(kernel
    src/bootloader/limine/preboot.c

    src/drivers/serial.c

    src/ui/console.c
    src/ui/font.c
    src/ui/vfx.c
    src/ui/fonts/ter-u16n.c

    src/std/string.c

    src/dt/gdt.c
    src/dt/gdt.nasm
    src/dt/idt.c
    src/dt/idt.nasm

    src/cpu/sse.c
    src/cpu/acpi.c

    src/interrupts/isr.c
    src/interrupts/apic.c

    src/mem/pmm.c
    src/mem/vmm.c
    src/mem/freelist.c
    src/mem/heap.c

    src/fs/vfs.c
    src/fs/tarfs.c

    src/boot.c
    src/panic.c
    src/panic.asm
)

target_compile_definitions(kernel PRIVATE
    PROTO_DEBUG
)

target_compile_options(kernel PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -m64
        -march=x86-64
        -mno-red-zone
        -mcmodel=kernel
        -fno-stack-protector
        -fno-pie
        -fno-pic
        -ffreestanding
        -Wall
        -Wextra
        -g
    >
)

target_include_directories(kernel PRIVATE include)

target_link_options(kernel PRIVATE
    -m64
    -nostdlib
    -nostartfiles
    -nodefaultlibs
    -no-pie
    "-Wl,-T,${CMAKE_SOURCE_DIR}/kernel/linker.ld"
    -Wl,--build-id=none
    -Wl,-z,max-page-size=0x1000
)

add_custom_command(
    TARGET      kernel
    POST_BUILD
    COMMAND     ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:kernel>" "${CMAKE_SOURCE_DIR}/.iso/protOS"
    COMMENT     "Copying kernel to ISO"
    VERBATIM
)

add_custom_command(
    TARGET      kernel
    POST_BUILD
    COMMAND     ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}"
    COMMAND     xorriso -as mkisofs -R -J
                        -b limine/limine-bios-cd.bin 
                        -no-emul-boot
                        -boot-load-size 4 -boot-info-table 
                        --efi-boot limine/limine-uefi-cd.bin
                        -efi-boot-part --efi-boot-image
                        --protective-msdos-label 
                        "${CMAKE_SOURCE_DIR}/.iso" -o "${CMAKE_BINARY_DIR}/protOS.iso" 
    COMMENT     "Generating ISO image"
    VERBATIM
)

add_custom_command(
    TARGET      kernel
    POST_BUILD
    COMMAND     tar -vcf "${CMAKE_SOURCE_DIR}/.iso/initrd.tar" -C "${CMAKE_SOURCE_DIR}/.initrd" .
    COMMENT     "Generating initrd"
    VERBATIM
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)