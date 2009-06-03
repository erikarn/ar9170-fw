# Type is AP or STA
TYPE ?= STA

CC = /usr/share/gnush_v0901_elf-1/bin/sh-elf-gcc
LD = /usr/share/gnush_v0901_elf-1/bin/sh-elf-ld
AS = /usr/share/gnush_v0901_elf-1/bin/sh-elf-as
OBJCOPY = /usr/share/gnush_v0901_elf-1/bin/sh-elf-objcopy

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

FW += boot.o
FW += USB_Boot.o
FW += cmd.o
FW += cphy.o
FW += desc.o
FW += gv.o
FW += hostif.o
FW += mon.o
FW += uart.o
FW += usb.o
FW += usb_fifo.o
FW += usb_intrq.o
FW += wlan.o

# These comes from the Linux kernel
FW += ashlsi3.o
#FW += udivsi3.o

%.o: %.S
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

.files:
	cp $(TYPE)/*.[ch] .
	cp usb/*.c .
	@touch $@

all: .files
	@echo
	@echo Now run make fw

fw: otus-$(TYPE).fw
	$(OBJCOPY) --strip-unneeded -O binary otus-$(TYPE).fw otus-STA.bin
	ln -sf otus-$(TYPE).fw ar9170.fw

otus-$(TYPE).fw: $(FW)
	$(LD) $(LDFLAGS) -o $@ $(FW)

install:
	install ar9170.fw /lib/firmware/

clean:
	rm -f *.c *.h *.o ap.fw sta.fw .files ar9170.fw otus-$(TYPE).*

.PHONY: all clean
