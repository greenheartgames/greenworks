## Building Spacewars (steamworksexample) for Linux

All native executables built for Steam for Linux should be built using the Steam for Linux runtime SDK.

For a complete developer guide regarding the Steam for Linux runtime, see [3]. For bugs and feature requests regarding the Steam for Linux runtime, see [4].

## Setup

Download, configure and open a shell in one of the Steam for Linux runtime SDKs. See [1] for details.

```
toolbox create -i registry.gitlab.steamos.cloud/steamrt/scout/sdk scout
toolbox enter scout
```

NOTE: You may also use the newer Linux runtime SDK for sniper. See [2].

## Build

Enter the toolbox shell, go to the `steamworksexample/` folder and build:

```
timo@eta-carinae ~/D/sdk> pwd
/home/timo/Downloads/sdk
timo@eta-carinae ~/D/sdk> toolbox enter scout
[timo@toolbox sdk]$ cd steamworksexample/
[timo@toolbox steamworksexample]$ make
g++ -g -DPOSIX -DSDL -I/usr/include/SDL2 -D_REENTRANT -DGNUC -O0 -std=c++0x -Wno-invalid-offsetof -I/home/timo/Downloads/sdk/steamworksexample/../public -DDEBUG   -c BaseMenu.cpp -o debug/BaseMenu.o -MD -MF debug/BaseMenu.dep
[..]
```

Use `make CONFIG=RELEASE` for a release build.

Use `ARCH=32` to build a 32 bit binary (do not use the legacy 32 bit scout SDK image, do this in the normal toolbox 64 bit container).

## Run

NOTE: You should not attempt to run out of your docker/podman/toolbox shell. Although it may work in some cases.

The Steam client should be running. The test program will initialize the Steamworks API on startup.

Launch with the `SteamworksExample.sh` script:

```
timo@eta-carinae ~/D/s/s/debug> pwd
/home/timo/Downloads/sdk/steamworksexample/debug
timo@eta-carinae ~/D/s/s/debug> ./SteamworksExample.sh
[S_API] SteamAPI_Init(): Loaded '/home/timo/steam/main/client/linux64/steamclient.so' OK.
Setting breakpad minidump AppID = 480
```

## Reference

1. [Steam for Linux runtime - scout SDK](https://gitlab.steamos.cloud/steamrt/scout/sdk)
2. [Steam for Linux runtime - scout SDK](https://gitlab.steamos.cloud/steamrt/sniper/sdk)
3. [Steam Linux Runtime - guide for game developers](https://gitlab.steamos.cloud/steamrt/steam-runtime-tools/-/blob/main/docs/slr-for-game-developers.md)
4. [Steam Linux Runtime on github](https://github.com/ValveSoftware/steam-runtime)
