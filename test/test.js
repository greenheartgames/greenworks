var assert = require("assert")
var os = require('os')
var steam;

if (os.platform() == 'darwin')
  steam = require('../build/release/greenworks-osx');
else if (os.platform() == 'win32 ' || os.platform() == 'win64')
  steam = require('../build/release/greenworks-win');
else if (os.platform() == 'linux') {
  if (os.arch() == 'x64')
    steam = require('../build/release/greenworks-linux64');
  else if (os.arch() == 'ia32')
    steam = require('../build/release/greenworks-linux32');
} else {
  console.log("Greenworks not support for " + os.platform() + " platform");
  process.exit(1);
}

describe('greenworks API', function() {
  if (!steam.initAPI()) {
    console.log('An error occured initializing Steam API.');
    process.exit(1);
  }

  describe('saveTextToFile', function() {
    it('Should save successfully.', function(done) {
      steam.saveTextToFile('test_file.txt', 'test_content',
          function() { done(); }, function(err) { throw err; done(); });
    });
  });

  describe('readTextFromFile', function() {
    it('Should read successfully.', function(done) {
      steam.readTextFromFile('test_file.txt', function(message) {
          assert(message == 'test_content'); done(); }, function(err) {
          throw err; done(); });
    });

    it('Should read failed.', function(done) {
      steam.readTextFromFile('not_exist.txt', function(message) {
       throw 'Error'; done() }, function(err) { done(); });
    })
  });

  describe('enableCloud&isCloudEnabled', function() {
    it('', function() {
       steam.enableCloud(false);
       assert.equal(false, steam.isCloudEnabled());
       steam.enableCloud(true);
       assert.equal(true, steam.isCloudEnabled());
    });
  });

  describe('getCloudQuota', function() {
    it('Should get successfully', function(done) {
      steam.getCloudQuota(function(total, avail) { done(); },
                          function(err) { throw err; });
    });
  });

  describe('activateAchievement', function() {
    it('Should get successfully', function(done) {
      steam.activateAchievement('achievement',
                                function() { done(); },
                                function(err) { throw err; });
    });
  });

  describe('Game setting', function() {
    it('Should get successfully', function() {
      console.log(steam.getCurrentGameLanguage());
      console.log(steam.getCurrentUILanguage());
      console.log(steam.getCurrentGameInstallDir());
    });
  });

});
