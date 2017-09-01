# Build Instructions for Nodejs

## Prerequisties

* Steamworks SDK 1.41
* nodejs
* node-gyp

Please follow the [instructions](get-steamworks-sdk.md) on getting Steamworks
SDK.

## Build Steps

```shell
# Change to the Greenworks source directory.
cd <greenworks_src_dir>

# Install the dependencies of Greenworks, "nan" module.
npm install

# Configure gyp project.
node-gyp configure

# Build Greenworks addon.
node-gyp rebuild
```

Once building is done, you can find `greenworks-(linux/win/osx).node` binary
(depending on your OS) under `build/Release`.
