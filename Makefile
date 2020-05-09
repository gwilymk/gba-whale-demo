PATH := $(DEVKITARM)/bin:$(PATH)

include $(DEVKITARM)/gba_rules

# --- Project details -------------------------------------------------

PROJ    := whale
TARGET  := $(PROJ)

IMAGES := $(shell find images -name '*.png')
IMAGE_OBJS := $(patsubst %.png,%.o,$(IMAGES))
IMAGE_HEADERS := $(patsubst %.png,%.h,$(IMAGES))

CFILES  := $(shell find -name '*.c')
HFILES  := $(shell find -name '*.h')
OBJS    := $(patsubst %.c,%.o,$(CFILES)) $(IMAGE_OBJS)
DEPS    := $(patsubst %.c,%.d,$(CFILES))

# --- Build defines ---------------------------------------------------

PREFIX  := arm-none-eabi-
CC      := $(PREFIX)gcc
LD      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy

ARCH    := -mthumb-interwork -mthumb
SPECS   := -specs=gba.specs

CFLAGS  := $(ARCH) -O2 -flto -g \
	-Wall -Wextra -fno-strict-aliasing -Werror=implicit-function-declaration -Wstrict-prototypes -Wwrite-strings -Wuninitialized \
	-I$(LIBGBA)/include -Iinclude -Iimages

LDFLAGS := $(ARCH) $(SPECS) -L$(LIBGBA)/lib -lgba -flto -g -O2

default: build

.PHONY : build clean default docs dump gdb
.SUFFIXES:
.SUFFIXES: .c .o .s .h .png

gdb: whale.elf
	$(PREFIX)gdb whale.elf

docs: docs/html/index.html

docs/html/index.html: Makefile Doxyfile $(CFILES) $(HFILES)
	@echo [DOXYGEN]
	@-doxygen

dump: $(TARGET).dump

$(TARGET).dump: $(TARGET).elf Makefile
	@echo [OBJDUMP]
	@$(PREFIX)objdump -Sd $< > $@

.SECONDARY: $(IMAGE_HEADERS)

%.o : %.c
%.o : %.s

%.o : %.c Makefile $(IMAGE_HEADERS)
	@echo [CC] $<
	@$(CC) -c $< $(CFLAGS) -o $@ -MMD -MP

%.o : %.s Makefile
	@echo [ASM] $<
	@$(CC) -c $< $(CFLAGS) -o $@

%.s %.h: %.png Makefile
	@echo [GRIT] $<
	@(cd `dirname $<` && grit `basename $<` -gB4 -gt -Mw2 -Mh2)

# --- Build -----------------------------------------------------------
# Build process starts here!
build: $(TARGET).gba

$(TARGET).gba : $(TARGET).elf
	@echo [OBJCOPY] $@
	@$(OBJCOPY) -v -O binary $< $@ > /dev/null
	@gbafix $@ > /dev/null

$(TARGET).elf : $(OBJS)
	@echo [LD] $<
	@$(LD) $^ $(LDFLAGS) -o $@

# --- Clean -----------------------------------------------------------

.PHONY: clean
clean :
	@rm -fv $(TARGET).gba $(TARGET).elf $(TARGET).dump
	@rm -fv $(OBJS) $(DEPS)
	@rm -rf images/*.h images/*.s

-include $(DEPS)