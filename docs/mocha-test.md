# Mocha Test

Greenworks uses [Mocha](http://visionmedia.github.io/mocha/) framework to test
the steamworks APIs.

Since Greenworks is interacting with Steamworks, you need to create a
`steam_appid.txt` file with a valid Steam Game Application ID
(your own AppID or Steamworks example AppID) in the
`<greenworks_src_dir>/test` directory, in order to run the tests.

```shell
cd <greenworks_src_dir>

./test/run-test
```

See [how to find the application ID for a Steam Game](https://support.steampowered.com/kb_article.php?ref=3729-WFJZ-4175).
