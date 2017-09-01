# Greenworks Troubleshooting

## Library not loaded: @loader_path/libsteam_api.dylib (or .dll for windows)

That means you didn't copy this file to your app directory and it can't link it
at runtime.

## SteamAPI_Init() failed; unable to locate a running instance of Steam, or a local steamclient.dll

Your steam client should be running (or you can copy the steamclient dynamic
library into your app directory).

## SteamAPI_Init() failed; no appID found.

Steam can't determine for which steam application you are trying to initialise
the API. Either launch the game from Steam, or put the file `steam_appid.txt`
containing the correct appID in your app folder. steam_appid.txt should contain
just your application id, nothing else.

## initAPI() returns false.

If initAPI is returning false, even when the above are correct, one possibility
is that the steam client is running with administrator privileges, but the app
is not. Try running the app as administrator
(right click, and 'run as administrator').

## PublishWorkShopFile API returns `k_EResultInsufficientPrivilege`

This indicated your Steam account is limited, missing some features of Steam.
You need to do one of the following things to access all Steam features:

1. Purchase a game for Steam store
2. Redeem a Steam gift
3. Complete a microtransaction

For more details, please refer to
[Steam support](https://support.steampowered.com/kb_article.php?ref=3330-IAGK-7663).

## Steam Overlay is unresponsive / frozen

In order for the Steam Overlay (activated by "Shift + Tab" by default) to work
properly, you need to have your NW.js draw every frame. Normally, the browser
won't redraw every frame when it's idle.

One possible solution to this would be, to insert a small 1x1 pixel canvas
element, which you update in every frame. Example:

```html
<html>
<head>
<script>
    function forceRefresh() {
        var canvas = document.getElementById("forceRefreshCanvas");
        var ctx = canvas.getContext("2d");
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        window.requestAnimationFrame(forceRefresh);
    }
</script>
<body onload="forceRefresh();">
    <canvas id="forceRefreshCanvas" width="1" height="1"></canvas>
</body>
</html>
```

Now NW.js will redraw every frame at the current refresh rate
(in most cases 60 fps). This hack might not be necessary for game using WebGL
because a canvas is probably already refreshed every frame for rendering.

If this doesn't seem to work, try adding the chromium flag "--in-process-gpu"
to your package.json as well.

If the overlay appears strange, like other background windows are mixing with it
try to remove the transparent flag in windows property of package json.

```
  "window": {
    "toolbar": false,
    "fullscreen": true,
    "frame": false,
    // "transparent": true, // now should work correctly
    "no-edit-menu" : true
  },
```

**Note**:

From the Steam
[game_overlay document](https://partner.steamgames.com/documentation/game_overlay):

> The game doesn't need to do anything to have the overlay work, it
> automatically hooks in for any game launched via Steam.
> Why isn't the Steam overlay showing up in my app?
> See requirements. If you do meet the requirements and it's still not showing
> up, make sure you're launching the app from the Steam client.

You need to launch the game via Steam Client Application to enable the game
overlay feature.

## Unable to get `node-gyp` on Windows.

On Windows, `node-gyp` requires additional tools (e.g. Visual C++ Build Tools)
and configurations. Please follow the
[`node-gyp` instructions](https://github.com/nodejs/node-gyp#on-window).
