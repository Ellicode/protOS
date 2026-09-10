# ProtOS (a.k.a Protogen Operating System)
# ==================================================================
# ProtOS is an x86-64 unix-like and POSIX compliant operating system
# made by Boki (also known as Ellicode) for educational and learning
# purposes.

cmake_minimum_required(VERSION 3.15)

project(ProtOS
    VERSION                     1.0.0
    DESCRIPTION                 "An x86-64 operating system"
    LANGUAGES                   C ASM_NASM
)

add_subdirectory(kernel)