
include TTY.mk

ifeq (_$(TTY)_,__)
    TTY = /dev/ttyUSB0
endif

ifeq (_$(BAUD)_,__)
    BAUD = 19200
endif

ifeq (_$(WIRE)_,__)
    WIRE = D
endif

ifeq (_$(REL_INC)_,__)
    REL_INC = ../..
endif

HC08SPRG = $(REL_INC)/tools/hc08sprg

HC08SPRG_EXE = $(HC08SPRG)/built/Linux-i686/hc08sprg

flash:   $(TARGET)-final.s19 kuser.sh $(HC08SPRG_EXE)
#       @echo -n switch power off, then press enter
#       $(shell read A)
	./kuser.sh $(TTY)
	$(HC08SPRG_EXE) $(TTY):$(WIRE) $(BAUD) $(TARGET)-final.s19

kuser.sh:
	ln -s $(HC08SPRG)/contrib/kuser.sh

$(HC08SPRG_EXE):
	( env -i bash -l -c "$(MAKE) -C $(HC08SPRG)" )


TTY.mk:
	@(echo "TTY = /dev/ttyUSB0" ;\
	  echo "WIRE = D") >TTY.mk; \
	  echo "!! You must adapt file TTY.mk !!";\
	  read A
