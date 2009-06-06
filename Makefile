# Type is AP or STA
TYPE ?= STA

PFX ?= $(shell pwd)/toolchain/inst/bin/sh-elf-
VALID_PFX = $(if $(wildcard $(PFX)gcc), $(PFX), \
	$(error Error: sh2 gcc not found. To build your own, invoke 'make -C toolchain'.))
CC = $(VALID_PFX)gcc
LD = $(VALID_PFX)ld
AS = $(VALID_PFX)as
OBJCOPY = $(VALID_PFX)objcopy

# CPU is a Little endian sh2a-nofpu-or-sh3-nommu
CPU = -m2

CPU_FLAGS = $(CPU) -ml
CFLAGS += -c $(CPU_FLAGS)  -I./include -D$(TYPE)_ENABLED=1
CFLAGS += -Wall
CFLAGS += -g3
CFLAGS += -Os
CFLAGS += -nostartfiles
CFLAGS += -ffunction-sections
LDFLAGS += -static -Tfw.lds
LDFLAGS += -EL
LDFLAGS += -x
LDFLAGS += -e _zfbooter
LDFLAGS += --gc-sections

FW += $(TYPE)/boot.o
FW += $(TYPE)/USB_Boot.o
FW += $(TYPE)/cmd.o
FW += $(TYPE)/cphy.o
FW += $(TYPE)/desc.o
FW += $(TYPE)/gv.o
FW += $(TYPE)/hostif.o
FW += $(TYPE)/mon.o
FW += $(TYPE)/uart.o
FW += usb/usb.o
FW += usb/usb_fifo.o
FW += usb/usb_intrq.o
FW += $(TYPE)/wlan.o
FW += dbg.o

# These comes from the Linux kernel
FW += ashlsi3.o

%.o: %.S
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.fw: %.elf
	$(OBJCOPY) --strip-unneeded -O binary -R .sram $< $@

all:	ar9170.fw

ar9170.fw:	otus-$(TYPE).fw
	ln -fs otus-$(TYPE).fw ar9170.fw

otus-$(TYPE).elf: $(FW) fw.lds
	$(LD) $(LDFLAGS) -o $@ $(FW)

install:
	install ar9170.fw /lib/firmware/

clean:
	@rm -f *.o ar9170.fw otus-AP.* otus-STA.* fw.map *~
	@rm -f AP/*.o STA/*.o usb/*.o
	@rm -f AP/*~ STA/*~ usb/*~ include/*~

.PHONY: all clean
