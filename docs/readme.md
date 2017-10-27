# Greenworks Documents

## FAQ

There are questions/issues being asked quite often. Check following pages before
creating an issue.
  * [Troubleshooting](troubleshooting.md)
  * [Gotchas](gotchas.md)

## Guides

* [Quick Start (NW.js)](quick-start-nwjs.md)

## Build Instructions

* [Build Instructions (NW.js)](build-instructions-nwjs.md)
* [Build Instructions (Electron)](build-instructions-electron.md)
* [Build Instructions (Node.js)](build-instructions-nodejs.md)

## Test

* [Mocha Test](mocha-test.md)

## APIs

The `greenworks` module gives you ability to access Steam APIs:

```js
var greenworks = require('./greenworks');

if (greenworks.init())
  console.log('Steam API has been initalized.');
```

## API References

* [Achievement](achievement.md)
* [Authentication](authentication.md)
* [Cloud](cloud.md)
* [DLC](dlc.md)
* [Events](events.md)
* [Friends](friends.md)
* [Setting](setting.md)
* [Stats](stats.md)
* [Utils](utils.md)
* [Workshop](workshop.md)
