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

// Publishing user generated content(ugc) to Steam contains following steps:
// 1. Save file and image to Steam Cloud.
// 2. Share the file and image.
// 3. publish the file to workshop.
greenworks.ugcPublish = function(file_name, title, description, image_name,
    success_callback, error_callback, progress_callback) {
  var files = [file_name];
  if (image_name)
    files.push(image_name);

  var error_process = function(err) {
    if (err && error_callback)
        error_callback(err);
  };
  var publish_file_process = function() {
    if (progress_callback)
      progress_callback("Completed on sharing files.");
    greenworks.publishWorkshopFile(file_name, image_name, title, description,
        function(publish_file_id) { success_callback(publish_file_id); },
        function(err) { error_process(err); });
  };
  var file_share_process = function() {
    if (progress_callback)
      progress_callback("Completed on saving files on Steam Cloud.");
    greenworks.fileShare(file_name, function() {
      if (image_name) {
        greenworks.fileShare(image_name, function() { publish_file_process(); },
            function(err) { error_process(err); });
        return;
      }
      publish_file_process();
    }, function(err) { error_process(err); });
  };
  greenworks.saveFilesToCloud(files, function() { file_share_process(); },
      function(err) { error_callback(err); });
}

// Greenworks Utils APIs implmentation.
greenworks.Utils.move = function(source_dir, target_dir) {
  fs.rename(source_dir, target_dir);
}

module.exports = greenworks;
