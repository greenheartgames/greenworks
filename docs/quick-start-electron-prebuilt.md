# Greenworks quick start using pre-built Greenworks binaries
Adapated from: https://github.com/greenheartgames/greenworks/blob/master/docs/build-instructions-electron.md

Greenworks builds a native addon for Node. Addons are very sensitive to which
version of Node you use, so use the appropriate Electron version and Greenworks pre-built binary files.

1) For demonstration purposes, we will be using the
[Electron Quick Start](https://github.com/electron/electron-quick-start)
application. Clone it:

```shell
git clone https://github.com/electron/electron-quick-start.git
cd electron-quick-start
```

2) Edit package.json to a supported Electron version of Greenworks pre-built binaries. Change `  "electron": "^3.0.0"` to `  "electron": "3.0.9"` and save package.json

3) Install all of the modules needed for the Electron Quick Start application:

```shell
npm install
```

4) Verify that the application works (without Greenworks, it will just show the versions of Electron, Node and Chromium):

```shell
npm start
```

4) Now, we can install Greenworks (without building the Greenworks library, because we will use the prebuilt versions)

```shell
npm install --save --ignore-scripts git+https://github.com/greenheartgames/greenworks.git
```

5) Create a directory for the prebuilt binaries and the Steamworks binaries.
```shell
cd node_modules/greenworks
mkdir lib
```

6) Download the prebuilt *.node file needed for your version of Electron and OS and copy it to node_modules/greenworks/lib. They are found at: https://github.com/greenheartgames/greenworks/releases For our example we will use: https://github.com/greenheartgames/greenworks/releases/download/v0.14.0/greenworks-v0.14.0-electron-v3.0.9-win-x64.zip we will copy lib/greenworks-win64.node from the zip file to node_modules/greenworks/lib.


7) Get the Steamworks SDK. Download the Steamworks SDK from the Steam partner site. Make sure the version matches the one specified for the Greenworks plugin version (1.42 for our example). From the redistributable_bin folder, copy the appropriate files for the platform. For our example add redistributable_bin/win64/steam_api64.dll to node_modules/greenworks/lib for the win64 build.

You also need to copy the sdkencryptedappticket library. This can be found in public/steam/lib in the Steamworks SDK. You need to copy this to node_modules/greenworks/lib in the same way you did for steam_api.dll. For example for the win64 version, copy public/steam/lib/win64/sdkencryptedappticket64.dll to node_modules/greenworks/lib

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

11) Test it (it should show that Greenworks is intialized and a few test functions, the achievement function will fail, because the wrong achievement name is being used in the sample code.)

```shell
npm start
```
