## Methods

### greenworks.getAuthSessionTicket(success_callback, [error_callback])

* `success_callback` Function(ticket)
  * `ticket` Object: Contains the hex encoded session `ticket` value as well as
    the `handle` integer value.
* `error_callback` Function(err)

The hex encoded `ticket` value can be used directly for the Web API
`ISteamUserAuth/AuthenticateUserTicket` to securely obtain an authenticated
Steam ID from your game server. The `handle` is needed to invalidate the ticket
after if it has not been used.

### greenworks.cancelAuthTicket(ticket_handle)

* `ticket_handle` Integer: The `handle` value returned from the ticket.

Invalidates a requested session ticket.

### greenworks.getEncryptedAppTicket(user_data, success_callback, [error_callback])

* `user_data` String: Arbitrary data that will be encrypted into the ticket.
  This will be utf-8 encoded when stored in the ticket.
* `success_callback` Function(ticket)
  * `ticket` String: Contains the hex encoded encrypted ticket.
* `error_callback` Function(err)

Encrypted tickets can be used to obtain authenticated Steam IDs from clients
without requiring network requests to Steam's API servers. These tickets can be
decrypted using your Encrypted App Ticket Key. Once decrypted, the user's
Steam ID, App ID, and VAC ban status can be read from the ticket using the
Steamworks Encrypted App Ticket library provided in the SDK.
