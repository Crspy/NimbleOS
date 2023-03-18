export # Makes variables from this file available in sub-makefiles

LC_ALL=C

HOST=i686-elf
PREFIX=usr
BOOTDIR=boot
ISODIR=isodir
ISO=$(PWD)/$(ISODIR)
TARGETROOT=$(PWD)/misc/root
SYSROOTDIR=sysroot
SYSROOT=$(PWD)/$(SYSROOTDIR)

INCLUDEDIR=$(SYSROOT)/$(PREFIX)/include
LIBDIR=$(SYSROOT)/$(PREFIX)/lib

PATH:=$(PATH):/sbin:$(PWD)/toolchain/compiler/bin

MAKE:=$(MAKE) -s
LD=$(HOST)-ld
AR=$(HOST)-ar
AS=$(HOST)-as
CC=$(HOST)-gcc
CXX=$(HOST)-g++

CFLAGS=-O1 -std=gnu11 -ffreestanding -Wall -Wextra
CXXFLAGS= -O1 -std=c++11 -ffreestanding -Wall -Wextra
ASFLAGS=--32
LDFLAGS=-nostdlib -L$(SYSROOT)/usr/lib -Xlinker -m elf_i386

ifeq ($(UBSAN),1)
	CFLAGS+=-fsanitize=undefined
endif

# Uncomment the following group of lines to compile with the system's
# clang installation

# CC=clang
# CXX=clang++
# LD=ld
# AR=ar
# AS=as
# CFLAGS+=-target i386-pc-none-eabi -m32
# CFLAGS+=-mno-mmx -mno-sse -mno-sse2

CC+=--sysroot=$(SYSROOT) -isystem=/$(PREFIX)/include
CXX+=--sysroot=$(SYSROOT) -isystem=/$(PREFIX)/include

# Make will be called on these folders
PROJECTS=libc kernel

# Generate project sub-targets
PROJECT_HEADERS=$(PROJECTS:=.headers) # appends .headers to every project name
PROJECT_CLEAN=$(PROJECTS:=.clean)


DISKIMAGE=$(ISODIR)/modules/disk.img
GRUBCFG=$(ISODIR)/boot/grub/grub.cfg

.PHONY: all build qemu bochs clean

all: build NimbleOS.iso

strict: CFLAGS += -Werror
strict: build

build: $(PROJECTS)

# Copy headers before building anything
$(PROJECTS): $(PROJECT_HEADERS) | $(TARGETROOT) $(LIBDIR)
	@$(MAKE) -C $@ build

# Specify dependencies
kernel: libc

qemu: NimbleOS.iso
	qemu-system-i386 --accel kvm -m 64M -cdrom NimbleOS.iso -monitor stdio -s -no-reboot -no-shutdown -serial file:serial.log
	cat serial.log

bochs: NimbleOS.iso
	bochs -q -rc .bochsrc_cmds
	cat serial.log

vbox: NimbleOS.iso
	# VBOX_GUI_DBG_ENABLED=1 VBOX_GUI_DBG_AUTO_SHOW=1 vboxmanage startvm NimbleOS
	vboxmanage startvm NimbleOS

clean: $(PROJECT_CLEAN)
	@rm -rf $(SYSROOTDIR)
	@rm -rf $(TARGETROOT)
	@rm -rf $(ISODIR)
	@rm -f NimbleOS.iso
	@rm -f misc/grub.cfg
	@rm -f misc/disk.img

NimbleOS.iso: $(PROJECTS) $(GRUBCFG)
	$(info [all] writing $@)
	@grub-mkrescue -o NimbleOS.iso $(ISODIR) 2> /dev/null


$(GRUBCFG):
	$(info [all] generating grub config)
	@mkdir -p $(ISODIR)/boot/grub
	@bash ./misc/grub-config.sh

$(TARGETROOT):
	@mkdir -p $(TARGETROOT)

$(INCLUDEDIR):
	@mkdir -p $(INCLUDEDIR)

$(LIBDIR):
	@mkdir -p $(LIBDIR)


# Automatic rules for our generated sub-targets
%.headers: %/ | $(INCLUDEDIR)
	@$(MAKE) -C $< install-headers

%.clean: %/
	@$(MAKE) -C $< clean
