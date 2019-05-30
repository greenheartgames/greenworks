var message = '';

function log(msg) {
  message = message + msg + '<br>';
  var logs = document.getElementById('logs');
  if (!logs) {
    logs = document.createElement('div');
    logs.setAttribute("id", "logs");
    logs.setAttribute("style", "width: 70%; border: 1px dashed #ccc; padding: 3px; margin-top:10px;");
    document.body.appendChild(logs);
  }
  logs.innerHTML = message;
}

function testSteamAPI() {
  var os = require('os');
  var greenworks;
  try {
    // if greenworks is installed in a node_modules folder, this will work
    greenworks = require('greenworks');
  } catch(e) {
    greenworks = require('../../greenworks');
  }
  if (!greenworks) {
    log('Greenworks not support for ' + os.platform() + ' platform');
  } else {
    if (!greenworks.init()) {
      log('Error on initializing steam API.');
    } else {
      log('Steam API initialized successfully.');

      log('Cloud enabled: ' + greenworks.isCloudEnabled());
      log('Cloud enabled for user: ' + greenworks.isCloudEnabledForUser());

      greenworks.on('steam-servers-connected', function() { log('connected'); });
      greenworks.on('steam-servers-disconnected', function() { log('disconnected'); });
      greenworks.on('steam-server-connect-failure', function() { log('connected failure'); });
      greenworks.on('steam-shutdown', function() { log('shutdown'); });

      greenworks.saveTextToFile('test_file.txt', 'test_content',
          function() { log('Save text to file successfully'); },
          function(err) { log('Failed on saving text to file'); });

      greenworks.readTextFromFile('test_file.txt', function(message) {
          log('Read text from file successfully.'); }, function(err) {
          log('Failed on reading text from file'); });

      greenworks.getCloudQuota(
          function() { log('Getting cloud quota successfully.') },
          function(err) { log('Failed on getting cloud quota.') });
      // The ACH_WIN_ONE_GAME achievement is available for the sample (id:480) game
      greenworks.activateAchievement('ACH_WIN_ONE_GAME',
          function() { log('Activating achievement successfully'); },
          function(err) { log('Failed on activating achievement.'); });

      greenworks.getNumberOfPlayers(
          function(a) { log("Number of players " + a) },
          function(err) { log ('Failed on getting number of players'); });

      log("Numer of friends: " +
          greenworks.getFriendCount(greenworks.FriendFlags.Immediate));
      var friends = greenworks.getFriends(greenworks.FriendFlags.Immediate);
      var friends_names = [];
      for (var i = 0; i < friends.length; ++i)
        friends_names.push(friends[i].getPersonaName());
      log("Friends: [" + friends_names.join(',') + "]");
    }
  }
}

document.addEventListener('DOMContentLoaded', function() { testSteamAPI() });
