##New Release 0.3
The newest release enables compatibility with node 0.11.x, node-webkit 0.10.x and atom-shell 0.8.4 and above. If you find the release useful please consider donating.

<a href='https://pledgie.com/campaigns/27218'><img alt='Click here to lend your support to: Greenworks v0.3+ and make a donation at pledgie.com !' src='https://pledgie.com/campaigns/27218.png?skin_name=chrome' border='0' ></a>

#Greenworks

Greenworks is a MIT-licensed node.js addon allowing you to integrate your HTML5 app or game with [Steamworks](http://www.steampowered.com/steamworks/) by exposing a number of Steamworks APIs to JavaScript.

Greenworks is developed by Greenheart Games, originally to enable Steam integration in [Game Dev Tycoon](http://www.greenheartgames.com/app/game-dev-tycoon/), and has been open-sourced and [used in other projects](https://github.com/greenheartgames/greenworks/wiki/Apps-games-using-greenworks). The project is funded by Greenheart Games and other generous [donors](https://pledgie.com/campaigns/27218/donors).

The project is built using [NAN](https://github.com/rvagg/nan) to support both node 0.10.x and 0.11.x. technologies.

Currently greenworks supports:

* node v0.10.X and v0.11.X
* node-webkit v0.8.X and v0.10.X
* atom-shell v0.8.4 or above

##Download

Prebuild binaries of greenworks for node-webkit can be found on
the [release](https://github.com/greenheartgames/greenworks/releases) page.

##APIs

Greenworks currently supports Steam Cloud, Steam Achievement and Workshop synchronization related methods.

For a complete list of supported methods see the [API Reference](https://github.com/greenheartgames/greenworks/wiki/API-Reference)
for details.

##Start Greenworks in node-webkit

1. Download the release binaries from [release](https://github.com/greenheartgames/greenworks/releases) page and unzip.
2. Copy the `steam_api.dll`/`libsteam_api.dylib`/`libsteam_api.so` from Steamworks SDK(`<steam_sdk_path>/redistributable_bin/`) to
`<greenworks_path>/lib`. Please make sure the architecture (32 bits or 64 bits) of the steam dynamic library is the same as node-webkit's.
3. Create a `steam_appid.txt` file with your Steam APP ID under `<greenworks_path>/` directory. (
    You only need the `steam_appid.txt` file for developing. If you launch the game from Steam, Steam will automatically know the APPID of your game)
4. Create your node-webkit app code under `<greenworks_path>/` directory.

**A hello-world sample**

Create `index.html`:

```
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

```
{
  "name": "greenworks-nw-demo",
  "main": "index.html"
}
```

The node-webkit v0.10.5 hello-world demo directory on Mac OS X like:
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
to greenworks binaries directory.
2. Create a `steam_appid.txt` file with your Steam APP ID under code directory.


##Building Instructions

###Prerequisties

* Steamworks SDK 1.30
* nodejs v0.10.X or v0.11.X
* node-gyp (or nwp-gyp if you use node-webkit)

Download [Steamworks SDK](https://partner.steamgames.com/) and unzip to `<greenworks_src_dir>/deps/steamworks_sdk_dir`
directory.

###Nodejs Addon Building Steps

```
// change to greenworks src directory.
cd greenworks
// install the dependencies.
npm install
// configure gyp project.
node-gyp configure
// build greenworks addon.
node-gyp rebuild
```

Once building is done, you can find `greenworks-(linux/win/osx).node` under
`build/Release`.

If you encounter any issues consult the
[troubleshooting](https://github.com/greenheartgames/greenworks/wiki/Troubleshooting) page.

###Node-webkit Building Steps

Using Greenworks in node-webkit, you need to use [nw-gyp](https://github.com/rogerwang/nw-gyp)
instead of 'node-gyp' to build.

```
// install nw-gyp
sudo npm install -g nw-gyp

cd greenworks
// generate the building files
nw-gyp configure --target=<0.10.5 or other nw version>

// build Greenworks
nw-gyp build
```

After building finished, you can find the `greenworks-(linux/win/osx).node` binaries in `build/Release`.

A sample node-webkit application is provided [here](https://github.com/greenheartgames/greenworks/tree/nan-compatible/samples/node-webkit).


###Atom-shell Building Steps

To build Greenworks for atom-shell, we use `node-gyp` with some custom settings.

```
cd greenworks

HOME=~/.atom-shell-gyp node-gyp rebuild --target=<0.17.1 or other atom-shell versions> --dist-url=https://gh-contractor-zcbenz.s3.amazonaws.com/atom-shell/dist
```

After building finished, you can find the `greenworks-(linux/win/osx).node` binaries in `build/Release`.

A sample atom-shell application is provided [here](https://github.com/greenheartgames/greenworks/tree/nan-compatible/samples/atom-shell).

For more details, you can refer to [Using native Node modules](https://github.com/atom/atom-shell/blob/master/docs/tutorial/using-native-node-modules.md) page.

##Test

Greenworks uses [Mocha](http://visionmedia.github.io/mocha/) framework to test the steamworks APIs.

Since Greenworks is interacting with Steamworks, you need to create a `steam_appid.txt` file with
a valid Steam Game Application ID in the `<greenworks_src_dir>/test` directory, in order to run the tests.

```bash
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
