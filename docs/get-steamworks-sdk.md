# Get Steamworks SDK

Steamworks SDK is required when building Greenworks and using Greenworks.
Greenworks doesn't provide Steamworks SDK because it is not legal to package the
Steamworks SDK. Game developers have to accept Valve's terms and download it
from official site.

## Steamworks SDK in Greenworks Building:

1. [Download it from Steam](https://partner.steamgames.com/downloads/list) (
You will need to login with your Steam account first in order to access the
downloads list).
2. Extract the contents of the zip file.
3. The extracted contents will contain one subdirectory, `sdk`. Rename it to
`steamworks_sdk`.
4. Copy the renamed directory `steamworks_sdk` to `<greenworks_src_dir>/deps/`.
