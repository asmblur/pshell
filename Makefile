# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#

EE_OBJS_DIR = obj
EE_LIB_DIR = lib
EE_SRC_DIR = src
EE_INC_DIR = include
EE_BIN_DIR = bin

EE_LOADER = pshellloader.elf
EE_BIN = pshell.elf
EE_KBIN = pshellk.elf
EE_KBIN_DUMP_N2E = pshellk.n2e

EE_OBJS = crt0.o pshell.o ee_debug.o ee_dbg_low.o ee_sio.o terminal.o vt100.o cmds.o low_stuff.o sbus_dbg.o iop_lib.o syscalls.o dasm_R5900.o tinyprintf.o
EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)/%)

EE_CFLAGS = -Iinclude \
	-DTINYPRINTF_DEFINE_TFP_PRINTF=1 \
	-DTINYPRINTF_DEFINE_TFP_SPRINTF=1 \
	-DTINYPRINTF_OVERRIDE_LIBC=0

#EE_CFLAGS += -D_KMODE

EE_LIBS =  -lsbus
# -lsio

all: $(EE_BIN) $(EE_KBIN) $(EE_KBIN_DUMP_N2E) $(EE_LOADER)

clean:
	rm -f $(EE_BIN_DIR)/*.elf $(EE_BIN_DIR)/*.bin $(EE_BIN_DIR)/*.n2e $(EE_OBJS_DIR)/*.o

include $(PS2SDK)/Defs.make
include ./Makefile.eeglobal

%.n2e : $(EE_KBIN)
	n2epack tmp $@

$(EE_LOADER) : $(EE_KBIN)
	$(EE_OBJCOPY) -O binary $(EE_BIN_DIR)/$< tmp
	bin2o tmp $@ binary_BIN_$*

%.n2e : $(EE_BIN_DIR)/%.elf
	$(EE_OBJCOPY) -O binary $< tmp.bin
	n2epack tmp.bin $@

$(EE_BIN_DIR)/%.bin : $(EE_BIN_DIR)/%.elf
	$(EE_OBJCOPY) -O binary $< $@

%.o : $(EE_BIN_DIR)/%.bin
	bin2o $< $@ binary_BIN_$*

$(EE_OBJS_DIR)/%.o : $(EE_SRC_DIR)/%.c
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

$(EE_OBJS_DIR)/%.o : $(EE_SRC_DIR)/%.cpp
	$(EE_CXX) $(EE_CXXFLAGS) $(EE_INCS) -c $< -o $@

$(EE_OBJS_DIR)/%.o : $(EE_SRC_DIR)/%.S
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

$(EE_OBJS_DIR)/%.o : $(EE_SRC_DIR)/%.s
	$(EE_AS) $(EE_ASFLAGS) $< -o $@

