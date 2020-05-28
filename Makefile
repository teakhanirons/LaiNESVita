TARGET		:= LaiNES
TITLE	    := LAINESEMU

LIBS = -lSDL2 -lvita2d -lSceTouch_stub -lmikmod -lvorbisfile -lvorbis -logg -lsndfile -lSceLibKernel_stub -lScePvf_stub \
	-lSceAppMgr_stub -lSceCtrl_stub -lm -lSceIofilemgr_stub -lSceAppUtil_stub -lScePgf_stub \
	-lfnblit -lc -lScePower_stub -lSceCommonDialog_stub -lz -lSceAudio_stub -lSceGxm_stub \
	-lSceDisplay_stub -lSceSysmodule_stub -lSceHid_stub -lSceKernelDmacMgr_stub

SOURCES		:=	lib \
				lib/include \
				lib/include/boost \
				src \
				src/mappers \
				src/include \
				src/include/mappers \
	
CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.cpp))
OBJS     := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

export INCLUDE	:= $(foreach dir,$(SOURCES),-I$(CURDIR)/$(dir))

PREFIX  = arm-dolce-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = $(INCLUDE) -g -Wl,-q -O3
CXXFLAGS  = $(INCLUDE) -g -Wl,-q -O3 -std=gnu++14 -fpermissive -fexceptions
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

$(TARGET).vpk: $(TARGET).velf
	dolce-make-fself -s $< eboot.bin
	dolce-mksfoex -s TITLE_ID=$(TITLE) "$(TARGET)" param.sfo
	cp -f param.sfo sce_sys/param.sfo
	7za a -tzip ./$(TARGET).vpk -r ./sce_sys ./eboot.bin ./res

%.velf: %.elf
	cp $< $<.unstripped.elf
	$(PREFIX)-strip -g $<
	dolce-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).velf $(TARGET).vpk $(TARGET).elf $(TARGET).elf.unstripped.elf eboot.bin param.sfo sce_sys/param.sfo $(OBJS)