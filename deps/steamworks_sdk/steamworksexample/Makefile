SOURCEFILES := \
	BaseMenu.cpp \
	Friends.cpp \
	Inventory.cpp \
	ItemStore.cpp \
	Leaderboards.cpp \
	Lobby.cpp \
	Main.cpp \
	MainMenu.cpp \
	OverlayExamples.cpp \
	PhotonBeam.cpp \
	QuitMenu.cpp \
	RemotePlay.cpp \
	RemoteStorage.cpp \
	ServerBrowser.cpp \
	Ship.cpp \
	SimpleProtobuf.cpp \
	SpaceWarClient.cpp \
	SpaceWarEntity.cpp \
	SpaceWarServer.cpp \
	StarField.cpp \
	StatsAndAchievements.cpp \
	Sun.cpp \
	timeline.cpp \
	VectorEntity.cpp \
	clanchatroom.cpp \
	gameenginesdl.cpp \
	htmlsurface.cpp \
	musicplayer.cpp \
	p2pauth.cpp \
	stdafx.cpp \
	voicechat.cpp \
	glew.c

TARGETNAME := SteamworksExampleLinux
#TARGETTYPE can be APP, STATIC or SHARED
TARGETTYPE := APP

include flags.mak

CONFIG ?= RELEASE

ALL_MACROS := $(COMMON_MACROS)

ifeq ($(CONFIG),DEBUG)
	BINARYDIR = debug
	CFLAGS += $(DEBUG_CFLAGS)
	CXXFLAGS += $(DEBUG_CXXFLAGS)
	LDFLAGS += $(DEBUG_LDFLAGS)
	ALL_MACROS += $(DEBUG_MACROS)
endif

ifeq ($(CONFIG),RELEASE)
	BINARYDIR = release
	CFLAGS += $(RELEASE_CFLAGS)
	CXXFLAGS += $(RELEASE_CXXFLAGS)
	LDFLAGS += $(RELEASE_LDFLAGS)
	ALL_MACROS += $(RELEASE_MACROS)
endif

ifeq ($(BINARYDIR),)
error:
	$(error Please specify CONFIG=DEBUG/RELEASE)
endif

ARCH ?= 64
ifeq ($(ARCH), 32)
    CFLAGS += -m32
    CXXFLAGS += -m32
    LDFLAGS += -m32
else
    ifneq ($(ARCH), 64)
        $(error Please specify ARCH=32/64)
    endif
endif

EXTERNAL_LIBS := 
EXTERNAL_LIBS_COPIED := $(foreach lib, $(EXTERNAL_LIBS),$(BINARYDIR)/$(notdir $(lib)))

CXXFLAGS += -Wno-invalid-offsetof

CFLAGS += $(addprefix -I,$(INCLUDE_DIRS))
CXXFLAGS += $(addprefix -I,$(INCLUDE_DIRS))

CFLAGS += $(addprefix -D,$(ALL_MACROS))
CXXFLAGS += $(addprefix -D,$(ALL_MACROS))

CXXFLAGS += $(addprefix -framework ,$(MACOS_FRAMEWORKS))
CFLAGS += $(addprefix -framework ,$(MACOS_FRAMEWORKS))
LDFLAGS += $(addprefix -framework ,$(MACOS_FRAMEWORKS))

LDFLAGS += $(addprefix -L,$(LIBRARY_DIRS))

LIBRARY_LDFLAGS = $(addprefix -l,$(LIBRARY_NAMES))

ifeq ($(IS_LINUX_PROJECT),1)
	RPATH_PREFIX := -Wl,--rpath='$$ORIGIN/../
	LIBRARY_LDFLAGS += $(EXTERNAL_LIBS)
	LIBRARY_LDFLAGS += -Wl,--rpath='$$ORIGIN'
	LIBRARY_LDFLAGS += $(addsuffix ',$(addprefix $(RPATH_PREFIX),$(dir $(EXTERNAL_LIBS))))
	
	ifeq ($(TARGETTYPE),SHARED)
		LIBRARY_LDFLAGS += -Wl,-soname,$(TARGETNAME)
	endif
	
else
	LIBRARY_LDFLAGS += $(EXTERNAL_LIBS)
endif


CFLAGS += $(MCUFLAGS)
CXXFLAGS += $(MCUFLAGS)
LDFLAGS += $(MCUFLAGS)

all_make_files := Makefile flags.mak $(ADDITIONAL_MAKE_FILES)

ifeq ($(STARTUPFILES),)
	all_source_files := $(SOURCEFILES)
else
	all_source_files := $(STARTUPFILES) $(filter-out $(STARTUPFILES),$(SOURCEFILES))
endif

source_obj1 := $(all_source_files:.cpp=.o)
source_obj2 := $(source_obj1:.c=.o)
source_objs := $(source_obj2:.S=.o)

all_objs := $(addprefix $(BINARYDIR)/, $(notdir $(source_objs)))

ifeq ($(GENERATE_BIN_FILE),1)
all: $(BINARYDIR)/$(basename $(TARGETNAME)).bin

$(BINARYDIR)/$(basename $(TARGETNAME)).bin: $(BINARYDIR)/$(TARGETNAME)
	$(OBJCOPY) -O binary $< $@

else
all: $(BINARYDIR)/$(TARGETNAME)
endif

ifeq ($(TARGETTYPE),APP)
$(BINARYDIR)/$(TARGETNAME): $(all_objs) $(EXTERNAL_LIBS) $(BINARYDIR)/$(STEAM_API) $(BINARYDIR)/SteamworksExample.sh $(BINARYDIR)/DejaVuSans.ttf
	$(LD) -o $@ $(START_GROUP) $(all_objs) $(LIBRARY_LDFLAGS) $(LDFLAGS) $(END_GROUP)
	@echo "You can start the game by running $(BINARYDIR)/SteamworksExample.sh"
endif

ifeq ($(TARGETTYPE),SHARED)
$(BINARYDIR)/$(TARGETNAME): $(all_objs) $(EXTERNAL_LIBS)
	$(LD) -shared -o $@ $(START_GROUP) $(all_objs) $(LIBRARY_LDFLAGS) $(LDFLAGS) $(END_GROUP)
endif

ifeq ($(TARGETTYPE),STATIC)
$(BINARYDIR)/$(TARGETNAME): $(all_objs)
	$(AR) -r $@ $^
endif

-include $(all_objs:.o=.dep)

clean:
ifeq ($(USE_DEL_TO_CLEAN),1)
	del /S /Q $(BINARYDIR)
else
	rm -f $(BINARYDIR)/*.o $(BINARYDIR)/*.dep $(BINARYDIR)/$(TARGETNAME) $(BINARYDIR)/SteamworksExample.sh
endif

$(BINARYDIR):
	mkdir $(BINARYDIR)

$(BINARYDIR)/$(STEAM_API): $(LIBRARY_DIRS)/$(STEAM_API)
	chmod +w $@ || true
	cp -v $< $@
	chmod +x $@

$(BINARYDIR)/SteamworksExample.sh: SteamworksExample.sh
	cp -v $< $@
	chmod +x $@

$(BINARYDIR)/DejaVuSans.ttf: DejaVuSans.ttf
	cp -v $< $@

$(BINARYDIR)/%.o : %.cpp $(all_make_files) |$(BINARYDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -MD -MF $(@:.o=.dep)

$(BINARYDIR)/%.o : %.c $(all_make_files) |$(BINARYDIR)
	$(CC) $(CFLAGS) -c $< -o $@ -MD -MF $(@:.o=.dep)

$(BINARYDIR)/%.o : %.S $(all_make_files) |$(BINARYDIR)
	$(CC) $(CFLAGS) $(ASFLAGS) -c $< -o $@ -MD -MF $(@:.o=.dep)
