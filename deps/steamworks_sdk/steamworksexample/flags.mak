ifeq "$(DOCKER_IMAGE_ALIAS)" ""
  # This is setup when using Valve's docker scripts, but not when using podman/toolbox, so try to guess
  $(shell grep -q -F VARIANT_ID=\"com.valvesoftware.steamruntime.sdk-amd64_i386-scout\" /etc/os-release)
  ifeq ($(.SHELLSTATUS),0)
    DOCKER_IMAGE_ALIAS := steamrt-scout-amd64
  endif
endif

ifeq "$(DOCKER_IMAGE_ALIAS)" ""
  $(info WARNING: No Steam for Linux runtime SDK detected - unsupported configuration.)
  $(info See tools/linux/README.md)
  $(info)
else
  DOCKER_IMAGE_BASE:=$(DOCKER_IMAGE_ALIAS:-fastlink=)
  DOCKER_IMAGE_BASE:=$(DOCKER_IMAGE_BASE:-i386=)
  DOCKER_IMAGE_BASE:=$(DOCKER_IMAGE_BASE:-amd64=)

  ifeq ($(DOCKER_IMAGE_BASE), steamrt-scout)
    $(info Configuring for Steam for Linux runtime 1.0 (scout))
    CC := gcc-9
    CXX := g++-9
    CXXFLAGS += -std=gnu++17
    # unlike gcc 4.8, gcc 9 is not native to the scout runtime, it is recommended to statically link
    LDFLAGS += -static-libgcc -static-libstdc++
  endif
endif

ifeq ($(ARCH), 32)
    ARCH_DIR := linux32
else
    ARCH_DIR := linux64
endif

INCLUDE_DIRS := $(PWD)/../public
LIBRARY_DIRS := $(PWD)/../../client/$(ARCH_DIR)
LIBRARY_NAMES := steam_api
STEAM_API := libsteam_api.so

ifeq (,$(wildcard $(LIBRARY_DIRS)/$(STEAM_API)))
  # Does not exist, substitue with a path valid for the public, zip version of the SDK
  LIBRARY_DIRS := $(PWD)/../redistributable_bin/$(ARCH_DIR)
endif

CC ?= gcc
CXX ?= g++
LD := $(CXX)
AR := ar
OBJCOPY := objcopy
CP := cp
SDL_CONFIG := sdl2-config

# Since this is an example, we'll build Debug by default
CONFIG ?= DEBUG

COMMON_MACROS := 
DEBUG_MACROS := DEBUG
RELEASE_MACROS := NDEBUG RELEASE

MCUFLAGS := 

CFLAGS += -g -DPOSIX -DSDL $(shell $(SDL_CONFIG) --cflags) -DGNUC
CXXFLAGS += -g -DPOSIX -DSDL $(shell $(SDL_CONFIG) --cflags) -DGNUC

# Valve uses SDL3 internally (the default if USE_SDL2 is not specified)
# The zip version of the SDK uses the SDL2 package from the runtime SDK
CXXFLAGS += -DUSE_SDL2

DEBUG_CFLAGS := -O0
RELEASE_CFLAGS := -O3
DEBUG_CXXFLAGS := $(DEBUG_CFLAGS)
RELEASE_CXXFLAGS := $(RELEASE_CFLAGS)

MACOS_FRAMEWORKS := 

LDFLAGS := $(shell $(SDL_CONFIG) --libs) -lSDL2_ttf -lfreetype -lz -lGL -lopenal
DEBUG_LDFLAGS := 
RELEASE_LDGLAGS :=

START_GROUP := -Wl,--start-group
END_GROUP := -Wl,--end-group

USE_DEL_TO_CLEAN := 0
GENERATE_BIN_FILE := 0
ADDITIONAL_MAKE_FILES :=
IS_LINUX_PROJECT := 1

include $(ADDITIONAL_MAKE_FILES)
