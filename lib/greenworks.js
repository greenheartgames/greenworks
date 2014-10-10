var fs = require('fs');
var greenworks;

if (process.platform == 'darwin') {
  if (process.arch == 'x64')
    greenworks = require('../build/Release/greenworks-osx64');
  else if (process.arch == 'ia32')
    greenworks = require('../build/Release/greenworks-osx32');
} else if (process.platform == 'win32') {
  if (process.arch == 'x64')
    greenworks = require('../build/Release/greenworks-win64');
  else if (process.arch == 'ia32')
    greenworks = require('../build/Release/greenworks-win32');
} else if (process.platform == 'linux') {
  if (process.arch == 'x64')
    greenworks = require('../build/Release/greenworks-linux64');
  else if (process.arch == 'ia32')
    greenworks = require('../build/Release/greenworks-linux32');
}

// Greenworks Utils APIs implmentation.
greenworks.Utils.move = function(source_dir, target_dir) {
  fs.rename(source_dir, target_dir);
}

module.exports = greenworks;
