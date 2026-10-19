#---------------------------------------------------------------------------------
# NDS-Shop — application Nintendo 3DS
#
#   make        -> NDS-Shop.3dsx (homebrew, sans installation)
#   make cia    -> NDS-Shop.cia  (nécessite bannertool + makerom)
#   make clean
#---------------------------------------------------------------------------------
.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

#---------------------------------------------------------------------------------
# TARGET/BUILD/SOURCES/DATA/INCLUDES/GRAPHICS/GFXBUILD : cf. gabarit devkitARM.
# GFXBUILD = $(BUILD) => l'atlas d'interface est lié dans le binaire et chargé
# depuis la mémoire (C2D_SpriteSheetLoadFromMem).
#---------------------------------------------------------------------------------
TARGET		:=	NDS-Shop
BUILD		:=	build
SOURCES		:=	source
DATA		:=	data
INCLUDES	:=	source
GRAPHICS	:=	assets/gfx
GFXBUILD	:=	$(BUILD)

APP_TITLE	:=	NDS-Shop
APP_AUTHOR	:=	NDS-Shop-Homebrew
APP_DESCRIPTION	:=	Boutique de jeux Nintendo DS pour Nintendo 3DS
ICON		:=	app/icon.png
BNR_IMAGE	:=	app/banner.png
BNR_AUDIO	:=	app/BannerAudio.wav
RSF_FILE	:=	app/build-cia.rsf

#---------------------------------------------------------------------------------
# Version : dernier tag git (vX.Y.Z), sinon 1.0.0.
#---------------------------------------------------------------------------------
APP_VERSION	:=	$(patsubst v%,%,$(shell git describe --tags --abbrev=0 2>/dev/null))
ifeq ($(strip $(APP_VERSION)),)
APP_VERSION	:=	1.0.0
endif
VMAJOR		:=	$(word 1,$(subst ., ,$(APP_VERSION)))
VMINOR		:=	$(word 2,$(subst ., ,$(APP_VERSION)))
VMICRO		:=	$(word 3,$(subst ., ,$(APP_VERSION)))
ifeq ($(strip $(VMAJOR)),)
VMAJOR		:=	0
endif
ifeq ($(strip $(VMINOR)),)
VMINOR		:=	0
endif
ifeq ($(strip $(VMICRO)),)
VMICRO		:=	0
endif

#---------------------------------------------------------------------------------
# Outils externes, uniquement pour la cible `cia`.
#---------------------------------------------------------------------------------
MAKEROM		?=	makerom
BANNERTOOL	?=	bannertool

#---------------------------------------------------------------------------------
# Options de génération de code.
#---------------------------------------------------------------------------------
ARCH		:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS		:=	-g -Wall -Wextra -Wno-unused-parameter -O2 -mword-relocations \
				-ffunction-sections -fdata-sections \
				-DAPP_VERSION_STR='"$(APP_VERSION)"' \
				$(ARCH)

CFLAGS		+=	$(INCLUDE) -D__3DS__

ASFLAGS		:=	-g $(ARCH)
LDFLAGS		=	-specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS		:=	-lcurl -lmbedtls -lmbedx509 -lmbedcrypto -lz -lcitro2d -lcitro3d -lctru -lm

#---------------------------------------------------------------------------------
# Répertoires de bibliothèques (doivent contenir include/ et lib/).
#---------------------------------------------------------------------------------
LIBDIRS		:=	$(PORTLIBS) $(CTRULIB)

#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
PICAFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.v.pica)))
SHLISTFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.shlist)))
GFXFILES	:=	$(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.t3s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# CXX pour les projets C++, CC pour le C.
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
ifeq ($(GFXBUILD),$(BUILD))
#---------------------------------------------------------------------------------
export T3XFILES :=  $(GFXFILES:.t3s=.t3x)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
export ROMFS_T3XFILES	:=	$(patsubst %.t3s, $(GFXBUILD)/%.t3x, $(GFXFILES))
export T3XHFILES		:=	$(patsubst %.t3s, $(BUILD)/%.h, $(GFXFILES))
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_SOURCES 	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES)) \
			$(PICAFILES:.v.pica=.shbin.o) $(SHLISTFILES:.shlist=.shbin.o) \
			$(addsuffix .o,$(T3XFILES))

export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES	:=	$(PICAFILES:.v.pica=_shbin.h) $(SHLISTFILES:.shlist=_shbin.h) \
			$(addsuffix .h,$(subst .,_,$(BINFILES))) \
			$(GFXFILES:.t3s=.h)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export _3DSXDEPS	:=	$(if $(NO_SMDH),,$(OUTPUT).smdh)

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.png)
	ifneq (,$(findstring $(TARGET).png,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).png
	else
		ifneq (,$(findstring icon.png,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.png
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_SMDH)),)
	export _3DSXFLAGS += --smdh=$(CURDIR)/$(TARGET).smdh
endif

ifneq ($(ROMFS),)
	export _3DSXFLAGS += --romfs=$(CURDIR)/$(ROMFS)
endif

.PHONY: all cia 3dsx clean

#---------------------------------------------------------------------------------
all: $(BUILD) $(GFXBUILD) $(DEPSDIR) $(ROMFS_T3XFILES) $(T3XHFILES)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile all

#---------------------------------------------------------------------------------
3dsx: $(BUILD) $(GFXBUILD) $(DEPSDIR) $(ROMFS_T3XFILES) $(T3XHFILES)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile 3dsx

#---------------------------------------------------------------------------------
cia: $(BUILD) $(GFXBUILD) $(DEPSDIR) $(ROMFS_T3XFILES) $(T3XHFILES)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile cia

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).3dsx $(OUTPUT).smdh $(TARGET).elf $(TARGET).cia

#---------------------------------------------------------------------------------
$(BUILD):
	@mkdir -p $@

#---------------------------------------------------------------------------------
$(GFXBUILD)/%.t3x	$(BUILD)/%.h	:	%.t3s
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@tex3ds -i $< -H $(BUILD)/$*.h -d $(DEPSDIR)/$*.d -o $(GFXBUILD)/$*.t3x

#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# Cibles principales (exécutées depuis build/).
#---------------------------------------------------------------------------------
all: 3dsx

3dsx: $(OUTPUT).3dsx

cia: $(OUTPUT).cia

$(OUTPUT).3dsx	:	$(OUTPUT).elf $(_3DSXDEPS)

$(OFILES_SOURCES) : $(HFILES)

$(OUTPUT).elf	:	$(OFILES)

#---------------------------------------------------------------------------------
# Forwarder Cia (installation sur la console).
#---------------------------------------------------------------------------------
$(OUTPUT).cia	:	$(OUTPUT).elf $(OUTPUT).smdh
	@echo $(notdir $@)
	@$(BANNERTOOL) makebanner -i "../$(BNR_IMAGE)" -a "../$(BNR_AUDIO)" -o "banner.bin"
	@$(BANNERTOOL) makesmdh -i "../$(ICON)" -s "$(APP_TITLE)" -l "$(APP_DESCRIPTION)" -p "$(APP_AUTHOR)" -o "icon.bin" \
		--flags visible,ratingrequired,recordusage --cero 153 --esrb 153 --usk 153 --pegigen 153 --pegiptr 153 --pegibbfc 153 --cob 153 --grb 153 --cgsrr 153
	@$(MAKEROM) -f cia -target t -o "../$(TARGET).cia" -elf "../$(TARGET).elf" -rsf "../$(RSF_FILE)" \
		-banner "banner.bin" -icon "icon.bin" \
		-major $(VMAJOR) -minor $(VMINOR) -micro $(VMICRO) -DAPP_VERSION_MAJOR="$(VMAJOR)"

#---------------------------------------------------------------------------------
# Données binaires (un motif par extension embarquée).
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
.PRECIOUS	:	%.t3x %.shbin
#---------------------------------------------------------------------------------
%.t3x.o	%_t3x.h :	%.t3x
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.shbin.o %_shbin.h : %.shbin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPSDIR)/*.d

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
