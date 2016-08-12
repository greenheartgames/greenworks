# A sample app

A sample app demonstrates usage of greenworks APIs.

To enable game-overlay feature on Windows, you need to customize
`chromium-args` field to `--in-process-gpu` and `--disable-transparency `
in `package.json`:

```
"chromium-args": "--in-process-gpu --disable-transparency"
```
