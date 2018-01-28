## Objects

### greenworks.UGCMatchingType

Represents Steam SDK `EUGCMatchingUGCType`, matching UGC types for queries.

* `Items`
* `ItemsMtx`
* `ItemsReadyToUse`
* `Collections`
* `Artwork`
* `Videos`
* `Screenshots`
* `AllGuides`
* `WebGuides`
* `IntegratedGuides`
* `UsableInGame`
* `ControllerBindings`

### greenworks.UGCQueryType

Represents Steam SDK `EUGCQuery`, combination of sorting and filtering for queries across all UGC.

* `RankedByVote`
* `RankedByPublicationDate`
* `AcceptedForGameRankedByAcceptanceDate`
* `RankedByTrend`
* `FavoritedByFriendsRankedByPublicationDate`
* `CreatedByFriendsRankedByPublicationDate`
* `RankedByNumTimesReported`
* `CreatedByFollowedUsersRankedByPublicationDate`
* `NotYetRated`
* `RankedByTotalVotesAsc`
* `RankedByVotesUp`
* `RankedByTextSearch`

### SteamUGCDetails

Represents Steam SDK `SteamUGCDetails_t`(return by `greenworks.ugcGetItems`), details for a single published file/UGC

* `acceptForUse` Boolean: Whether is flaged as accepted in Steam workshop.
* `banned` Boolean: Whether is banned
* `tagsTruncated` Boolean: Whether the list of tags is too long to be returned in provided buffer.
* `fileType` Integer: Type of the file
* `result` Integer: Result of the operation.
   * `1`: Success
   * `Others`: Fail
* `visibility` Integer: the visiblility of the file
  * `0`: Public
  * `1`: FriendsOnly
  * `2`: Private
* `score` Double: Calculated score
* `file` String: Represents uint64, file handle
* `fileName` String: Cloud file name of the primary file
* `fileSize` Integer: Size of the primary file
* `previewFile` String: Represents uint64, handle of preview file
* `previewFileSize` Integer: Size of preview file
* `steamIDOwner` String: Represents uint64, Steam ID of user who created the file.
* `consumerAppID` Integer: ID of app that consumes the file
* `creatorAppID` Integer: ID of app that created the file
* `publishedFileId` String: Represents uint64, the file ID
* `title` String: Title of the file
* `description` String: Description of the file
* `URL` String:
* `tags` String: List of tags, separated by comma.
* `timeAddedToUserList` Integer: Time when user added this file to list.
* `timeCreated` Integer: Time when the file was created
* `timeUpdated` Integer: Time when the file is last updated.
* `votesDown` Integer: Number of votes down
* `votesUp` Integer: Number of votes up

### greenworks.UserUGCList

Represents Steam SDK `EUserUGCList`, different lists of published UGC for a user.

* `Published`
* `VotedOn`
* `VotedUp`
* `VotedDown`
* `WillVoteLater`
* `Favorited`
* `Subscribed`
* `UsedOrPlayer`
* `Followed`

### greenworks.UserUGCListSortOrder

Represents Steam SDK `EUserUGCListSortOrder`, sort order for user published UGC lists

* `CreationOrderDesc`
* `CreationOrderAsc`
* `TitleAsc`
* `LastUpdatedDesc`
* `SubscriptionDateDesc`
* `VoteScoreDesc`
* `ForModeration`

## Methods

### greenworks.fileShare(file_path, success_callback, [error_callback])

* `file_path` String
* `success_calback` Function(file_handle)
  * `file_handle` String: Represents uint64, the file handle that can be shared
    with users and features.
* `error_callback` Function(err)

### greenworks.publishWorkshopFile(file_path, image_path, title, description, success_callback, [error_callback])

* `file_path` String
* `image_path` String
* `title` String
* `description` String
* `success_callback` Function(publish_file_handle)
  * `publish_file_handle` String: Represents uint64, the published file handle
* `error_callback` Function(err)

Publishes `file_path` workshop item on Steam. The files `file_path` and
`image_path` need to be existed on Steam Cloud (Using `writeTextToFile` or
`saveFilesToCloud` API) and get shared (Using `Greenworks.fileShare`) first.

An empty String of `image_path` means no image for the workshp item.

### greenworks.updatePublishedWorkshopFile(published_file_handle, file_path, image_path, title, description, success_callback, [error_callback])

* `published_file_handle` String: Represents uint64, the published file handle.
* `file_path` String
* `image_path` String
* `title` String
* `description` String
* `success_callback` Function()
* `error_callback` Function(err)

An empty String of `file_path`/`image_path`/`title`/`description` means no update of that field.

### greenworks.ugcPublish(file_path, title, description, image_path, success_callback, [error_callback], [progress_callback])

* `file_path` String
* `title` String
* `description` String
* `image_path` String
* `success_callback` Function(published_file_handle)
   * `published_file_handle` String: Represents uint64, the published file handle
* `error_callback` Function(err)
* `progress_callback` Function(progress_msg)
   * `progress_msg` String: current process during publish period:
     `Completed on saving files to Steam Cloud`, `Completed on sharing files`.

Publishes user generated content(ugc) to Steam workshop.

### greenworks.ugcPublishUpdate(published_file_handle, file_path, title, description, image_path, success_callback, [error_callback], [progress_callback])

* `published_file_handle` String: Represents uint64, the published file handle
* `file_path` String
* `title` String
* `description` String
* `image_path` String
* `success_callback` Function()
* `error_callback` Function(err)
* `progress_callback` Function(progress_msg)
   * `progress_msg` String: current process during publish-update period:
     `Completed on saving files to Steam Cloud`, `Completed on sharing files`.

Updates published ugc.

### greenworks.ugcGetItems(ugc_matching_type, ugc_query_type, unPage, success_callback, [error_callback])

* `ugc_matching_type` greenworks.UGCMatchingType
* `ugc_query_type` greenworks.UGCQueryType
* `unPage` Integer: The page number of the results to receive
* `success_callback` Function(items)
  * `items` Array of `SteamUGCDetails` Object
* `error_callback` Function(err)

### greenworks.ugcGetUserItems(ugc_matching_type, ugc_list_sort_order, ugc_list, unPage, success_callback, [error_callback])

* `ugc_matching_type` greenworks.UGCMatchingType
* `ugc_list_sort_order` greenworks.UserUGCListSortOrder
* `ugc_list` greenworks.UserUGCList
* `unPage` Integer: The page number of the results to receive
* `success_callback` Function(items)
  * `items` Array of `SteamUGCDetails` Object
* `error_callback` Function(err)

### greenworks.ugcDownloadItem(download_file_handle, download_dir, success_callback, [error_callback])

* `download_file_handle` String: Represents uint64, the download file handle
* `download_dir` String: The file path saving the download file on local machine
* `success_callback` Function()
* `error_callback` Function(err)

### greenworks.ugcSynchronizeItems(sync_dir, success_callback, [error_callback])

* `sync_dir` String: The directory to download the sync workshop item.
* `success_callback` Function(items)
  * `items` Array of Object
     * `SteamUGCDetails`
     * `isUpdated` Boolean: Whether the item is updated in this function call
* `error_callback` Function(err)

Downloads/Synchronizes user's workitems(`UserUGCList.Subscribed`,
`UserMatchingType.Items`) to the local `sync_dir` (Only updated if the last
updated time of the item is different with Steam Cloud or the workitem isn't
existed in local).

### greenworks.ugcUnsubscribe(published_file_handle, success_callback, [error_callback])

* `published_file_handle` String: Represent uint64, the file handle of
  unsubscribed published workshp item(SteamUGCDetails.publishedFileId).
* `success_callback` Function()
* `error_callback` Function(err)

### greenworks.ugcShowOverlay([published_file_id])

* `published_file_id` String: Represent uint64, the id of published file.

Shows the Steam overlay pointed to Steam's workshop page or to the specified
workshop item.
