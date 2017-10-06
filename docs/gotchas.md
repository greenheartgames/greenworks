# General Greenworks "Gotchas"

## The Steam AppID

* The current user logged into Steam must own the Steam AppID specified in the
`steam_appid.txt` file, or else the Greenworks initialization will fail.
* Once Greenworks is initialized, Steam will show the current user logged into
Steam as "playing" the Steam AppID specified in the `steam_appid.txt` file.
* If you are using the Steam AppID of a different/existing game after Greenworks
is initialized, Steam will prevent the user from opening that game, saying that
"that game is already open".

## Using Greenworks with Multi-Platform Projects

* Greenworks is a native node addon. When you compile it, the binary will only
for your OS. This means that if you want to build Greenworks for all 3 platforms
(Windows, Mac, Linux), you need to setup 3 different platforms.

## Using Greenworks with electron-builder

* If you are using Electron, that then you might be using
[electron-builder](https://github.com/electron-userland/electron-builder),
as it is the most common solution to build and package Electron applications. If
you are, then you will want to make sure that the Greenworks dependencies don't
get included or compiled in the actual end-user release. You can do this by
having the following in your `package.json` file:

```
"build": {
  "files": [
    "!node_modules/greenworks/deps/**/*"
  ]
},
```

* If you are using Greenworks in an Electron renderer process, you must call
`process.activateUvLoop()` before initializing Greenworks, or else the eventloop
of Greenworks won't get run. For more information, see
[issue #61](https://github.com/greenheartgames/greenworks/issues/61).
