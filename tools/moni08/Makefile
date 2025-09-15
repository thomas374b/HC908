
####
## universal Makefile by new_make (pantecs Makefile templater)
##

TARGET = moni08
LIBS   = -ltns_util
DEFS = 

EXTRA_INC =  
EXTRA_LIB = -L/opt/Pantec/lib 

BINDIR = bin
# where to install, choose one of bin, sbin or libexec

SUBPREFIX = /Pantec
# directory relative to your rootprefix (defaults to /opt/Pantec ) 

ifneq (_$(DEBUG_FLAGS)_,__)
GDBPREFIX = G
endif

CROSS = 
	# exec prefix for cross compiler e.g. arm-v4l-linux-
	
OPTS	= -O3 # -funroll-loops 
WARN	= -Wall -Wno-unused
DEBUG	= -ggdb -DDEBUG
CODE	= -fpic # -fpcc-struct-return 

MAJOR = 0
MINOR = 1
PATCHLEVEL = 1
VERSION = $(MAJOR).$(MINOR).$(PATCHLEVEL)

ifeq (_${MK_DEBPKG}_,__)
MK_DEBIAN_PKG = 0
	# set to 1 to create and install debian packages
else
MK_DEBIAN_PKG = 1
	# to create debian packages was selected by environment variable MK_DEBPKG
endif

include .device

ifneq (_$(DEVICE)_,__)
include .cache-$(DEVICE)
endif

include built/.versions

UMA = $(UNAME)-$(MACHINE)

ifeq (_$(UNAME)_,_AVR_)

	MCU = $(MACHINE)
	CROSS = avr-

	CODE = -mmcu=$(MCU) -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields
	CODE += -ffunction-sections -fdata-sections
	OPTS = -Os
	
	DEBUG = -gdwarf-2 

	HEX_FLASH_FLAGS = -R .eeprom
	HEX_EEPROM_FLAGS = -j .eeprom
	HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
	HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

	AVRDUDE = $(AVRDUDE_PREAMBEL) avrdude -v $(FLASH_CONFIG) -p $(FLASH_TARGET) -c $(FLASH_PROGRAMMER) -P $(FLASH_DEV)

ifeq (_$(WRITE_FUSES)_,__)
	WRITE_FUSES = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m
	READ_FUSES = -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h
endif

else
ifeq (_$(MACHINE)_,_strongarm_)
	# Neo
	# zaurus (all models, kernel), use original Sharp compiler
	CROSS = arm-linux-
else
ifeq (_$(MACHINE)_,_armv5te_)
		# optimized for zaurus SLC-400 and higher, applications only
		CROSS = arm-v5te-linux-
else
ifeq (_$(MACHINE)_,_etrax100lx_)
			# FOX board
			CROSS = cris-axis-linux-gnu-
else
ifeq (_$(MACHINE)_,_armv7_)
ifeq (_$(shell uname -m)_,_armv7l_) 
		# compiling on banana-pi directly
else
				# Kobo Mini
				# small executables, no float/double, knows -march=armv7
				# CROSS = arm-linux-gnueabihf-				
				# for kobo kernel
				CROSS = arm-none-linux-gnueabi-
				# supports -msoft-float, denies -march=armv7
				DEFINES += -D__LINUX_ARM_ARCH__=7
endif				
else
ifeq (_$(MACHINE)_,_armv5_)
					# Olinuxino iMX233 for kernels  
					# CROSS = arm-none-eabi-
					CROSS = arm-none-linux-gnueabi-
					EXTRA_INC += -I/opt/Toolchains/arm-none-linux-gnueabi/arm-none-linux-gnueabi/libc/usr/include		
endif
endif
endif
endif	
endif	
endif	

ifeq (_$(_GCC)_,__)
_GCC = g++
# _GCC = gcc
endif

CC	= $(CROSS)$(_GCC)
LD	= $(CROSS)ld
OBJDUMP = $(CROSS)objdump
OBJCOPY = $(CROSS)objcopy
AR	= $(CROSS)ar
STRIP = $(CROSS)strip
CC_LINK	= $(CROSS)$(_GCC)
CC_VERSION = $(shell $(CC) -dumpversion)

SBIN_TARGET = $(shell test -f man/man8/$(TARGET).8 && echo "s")

#################################################################
#		first fence
#################################################################

# enable flags for various toolkits
WITH_XAW = 
WITH_MOTIF = 
WITH_XFORMS = 
WITH_MESAGL = 
WITH_QTOPIA = 
WITH_WXWIDGETS = 
WITH_X = 
WITH_XINC = 

# REVISION = $(shell date +%Y/%m/%d,%H:%M)
UID = $(shell id |sed -e 's/[0-9][0-9]*/ & /g'|cut -f2 -d' ')

	OPTROOT = /opt
		# choose one of /Software, /public, /usr/local, /opt or whatever your prefix is 

ifeq (_$(PREFIX)_,__)
ifeq (_$(MACHINE)_,_strongarm_)
ifeq (_$(DEVICE)_,_neo_)
			SUBPREFIX = /qtmoko
else
			OPTROOT = /mnt/SD/
			SUBPREFIX = /armv4l
endif
else
ifeq (_$(MACHINE)_,_armv5te_)
			OPTROOT = /mnt/SD/
			SUBPREFIX = /armv5te
else
ifeq (_$(MACHINE)_,_etrax100lx_)
				OPTROOT = /mnt/SD/
				SUBPREFIX = /etrax			
endif
endif	
endif	


ifeq (_$(CROSS)_,__)
ifeq (_$(UID)_,_0_)
ifeq ($(MK_DEBIAN_PKG),1)
				# debian package is being built, PREFIX must be /usr
				PREFIX = /usr
else
		    PREFIX = $(OPTROOT)$(SUBPREFIX)
endif
else
ifeq (_$(SUBPREFIX)_,_/Pantec_)
			# unchanged
		PREFIX = ${HOME}
		# private users installation goes to your homedir
else
			# customized
			PREFIX = $(OPTROOT)$(SUBPREFIX)
endif			
endif
else	
		PREFIX = $(OPTROOT)$(SUBPREFIX)
endif
else
	PREFIX = ${PREFIX}
		# use prefix variable from environment
endif	# PREFIX __


ifeq (_$(CROSS)_,__)
ifeq (_$(UID)_,_0_)
		PKGLOG = /var/log/packages
else
		PKGLOG = $(PREFIX)/.packages
endif
else
	PKGLOG = $(PREFIX)/.packages
endif

ETCRT = $(PREFIX)

ifeq (_$(UID)_,_0_)
	# root is compiling
	CODEPREFIX=$(PREFIX)
else
ifeq (_$(PREFIX)_,_/usr_)
		# a debian package is being built
		CODEPREFIX=$(PREFIX)
		ETCRT = /
else
		# machine dependent prefix for user home installation
ifeq (_$(CROSS)_,__)		
ifeq (_$(SUBPREFIX)_,_/Pantec_)
		CODEPREFIX=$(PREFIX)/$(UMA)
else
			CODEPREFIX=$(PREFIX)
endif # SUBPREFIX /Pantec	
else
			CODEPREFIX=$(PREFIX)
endif # CROSS __	
endif # REFIX /usr
endif # UID 0

DEFINES += -DETC_PREFIX=\"$(ETCRT)\"


UISRC = 
MOCSRC = 
RCCSRC = 

ifeq ($(WITH_QTOPIA),y)
# include qtopia1.mk
endif	# i f e q  WITH_QTOPIA

MANSUFFIX = 1

GPREFIX=g
# on Solaris, HP-UX or IBM AIX gnu utils are prefixed like "gmake", "ginstall", "gtar" etc.
ifeq (_$(UNAME)_,_Linux_)
	# native
	GPREFIX=
endif

ifeq (_$(UNAME)_,_cygwin_)
	GPREFIX=
endif

ifeq (_$(UNAME)_,_AVR_)
	GPREFIX=
endif


TAR = $(GPREFIX)tar
INSTALL = $(GPREFIX)install
 
ifeq (_$(UNAME)_,_Linux_)
ifeq (_$(MACHINE)_,_sparc64_)
		OPTS += -msupersparc
		LIBS += -lNoVersion
else
ifeq (_$(MACHINE)_,_strongarm_)
			# special optimizations for xscale cpu
			OPTS += -mtune=$(MACHINE) 
else
ifeq (_$(MACHINE)_,_armv5te_)
			# special optimizations for xscale cpu
			OPTS += -march=$(MACHINE) -Wa,-mfpu=fpa
else
ifeq (_$(LIBC)_,_5_)
			OPTS += -m486
				DEFS += -DLIBC$(LIBC)
else
ifeq (_$(CC)_,_arm-none-linux-gnueabi-$(_GCC)_)
				# -march=armv7 not supported
				#       OPTS += -mno-thumb
else
				OPTS += -march=$(MACHINE) 
endif # arm-none-linux-gnueabi				
endif # LIBC	
endif # _armv5te_	
endif # _strongarm_
endif # _sparc64_	
endif # _Linux_	

ifeq (_$(UNAME)_,_SunOS_)
	EXTRA_INC += -I/usr/openwin/include # -I/usr/ucbinclude 
	LD      = /usr/ccs/bin/ld
	STRIP   = /usr/ccs/bin/strip
	OPTS += -msupersparc
	LIBS += -lsocket -lnsl
endif

ifeq ($(WITH_QTOPIA),y)
	SHARES_PATH = $(QTDIR)/share/$(TARGET)
else
	SHARES_PATH = $(PREFIX)/share/$(TARGET)
endif


DEFS += -D$(UNAME) -D__$(MACHINE)__ 
DEFS += -DCOMPILER_HOST=\"$(shell hostname)\"
DEFS += -DCOMPILER_EXE=\"$(CC)\"
DEFS += -DCOMPILER_VERSION=\"$(CC_VERSION)\"


TARGETPREFIX=$(CODEPREFIX)

# i f e q (_$(UID)_,_0_)
ifneq (_$(CROSS)_,__)
ifeq (_$(MACHINE)_,_etrax100lx_)
			TARGETPREFIX = /mnt/flash
endif
		SHARES_PATH = $(TARGETPREFIX)/share/$(TARGET)
endif
# e n d i f	

DEFS += -DPREFIX=\"$(TARGETPREFIX)\" 
DEFS += -DSHARES_PATH=\"$(SHARES_PATH)\"
INCLDIR = -I$(PREFIX)/include $(EXTRA_INC)
#LIBDIR	= -L$(PREFIX)/$(UMA)/lib 
LIBDIR= -L$(CODEPREFIX)/lib$(GDBPREFIX) $(EXTRA_LIB)

ifeq (_$(UNAME)_,_hpux_)
	SHARED_LIB = -b +b $(CODEPREFIX)/lib -B immediate -B nonfatal                                       
	LINK_OPTS = -Wl,+b,$(CODEPREFIX)/lib,-B,immediate,-B,nonfatal                                       
	MACHINE = $(shell uname -m|sed -e "s/\//\./g")
endif

ifeq (_$(MACHINE)_,_etrax100lx_)
	LIBS += -lstdc++
	LIBDIR	+= -L/opt/cris/cris-axis-linux-gnu/lib
	INCLDIR += -I/opt/cris/cris-axis-linux-gnu/sys-include
#	DEFS += -DQT_NO_PROPERTIES
endif


XINCLDIR =
XLIBDIR =
XLIBPATH =

ifeq ($(WITH_MESAGL),y)
	LIBS += -lMesaGL -lpthread 
	XLIBDIR += -L$(OPTROOT)/lib/MesaGL
endif

ifeq ($(WITH_WXWIDGETS),y)
	XLIBDIR	+= $(shell wx-config --libs)
	XINCLDIR += $(shell wx-config --cflags)
endif

ifeq ($(WITH_MOTIF),y)
ifeq (_$(UNAME)_,_SunOS_)  
		XINCLDIR += -I/usr/dt/include
		XLIBDIR  += -L/usr/dt/lib
else
ifeq (_$(UNAME)_,_Linux_)  
#			INCLDIR += -I$(OPTROOT)/LessTif/Motif1.2/include
			XINCLDIR += -I$(OPTROOT)/include/Lesstif-2.1
#			LIBDIR  += -L$(OPTROOT)/LessTif/Motif1.2/lib
else
errortarget:
			echo not defined for $(UNAME)
endif # _Linux_	
endif # _SunOS_
	LIBS += -lXm -lXmu -lXt 
	WITH_X=y
endif # WITH_MOTIF


ifeq ($(WITH_XFORMS),y)
	XINCLDIR += -I/opt/include
	XLIBDIR  += -L/opt/lib
	LIBS += -lforms -lm -lXpm
ifeq (_$(LIBC)_,_5_)
		LIBS += -lMesaGL -L$(OPTROOT)/lib/MesaGL -lXext
endif # LIBC5	
	WITH_X=y
endif # WITH_XFORMS

ifeq ($(WITH_XAW),y)
  LIBS += -lXmu -lXaw3d -lXt
  WITH_X=y
endif  

ifeq (_$(WITH_X)_,_y_)
ifeq (_$(UNAME)_,_SunOS_)  
  XINCLDIR += -I/usr/openwin/include/X11
  XLIBDIR  += -L/usr/openwin/lib
  XLIBPATH = /usr/openwin/lib/X11
else
ifeq (_$(UNAME)_,_Linux_)
  XINCLDIR += -I/opt/Xorg/include -I/usr/X11R6/include
  XLIBDIR  += -L/opt/Xorg/lib -L/usr/X11R6/lib
  XLIBPATH = /opt/Xorg/lib/X11
else
ifeq (_$(UNAME)_,_hpux_)                                                                         
  XINCLDIR += -I/usr/include/X11R5                                                              
  XLIBDIR  += -L/usr/lib/X11R5     
  XLIBPATH = /usr/lib/X11
else
errortarget:
	echo X-includes not defined for $(UNAME)
endif
endif
endif


endif


ifeq ($(WITH_X),y)
  WITH_XINC=y
 	
  LIBS += $(XLIBDIR)	
  LIBS += -lX11 # -lXext # -lXt -lXmu -lSM -lICE 

###### the following X-libs might be useful, so remember to edit 
#  LIBS	  += -lXm -lXaw3d -lXintl -lforms -lXpm -lMesaGL

  BINDIR_ = $(BINDIR)/X11
else
  BINDIR_ = $(SBIN_TARGET)$(BINDIR)
ifeq (_$(SBIN_TARGET)_,_s_)
  MANSUFFIX = 8
endif
endif

ifeq ($(WITH_XINC),y)
  INCLDIR += $(XINCLDIR)
  DEFS += -DXLIBPATH=\"$(XLIBPATH)\"
endif

DYNLINK=0
ifeq (_$(UNAME)_,_Linux_)
	DYNLINK=1
else
ifeq (_$(UNAME)_,_SunOS_)
	DYNLINK=1
else
ifeq (_$(UNAME)_,_hpux_)                                                                         
	DYNLINK=1
else                                                                                            
endif   # hpux                                                                                        
endif	# SunOS
endif	# Linux

DEVLIBSYM = lib$(TARGET).so
SONAME = lib$(TARGET).so.$(MAJOR)
LIBNAME = lib$(TARGET)-$(VERSION).so

INCNAME = $(TARGET)
KMOD	= module/$(TARGET).o

ifeq (_$(DYNLINK)_,_0_)
	LIBNAME = lib$(TARGET).a
endif


include .headers
include .sources
include .defines

DEFS += $(DEFINES)

CFLAGS	= $(CODE) $(OPTS) $(WARN) $(DEFS) $(DEBUG_FLAGS) 

BINTARGET = $(TARGET)
ifeq (_$(UNAME)_,_cygwin_)
	BINTARGET = $(TARGET).exe	
endif

MAKE_T=$(BINTARGET)
      # only  $(BINTARGET) or $(LIBNAME) or $(KMOD)   allowed
      # $(LIBNAME) means we are compiling a library	
	  # $(KMOD) means we are compiling a kernel module


ifeq ($(MAKE_T),$(KMOD))
	INCLDIR = -I/usr/src/linux-$(KERNEL_VERSION)-$(MACHINE)/include
	CFLAGS = -O6 -D__$(MACHINE)__ -D__$(DEVICE)__ -DMODULE -D__KERNEL__ -Wstrict-prototypes -fomit-frame-pointer $(DEFINES) $(WARN) $(DEBUG_FLAGS) $(EXTRA_INC)
	TARGETNAME = $(TARGET)
	UMA = $(UNAME)-$(MACHINE)-$(KERNEL_VERSION)
	OPA = built/$(UMA)
	OPA_M = $(OPA)/module
	KMODDIR = /lib/modules/$(KERNEL_VERSION)/vendor
	MANSUFFIX = 8
else
	OPA = built/$(UMA)
	OPA_M = $(OPA)
endif
	
ifeq ($(MAKE_T),$(LIBNAME))
	TARGETNAME = $(SONAME)
	MANSUFFIX = 3
else
	TARGETNAME = $(TARGET)
endif

ifeq (_${LOGNAME}_,_root_)
ifeq (_$(CROSS)_,__)
	INST_PREFIX = $(PREFIX)
else
	INST_PREFIX = $(CODEPREFIX)
endif
else
ifeq (_$(PREFIX)_,_/usr_)
	# a debian package is being built
	INST_PREFIX = $(PREFIX)
else
ifeq (_$(CROSS)_,__)
ifeq (_$(SUBPREFIX)_,_/Pantec_)
	INST_PREFIX = $(PREFIX)/$(UMA)
else
	INST_PREFIX = $(CODEPREFIX)
endif # SUBPREFIX /Pantec
else
	INST_PREFIX = $(CODEPREFIX)
endif #	CROSS __
endif # PREFIX /usr
endif # LOGNAME root

ifneq (_$(UNAME)$(MACHINE)_,__)
include $(OPA)/.objects
endif

CFLAGS += -DVERSION=\"$(VERSION)\" 
CFLAGS += -DBUILDDATE=\"$(shell date +%Y/%m/%d,%H:%M)\"
CFLAGS += -DTARGETNAME=\"$(TARGETNAME)\"

DEPEND=$(OPA)/.depend
O_MAKE_T=$(OPA)/$(MAKE_T)

O_TARGET=$(OPA)/$(BINTARGET)
O_LIBNAME=$(OPA)/$(LIBNAME)
O_KMOD=$(OPA)/$(KMOD)

ifeq ($(MAKE_T),$(BINTARGET)) 
	# we are going to compile a ordinary binary
	INSTALL_HELP = install_bin
else
ifeq ($(MAKE_T),$(KMOD)) 
	# we are going to compile a kernel module
	INSTALL_HELP = install_mod
else
	# we are going to compile a library
	INSTALL_HELP = install_lib, static_lib
endif
endif

ifeq ($(MAKE_T),$(LIBNAME))
	ifeq (_$(MACHINE)_,_etrax100lx_)
		# the cris architecture requires the -fPIC flag for libraries
		CFLAGS += -fPIC
endif
endif

ifeq (_$(UNAME)_,_AVR_)
$(OPA)/$(TARGET).hex: $(OPA)/$(TARGET).elf
	@avr-size $(OPA)/$(TARGET).elf
	@$(OBJCOPY) -R .eeprom -O ihex $(OPA)/$(TARGET).elf $(OPA)/$(TARGET).hex
	@$(MAKE) $(OPA)/$(TARGET).size 

$(OPA)/$(TARGET).bin: $(OPA)/$(TARGET).elf
	@cp $(OPA)/$(TARGET).bin $(OPA)/$(TARGET).bin.old | touch $(OPA)/$(TARGET).bin.old	
	@$(OBJCOPY) -R .eeprom -O binary $(OPA)/$(TARGET).elf $(OPA)/$(TARGET).bin

$(OPA)/$(TARGET).size: $(OPA)/$(TARGET).bin 
	@echo $(shell echo $(shell ls -l $(OPA)/$(TARGET).bin | awk '{print $$5}') $(shell ls -l $(OPA)/$(TARGET).bin.old | awk '{print $$5}') | awk '{printf("size is %d,\t delta: %d\n", $$1, $$1 - $$2)}')	

$(OPA)/$(TARGET).lss: $(OPA)/$(TARGET).elf
	$(OBJDUMP) -h -S $(OPA)/$(TARGET).elf  >$(OPA)/$(TARGET).lss 

$(OPA)/$(TARGET).eep: $(OPA)/$(TARGET).elf
	$(OBJCOPY) $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0 
#	 -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0
	 
$(OPA)/$(TARGET).elf: $(OBJS)
	$(CC) -Os -Wl,--gc-sections $(CFLAGS) -Wl,-Map,$(OPA)/$(TARGET).map -mmcu=$(MCU) -o $(OPA)/$(TARGET).elf $(OBJS) $(LIBS) 
	
all: $(DEPEND) $(OPA)/$(TARGET).hex
	@ls -l $(OPA)/$(TARGET).hex
	
else
all: $(DEPEND) $(O_MAKE_T) 
	@ls -l $(O_MAKE_T)          

endif 


help:
	@(echo "useful targets are"			;\
	echo "		$(O_MAKE_T) (default target)"	;\
	echo "		install:"			;\
	echo "		$(INSTALL_HELP):"		;\
	echo "		clean:"				;\
	echo "		tgz:"				;\
	echo "		depend:"			;\
	echo "		gdb:"				;\
	echo " press [ENTER] to continue:"		;\
	read A )


ifeq ($(WITH_QTOPIA),y)
# include qtopia2.mk
$(OPA)/%.dep: $(UMAUIC)/.depend
else
$(OPA)/%.dep: $(SRCS) $(HDRS)
endif
ifneq (_$(UNAME)$(MACHINE)_,__)
	@echo "refreshing dependency $@" 	
	@$(CC) $(CFLAGS) -DMODNAME=\"$*\" $(INCLDIR) -M $*  		|\
	sed -e "s/^.*\.o\:\ /built\/$(UMA)\/&/g"  	>$@
endif	
	rm -f built/.versions

$(OPA)/%.o: 
	@echo [$(CC)] $< \-\> $@
	@$(CC) $(CFLAGS) -DMODNAME=\"$*\" -DREVISION=\"$($<.version)\" $(INCLDIR) -c $< -o $@

ifneq (_$(UNAME)$(MACHINE)_,__)
.$(UMA)_o: 
	@(echo "making target directory $(UMA)" ;\
	 rm -rf $(OPA) ;\
	 mkdir -p $(OPA_M) ;\
	 touch .$(UMA)_o ;\
	 $(MAKE) help     )
endif

cflag_info:
	@echo CROSS=$(CROSS) UID=$(UID) OPTROOT=$(OPTROOT) SUBPREFIX=$(SUBPREFIX) PREFIX=$(PREFIX)  INST_PREFIX=$(INST_PREFIX)  CODEPREFIX=$(CODEPREFIX)
	@echo using flags $(CFLAGS) $(INCLDIR)
	@echo

$(TARGET)/Makefile:
	@if test -f $(TARGET)/Makefile; then echo ok; else ln -s . $(TARGET); fi

depclean:
	@find built/ -type f -name "*.dep" -empty -exec rm {} \;	

clean: depclean
	@rm -f a.out $(OPA)/*.o core $(OPA)/lib*.so* $(OPA)/lib*.a $(OPA)/$(TARGET).pc $(O_MAKE_T) built/.versions	

ifeq ($(MK_DEBIAN_PKG),1)
distclean: debclean
else
distclean:
endif	
	if test -L $(TARGET); then rm $(TARGET); fi
	rm -rf ./built moc_*.cpp uic_*.cpp .*_o core a.out *.bck *.bak .*.bak .*.bck Makefile-* .hc~*~
	chmod 440 .headers .sources .defines

$(O_KMOD): cflag_info $(OBJS)
ifeq (_$(UNAME)_,_Linux_)
	@echo kernel version $(KERNEL_VERSION)
	$(LD) -S -r -o $(O_KMOD) $(OBJS)
else
	@echo I will make kernel modules on linux systems only
endif


ifeq (_$(DYNLINK)_,_1_)
$(O_LIBNAME): cflag_info $(OBJS)
ifeq (_$(UNAME)_,_Linux_)
	$(CC_LINK) $(LIBDIR) -shared -Wl,-soname,$(SONAME) -o $(O_LIBNAME) $(OBJS) $(LIBS)
else
ifeq (_$(UNAME)_,_SunOS_)
	$(LD) -B dynamic -G -h $(SONAME) -o $(O_LIBNAME) $(OBJS) 
else
ifeq (_$(UNAME)_,_hpux_)                                                                         
	$(LD) $(SHARED_LIB) -o $(O_LIBNAME) $(OBJS)                                               
else                                                                                            
	@echo dynamic linking not implemented for $(UNAME)                                           
endif   # hpux                                                                                        
endif	# SunOS
endif	# Linux
else
$(O_LIBNAME): static_lib
	@echo dynamic linking not defined for $(UNAME)                                           
endif	# DYNLINK 1

static_lib: cflag_info $(OBJS)
	$(AR) -rc $(OPA)/lib$(TARGET).a $(OBJS)

$(O_TARGET): cflag_info $(OBJS)
	$(CC) $(LINK_OPTS) $(LIBDIR) -o $(O_TARGET) $(OBJS) $(LIBS)	

ifeq (_$(RCP)_,_cp_)
$(RCPBIN):
	mkdir -p $(RCPBIN)
	
$(RCPLIB):
	mkdir -p $(RCPLIB)
endif

$(OPA)/$(TARGET).pc:
	@(echo prefix=$(INST_PREFIX);\
	echo exec_prefix=$(INST_PREFIX);\
	echo libdir=$(INST_PREFIX)/lib;\
	echo includedir=$(INST_PREFIX)/include/$(TARGET);\
	echo "";\
	echo Name: $(TARGET);\
	echo Description: $(TARGET) library;\
	echo Requires:;\
	echo Version: $(VERSION);\
	echo Libs: -L\$\{libdir\} -l$(TARGET);\
	echo Cflags: -I\$\{includedir\};\
	) >	$(OPA)/$(TARGET).pc
		
install_pkg_conf: $(OPA)/$(TARGET).pc
	@if test -d $(PREFIX)/lib/pkgconfig;\
	then $(INSTALL) -m 644 $(OPA)/$(TARGET).pc $(PREFIX)/lib/pkgconfig;\
	fi
		
ifeq (_$(DYNLINK)_,_0_)
install_lib: install_pkg_conf $(OPA)/lib$(TARGET).a $(INST_PREFIX)/lib$(GDBPREFIX)
	$(INSTALL) -m 644 $(OPA)/lib$(TARGET).a $(INST_PREFIX)/lib$(GDBPREFIX)	
else
install_lib: install_pkg_conf $(O_LIBNAME) $(INST_PREFIX)/lib$(GDBPREFIX) $(RCPLIB)
ifeq (_$(DEBUG_FLAGS)_,__)	
	$(STRIP) --strip-unneeded --discard-locals $(O_LIBNAME) 
endif
	@if test -r $(OPA)/lib$(TARGET).a ; then $(INSTALL) -m 644 $(OPA)/lib$(TARGET).a $(INST_PREFIX)/lib$(GDBPREFIX) ; fi
	$(INSTALL) -m 755 $(O_LIBNAME) $(INST_PREFIX)/lib$(GDBPREFIX)
ifneq (_$(CROSS)_,__)
ifneq (_$(RCP)_,__)
	$(RCP) $(O_LIBNAME) $(REMOTE_PREFIX)/lib$(GDBPREFIX)/
endif	
endif	
ifeq (_${LOGNAME}_,_root_)
ifeq (_$(CROSS)$(DEBUG_FLAGS)_,__)
	ldconfig
	@(cd $(INST_PREFIX)/lib$(GDBPREFIX);\
	ln -sf $(LIBNAME) $(DEVLIBSYM) )
else
	@(cd $(INST_PREFIX)/lib$(GDBPREFIX);\
	rm -f $(DEVLIBSYM) $(SONAME);\
	ln -s $(LIBNAME) $(SONAME);\
	ln -s $(LIBNAME) $(DEVLIBSYM) )
endif	
else
	@(cd $(INST_PREFIX)/lib$(GDBPREFIX);\
	rm -f $(DEVLIBSYM) $(SONAME);\
	ln -s $(LIBNAME) $(SONAME);\
	ln -s $(LIBNAME) $(DEVLIBSYM) )
ifeq (_$(RCP)_,_cp_)
	(cd $(REMOTE_PREFIX)/lib$(GDBPREFIX);\
	rm -f $(DEVLIBSYM) $(SONAME);\
	ln -s $(LIBNAME) $(SONAME);\
	ln -s $(LIBNAME) $(DEVLIBSYM) )
endif
endif
endif

install_inc: $(INST_PREFIX)/include/$(TARGET)
	@echo installing links to include files into $(INST_PREFIX)/include/$(TARGET)
	@(SRCPATH=`pwd` ;\
	  cd $(INST_PREFIX)/include/$(TARGET) ; \
	  for I in $(HDRS) ;\
	    do rm -f $$I ;\
	    ln -s $$SRCPATH/$$I $$I ;\
	  done ) 		

compact: $(O_TARGET)
	$(STRIP) $(O_TARGET)
	ls -l $(O_MAKE_T)
	
ifeq ($(MAKE_T),$(KMOD))
$(KMODDIR):
	mkdir -m 755 -p $(KMODDIR) 
	
install_bin: $(O_KMOD) $(KMODDIR)
	@if test -w $(KMODDIR); \
	then $(INSTALL) -m 755 $(O_KMOD) $(KMODDIR); \
	ls -l $(KMODDIR)/$(TARGET).o; else echo must be root to write in $(KMODDIR); fi
ifeq (_$(UID)_,_0_)
	depmod -a
else
	@echo must be root to write dependancies
endif
else # MAKE_T == KMOD
ifeq (_$(UNAME)_,_AVR_)
install_bin: $(OPA)/$(TARGET).hex .fuses-read
	$(AVRDUDE) -U flash:w:$(OPA)/$(TARGET).hex

.fuses-read:
ifeq (_$(READ_FUSES)_,__)
	@echo "READ_FUSES = -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h" >>.cache-$(DEVICE)
	@echo 'WRITE_FUSES = -U lfuse:w:$$(LFUSE):m -U hfuse:w:$$(HFUSE):m -U efuse:w:$$(EFUSE):m' >>.cache-$(DEVICE)
	@echo "Edit READ_FUSES in .cache-$(DEVICE)"
else
	@echo "# https://eleccelerator.com/fusecalc/fusecalc.php" >.fuses-read
	$(AVRDUDE) $(READ_FUSES) | awk '{c[0] = "L"; c[1] = "H"; c[2] = "E"; n++; printf("%sFUSE = %s\n", c[n-1], $$1)}' >>.fuses-read
endif

ifeq (_$(HFUSE)_,__)
fuses: 	.fuses-read
	@echo copy fuses variables to .cache-$(DEVICE)	
else
fuses: 
	$(AVRDUDE) $(WRITE_FUSES)
endif

else # UNAME == AVR
install_bin: compact  $(INST_PREFIX)/$(BINDIR_) $(RCPBIN)  # 
	$(INSTALL) -m 755 $(O_TARGET) $(INST_PREFIX)/$(BINDIR_)
ifneq (_$(CROSS)_,__)
ifneq (_$(RCP)_,__)
	$(RCP) $(O_TARGET) $(REMOTE_PREFIX)/$(BINDIR_)
endif # RCP !=  	
endif # CROSS != 		
endif # UNAME == AVR
endif # MAKE_T != KMOD

install_mod: $(O_KMOD)
ifeq (_$(UID)_,_0_)
	@if [ "`lsmod |grep $(TARGETNAME)`" != "" ]; then rmmod $(TARGETNAME); fi
	insmod $(O_KMOD)
else
	@echo must be root to (un)load modules
endif

$(INST_PREFIX)/$(BINDIR_):
	mkdir -m 755 -p $(INST_PREFIX)/$(BINDIR_)

$(INST_PREFIX)/lib$(GDBPREFIX):
	mkdir -m 755 -p $(INST_PREFIX)/lib$(GDBPREFIX)

$(PREFIX)/share/$(TARGET):
	mkdir -p $(PREFIX)/share/$(TARGET)

$(INST_PREFIX)/include/$(TARGET):
	mkdir -m 755 -p $(INST_PREFIX)/include/$(TARGET)

$(PREFIX)/man:
	mkdir -m 755 -p $(PREFIX)/man

$(PREFIX)/doc/$(TARGET):
	mkdir -m 755 -p $(PREFIX)/doc/$(TARGET)

share:
	mkdir -m 755 -p ./share
	
doc:
	mkdir -m 755 -p ./doc	

htmldoc: doc built/.versions 	
	@(if test -r man/man$(MANSUFFIX)/$(TARGET).$(MANSUFFIX) ; then \
	nroff -man <man/man$(MANSUFFIX)/$(TARGET).$(MANSUFFIX) | man2html >doc/$(TARGET).html ;\
	fi )

install_doc: $(PREFIX)/doc/$(TARGET) htmldoc
	@(if test -r man/man$(MANSUFFIX)/$(TARGET).$(MANSUFFIX) ; then \
	cp doc/$(TARGET).html $(PREFIX)/doc/$(TARGET)/$(TARGET).html ;\
	echo $(PREFIX)/doc/$(TARGET)/$(TARGET).html >>$(PKGLOG)/$(TARGET) ;\
	fi )
#	cd $(PREFIX)/doc/$(TARGET) && ln -sf $(PREFIX)/share/$(TARGET)/doc/* .	

$(PKGLOG):
	mkdir -m 755 -p $(PKGLOG)
	
sort_pkglog:
	@(if test -f $(PKGLOG)/$(TARGET) ; then \
	  sort $(PKGLOG)/$(TARGET) | egrep -v "/(.svn|CVS)/" | uniq >/tmp/.pkg.$(TARGET)	;\
	  mv /tmp/.pkg.$(TARGET) $(PKGLOG)/$(TARGET)	; fi )

install_share: $(PREFIX)/share/$(TARGET) 
	@(if test -d ./share ; then 	\
	  umask 022			;\
	  (cd ./share; $(TAR) --exclude CVS --exclude .svn -cf - .)|(cd $(PREFIX)/share/$(TARGET); $(TAR) -xf -) ;\
	fi)
	
install_man: $(PREFIX)/man $(PKGLOG)
	@(if test -d ./man ; then 	\
	  umask 022			;\
	  LIST=`ls *.[1-9] 2>/dev/null |\
	  sed -e "s/\ [A-Za-z0-9\._-]*.//g" `	;\
	  (cd ./man; $(TAR) --exclude CVS --exclude .svn -cf - .)|(cd $(PREFIX)/man; $(TAR) -xf -) ;\
	  find man -type f | egrep -v "CVS|\.svn" |\
	  awk '{print "$(PREFIX)/" $$0 }' >>$(PKGLOG)/$(TARGET)	;\
	  fi )

THISPATH = $(shell pwd)

ifeq ($(MK_DEBIAN_PKG),1)
install: deb
ifeq (_$(UID)_,_0_)
	dpkg -i $(THISPATH)/$(O_DEBPKG)
else
	su - root -c 'dpkg -i '$(THISPATH)/$(O_DEBPKG)
endif	

else

ifeq ($(MAKE_T),$(KMOD))
install: install_bin install_mod
else
ifeq ($(MAKE_T),$(BINTARGET))
install: install_bin install_share install_man install_doc 
	@(echo $(INST_PREFIX)/$(BINDIR_)/$(BINTARGET)	;\
	find $(PREFIX)/share/$(TARGET) | egrep -v "/(CVS|\.svn)/" ) >>$(PKGLOG)/$(TARGET)
	@$(MAKE) sort_pkglog
	@echo target was _$(MAKE_T)_ _$(BINTARGET)_ 
else
install: install_inc install_lib
	echo target was _$(MAKE_T)_ _$(TARGET)_
ifeq (_$(UNAME)_,_AIX_)                                                                         
else
endif
endif
endif
endif

uninstall:
ifeq ($(MK_DEBIAN_PKG),1)
ifeq (_$(UID)_,_0_)
	dpkg -r $(O_DEBPKG)
else
	su - root -c 'dpkg -r '$(DEBPKG)
endif
else
	rm -rf `cat $(PKGLOG)/$(TARGET)` 
	rm -f $(PKGLOG)/$(TARGET)
endif
	
tgz:
	@($(MAKE) distclean	;\
	  inode=`ls -id . | awk '{print $$1}' `; \
	  cd .. ; \
	  ZIPDIR=`ls -ia | grep $$inode | awk '{print $$2}' ` ; \
	  $(TAR) --exclude CVS --exclude .svn --exclude .cvsignore -cvzf $$ZIPDIR-$(VERSION).tgz $$ZIPDIR $$ZIPDIR/.svn/entries ) 	

ifeq ($(MAKE_T),$(BINTARGET))
gdb: 
	@$(MAKE) DEBUG_FLAGS="$(DEBUG)" WARN="-Wall" OPTS="" && gdb $(O_TARGET)
else
gdb: 
	@$(MAKE) DEBUG_FLAGS="$(DEBUG)" WARN="-Wall" OPTS="" && make DEBUG_FLAGS="$(DEBUG)" install_lib
endif



gdball:  clean gdb


dep:	depend
depend:
	rm $(OPA)/*.dep $(OPA)/.depend
	$(MAKE) $(DEPEND)

$(DEPEND): .sources .headers .$(UMA)_o
ifeq (_$(SRCS)_,__)
	$(MAKE) $(DEPEND)
else	
	@(echo creating new dependency list for $(UMA)  >&2 ; \
	    for i in $(SRCS) $(UISRCS) $(MOCSRCS)		; \
	    do echo "$$i" >&2                  	; \
	    echo include built/$(UMA)/$$i.dep	; \
	    done ) >$(DEPEND)
endif


$(OPA)/.objects: .sources .$(UMA)_o 
	@(echo creating .objects >&2; \
	cat .sources |\
	sed -e "s/\.[cC][pc]*/\.o/g;s/SRCS/OBJS/g;s/[-0-9A-Za-z_]*\.o/built\/$(UMA)\/&/g" >$(OPA)/.objects )

ifeq ($(WITH_QTOPIA),y)
.sources: $(UMAUIC)/.depend
else
.sources: 
endif
	@(echo " " | awk '{printf("SRCS = ")}' 		;\
	ls *.cc *.cpp *.C *.c 2>/dev/null	|		 \
	awk '{printf("\\"); printf("\n\t%s\t",$$1); }'	;\
	echo "	"	) >.sources 

.headers:
	@(echo " " | awk '{printf("HDRS = ")}'   	;\
	ls *.h *.H 2>/dev/null 		|		 \
	awk '{printf("\\"); printf("\n\t%s\t",$$1)}' 	;\
	echo "	"	) >.headers 


link:	$(OBJS) .rmlink $(O_TARGET)
.rmlink:
	rm -f $(O_TARGET)

forcedep: .rmdep $(DEPEND)
.rmdep:
	rm -f $(DEPEND)

srcs: .rmsrcs $(OPA)/.objects .headers forcedep
.rmsrc:
	rm -f .sources .headers	$(OPA)/.objects 

.defines: 		# $(HDRS) $(SRCS)
	@(echo scanning for additional defines >&2 ; \
	egrep -v '^//' *.[cChH] *.cc *.cpp 2>/dev/null	|\
	egrep '\#define [A-Z][A-Z_]*[ \	]*(\"[0-9A-Za-z_/\\]*\"|[0-9][0-9]*[ ]*)$$' |\
	awk '{for (i=2; i<=NF; i++) { \
	if (i==2) printf("# DEFINES += -D%s=",$$i);	\
	else printf("%s",$$i); } printf("\n");}' 	|\
	sed -e 's/\"/\\&/g' >>.defines )


HAVESVN = $(shell if svn log >/dev/null 2>&1 && test -d .svn; then echo -n yes; else echo -n no; fi)
SVNENTRIES =

ifeq (_$(HAVESVN)_,_yes_)
	SVNENTRIES = .svn/entries 
endif

## dependencies follow

ifneq (_$(UNAME)$(MACHINE)_,__)
include $(DEPEND)
endif

ifeq (_$(WITH_QTOPIA)_,_y_)
include $(UMAUIC)/.depend
endif 


.device:
	echo "DEVICE = $(shell uname -n)" >.device

.cache-$(DEVICE):
	@(echo UNAME = $(shell uname|sed -e "s/HP\-UX/hpux/g;s/CYGWIN_NT-\([45]\).\([01]\)/cygwin/g"); \
	if [ "$(CROSS)" = "" ]; then echo MACHINE = $(shell uname -m|sed -e "s/0000[0-9A-F]*00/rs6000/g;s/armv7l/armv7/g;s/x86_64/x86-64/g"); \
	if [ "$(shell uname)" != "Linux" ]; then echo KERNEL_VERSION = "$(shell uname -r)"; else 	\
	echo KERNEL_VERSION = $(shell grep UTS_RELEASE /usr/src/linux/include/linux/*.h|cut -d\" -f 2|head -1);	\
	fi; else echo MACHINE = strongarm; \
	echo KERNEL_VERSION = 2.4.18-rmk7-pxa3-embedix;	\
	fi	;\
	echo LIBC = $(shell ldd /bin/ls | grep libc.so | cut -f1 -d" " | cut -f3 -d"." | head -1); \
	) >.cache-$(DEVICE)


ifeq (_$(SRCS)_,__)
built/.versions:
	@mkdir -p built ; touch built/.versions 
else	
SRCREGEX = $(shell echo `cat .sources `|sed 's/\\//g;s/^.*=[ ]*//g;s/  /|/g')
built/.versions: $(SVNENTRIES) .sources .headers
	@mkdir -p built ; \
	(echo creating new ls_fulltime version list >&2 ; \
	    for i in $(SRCS)		; \
	    do echo "$$i" >&2                  	; \
			echo "$${i}.version=$${i}@@unknown,`ls --full-time $$i | awk '{print $$6"T"$$7"Z"}' | sed 's/-//g'`"; \
			if test -L $$i && [ $(HAVESVN) = yes ] ; then svn ls -v `/bin/ls -l $$i | awk '{print $$NF}'` | awk '{printf("%s.version=%s@@%d,%s,%s%sT%s\n",$$7,$$7,$$1,$$2,$$4,$$5,$$6)}'; fi ; \
	    done; \
		echo "# end-of-versions created from ls --full-time"; \
		if test -f .svn/entries; \
		then \
			(echo creating new version list >&2 ; \
				sed 's/<//g;s/>//g;s/-//g;s/ //g;s/=/ /g;s/"//g' .svn/entries |	awk '{\
					if (oldfmt != 1) {\
						if (($$1 == "entry") && (NF == 1)) { printf("\n"); } else {\
						if ($$1 == "committeddate") { printf("%s.version=%s@@%s,%s\n",cname,cname,cversion,$$2); }\
						if ($$1 == "committedrev") { cversion=$$2 }\
						if ($$1 == "name") { cname=$$2 } } \
					} else {\
						n++;if (n == 7) {cdate = $$1}; if (n == 8) {cversion = $$1}; \
					} \
					if ((($$1 == "file") || ($$1 == "hasprops")) && (NF == 1)) {\
						printf("%s.version=%s@@%s,%s\n",cname,cname,cversion,cdate); \
						cname=lasts;	oldfmt = 1;	n = 0;\
					};\
					lasts=$$1; \
				}' | egrep "$(SRCREGEX)"; \
			);		\
		fi; \
		echo "# end-of-versions created from .svn/entries";	\
	) >built/.versions

endif
# SRCS __

include .mklinks.mk


