var message = '';

function log(msg) {
  message = message + msg + '<br>';
  document.getElementById('logs').innerHTML = message;
}

function testSteamAPI() {
  var os = require('os')
  var steam;
  if (os.platform() == 'darwin')
    steam = require('../../build/release/greenworks-osx');
  else if (os.platform() == 'win32 ' || os.platform() == 'win64')
    steam = require('../../build/release/greenworks-win');
  else if (os.platform() == 'linux') {
    if (os.arch() == 'x64')
      steam = require('../../build/release/greenworks-linux64');
    else if (os.arch() == 'ia32')
      steam = require('../../build/release/greenworks-linux32');
  }
  if (!steam) {
    log('Greenworks not support for ' + os.platform() + ' platform');
  } else {
    if (!steam.initAPI()) {
      log('Error on initializing steam API.');
    } else {
      log('Steam API initialized successfully.');

      steam.saveTextToFile('test_file.txt', 'test_content',
          function() { log('Save text to file successfully'); },
          function(err) { log('Failed on saving text to file'); });

      steam.readTextFromFile('test_file.txt', function(message) {
          log('Read text from file successfully.'); }, function(err) {
          log('Failed on reading text from file'); });

      steam.getCloudQuota(
          function() { log('Getting cloud quota successfully.') },
          function(err) { log('Failed on getting cloud quota.') });

      steam.activateAchievement('achievement',
          function() { log('Activating achievement successfully'); },
          function(err) { log('Failed on activating achievement.'); });
      log('Cloud Enabled: ' + steam.isCloudEnabled());
    }
  }
}

document.addEventListener('DOMContentLoaded', function() { testSteamAPI() });
