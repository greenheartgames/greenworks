var os = require('os');
var fs = require('fs');
var greenworks;
if (os.platform() == 'darwin')
  greenworks = require('../build/Release/greenworks-osx');
else if (os.platform() == 'win32' || os.platform() == 'win64')
  greenworks = require('../build/Release/greenworks-win');
else if (os.platform() == 'linux') {
  if (os.arch() == 'x64')
    greenworks = require('../build/Release/greenworks-linux64');
  else if (os.arch() == 'ia32')
    greenworks = require('../build/Release/greenworks-linux32');
}

// Greenworks Utils APIs implmentation.
greenworks.Utils.move = function(source_dir, target_dir) {
  fs.rename(source_dir, target_dir);
}

module.exports = greenworks;
