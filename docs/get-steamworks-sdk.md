# The Steamworks SDK

* The Steamworks SDK is required when building Greenworks and using Greenworks.
We can't just include this within the repository because it is not legal to do
so; game developers have to accept Valve's terms and download it from the
official site.
* Greenworks is designed to function with v1.41 of the SDK, which is the latest
version of the time of this writing. However, every so often, Steam releases a
new version of the SDK, so make sure you download the appropriate version.

## Give It to Greenworks

1. [Download the SDK from the official Steamworks website](https://partner.steamgames.com/downloads/list).
(You will need to login with your Steam account first in order to access the
downloads list.)
2. Extract the contents of the zip file.
3. The extracted contents will contain one subdirectory, `sdk`. Rename it to
`steamworks_sdk`.
4. Copy the renamed directory `steamworks_sdk` to `<greenworks_src_dir>/deps/`.
Alternatively, you can also set a `STEAMWORKS_SDK_PATH` environment variable
which points to the `steamworks_sdk` directory.
