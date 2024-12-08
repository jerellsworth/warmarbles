export GDK ?= /opt/toolchains/mars/m68k-elf
MAKEFILE_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR := $(subst \,/,$(MAKEFILE_DIR))

ifneq ("$(wildcard $(MAKEFILE_DIR)bin/rescomp.jar)","")
    GDK := $(patsubst %/,%,$(MAKEFILE_DIR))
endif

include $(GDK)/common.mk

SRC := src
RES := res
INCLUDE := inc


SRC_C= $(wildcard *.c)
SRC_C+= $(wildcard $(SRC)/*.c)
SRC_C+= $(wildcard $(SRC)/*/*.c)
SRC_C+= $(wildcard $(SRC)/*/*/*.c)
SRC_C:= $(filter-out $(SRC)/boot/rom_head.c,$(SRC_C))
SRC_S= $(wildcard *.s)
SRC_S+= $(wildcard $(SRC)/*.s)
SRC_S+= $(wildcard $(SRC)/*/*.s)
SRC_S+= $(wildcard $(SRC)/*/*/*.s)
SRC_S:= $(filter-out $(SRC)/boot/sega.s,$(SRC_S))
SRC_ASM= $(wildcard *.asm)
SRC_ASM+= $(wildcard $(SRC)/*.asm)
SRC_ASM+= $(wildcard $(SRC)/*/*.asm)
SRC_ASM+= $(wildcard $(SRC)/*/*/*.asm)
SRC_S80= $(wildcard *.s80)
SRC_S80+= $(wildcard $(SRC)/*.s80)
SRC_S80+= $(wildcard $(SRC)/*/*.s80)
SRC_S80+= $(wildcard $(SRC)/*/*/*.s80)

RES_C= $(wildcard $(RES)/*.c)
RES_S= $(wildcard $(RES)/*.s)
RES_RES= $(wildcard *.res)
RES_RES+= $(wildcard $(RES)/*.res)

RES_RS= $(RES_RES:.res=.rs)
RES_H= $(RES_RES:.res=.h)
RES_DEP= $(RES_RES:.res=.d)
RES_DEPS= $(addprefix out/, $(RES_DEP))

OBJ= $(RES_RES:.res=.o)
OBJ+= $(RES_S:.s=.o)
OBJ+= $(RES_C:.c=.o)
OBJ+= $(SRC_S80:.s80=.o)
OBJ+= $(SRC_ASM:.asm=.o)
OBJ+= $(SRC_S:.s=.o)
OBJ+= $(SRC_C:.c=.o)
OBJS:= $(addprefix out/, $(OBJ))

DEPS:= $(OBJS:.o=.d)

#-include $(DEPS)

LST:= $(SRC_C:.c=.lst)
LSTS:= $(addprefix out/, $(LST))

INCS:= -I$(INCLUDE) -I$(SRC) -I$(RES) -I$(INCLUDE_LIB) -I$(RES_LIB)
DEFAULT_FLAGS= $(EXTRA_FLAGS) -DSGDK_GCC -m68000 -Wall -Wextra -Wno-shift-negative-value -Wno-main -Wno-unused-parameter -Wno-misleading-indentation -fno-builtin -fms-extensions $(INCS) -B$(BIN)
FLAGSZ80:= -i$(SRC) -i$(INCLUDE) -i$(RES) -i$(SRC_LIB) -i$(INCLUDE_LIB)

#release: FLAGS= $(DEFAULT_FLAGS) -Os -fomit-frame-pointer -fuse-linker-plugin -flto
release: FLAGS= $(DEFAULT_FLAGS) -O3 -fuse-linker-plugin -fno-web -fno-gcse -fno-unit-at-a-time -fomit-frame-pointer -flto
release: CFLAGS= $(FLAGS)
release: AFLAGS= $(FLAGS)
release: LIBMD= $(LIB)/libmd.a
release: pre-build out/rom.bin out/symbol.txt
#release: $(info $$var is [${SRC_C}])

debug: FLAGS= $(DEFAULT_FLAGS) -O1 -DDEBUG=1
debug: CFLAGS= $(FLAGS) -ggdb
debug: AFLAGS= $(FLAGS)
debug: LIBMD= $(LIB)/libmd_debug.a
debug: pre-build out/rom.bin out/rom.out out/symbol.txt

asm: FLAGS= $(DEFAULT_FLAGS) -O3 -fuse-linker-plugin -fno-web -fno-gcse -fno-unit-at-a-time -fomit-frame-pointer -S
asm: CFLAGS= $(FLAGS)
asm: AFLAGS= $(FLAGS)
asm: LIBMD= $(LIB)/libmd.a
asm: pre-build $(LSTS)

# include ext.mk if it exists (better to do it after release rule definition)
ifneq ("$(wildcard $(GDK)/ext.mk)","")
    include $(GDK)/ext.mk
endif

all: release
default: release

Default: release
Debug: debug
Release: release
Asm: asm

.PHONY: clean

cleantmp:
	$(RM) -f $(RES_RS)

cleandep:
	$(RM) -f $(DEPS)

cleanlst:
	$(RM) -f $(LSTS)

cleanres: cleantmp
	$(RM) -f $(RES_H) $(RES_DEP) $(RES_DEPS)

cleanobj:
	$(RM) -f $(OBJS) out/sega.o out/rom_head.bin out/rom_head.o out/rom.out

clean: cleanobj cleanres cleanlst cleandep
	$(RM) -f out.lst out/cmd_ out/symbol.txt out/rom.nm out/rom.wch out/rom.bin

cleanrelease: clean

cleandebug: clean

cleanasm: cleanlst

cleandefault: clean
cleanDefault: clean

cleanRelease: cleanrelease
cleanDebug: cleandebug
cleanAsm: cleanasm

pre-build:
	$(MKDIR) -p $(SRC)/boot
	$(MKDIR) -p out


out/rom.bin: out/rom.out
	$(OBJCPY) -O binary out/rom.out out/rom.bin
	$(SIZEBND) out/rom.bin -sizealign 131072 -checksum

out/symbol.txt: out/rom.out
	$(NM) $(LTO_PLUGIN) -n out/rom.out > out/symbol.txt

out/rom.out: out/sega.o out/cmd_ $(LIBMD)
	$(CC) -B$(BIN) -n -T $(GDK)/md.ld -nostdlib out/sega.o @out/cmd_ $(LIBMD) $(LIBGCC) -o out/rom.out -Wl,--gc-sections
	$(RM) out/cmd_

out/cmd_: $(OBJS)
	$(ECHO) "$(OBJS)" > out/cmd_

out/sega.o: $(SRC)/boot/sega.s out/rom_head.bin
	$(CC) -x assembler-with-cpp -Wa,--register-prefix-optional,--bitwise-or $(AFLAGS) -c $(SRC)/boot/sega.s -o $@

out/rom_head.bin: out/rom_head.o
	$(OBJCPY) -O binary $< $@

out/rom_head.o: $(SRC)/boot/rom_head.c
	$(CC) $(DEFAULT_FLAGS) -c $< -o $@

$(SRC)/boot/sega.s: $(SRC_LIB)/boot/sega.s
	$(CP) $< $@

$(SRC)/boot/rom_head.c: $(SRC_LIB)/boot/rom_head.c
	$(CP) $< $@


out/%.lst: %.c
	$(MKDIR) -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

out/%.o: %.c
	$(MKDIR) -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

out/%.o: %.s
	$(MKDIR) -p $(dir $@)
	$(CC) -x assembler-with-cpp -Wa,--register-prefix-optional,--bitwise-or $(AFLAGS) -MMD -c $< -o $@

out/%.o: %.rs
	$(MKDIR) -p $(dir $@)
	$(CC) -x assembler-with-cpp -Wa,--register-prefix-optional,--bitwise-or $(AFLAGS) -c $*.rs -o $@
	$(CP) $*.d out/$*.d
	$(RM) $*.d

%.rs: %.res
	$(RESCOMP) $*.res $*.rs -dep out/$*.o

%.s: %.asm
	$(MACCER) -o $@ $<

%.o80: %.s80
	$(ASMZ80) $(FLAGSZ80) $< $@ out.lst

%.s: %.o80
	$(BINTOS) $<
