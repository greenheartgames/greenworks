var message = '';

function log(msg) {
  message = message + msg + '<br>';
  document.getElementById('logs').innerHTML = message;
}

function testSteamAPI() {
  var os = require('os')
  var greenworks = require('../../lib/greenworks');
  if (!greenworks) {
    log('Greenworks not support for ' + os.platform() + ' platform');
  } else {
    if (!greenworks.initAPI()) {
      log('Error on initializing steam API.');
    } else {
      log('Steam API initialized successfully.');

      greenworks.saveTextToFile('test_file.txt', 'test_content',
          function() { log('Save text to file successfully'); },
          function(err) { log('Failed on saving text to file'); });

      greenworks.readTextFromFile('test_file.txt', function(message) {
          log('Read text from file successfully.'); }, function(err) {
          log('Failed on reading text from file'); });

      greenworks.getCloudQuota(
          function() { log('Getting cloud quota successfully.') },
          function(err) { log('Failed on getting cloud quota.') });

      greenworks.activateAchievement('achievement',
          function() { log('Activating achievement successfully'); },
          function(err) { log('Failed on activating achievement.'); });
      log('Cloud Enabled: ' + greenworks.isCloudEnabled());
    }
  }
}

document.addEventListener('DOMContentLoaded', function() { testSteamAPI() });

