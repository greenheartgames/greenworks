##New Release 0.4
The newest release enables compatibility with node v0.11.x and v0.12.x, NW.js v0.12.X and Electron v0.21.X. If you find the release useful, please consider donating.

<a href='https://pledgie.com/campaigns/27218'><img alt='Click here to lend your support to: Greenworks v0.3+ and make a donation at pledgie.com !' src='https://pledgie.com/campaigns/27218.png?skin_name=chrome' border='0' ></a>

#Greenworks

Greenworks is a MIT-licensed node.js addon allowing you to integrate your HTML5 app or game with [Steamworks](http://www.steampowered.com/steamworks/) by exposing a number of Steamworks APIs to JavaScript.

Greenworks is developed by Greenheart Games, originally to enable Steam integration in [Game Dev Tycoon](http://www.greenheartgames.com/app/game-dev-tycoon/), and has since been open-sourced and [used in other projects](https://github.com/greenheartgames/greenworks/wiki/Apps-games-using-greenworks). The project is funded by Greenheart Games and other [donors](https://pledgie.com/campaigns/27218#donors).

The project is built using [NAN](https://github.com/rvagg/nan) to support both different node versions.

Currently greenworks supports:

* node v0.10.X, v0.11.X and v0.12.X
* io.js v3.2.0
* NW.js(formerly node-webkit) v0.8.X, v0.11.X and v0.12.X
* Electron(formerly atom-shell) v0.21.X or above

##Download

Prebuild binaries of greenworks for NW.js can be found on
the [release](https://github.com/greenheartgames/greenworks/releases) page.

##APIs

Greenworks currently supports Steam Cloud, Steam Achievement and Workshop synchronization related methods.

For a complete list of supported methods see the [API Reference](https://github.com/greenheartgames/greenworks/wiki/API-Reference)
for details.

##Start Greenworks in NW.js

1. Download the release binaries from [release](https://github.com/greenheartgames/greenworks/releases) page and unzip.
2. Copy the `steam_api.dll`/`libsteam_api.dylib`/`libsteam_api.so` from Steamworks SDK(`<steam_sdk_path>/redistributable_bin/`) to
`<greenworks_path>/lib`. Please make sure the architecture (32 bits or 64 bits) of the steam dynamic library is the same as NW.js's.
3. Create a `steam_appid.txt` file with your Steam APP ID under `<greenworks_path>/` directory. (
    You only need the `steam_appid.txt` file for developing. If you launch the game from Steam, Steam will automatically know the APPID of your game)
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

##Usage Requirement

Before using greenworks, you need to:

1. Copy `steam_api.dll`/`libsteam_api.dylib`/`libsteam_api.so` library from the Steamworks SDK(`<steam_sdk_path>/redistributable_bin/`)
to `<app-dir>/lib/`.
2. Create a `steam_appid.txt` file with your Steam APP ID under `<app-dir>` directory.


##Building Instructions

###Prerequisties

* Steamworks SDK 1.34
* nodejs v0.10.X or v0.11.X
* node-gyp (or nw-gyp if you use NW.js)

Download [Steamworks SDK](https://partner.steamgames.com/) and unzip to `<greenworks_src_dir>/deps/steamworks_sdk`
directory.

###Nodejs Addon Building Steps

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

If you encounter any issues consult the
[troubleshooting](https://github.com/greenheartgames/greenworks/wiki/Troubleshooting) page.

###NW.js Building Steps

Using Greenworks in NW.js, you need to use [nw-gyp](https://github.com/nwjs/nw-gyp)
instead of `node-gyp` to build.

```shell
# install nw-gyp
sudo npm install -g nw-gyp

cd greenworks
# generate the building files
nw-gyp configure --target=<0.10.5 or other nw version> --arch=<x64 or ia32>

# build Greenworks
nw-gyp build
```

After building finished, you can find the `greenworks-(linux/win/osx).node` binaries in `build/Release`.

A sample NW.js application is provided [here](https://github.com/greenheartgames/greenworks/tree/master/samples/nw.js).


###Electron Building Steps

To build Greenworks for Electron, we use `node-gyp` with some custom settings.

```shell
cd greenworks

HOME=~/.electron-gyp node-gyp rebuild --target=<0.29.2 or other versions> --arch=x64 --dist-url=https://atom.io/download/atom-shell
```

After building finished, you can find the `greenworks-(linux/win/osx).node` binaries in `build/Release`.

A sample for Electron application is provided [here](https://github.com/greenheartgames/greenworks/tree/master/samples/electron).

For more details, you can refer to [Using native Node modules](https://github.com/atom/electron/blob/master/docs/tutorial/using-native-node-modules.md) page.

##Test

Greenworks uses [Mocha](http://visionmedia.github.io/mocha/) framework to test the steamworks APIs.

Since Greenworks is interacting with Steamworks, you need to create a `steam_appid.txt` file with
a valid Steam Game Application ID in the `<greenworks_src_dir>/test` directory, in order to run the tests.

```shell
cd greenworks
./test/run-test
```

See [how to find the application ID for a Steam Game](https://support.steampowered.com/kb_article.php?ref=3729-WFJZ-4175).

##License

Greenworks is published under the MIT license. See `LICENSE` file for details.

If you use Greenworks, please let us know at [@GreenheartGames](https://twitter.com/GreenheartGames) and feel free to add your product to our  [product list](https://github.com/greenheartgames/greenworks/wiki/Apps-games-using-greenworks).

##Donate

Please consider donating to the project:

<a href='https://pledgie.com/campaigns/27218'><img alt='Click here to lend your support to: Greenworks v0.3+ and make a donation at pledgie.com !' src='https://pledgie.com/campaigns/27218.png?skin_name=chrome' border='0' ></a>
