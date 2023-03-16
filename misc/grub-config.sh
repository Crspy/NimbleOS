#!/usr/bin/bash

# Expects to be run from the main Makefile
cat > "$GRUBCFG" << EOF
menuentry "NimbleOS" {
	multiboot /boot/NimbleOS.kernel
}
EOF
