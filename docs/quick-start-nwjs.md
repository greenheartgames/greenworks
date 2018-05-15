# Using Prebuilt Binaries in NW.js (Quick Start)

1. Download the release binaries from
[releases](https://github.com/greenheartgames/greenworks/releases) page and
unzip them to `<greenworks_path>` directory.
2. Download [Steamworks SDK](https://partner.steamgames.com/downloads/list) and
unzip it to `<steam_sdk_path>`. Make sure the version of Steamworks SDK is the
supported version of Greenworks prebuilt binaries.
3. Copy `steam_api.dll`/`libsteam_api.dylib`/`libsteam_api.so` (based on your
OS, e.g. `dll` for windows) from
`<steam_sdk_path>/redistributable_bin/[win64|linux32|linux64|osx32]` to
`<greenworks_path>/lib/`.
4. Copy `sdkencryptedappticket.dll`/`libsdkencryptedappticket.dylib`/
`libsdkencryptedappticket.so` (based on your OS) from
`<steam_sdk-path>/public/steam/lib/[win32|win64|linux32|linux64|osx32]` to
`<greenworks_path>/lib`.
5. Create a `steam_appid.txt` file with your Steam AppID (or the steamworks
example AppID `480`) under `<greenworks_path>/` directory.
4. Develop your NW.js app code under `<greenworks_path>/` directory.

Note:

* When copying the Steam SDK binaries in step #3 and #4 above, please make sure
the architecture of the steam dynamic library matches your OS and NW.js. For
example, if you are using 64-bits NW.js on 64-bits Windows, copy
`<steam_sdk_path>/redistributable_bin/win64/steam_api64.dll` and
`<steam_sdk_path>/public/steam/lib/win64/sdkencryptedappticket64.dll`.

* For step #5, you only need the `steam_appid.txt` for development. If you
launch the game from Steam, Steam will automatically know the AppID of your
game).

## Minimal App

A [hello-world](../samples/nw.js) app:

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
    document.write(greenworks.init());
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

The NW.js `hello-world` demo directory on Mac OS X like:

```
|-- greenworks.js
|-- index.html
|-- lib
|   |-- greenworks-osx64.node
|   |-- libsdkencryptedappticket.dylib
|   `-- libsteam_api.dylib
|-- package.json
`-- steam_appid.txt
```
