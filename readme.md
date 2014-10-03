#Greenworks

Greenworks is a nodejs addon integrated with [Steamworks](http://www.steampowered.com/steamworks/).
The addon aims to expose Steam APIs to javascript for easy usage. It supports
both node v0.10 and v0.11.

It's created and developed by Greenheart Games Pty. Ltd for [Game Dev Tycoon](http://www.greenheartgames.com/app/game-dev-tycoon/),
a game powered by [node-webkit](https://github.com/rogerwang/node-webkit).

Greenworks is also used in other games, see [real apps games list](https://github.com/greenheartgames/greenworks/wiki/Apps-games-using-greenworks).

##Download

Prebuild binaries of greenworks for Windows, Mac OSX and Linux can be found on
the [release](https://github.com/greenheartgames/greenworks/releases) page.

##APIs

Greenworks supports Steam related APIs based on Steamworks SDK, such as
Steam Cloud, Steam Achievement and Workshop synchronization.

See [docs](https://github.com/greenheartgames/greenworks/wiki/Greenworks-API) for details.

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

###Building steps

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

Once building is done, you can fine `greenworks-(linux/win/osx).node` under
`build/Release`.

If you have any issues on building/running, consult to
[troubleshooting](https://github.com/greenheartgames/greenworks/wiki/Troubleshooting) page.

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
