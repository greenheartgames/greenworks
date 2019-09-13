## Methods

### greenworks.getDLCCount()

Returns the number of DLC pieces for the current running app.

### greenworks.getDLCDataByIndex(index)

* `index` Integer: The index of the DLC between 0 and `greenworks.getDLCCount`.

Returns `undefined` if no DLC matching the index could be found, otherwise an object containing:
* `appId` Integer: The APPID of the DLC.
* `available` Boolean: Flag whether the DLC is currently available.
* `name` String: The name of the DLC.

### greenworks.isDLCInstalled(dlc_app_id)

* `dlc_app_id` Integer: The APPID of a DLC.

Checks if the user owns the DLC and if the DLC is installed.

### greenworks.installDLC(dlc_app_id)

* `dlc_app_id` Integer: The APPID of a DLC.

Install a specific DLC.

### greenworks.uninstallDLC(dlc_app_id)

* `dlc_app_id` Integer: The APPID of a DLC.

Uninstall a specific DLC.
