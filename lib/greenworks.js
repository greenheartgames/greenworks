var os = require('os')
var greenworks;
if (os.platform() == 'darwin')
  greenworks = require('../build/release/greenworks-osx');
else if (os.platform() == 'win32 ' || os.platform() == 'win64')
  greenworks = require('../build/release/greenworks-win');
else if (os.platform() == 'linux') {
  if (os.arch() == 'x64')
    greenworks = require('../build/release/greenworks-linux64');
  else if (os.arch() == 'ia32')
    greenworks = require('../build/release/greenworks-linux32');
}
module.exports = greenworks;
