## Methods

### greenworks.saveTextToFile(file_name, file_content, success_callback, [error_callback])

* `file_name` String
* `file_content` String
* `success_callback` Function()
* `error_callback` Function(err)

### greenworks.readTextFromFile(file_name, success_callback, [error_callback])

* `file_name` String
* `success_callback` Function(file_content)
  * `file_content` String: represents the content of `file_name` file.
* `error_callback` Function(err)

### greenworks.deleteFile(file_name, success_callback, [error_callback])

* `file_name` String
* `success_callback` Function()
* `error_callback` Function(err)

### greenworks.saveFilesToCloud(files_path, success_callback, [error_callback])

* `files_path` Array of String: The files' path on local machine.
* `success_callback` Function()
* `error_callback` Function(err)

Writes mutilple local files to Steam Cloud.

### greenworks.isCloudEnabledForUser()

Returns a `Boolean` indicates whether cloud is enabled in general for the
current Steam account.

### greenworks.isCloudEnabled()

Returns a `Boolean` indicates whether cloud is enabled for the current app.
This might return `true` independently of `greenworks.isCloudEnabledForUser()`.
Keep in mind that the general account setting has priority over the app specific
setting. So you might want to check `isCloudEnabledForUser()` first.

### greenworks.enableCloud(flag)

* `flag` Boolean

Enables/Disables the cloud feature for the current app. Keep in mind that your
app won't sync anything to the user's cloud if he disabled it at top level
(see `greenworks.isCloudEnabledForUser()`).

### greenworks.getCloudQuota(success_callback, [error_callback])

* `success_callback` Function(total_bytes, available_bytes)
  * `total_bytes` uint64 String: total bytes of quota
  * `available_bytes` uint64 String: available bytes of quota
* `error_callback` Function(err)

### greenworks.getFileCount()

Gets the number of files on the cloud.

### greenworks.getFileNameAndSize(index)

* `index` Integer: the index of the file

Returns an `Object`:

* `name` String: The file name
* `size` Integer: The file size
