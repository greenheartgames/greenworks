# Greenworks Documents

The `greenworks` module gives you ability to access Steam APIs:

```js
var greenworks = require('./greenworks');

if (greenworks.initAPI()) {
  console.log('Steam API has been initalized.');
} else {
  console.log('Error on initializing Steam API.');
}
```

## API References

* [Authentication](authentication.md)
* [Cloud](cloud.md)
* [Events](events.md)
* [Friends](friends.md)
* [Setting](setting.md)
* [Utils](utils.md)
* [Workshop](workshop.md)

## Troubleshooting

* [Troubleshooting](troubleshooting.md)
