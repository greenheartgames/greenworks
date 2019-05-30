var message = '';

function log(msg) {
  message = message + msg + '<br>';
  document.getElementById('logs').innerHTML = message;
}

function testSteamAPI() {
  var os = require('os');
  var greenworks = require('../../greenworks.js');
  if (!greenworks) {
    log('Greenworks not support for ' + os.platform() + ' platform');
  } else {
    if (!greenworks.init()) {
      log('Error on initializing steam API.');
    } else {
      log('Steam API initialized successfully.');

      log('Cloud enabled: ' + greenworks.isCloudEnabled());
      log('Cloud enabled for user: ' + greenworks.isCloudEnabledForUser());

      greenworks.on('game-overlay-activated', function(is_active) { log('overlay active: '+is_active); });
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
          function(quota, used) { log('Getting cloud quota successfully: '+used+' of '+quota) },
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

var fps = 30;
function forceRefresh() {
  // this function updates the renderer at a given frame rate, even if the user is idle.
  // without this function, the Steam overlay would feel like "frozen".
  setTimeout(function() {
    document.getElementById("forceRefresh").getContext("2d").clearRect(0, 0, 1, 1);
    window.requestAnimationFrame(forceRefresh);
  }, 1000 / fps);
}
