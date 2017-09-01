# General Greenworks "Gotchas"

* The current user logged into Steam must own the Steam AppID specified in the
`steam_appid.txt` file, or else the Greenworks initialization will fail.
* Once Greenworks is initialized, Steam will show the current user logged into
Steam as "playing" the Steam AppID specified in the `steam_appid.txt` file.
* If you are using the Steam AppID of a different/existing game after Greenworks
is initialized, Steam will prevent the user from opening that game, saying that
"that game is already open".
* If you are using Greenworks in an Electron renderer process, you must call
`process.activateUvLoop()` before initializing Greenworks, or else the eventloop
of Greenworks won't get run. For more information, see
[issue #61](https://github.com/greenheartgames/greenworks/issues/61).
* Greenworks is a native node addon. It is only being able to compile the binary
for your environment OS, which means if you want to build your own Greenworks
for 3 platforms (Windows, Mac, Linux), you need to setup 3 different platforms.
