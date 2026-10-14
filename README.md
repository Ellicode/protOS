# protOS

ProtOS is an x86-64 unix-like and POSIX compliant operating system made by Boki (also known as Ellicode) for educational and learning purposes.

## History and previous versions

It all started when I wanted to learn C and had the not-so-brilliant idea to make an operating system right away... In short, it went miserably. The first version was an awful vibe-coded kernel, and the second was a slightly better and less AI-Generated piece of unstable and non-compliant software. This is now the third and hopefully the final version of this operating system journey.

## Building

First, install the required limine dependencies using the builtin scaffolding script

```sh
./scripts/scaffold.sh
```

Since my build system of choice is CMake, compiling the kernel is actually pretty simple. Here are the commands below:

```sh
cmake -S . -B build
cmake --build build
```

> [!NOTE]
> If you are compiling on another architecture than x86-64, you'll need to **cross-compile** the kernel. Add the `--toolchain ./toolchain/x86_64.cmake` flag after the first command.

To run the kernel in QEMU, execute the command below:

```sh
./scripts/run.sh
```

## Coding guidelines

### AI-Generated content

Usage of generative artificial intelligence is strictly prohibited within the project. The goal of this OS is to learn computer fundamentals, and AI hinders this objective.

### Text case & Prefixes

Here are the different writing conventions used throughout the project (heavily inspired by GNU C):

```
Macros:                     SCREAMING_SNAKE_CASE
Functions:                  snake_case (should be 16 characters or less for readability)
Structs & Typedefs:         snake_case
Enums:                      snake_case

Globals:                    g_*     (e.g: g_lim_hhdm_offset)
File operations:            f*      (e.g: fprintf, fread)
Kernel-specific operations: k*      (e.g: kalloc, kmain)
Structs & Typedefs:         *_t     (e.g: framebuffer_t)
```

---

Made with love by Boki :3
