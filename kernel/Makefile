CFLAGS:=$(CFLAGS) -D_KERNEL_
LDFLAGS:=$(LDFLAGS) -T linker.ld -Xlinker -Map=linker.map
LIBS=-lk -lgcc

CRTI_OBJ=$(patsubst %.S,%.o,$(shell find src -name 'crti.S'))
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ=$(patsubst %.S,%.o,$(shell find src -name 'crtn.S'))

OBJS+=$(patsubst %.c,%.o,$(shell find src -name '*.c'))
OBJS+=$(patsubst %.cpp,%.o,$(shell find src -name '*.cpp'))
OBJS+=$(patsubst %.S,%.o,$(shell find src -name '*.S' ! -name 'crt*.S'))

OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJS) $(CRTEND_OBJ) $(CRTN_OBJ)
INTERNAL_OBJS:=$(CRTI_OBJ) $(OBJS) $(CRTN_OBJ)

LIBK_DEP=$(LIBDIR)/libk.a

KERNEL=$(ISO)/$(BOOTDIR)/NimbleOS.kernel
SYMBOLS=$(ISO)/modules/symbols.map

.PHONY: all clean build install-headers install-kernel

$(KERNEL) $(SYMBOLS): $(OBJ_LINK_LIST) $(LIBK_DEP) linker.ld
	$(info [kernel] linking)
	@mkdir -p $(ISO)/$(BOOTDIR)
	@$(CC) $(LDFLAGS) -o $(KERNEL) $(OBJ_LINK_LIST) $(LIBS)
	$(info [kernel] generating symbol table)
	@mkdir -p $(ISO)/modules
	@awk '$$1 ~ /0x[0-9a-f]{16}/ {print substr($$1, 3), $$2}' linker.map > $(SYMBOLS)
	@rm linker.map

%.o: %.c
	$(info [kernel] $@)
	@$(CC) -c $< -o $@ $(CFLAGS)

%.o: %.cpp
	$(info [kernel] $@)
	@$(CXX) -c $< -o $@ $(CXXFLAGS)

%.o: %.S
	$(info [kernel] $@)
	@$(CC) -c $< -o $@ $(CFLAGS) -D__ASSEMBLY__

build: $(KERNEL)

install-headers:
	$(info [kernel] installing headers)
	@cp -rT include $(INCLUDEDIR)

clean:
	$(info [kernel] $@)
	@rm -f NimbleOS.kernel $(INTERNAL_OBJS) *.o */*.o */*/*.o *.map