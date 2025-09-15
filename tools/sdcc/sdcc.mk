
ifeq (_$(SDCC_DIR)_,__)
    SDCC_DIR    = /opt/SDCC
endif

SDCC_BIN    = $(SDCC_DIR)/bin
INCLUDE_DIR = $(SDCC_DIR)/include
LIB_DIR     = $(SDCC_DIR)/share/sdcc/lib/hc08

CC  = $(SDCC_BIN)/sdcc -mhc08
AS  = $(SDCC_BIN)/sdas6808
CPP = $(SDCC_BIN)/sdcpp
LNK = $(SDCC_BIN)/sdld6808

CPPFLAGS = -I. -I$(INCLUDE_DIR) -DVARIANT=\"$(VARIANT)\"

OPTS += --nooverlay         #  Disable overlaying leaf function auto variables
OPTS += --nogcse            #  Disable the GCSE optimisation
OPTS += --nolabelopt        #  Disable label optimisation
OPTS += --noinvariant       #  Disable optimisation of invariants
OPTS += --noinduction       #  Disable loop variable induction
OPTS += --noloopreverse     #  Disable the loop reverse optimisation
OPTS += --no-peep           #  Disable the peephole assembly file optimisation
#OPTS += --no-reg-params     #  On some ports, disable passing some parameters in registers
#OPTS += --peep-asm          #  Enable peephole optimization on inline assembly
#OPTS += --peep-return       #  Enable peephole optimization for return instructions
OPTS += --no-peep-return    #  Disable peephole optimization for return instructions
#OPTS += --peep-file         #  <file> use this extra peephole file
#OPTS += --opt-code-speed    #  Optimize for code speed rather than size
#OPTS += --opt-code-size      #  Optimize for code size rather than speed
#OPTS += --out-fmt-ihx

OPTS += --verbose
# OPTS += --stack-auto
# OPTS += --int-long-reent
# OPTS += --float-reent
OPTS += --fomit-frame-pointer
# OPTS += --fno-omit-frame-pointer
# OPTS += --callee-saves
# OPTS += --stack-probe
OPTS += --dataseg _GLOBAL_VARS
OPTS += --codeseg HOME
OPTS += --constseg HOME
OPTS += $(MEM_MODEL)
# OPTS += --model-small
# OPTS += --oldralloc
# OPTS += --iram-size 184
OPTS += --iram-size $(RAM_SIZE)
OPTS += --code-size $(FLASH_SIZE)
OPTS += --code-loc $(FLASH_START)
OPTS += --data-loc $(RAM_START)
OPTS += --stack-loc $(STACK_LOC)
OPTS += --fverbose-asm

OPTS += --nostdlib
# OPTS += --nostdlibcall

CFLAGS   = -I. -I$(INCLUDE_DIR) $(OPTS)
AFLAGS  = -glosw

include .old_size

# do everything (comple,assembly,link) in one go
$(TARGET).rel: $(TARGET).c $(MAKEFILES) $(DEPEND) $(TARGET).dep objsize.pl
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET).s19 && ./objsize.pl $(TARGET).map $(RAM_START) $(RAM_SIZE) $(FLASH_START) $(FLASH_SIZE) $(USED_RAM) $(USED_FLASH) >.old_size

sdcc_clean:
	@echo Deleting all object, and list files...
	rm -f *.a08 *.rel *.lst *.lk *.mem *.cdb *.rst *.sym $(TARGET)-final.s19 $(TARGET)-$(FLASH_START).bin $(TARGET).asm $(TARGET).elf *.map.

# rules to show RAM and FLASH usage of the new build
$(TARGET).map: $(TARGET).rel

ifeq (_$(USED_RAM)_,__)
.old_size: 
	echo "USED_RAM = 0 \
USED_FLASH = 0"	>.old_size
endif

# how to find the script
objsize.pl:
	ln -s $(REL_INC)/tools/sdcc/$@

# force recreation of dependencies
depend:
	rm -f $(TARGET).dep
	$(MAKE) $(TARGET).dep

# how to generate the later to be included deopendency file
$(TARGET).dep: $(DEPEND) .CPU
	$(CC) -M $(CFLAGS) $(TARGET).c | egrep -v sdcc: >$(TARGET).dep

# dependencies
$(TARGET).s19:	$(TARGET).rel $(TARGET).dep $(DEPEND) $(MAKEFILES)

# compiler generated dependency file
include $(TARGET).dep

# how to create symbolic links to the compiler library sources
_mulint.c:
	ln -s $(SDCC_DIR)/src/device/lib/hc08/$@

_divsint.c:
	ln -s $(SDCC_DIR)/src/device/lib/$@

_modsint.c:
	ln -s $(SDCC_DIR)/src/device/lib/$@

_divuint.c:
	ln -s $(SDCC_DIR)/src/device/lib/$@

_moduint.c:
	ln -s $(SDCC_DIR)/src/device/lib/$@

_mullong.c:
	ln -s $(SDCC_DIR)/src/device/lib/$@
	
_divulong.c:
	ln -s $(SDCC_DIR)/src/device/lib/$@

