## New Release 0.7

The newest release adds more Steam friends functionality, and provides
prebuilt binaries for Electron. You can find the full list of added features on
the [releases page](https://github.com/greenheartgames/greenworks/releases).

If you find the release useful, please consider donating.

<a href='https://pledgie.com/campaigns/27218'><img alt='Click here to lend your support to: Greenworks v0.3+ and make a donation at pledgie.com !' src='https://pledgie.com/campaigns/27218.png?skin_name=chrome' border='0' ></a>

# Greenworks

Greenworks is a MIT-licensed node.js addon allowing you to integrate your HTML5
app or game with [Steamworks](http://www.steampowered.com/steamworks/) by
exposing a number of Steamworks APIs to JavaScript.

Greenworks is developed by Greenheart Games, originally to enable Steam
integration in [Game Dev Tycoon](http://www.greenheartgames.com/app/game-dev-tycoon/),
and has since been open-sourced and
[used in other projects](https://github.com/greenheartgames/greenworks/wiki/Apps-games-using-greenworks).
The project is funded by Greenheart Games and other
[donors](https://pledgie.com/campaigns/27218#donors).

The project is built using [NAN](https://github.com/nodejs/nan) module to
support different node versions.

Currently greenworks supports:

* node v0.8, v0.10, v0.12, v4, v5, and v6
* NW.js (formerly node-webkit) v0.8, v0.11, v0.12, v0.13, v0.14, v0.15 and v0.16
* Electron (formerly atom-shell) v1.0.0 or above

## Download

Prebuilt binaries of greenworks for NW.js & Electron can be found on
the [releases](https://github.com/greenheartgames/greenworks/releases) page.

## APIs

Greenworks currently supports Steam Cloud, Steam Achievement and Workshop
synchronization related methods.

API references are located in
[docs](https://github.com/greenheartgames/greenworks/tree/master/docs) directory.

## Start Greenworks in NW.js

1. Download the release binaries from
[releases](https://github.com/greenheartgames/greenworks/releases) page and
unzip them.
2. Copy the `steam_api.dll`/`libsteam_api.dylib`/`libsteam_api.so` from
Steamworks SDK (`<steam_sdk_path>/redistributable_bin/`) to
`<greenworks_path>/lib`. Please make sure the architecture (32 bits or 64 bits)
of the steam dynamic library is the same as NW.js's.
3. Create a `steam_appid.txt` file with your Steam APP ID under
`<greenworks_path>/` directory. (You only need the `steam_appid.txt` file for
developing. If you launch the game from Steam, Steam will automatically know the
APP ID of your game.)
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
|   `-- libsteam_api.dylib
|-- package.json
`-- steam_appid.txt
```

## Usage Requirement

Before using greenworks, you need to:

1. Copy `steam_api.dll`/`libsteam_api.dylib`/`libsteam_api.so` library from the
Steamworks SDK(`<steam_sdk_path>/redistributable_bin/`) to `<app-dir>/lib/`.
2. Create a `steam_appid.txt` file with your Steam APP ID under `<app-dir>`
directory.

## Building Instructions

### Prerequisties

* Steamworks SDK 1.34
* nodejs v0.10.X or v0.11.X
* node-gyp (or nw-gyp if you use NW.js)

Download [Steamworks SDK](https://partner.steamgames.com/) and unzip to
`<greenworks_src_dir>/deps/steamworks_sdk` directory.

### Nodejs Addon Building Steps

```shell
# change to greenworks src directory.
cd greenworks
# install the dependencies.
npm install
# configure gyp project.
node-gyp configure
# build greenworks addon.
node-gyp rebuild
```

Once building is done, you can find `greenworks-(linux/win/osx).node` under
`build/Release`.

If you encounter any issues, please check the
[troubleshooting](https://github.com/greenheartgames/greenworks/tree/master/docs/troubleshooting.md)
page out before creating an issue.

### NW.js Building Steps

Using Greenworks in NW.js (before v0.13 and from v0.15), you need to use [nw-gyp](https://github.com/nwjs/nw-gyp)
instead of `node-gyp` to build.

**Note**:
NW.js v0.13 and v0.14 don't require to use `nw-gyp`. The native modules built
by `node-gyp` are supported, see [tutorial](https://groups.google.com/forum/#!msg/nwjs-general/UqEq8ito2gI/W-ld9LSoDQAJ).

NW.js v0.15+ needs nw-gyp again see http://docs.nwjs.io/en/latest/For%20Users/Advanced/Use%20Native%20Node%20Modules.

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
[samples/nw.js](https://github.com/greenheartgames/greenworks/tree/master/samples/nw.js).

### Electron Building Steps

Greenworks builds a native addon for node. Addons are very sensitive to which
version of node you use. For this reason, Electron provides an
[`electron-rebuild`](https://github.com/electron/electron-rebuild)
tool for rebuilding all of the addons in a `node_modules` folder.

A sample Electron application is provided in
[samples/electron](https://github.com/greenheartgames/greenworks/tree/master/samples/electron),
but you'll need to install `greenworks` into _another_ npm module in order to use
`electron-rebuild`.

```shell
mkdir sample-shell
cd sample-shell
npm init -y
```

... or just `cd` to your own npm module, e.g. the
[Electron Quick Start](https://github.com/electron/electron-quick-start)
repository.

```shell
# Install greenworks without building it, because it doesn't have Steamworks yet
npm install --ignore-scripts git+https://github.com/greenheartgames/greenworks.git

# Here, you must install Steamworks in node_modules/greenworks/deps/steamworks_sdk

# npm install in your project's directory should now succeed
npm install

npm install --save electron-prebuilt
npm install --save-dev electron-rebuild
node_modules/.bin/electron-rebuild

# create a steam_appid.txt file with your Steam App ID
echo 123456 > steam_appid.txt

node_modules/.bin/electron node_modules/greenworks/samples/electron/start.js
```

If you're using the Electron Quick Start, you can replace Quick Start's
`renderer.js` with the `greenworks/sample/electron/main.js` file, and that'll
work, too. Then you can run it with `npm start`.

Electron's guide to
[using native node modules](http://electron.atom.io/docs/tutorial/using-native-node-modules/)
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
`steam_appid.txt` file with a valid Steam Game Application ID in the
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
