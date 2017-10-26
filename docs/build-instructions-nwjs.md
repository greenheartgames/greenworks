# Build Instructions for NW.js

## Prerequisties

* Steamworks SDK 1.41
* nodejs
* nw-gyp

Note: Using Greenworks in NW.js (before v0.13 and from v0.15), you need to use
[nw-gyp](https://github.com/nwjs/nw-gyp) instead of `node-gyp` to build.
NW.js v0.13 and v0.14 don't require to use `nw-gyp`. The native modules built
by `node-gyp` are supported, see
[tutorial](https://groups.google.com/forum/#!msg/nwjs-general/UqEq8ito2gI/W-ld9LSoDQAJ).
NW.js v0.15+ needs `nw-gyp` again see http://docs.nwjs.io/en/latest/For%20Users/Advanced/Use%20Native%20Node%20Modules.

Please follow the [instructions](get-steamworks-sdk.md) on getting Steamworks SDK.

## Build Steps

Install additional needed tools depending on your platform see
https://github.com/nwjs/nw-gyp#installation.

```shell
# install nw-gyp
sudo npm install -g nw-gyp

cd <greenworks_src_dir>
# generate the building files
nw-gyp configure --target=<0.10.5 or other nw version> --arch=<x64 or ia32>

# If you have an error stating that require('nan') fails, then do a local.
npm install nan

# build Greenworks
nw-gyp build

# You will have error if you forget to install the additional tools as stated
# above, you may also experience an issue building ia32 on linux 64,
# try installing g++-multilib.
sudo apt-get install g++-multilib
```

After building finished, you can find the `greenworks-(linux/win/osx).node`
binary (depending on your OS) in `build/Release`.

A hello-world NW.js application is provided at [samples/nw.js](../samples/nw.js).
