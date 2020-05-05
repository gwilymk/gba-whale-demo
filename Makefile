PATH := $(DEVKITARM)/bin:$(PATH)

include $(DEVKITARM)/gba_rules

# --- Project details -------------------------------------------------

PROJ    := whale
TARGET  := $(PROJ)

IMAGES := $(shell find images -name '*.png')
IMAGE_OBJS := $(patsubst %.png,%.o,$(IMAGES))
IMAGE_HEADERS := $(patsubst %.png,%.h,$(IMAGES))

CFILES  := $(shell find -name '*.c')
OBJS    := $(patsubst %.c,%.o,$(CFILES)) $(IMAGE_OBJS)
DEPS    := $(patsubst %.c,%.d,$(CFILES))

# --- Build defines ---------------------------------------------------

PREFIX  := arm-none-eabi-
CC      := $(PREFIX)gcc
LD      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy

ARCH    := -mthumb-interwork -mthumb
SPECS   := -specs=gba.specs

CFLAGS  := $(ARCH) -O2 -Wall -Wextra -fno-strict-aliasing -I$(LIBGBA)/include -Iinclude -Iimages -flto -Werror=implicit-function-declaration -g
LDFLAGS := $(ARCH) $(SPECS) -L$(LIBGBA)/lib -lgba -flto -g -O2

default: build

.PHONY : build clean default
.SUFFIXES:
.SUFFIXES: .c .o .s .h .png

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
	$(OBJCOPY) -v -O binary $< $@
	-@gbafix $@

$(TARGET).elf : $(OBJS)
	@echo [LD] $<
	@$(LD) $^ $(LDFLAGS) -o $@

# --- Clean -----------------------------------------------------------

.PHONY: clean
clean : 
	@rm -fv *.gba
	@rm -fv *.elf
	@rm -fv *.o *.d
	@rm -rf images/*.o images/*.h images/*.s

-include $(DEPS)