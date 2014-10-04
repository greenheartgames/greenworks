#Greenworks

Greenworks is a nodejs addon integrated with [Steamworks](http://www.steampowered.com/steamworks/).
The addon aims to expose Steam APIs to javascript for easy usage.

Greenworks supports:

* node v0.10.X and v0.11.X
* node-webkit v0.8.X and v0.10.X
* atom-shell v0.8.4 or above

It's created and developed by Greenheart Games Pty. Ltd for [Game Dev Tycoon](http://www.greenheartgames.com/app/game-dev-tycoon/),
a game powered by [node-webkit](https://github.com/rogerwang/node-webkit).

Greenworks is also used in other games, see [real apps games list](https://github.com/greenheartgames/greenworks/wiki/Apps-games-using-greenworks).

##Download

Prebuild binaries of greenworks for Windows, Mac OSX and Linux can be found on
the [release](https://github.com/greenheartgames/greenworks/releases) page.

##APIs

Greenworks supports Steam related APIs based on Steamworks SDK, such as
Steam Cloud, Steam Achievement and Workshop synchronization.

See [API Reference](https://github.com/greenheartgames/greenworks/wiki/API-Reference)
for details.

##Usage

Before using greenworks, you need to copy `libsteam_api.dll`/`libsteam_api.dylib`/`libsteam_api.so`
library from Steamworks SDK and `greenworks.node` to your app directory.

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
* node-gyp

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

If you have any issues on building/running, consult to
[troubleshooting](https://github.com/greenheartgames/greenworks/wiki/Troubleshooting) page.

###Node-webkit Building Steps

Using Greenworks in node-webkit, you need the [nw-gyp](https://github.com/rogerwang/nw-gyp)
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

After building finished, you can find `greenworks-(linux/win/osx).node` file
ends up in `build/Release`.

A sample nw application is provided [here](https://github.com/greenheartgames/greenworks/tree/nan-compatible/samples/node-webkit).

For more details, you can refer to [nw-gyp](https://github.com/rogerwang/nw-gyp) page.

###Atom-shell Building Steps

Using Greenworks in atom-shll, we use `node-gyp` with some custom settings to build
greenworks module.

```
cd greenworks

HOME=~/.atom-shell-gyp node-gyp rebuild --target=<0.17.1 or other atom-shell versions> --dist-url=https://gh-contractor-zcbenz.s3.amazonaws.com/atom-shell/dist
```

After building finished, you can find `greenworks-(linux/win/osx).node` file
ends up in `build/Release`.

A sample atom-shell application is provided [here](https://github.com/greenheartgames/greenworks/tree/nan-compatible/samples/atom-shell).

For more details, you can refer to [Using native Node modules](https://github.com/atom/atom-shell/blob/master/docs/tutorial/using-native-node-modules.md) page.

##Test

Greenworks uses [Mocha](http://visionmedia.github.io/mocha/) framework to test
steam APIs.

Before running the test, you should create a `steam_appid.txt` with
a Steam Game Application ID in `<greenworks_src_dir>/test` directory.

```bash
cd greenworks
./test/run-test
```

See [how to find the application ID for a Steam Game](https://support.steampowered.com/kb_article.php?ref=3729-WFJZ-4175).

##License

Greenworks is published under the MIT license. See `LICENSE` file for details.
