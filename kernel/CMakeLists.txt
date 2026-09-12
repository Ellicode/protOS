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

    src/interrupts/isr.c

    src/mem/pmm.c
    src/mem/freelist.c

    src/boot.c
    src/panic.c
    src/panic.asm
)

target_compile_definitions(kernel PRIVATE
#    PROTO_DEBUG
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
    COMMAND     ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:kernel>" "../../.iso/protOS"
    COMMENT     "Copying kernel to ISO"
    VERBATIM
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)