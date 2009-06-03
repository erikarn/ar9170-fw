# Type is AP or STA
TYPE ?= STA

PFX ?= /usr/share/gnush_v0901_elf-1/bin/sh-elf-
CC = $(PFX)gcc
LD = $(PFX)ld
AS = $(PFX)as
OBJCOPY = $(PFX)objcopy

# CPU is a Little endian sh2a-nofpu-or-sh3-nommu
#
# This should be the correct CPU to use but the assmebler is
# buggy when used with little endian option (-ml)...
#CPU = -m2a-nofpu
CPU = -m2

CPU_FLAGS = $(CPU) -ml
CFLAGS += -c $(CPU_FLAGS)  -I./include -D$(TYPE)_ENABLED=1
CFLAGS += -Wall
CFLAGS += -Os -nostartfiles
#LDFLAGS += -static --oformat binary -Map fw.map -Tfw.lnk
LDFLAGS += -static -Map fw.map -Tfw.lnk
LDFLAGS += --strip-all
LDFLAGS += -EL
LDFLAGS += -x
LDFLAGS += -s
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

# These comes from the Linux kernel
FW += ashlsi3.o
#FW += udivsi3.o

%.o: %.S
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

fw: otus-$(TYPE).fw
	$(OBJCOPY) --strip-unneeded -O binary otus-$(TYPE).fw otus-STA.bin
	ln -sf otus-$(TYPE).fw ar9170.fw

otus-$(TYPE).fw: $(FW)
	$(LD) $(LDFLAGS) -o $@ $(FW)

install:
	install ar9170.fw /lib/firmware/

clean:
	rm -f *.o ap.fw sta.fw .files ar9170.fw otus-$(TYPE).*
	rm -f $(TYPE)/*.o usb/*.o

.PHONY: all clean
