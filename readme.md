## May 28, 2017 - New Release v0.10.0

The newest release adds Steam Stats functionality. You can find the
full list of added features on the
[releases page](https://github.com/greenheartgames/greenworks/releases).

If you find the release useful, please consider donating.

<a href='https://pledgie.com/campaigns/27218'><img alt='Click here to lend your support to: Greenworks v0.3+ and make a donation at pledgie.com !' src='https://pledgie.com/campaigns/27218.png?skin_name=chrome' border='0' ></a>

# Greenworks

Greenworks is a MIT-licensed node.js addon allowing you to integrate your HTML5
app or game with [Steamworks](http://www.steampowered.com/steamworks/) by
exposing a number of Steamworks APIs to JavaScript.

Greenworks is developed by Greenheart Games, originally to enable Steam
integration in [Game Dev Tycoon](http://www.greenheartgames.com/app/game-dev-tycoon/),
and has been open-sourced and [used in other projects](https://github.com/greenheartgames/greenworks/wiki/Apps-games-using-greenworks).
The project is funded by Greenheart Games and other
[donors](https://pledgie.com/campaigns/27218#donors).

The project is built using [NAN](https://github.com/nodejs/nan) module to
support different node versions.

Currently Greenworks supports:

* node v0.8, v0.10, v0.12, v4, v5, v6 and v7
* NW.js (formerly node-webkit) v0.8, v0.11 or above
* Electron (formerly atom-shell) v1.0.0 or above

## Download

Prebuilt binaries of Greenworks for NW.js & Electron can be found on
the [releases](https://github.com/greenheartgames/greenworks/releases) page.

## APIs

Greenworks currently supports Steam Cloud, Steam Achievement and Workshop
synchronization related methods.

API references are located in [docs](docs) directory.

## General Usage Requirements

Before using Greenworks, you need to:

1. Copy `steam_api.dll`/`libsteam_api.dylib`/`libsteam_api.so` (from
`<steam_sdk_path>/redistributable_bin/`) to `<app-dir>/lib/`.
2. Copy `sdkencryptedappticket.dll`/`libsdkencryptedappticket.dylib`/
`libsdkencryptedappticket.so` (from `<steam_sdk-path>/public/steam/lib/`) to
`<greenworks_path>/lib`.
3. Create a `steam_appid.txt` file with your Steam AppID
(or the Steamworks example AppID of 480) in the `<greenworks_path>/` directory.

## General Greenworks "Gotchas"

* The current user logged into Steam must own the Steam AppID specified in the
`steam_appid.txt` file, or else the Greenworks initialization will fail.
* Once Greenworks is initialized, Steam will show the current user logged into
Steam as "playing" the Steam AppID specified in the `steam_appid.txt` file.
* If you are using the Steam AppID of a different/existing game after Greenworks
is initialized, Steam will prevent the user from opening that game, saying that
"that game is already open".
* If you are using Greenworks in an Electron renderer process, you must call
`process.activateUvLoop()` before initializing Greenworks, or else the eventloop
of Greenworks won't get run. For more information, see [issue #61](https://github.com/greenheartgames/greenworks/issues/61).

## Using the Greenworks Prebuilt Binaries in NW.js (Quick Start)

1. Download the release binaries from
[releases](https://github.com/greenheartgames/greenworks/releases) page and
unzip them.
2. Copy `steam_api.dll`/`libsteam_api.dylib`/`libsteam_api.so` (from
`<steam_sdk_path>/redistributable_bin/`) to `<greenworks_path>/lib`.
Please make sure the architecture (32 bits or 64 bits) of the steam dynamic
library is the same as NW.js's.
3. Copy `sdkencryptedappticket.dll`/`libsdkencryptedappticket.dylib`/
`libsdkencryptedappticket.so` (from `<steam_sdk-path>/public/steam/lib/`) to
`<greenworks_path>/lib`.
4. Create a `steam_appid.txt` file with your Steam AppID (or the steamworks
example AppID) under `<greenworks_path>/` directory (You only need the file for
development. If you launch the game from Steam, Steam will automatically know
the AppID of your game).
4. Create your NW.js app code under `<greenworks_path>/` directory.

**A hello-world sample**

Create `index.html`:

```html
<html>
<head>
  <meta charset="utf-8">
  <title>Hello Greenworks</title>
</head>

<body>
  <h1>Greenworks Test</h1>
  SteamAPI Init:
  <script>
    var greenworks = require('./greenworks');
    document.write(greenworks.initAPI());
  </script>
</body>
```

Create `package.json`:

```json
{
  "name": "greenworks-nw-demo",
  "main": "index.html"
}
```

The NW.js v0.11.2 hello-world demo directory on Mac OS X like:

```
|-- greenworks.js
|-- index.html
|-- lib
|   |-- greenworks-linux32.node
|   |-- greenworks-linux64.node
|   |-- greenworks-osx32.node
|   |-- greenworks-osx64.node
|   |-- greenworks-win32.node
|   |-- libsdkencryptedappticket.dylib
|   `-- libsteam_api.dylib
|-- package.json
`-- steam_appid.txt
```

## Building Instructions

### General Prerequisties

* Steamworks SDK 1.38a
* nodejs
* node-gyp (or nw-gyp if you use NW.js)

To get the Steamworks SDK:

1. [Download it from Steam](https://partner.steamgames.com/downloads/list). (You will need to login with your Steam account first in order to access the downloads list.)
2. Extract the contents of the zip file.
3. The extracted contents will contain one subdirectory, `sdk`. Rename this to
`steamworks_sdk`.
4. Copy this directory to `<greenworks_src_dir>/deps/`.

### Nodejs Addon Building Steps

```shell
# Change to the Dreenworks source directory
cd greenworks
# Install the dependencies
npm install
# Configure gyp project
node-gyp configure
# Build Greenworks addon
node-gyp rebuild
```

Once building is done, you can find `greenworks-(linux/win/osx).node` under
`build/Release`.

If you encounter any issues, please check the
[troubleshooting](docs/troubleshooting.md) page out before creating an issue.

### NW.js Building Steps

Using Greenworks in NW.js (before v0.13 and from v0.15), you need to use
[nw-gyp](https://github.com/nwjs/nw-gyp) instead of `node-gyp` to build.

**Note**:
NW.js v0.13 and v0.14 don't require to use `nw-gyp`. The native modules built
by `node-gyp` are supported, see
[tutorial](https://groups.google.com/forum/#!msg/nwjs-general/UqEq8ito2gI/W-ld9LSoDQAJ).
NW.js v0.15+ needs `nw-gyp` again see http://docs.nwjs.io/en/latest/For%20Users/Advanced/Use%20Native%20Node%20Modules.

Install additional needed tools depending on your platform see https://github.com/nwjs/nw-gyp#installation.

```shell
# install nw-gyp
sudo npm install -g nw-gyp

cd greenworks
# generate the building files
nw-gyp configure --target=<0.10.5 or other nw version> --arch=<x64 or ia32>

# If you have an error stating requiure('nan') fails then do a local
sudo npm install nan

# build Greenworks
nw-gyp build

# You will have error if you forget to install the additional tools as stated above, you may also
# experience an issue building ia32 on linux 64, try installing g++-multilib
sudo apt-get install g++-multilib
```

After building finished, you can find the `greenworks-(linux/win/osx).node`
binaries in `build/Release`.

A sample NW.js application is provided
[samples/nw.js](samples/nw.js).

### Electron Building Steps

Greenworks builds a native addon for Node. Addons are very sensitive to which
version of Node you use. For this reason, Electron provides an
[`electron-rebuild`](https://github.com/electron/electron-rebuild)
tool for rebuilding all of the addons in a `node_modules` folder. This means
that building/installing Greenworks is a multi-step process.

1) For demonstration purposes, we will be using the [Electron Quick Start](https://github.com/electron/electron-quick-start)
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
does not have Steamworks yet:

```shell
npm install --save --ignore-scripts git+https://github.com/greenheartgames/greenworks.git
```

5) Provide the Steamworks dependency to Greenworks by following the steps from
the "General Prerequisties" section earlier on the page (
You need to create and populate the `node_modules/greenworks/deps/steamworks_sdk`
folder.).

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

9) Finally, create a `steam_appid.txt` in the root of the application with the
Steam AppID you want to use. For testing purposes, we can use `480`, the AppID
for Spacewar, which is the example game included in the Steamworks SDK (Everyone
on Steam automatically owns this game by default.).

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
cp node_modules/greenworks/sample/electron/main.js renderer.js
```

11) Test it:

```shell
npm start
```

### Electron Building Steps for a Specific Version

Electron's guide to
[using native Node modules](http://electron.atom.io/docs/tutorial/using-native-node-modules/)
explains that you can also use `node-gyp` directly with custom settings to build
a native module for a given Electron version.

```shell
cd greenworks
HOME=~/.electron-gyp node-gyp rebuild --target=<1.2.3 or other versions> --arch=x64 --dist-url=https://atom.io/download/atom-shell
```

The `--target` is the Electron version you're using (e.g. `1.2.3`), the `--arch`
may be either `ia32` or `x64`. (If you want to use 32-bit Electron, we recommend
installing it with the 32-bit version of node.)

After the `node-gyp` command is finished, you can find the
`greenworks-(linux/win/osx).node` binaries in `build/Release`.

## Test

Greenworks uses [Mocha](http://visionmedia.github.io/mocha/) framework to test
the steamworks APIs.

Since Greenworks is interacting with Steamworks, you need to create a
`steam_appid.txt` file with a valid Steam Game Application ID
(your own AppID or Steamworks example AppID) in the
`<greenworks_src_dir>/test` directory, in order to run the tests.

```shell
cd greenworks
./test/run-test
```

See [how to find the application ID for a Steam Game](https://support.steampowered.com/kb_article.php?ref=3729-WFJZ-4175).

## License

Greenworks is published under the MIT license. See `LICENSE` file for details.

If you use Greenworks, please let us know at
[@GreenheartGames](https://twitter.com/GreenheartGames)
and feel free to add your product to our
[product list](https://github.com/greenheartgames/greenworks/wiki/Apps-games-using-greenworks).

## Donate

Please consider donating to the project:

<a href='https://pledgie.com/campaigns/27218'><img alt='Click here to lend your support to: Greenworks v0.3+ and make a donation at pledgie.com !' src='https://pledgie.com/campaigns/27218.png?skin_name=chrome' border='0' ></a>
