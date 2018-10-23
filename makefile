 

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
 
include $(DEVKITPRO)/libnx/switch_rules

export BUILD_EXEFS_SRC := build/exefs

APP_TITLE := kinx
APP_DESCRIPTION := kinx
APP_AUTHOR := MVG
APP_VERSION := 1.0.0
ICON := logo2.jpg

WINDRES   = windres.exe
OBJ       = obj/2100dasm.o obj/adsp2100.o obj/iMemory.o obj/iMemoryOps.o obj/iBranchOps.o obj/iCPU.o obj/iFPOps.o obj/iATA.o obj/iMain.o obj/hleDSP.o obj/iRom.o obj/EmuObject1.o obj/ki.o obj/iGeneralOps.o
LINKOBJ   = obj/2100dasm.o obj/adsp2100.o obj/iMemory.o obj/iMemoryOps.o obj/iBranchOps.o obj/iCPU.o obj/iFPOps.o obj/iATA.o obj/iMain.o obj/hleDSP.o obj/iRom.o obj/EmuObject1.o obj/ki.o obj/iGeneralOps.o
LIBS      = -specs=$(DEVKITPRO)/libnx/switch.specs -g -march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE -L$(DEVKITPRO)/libnx/lib -L$(DEVKITPRO)/portlibs/switch/lib -lnx
INCS      = -I$(DEVKITPRO)/portlibs/switch/include/SDL2 -I"src/main" -I$(DEVKITPRO)/libnx/include -I$(DEVKITPRO)/portlibs/switch/include
CXXINCS   = -I$(DEVKITPRO)/portlibs/switch/include/SDL2 -I"src/main" -I$(DEVKITPRO)/libnx/include -I$(DEVKITPRO)/portlibs/switch/include
BIN       = release/kinx.elf
BUILD	  =	build
BINDIR	  = release
DEFINES   =  -DSWITCH
CXXFLAGS  = $(CXXINCS) $(DEFINES) -march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE -O3 -w
CFLAGS    = $(INCS) $(DEFINES)    -march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE -O3 -w
GPROF     = gprof.exe
RM        = rm -f
LINK      = aarch64-none-elf-g++ 
CPP		  = aarch64-none-elf-g++
OUTPUT    = kinx


ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(BINDIR)/$(OUTPUT).nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif


.PHONY: all all-before all-after clean clean-custom
all: all-before $(BIN) all-after

clean: clean-custom
	$(RM) $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(LINK) $(LINKOBJ) -o "release/kinx.elf" $(LIBS)

obj/2100dasm.o: $(GLOBALDEPS) 2100dasm.cpp
	$(CPP) -c 2100dasm.cpp -o obj/2100dasm.o $(CXXFLAGS)
obj/adsp2100.o: $(GLOBALDEPS) adsp2100.cpp
	$(CPP) -c adsp2100.cpp -o obj/adsp2100.o $(CXXFLAGS)	
obj/iMemory.o: $(GLOBALDEPS) iMemory.cpp
	$(CPP) -c iMemory.cpp -o obj/iMemory.o $(CXXFLAGS)		
obj/iMemoryOps.o: $(GLOBALDEPS) iMemoryOps.cpp
	$(CPP) -c iMemoryOps.cpp -o obj/iMemoryOps.o $(CXXFLAGS)	
obj/iBranchOps.o: $(GLOBALDEPS) iBranchOps.cpp
	$(CPP) -c iBranchOps.cpp -o obj/iBranchOps.o $(CXXFLAGS)	
obj/iCPU.o: $(GLOBALDEPS) iCPU.cpp
	$(CPP) -c iCPU.cpp -o obj/iCPU.o $(CXXFLAGS)		
obj/iFPOps.o: $(GLOBALDEPS) iFPOps.cpp
	$(CPP) -c iFPOps.cpp -o obj/iFPOps.o $(CXXFLAGS)		
obj/iATA.o: $(GLOBALDEPS) iATA.cpp
	$(CPP) -c iATA.cpp -o obj/iATA.o $(CXXFLAGS)	
obj/iMain.o: $(GLOBALDEPS) iMain.cpp
	$(CPP) -c iMain.cpp -o obj/iMain.o $(CXXFLAGS)		
obj/hleDSP.o: $(GLOBALDEPS) hleDSP.cpp
	$(CPP) -c hleDSP.cpp -o obj/hleDSP.o $(CXXFLAGS)		
obj/iRom.o: $(GLOBALDEPS) iRom.cpp
	$(CPP) -c iRom.cpp -o obj/iRom.o $(CXXFLAGS)	
obj/EmuObject1.o: $(GLOBALDEPS) EmuObject1.cpp
	$(CPP) -c EmuObject1.cpp -o obj/EmuObject1.o $(CXXFLAGS)	
obj/ki.o: $(GLOBALDEPS) ki.cpp
	$(CPP) -c ki.cpp -o obj/ki.o $(CXXFLAGS)	
obj/iGeneralOps.o: $(GLOBALDEPS) iGeneralOps.cpp
	$(CPP) -c iGeneralOps.cpp -o obj/iGeneralOps.o $(CXXFLAGS)	
 
#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all	:	$(BINDIR)/$(OUTPUT).pfs0 $(BINDIR)/$(OUTPUT).nro

$(BINDIR)/$(OUTPUT).pfs0	:	$(BINDIR)/$(OUTPUT).nso

$(BINDIR)/$(OUTPUT).nso	:	$(BINDIR)/$(OUTPUT).elf

ifeq ($(strip $(NO_NACP)),)
$(BINDIR)/$(OUTPUT).nro	:	$(BINDIR)/$(OUTPUT).elf $(BINDIR)/$(OUTPUT).nacp
else
$(BINDIR)/$(OUTPUT).nro	:	$(BINDIR)/$(OUTPUT).elf
endif

$(BINDIR)/$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)
	
# end of Makefile ...
