#Greenworks

Greenworks is a node.js addon which allows you to integrate your HTML5 app or game with [Steamworks](http://www.steampowered.com/steamworks/).
The addon exposes a number of useful Steamworks APIs to JavaScript for easy usage.

Greenworks supports:

* node v0.10.X and v0.11.X
* node-webkit v0.8.X and v0.10.X
* atom-shell v0.8.4 or above

It was originally created and developed by Greenheart Games Pty. Ltd for [Game Dev Tycoon](http://www.greenheartgames.com/app/game-dev-tycoon/),
a game powered by [node-webkit](https://github.com/rogerwang/node-webkit).

Greenworks is also used in other games, see [product list](https://github.com/greenheartgames/greenworks/wiki/Apps-games-using-greenworks).

##Download

Prebuild binaries of greenworks for Windows, Mac OSX and Linux  can be found on
the [release](https://github.com/greenheartgames/greenworks/releases) page.

##APIs

Greenworks currently supports Steam Cloud, Steam Achievement and Workshop synchronization related methods.

For a complete list of supported methods see the [API Reference](https://github.com/greenheartgames/greenworks/wiki/API-Reference)
for details.

##Usage

Before using greenworks, you need to copy `libsteam_api.dll`/`libsteam_api.dylib`/`libsteam_api.so`
library from the Steamworks SDK and copy the greenworks binaries to your app directory.

A simple application on Mac OS X which inits Steam API.
```
var greenworks = require('./greenworks-osx');

if (greenworks.initAPI()) {
  console.log('Steam API has been initalized.');
} else {
  console.log('Error on initializing Steam API');
}
```

##Building Instructions

###Prerequisties

* Steamworks SDK 1.30
* nodejs v0.10 or v0.11
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

If you use Greenworks, please tel us know at [@GreenheartGames](https://twitter.com/GreenheartGames) and freel free to add your product to our  [product list](https://github.com/greenheartgames/greenworks/wiki/Apps-games-using-greenworks).