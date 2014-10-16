var archiver = require('archiver');
var fs = require('fs');
var unzip = require('unzip')

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

function error_process(err, error_callback) {
  if (err && error_callback)
    error_callback(err);
}

// An utility function for publish related APIs.
// It processes remains steps after saving files to Steam Cloud.
function file_share_process(file_name, image_name, next_process_func,
    error_callback, progress_callback) {
  if (progress_callback)
    progress_callback("Completed on saving files on Steam Cloud.");
  greenworks.fileShare(file_name, function() {
    greenworks.fileShare(image_name, function() {
      next_process_func();
    }, function(err) { error_process(err, error_callback); });
  }, function(err) { error_process(err, error_callback); });
}

// Publishing user generated content(ugc) to Steam contains following steps:
// 1. Save file and image to Steam Cloud.
// 2. Share the file and image.
// 3. publish the file to workshop.
greenworks.ugcPublish = function(file_name, title, description, image_name,
    success_callback, error_callback, progress_callback) {
  var publish_file_process = function() {
    if (progress_callback)
      progress_callback("Completed on sharing files.");
    greenworks.publishWorkshopFile(file_name, image_name, title, description,
        function(publish_file_id) { success_callback(publish_file_id); },
        function(err) { error_process(err, error_callback); });
  };
  greenworks.saveFilesToCloud([file_name, image_name], function() {
    file_share_process(file_name, image_name, publish_file_process,
        error_callback, progress_callback);
  }, function(err) { error_process(err, error_callback); });
}

// Update publish ugc steps:
// 1. Save new file and image to Steam Cloud.
// 2. Share file and images.
// 3. Update published file.
greenworks.ugcPublishUpdate = function(published_file_id, file_name, title,
    description, image_name, success_callback, error_callback,
    progress_callback) {
  var update_published_file_process = function() {
    if (progress_callback)
      progress_callback("Completed on sharing files.");
    greenworks.updatePublishedWorkshopFile(published_file_id,
        file_name, image_name, title, description,
        function() { success_callback(); },
        function(err) { error_process(err, error_callback); });
  };

  greenworks.saveFilesToCloud([file_name, image_name], function() {
    file_share_process(file_name, image_name, update_published_file_process,
        error_callback, progress_callback);
  }, function(err) { error_process(err, error_callback); });
}

// Greenworks Utils APIs implmentation.
greenworks.Utils.move = function(source_dir, target_dir) {
  fs.rename(source_dir, target_dir);
}

greenworks.Utils.createArchive = function(zip_path, source_dir,
    success_callback, error_callback) {
  var output = fs.createWriteStream(zip_path);
  var archive = archiver('zip');
  output.on('close', success_callback);
  if (error_callback)
    archive.on('error', error_callback);

  archive.pipe(output);

  // Replace '\' with '/'
  source_dir = source_dir.replace(/\\/g, '/');
  // Remove last '/' character.
  if (source_dir[source_dir.length-1] == '/')
    source_dir = source_dir.slice(0, -1);
  var cwd_path = source_dir.substr(0, source_dir.lastIndexOf('/'));
  var cur_dir = source_dir.substr(source_dir.lastIndexOf('/')+1);

  archive.bulk([{ expand: true, cwd: cwd_path, src: [cur_dir+'/*'] }]);
  archive.finalize();
}

greenworks.Utils.extractArchive = function(zip_file_path, extract_dir,
    success_callback, error_callback) {
  var unzip_extractor = unzip.Extract({ path: extract_dir });
  if (error_callback)
    unzip_extractor.on('error', error_callback);
  unzip_extractor.on('close', success_callback);
  fs.createReadStream(zip_file_path).pipe(unzip_extractor);
}

module.exports = greenworks;
