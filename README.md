# NimbleOS

My first hobby OS to get into the world of OS Engineering.\
currently only supports :
- :white_check_mark: multboot compliant kernel
- :white_check_mark: higher half kernel
- :white_check_mark: 80x25 text mode
- :white_check_mark: Handle hardware exceptions and IRQs
- :white_check_mark: IPT Timer
- :white_check_mark: PS2 Keyboard
- :white_check_mark: Memory paging
- :white_check_mark: Physical memory manager (PMM)

## Building & running

### Dependencies

Install the following packages:
+ `grub`
+ `xorriso` for Debian/Ubuntu; `libisoburn` on Archlinux
+ `mtools`
+ `qemu` (recommended)
+ `bochs` (optional)
+ `i686 cross compiler or clang`

### Cross-compiler

#### Using a preinstalled cross-compiler

If your distro provides you with a cross compiler like i686-elf-gcc on Archlinux, you may want to save time and use it. To do so, you must edit the following variables in the main `Makefile` so that they match the executables of your cross compiler:

    AR=$(HOST)-ar
    AS=$(HOST)-as
    LD=$(HOST)-ld
    CC=$(HOST)-gcc --sysroot=$(SYSROOT) -isystem=/$(INCLUDEDIR)

You may edit `HOST=i686`, or hardcode the executables names directly.

#### Using clang directly

Instead of building your own toolchain, you can compile NimbleOS with your system's `clang`: simply uncomment the following group of lines in the main `Makefile`:
```shell
# CC=clang
# LD=ld
# AR=ar
# AS=as
# CFLAGS+=-target i386-pc-none-eabi -m32
# CFLAGS+=-mno-mmx -mno-sse -mno-sse2
```

### Running NimbleOS

Run either

    make qemu # or
    make bochs

