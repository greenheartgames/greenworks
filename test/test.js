var assert = require("assert")
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
} else {
  console.log("Greenworks not support for " + os.platform() + " platform");
  process.exit(1);
}

describe('greenworks API', function() {
  if (!greenworks.initAPI()) {
    console.log('An error occured initializing Steam API.');
    process.exit(1);
  }

  describe('saveTextToFile', function() {
    it('Should save successfully.', function(done) {
      greenworks.saveTextToFile('test_file.txt', 'test_content',
          function() { done(); }, function(err) { throw err; done(); });
    });
  });

  describe('readTextFromFile', function() {
    it('Should read successfully.', function(done) {
      greenworks.readTextFromFile('test_file.txt', function(message) {
          assert(message == 'test_content'); done(); }, function(err) {
          throw err; done(); });
    });

    it('Should read failed.', function(done) {
      greenworks.readTextFromFile('not_exist.txt', function(message) {
       throw 'Error'; done() }, function(err) { done(); });
    })
  });

  describe('enableCloud&isCloudEnabled', function() {
    it('', function() {
       greenworks.enableCloud(false);
       assert.equal(false, greenworks.isCloudEnabled());
       greenworks.enableCloud(true);
       assert.equal(true, greenworks.isCloudEnabled());
    });
  });

  describe('getCloudQuota', function() {
    it('Should get successfully', function(done) {
      greenworks.getCloudQuota(function(total, avail) { done(); },
                          function(err) { throw err; });
    });
  });

  describe('activateAchievement', function() {
    it('Should get successfully', function(done) {
      greenworks.activateAchievement('achievement',
                                function() { done(); },
                                function(err) { throw err; });
    });
  });

  describe('Output Steam APIs calling result', function() {
    it('Should be called successfully', function() {
      console.log(greenworks.getCurrentGameLanguage());
      console.log(greenworks.getCurrentUILanguage());
      console.log(greenworks.getCurrentGameInstallDir());
      console.log(greenworks.getSteamId());
    });
  });

});
