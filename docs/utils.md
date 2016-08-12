## Methods

### greenworks.Utils.move(source_dir, target_dir, [success_callback], [error_callback])

* `source_dir` String
* `target_dir` String
* `success_callback` Function()
* `error_callback` Function(err)

Moves `source_dir` to `target_dir`.

### greenworks.Utils.createArchive(zip_file_path, source_dir, password, compress_level, success_callback, [error_callback])

* `zip_file_path` String
* `source_dir` String
* `password` String: Empty represents no password
* `compress_level` Integer: Compress factor 0-9, store only - best compressed.
* `success_callback` Function()
* `error_callback` Function(err)

Creates a zip archive of `source_dir`.

### greenworks.Utils.extractArchive(zip_file_path, extract_dir, password, success_callback, [error_callback])

* `zip_file_path` String
* `extract_dir` String
* `password` String: Empty represents no password
* `success_callback` Function()
* `error_callback` Function(err)

Extracts the `zip_file_path` to the specified `extract_dir`.
