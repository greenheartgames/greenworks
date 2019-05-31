# Build Instructions for Electron

You can choose to build Greenworks with either `node-gyp` or `electron-rebuild`. If you are unsure, use `electron-rebuild`. You can also use prebuilt libraries (see the alternative option under `electron-rebuild`.)

## Building with node-gyp

First, follow these [instructions](get-steamworks-sdk.md) on getting the Steamworks SDK.

Electron's guide to
[using native Node modules](http://electron.atom.io/docs/tutorial/using-native-node-modules/)
explains that you can use `node-gyp` directly with custom settings to build a
native module for a given Electron version.

```shell
cd <greenworks_src_dir>

# install dependencies of Greenworks, "nan" module.
npm install

HOME=~/.electron-gyp node-gyp rebuild --target=<1.2.3 or other versions> --arch=x64 --dist-url=https://atom.io/download/atom-shell
```

The `--target` is the Electron version you're using (e.g. `1.2.3`), the `--arch`
may be either `ia32` or `x64`. (If you want to use 32-bit Electron, we recommend
installing it with the 32-bit version of node.)

After the `node-gyp` command is finished, you can find the
`greenworks-(linux/win/osx).node` binary (depending on your OS) in
`build/Release`.

## Building with electron-rebuild

Greenworks builds a native addon for Node. Addons are very sensitive to which
version of Node you use. For this reason, Electron provides an
[`electron-rebuild`](https://github.com/electron/electron-rebuild)
tool for rebuilding all of the addons in a `node_modules` folder. This means
that building and installing Greenworks is a multi-step process.

1) For demonstration purposes, we will be using the
[Electron Quick Start](https://github.com/electron/electron-quick-start)
application (You can skip to step 4 if you are following along directly with
your own application). Clone it:

```shell
git clone https://github.com/electron/electron-quick-start.git
cd electron-quick-start
```

2) Install all of the modules needed for the Electron Quick Start application:

```shell
npm install
```

3) Verify that the application works:

```shell
npm start
```

4) Now, we can install Greenworks. However, we can't build it yet, because it
does not have Steamworks SDK yet:

```shell
npm install --save --ignore-scripts git+https://github.com/greenheartgames/greenworks.git
```
   Note: If you would like to use pre built greenworks libraries, after the above is complete go to [Using Prebuilt](#using-prebuilt) below, when done skip to step 9.

5) Provide the Steamworks SDK dependency to Greenworks by following
[these steps](get-steamworks-sdk.md). (Essentially, You need to create and
populate the `node_modules/greenworks/deps/steamworks_sdk` folder.)

6) Now, installing Greenworks should succeed:

```shell
npm install
```

7) We need to rebuild the module, so install `electron-rebuild`:

```shell
npm install --save-dev electron-rebuild
```

8) Then, run it:

```shell
node_modules/.bin/electron-rebuild
```

Or, if you are on Windows:

```
node_modules\.bin\electron-rebuild
```

9) Finally, create a `steam_appid.txt` in the root of the application with the
Steam AppID you want to use. For testing purposes, we can use `480`, the AppID
for Spacewar, which is the example game included in the Steamworks SDK (Everyone
on Steam automatically owns this game by default).

```shell
echo 480 > steam_appid.txt
```

Note the following things about the AppID that you specify in this file:
* The current user logged into Steam must own this AppID, or else the Greenworks
initialization will fail.
* Once Greenworks is initialized, Steam will show the current user logged into
Steam as "playing" this AppID.
* If you specify the AppID of a different/existing game, after Greenworks is
initialized, Steam will prevent the user from opening that game, saying that
"that game is already open".

10) Overwrite the Electron Quick Start renderer.js with one that does some
"Hello World"-style Greenworks code:

```shell
mv renderer.js renderer.original.js
cp node_modules/greenworks/samples/electron/main.js renderer.js
```

Or, if you are on Windows:

```
move renderer.js renderer.original.js
copy node_modules\greenworks\samples\electron\main.js renderer.js
```

11) Test it:

```shell
npm start
```

## Using Prebuilt

Using prebuilt binary files for greenworks, instead of building them locally.

1) Download the prebuilt *.node file needed for your version of Electron and OS and copy it to node_modules/greenworks/lib. They are found at: https://github.com/greenheartgames/greenworks/releases For example for windows we would use: https://github.com/greenheartgames/greenworks/releases/download/v0.14.0/greenworks-v0.14.0-electron-v3.0.9-win-x64.zip we will copy lib/greenworks-win64.node from the zip file to node_modules/greenworks/lib in our project directory.

2) Get the Steamworks SDK. Download the Steamworks SDK from the Steam partner site. Make sure the version matches the one specified for the Greenworks plugin version (1.42 for our example). Unzip the file in a temporary folder. From the redistributable_bin folder, copy the appropriate files for the platform. For windows for example add redistributable_bin/win64/steam_api64.dll to node_modules/greenworks/lib in the project directory for the win64 build.

   You also need to copy the sdkencryptedappticket library. This can be found in public/steam/lib in the Steamworks SDK. You need to copy this to node_modules/greenworks/lib in the same way you did for steam_api.dll. For example for the win64 version, copy public/steam/lib/win64/sdkencryptedappticket64.dll to node_modules/greenworks/lib in the project directory.
