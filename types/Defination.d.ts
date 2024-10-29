/**好友标识位*/
export const enum FriendFlags {
  /**无。*/
  None = 0,
  /**当前用户已阻止联系的用户。*/
  Blocked = 1,
  /**已向当前用户发送好友邀请的用户。*/
  FriendshipRequested = 2,
  /**当前用户的“常规”朋友。*/
  Immediate = 4,
  /**与当前用户属于同一Steam（小）组之一的用户。*/
  ClanMember = 8,
  /**在同一游戏服务器上的用户；如SetPlayedWith所设置。*/
  OnGameServer = 16,
  /**当前用户已向其发送好友邀请的用户。*/
  RequestingFriendship = 128,
  /**在调用请求指定用户的昵称及头像接口后，当前正在发送有关自己的其他信息的用户*/
  RequestingInfo = 256,
  /**当前用户已忽略与他们联系的用户。*/
  Ignored = 512,
  /**忽略当前用户的用户；但是当前用户仍然知道它们。*/
  IgnoredFriend = 1024,
  /**同一聊天中的用户。*/
  ChatMember = 4096,
  /**返回所有好友标志。*/
  All = 65535,
}

/**声明Steam用户可能拥有的一组关系。*/
export const enum FriendRelationship {
  /**没有关系的用户*/
  None = 0,
  /**用户刚刚点击了好友邀请上的忽略。这不会被存储*/
  Blocked = 1,
  /**已经申请与当前用户成为朋友的用户*/
  RequestRecipient = 2,
  /**"常规"好友*/
  Friend = 3,
  /**当前用户已经发送好友申请的用户*/
  RequestInitiator = 4,
  /**当前用户已经明确屏蔽此用户发表的评论、聊天等。这是会被存储的*/
  Ignored = 5,
  /**已经屏蔽当前用户的用户*/
  IgnoredFriend = 6,
  /**已废弃-未使用*/
  Suggested = 7,
}

/**它描述了客户端最近了解到发生了哪些变化，所以在启动时，你会看到每个好友的名字、头像以及好友关系的变化。*/
export const enum PersonaChange {
  /**名字*/
  Name = 1,
  /**状态*/
  Status = 2,
  /**上线*/
  ComeOnline = 4,
  /**离线*/
  GoneOffline = 8,
  /**玩过的游戏*/
  GamePlayed = 16,
  /**游戏服务器*/
  GameServer = 32,
  /**头像*/
  Avatar = 64,
  /**加入源*/
  JoinedSource = 128,
  /**离开源*/
  LeftSource = 256,
  /**关系变化*/
  RelationshipChanged = 512,
  /**第一次设置名字*/
  NameFirstSet = 1024,
  /**昵称*/
  NickName = 4096,
  /**steam等级*/
  SteamLevel = 8192,
}

/**账户类型*/
export const enum AccountType {
  /**0.用于无效 Steam ID*/
  Invalid = 0,
  /**1.普通用户帐户*/
  Individual = 1,
  /**2.多座位（如网吧）帐户*/
  Multiseat = 2,
  /**3.持久（非匿名）游戏服务器帐户*/
  GameServer = 3,
  /**4.匿名游戏服务器帐户*/
  AnonymousGameServer = 4,
  /**5.待处理*/
  Pending = 5,
  /**6.Valve 内部内容服务器帐户*/
  ContentServer = 6,
  /**7.Steam 组（群组）*/
  Clan = 7,
  /**8.Steam 组聊天或大厅*/
  Chat = 8,
  /**9.虚拟 Steam ID，用于 PS3 本地 PSN 帐户或 360 Live 帐户等*/
  ConsoleUser = 9,
  /**10.匿名用户帐户 （用来创建帐户或重置密码）*/
  AnonymousUser = 10
}

/**聊天消息类型*/
export const enum ChatEntryType {
  /**0.无效。*/
  Invalid,
  /**1.来自其他用户的普通文本消息。*/
  ChatMsg = 1,
  /**2.另一用户正在输入，不用于多用户群聊。*/
  Typing = 2,
  /**3.其他用户邀请进入当前进行的游戏。*/
  InviteGame = 3,
  /**4.文本表情包（已弃用，应视作 ChatMsg 处理）。*/
  Emote = 4,
  /**6.一名用户离开了对话（关闭了对话窗口）。*/
  LeftConversation = 6,
  /**7.用户加入聊天，用于多用户群聊与组聊天。*/
  Entered = 7,
  /**8.用户被踢出（数据：执行踢出操作的用户的 Steam ID）。*/
  WasKicked = 8,
  /**9.用户被封（数据：执行封禁操作的用户的 Steam ID）。*/
  WasBanned = 9,
  /**10.用户掉线。*/
  Disconnected = 10,
  /**11.来自用户的聊天历史记录的聊天消息或离线消息。*/
  HistoricalChat = 11,
  /**14.聊天过滤器移除了一个链接。*/
  LinkBlocked = 14
}

/**Steam 错误结果代码。*/
export const enum Result {
  /**成功。*/
  OK = 1,
  /**一般失败。*/
  Fail = 2,
  /**您的 Steam 客户端没有连接后端。*/
  NoConnection = 3,
  /**密码/票证无效。*/
  InvalidPassword = 5,
  /**用户在别处登录。*/
  LoggedInElsewhere = 6,
  /**协议版本错误。*/
  InvalidProtocolVer = 7,
  /**参数不正确。*/
  InvalidParam = 8,
  /**未找到文件。*/
  FileNotFound = 9,
  /**调用的方法繁忙，未进行操作。*/
  Busy = 10,
  /**调用的对象处于无效状态。*/
  InvalidState = 11,
  /**  名称无效。*/
  InvalidName = 12,
  /**电子邮件无效。*/
  InvalidEmail = 13,
  /**名称不是唯一的。*/
  DuplicateName = 14,
  /**访问被拒绝。*/
  AccessDenied = 15,
  /**操作超时。*/
  Timeout = 16,
  /**用户受到 VAC 封禁。*/
  Banned = 17,
  /**未找到帐户。*/
  AccountNotFound = 18,
  /**Steam ID 无效。*/
  InvalidSteamID = 19,
  /**请求的服务当前不可用。*/
  ServiceUnavailable = 20,
  /**用户未登录。*/
  NotLoggedOn = 21,
  /**请求正待处理，可能已在进程中或在等待第三方。*/
  Pending = 22,
  /**加密或解密失败。*/
  EncryptionFailure = 23,
  /**权限不足。*/
  InsufficientPrivilege = 24,
  /**过量。*/
  LimitExceeded = 25,
  /**访问已被撤销（用于被撤销的玩家通行证）。*/
  Revoked = 26,
  /**用户尝试访问的许可/玩家通行证已失效。*/
  Expired = 27,
  /**玩家通行证已被帐户兑换，不能再次使用。*/
  AlreadyRedeemed = 28,
  /**重复请求，操作已进行，忽略此请求。*/
  DuplicateRequest = 29,
  /**用户已拥有此玩家通行证兑换请求中的所有游戏。*/
  AlreadyOwned = 30,
  /**未找到 IP 地址。*/
  IPNotFound = 31,
  /**将更改写入数据存储失败。*/
  PersistFailed = 32,
  /**为此操作获取访问锁定失败。*/
  LockingFailed = 33,
  /**登录会话已被替换。*/
  LogonSessionReplaced = 34,
  /**连接失败。*/
  ConnectFailed = 35,
  /**验证握手失败。*/
  HandshakeFailed = 36,
  /**出现一般 IO 失败。*/
  IOFailure = 37,
  /**远程服务器已断开连接。*/
  RemoteDisconnect = 38,
  /**未找到请求的购物车。*/
  ShoppingCartNotFound = 39,
  /**一个用户阻止了操作。*/
  Blocked = 40,
  /**目标忽略发送方。*/
  Ignored = 41,
  /**未找到与请求匹配的信息。*/
  NoMatch = 42,
  /**帐户被禁用。*/
  AccountDisabled = 43,
  /**此服务目前不接受内容更改。*/
  ServiceReadOnly = 44,
  /**帐户未充值，此功能不可用。*/
  AccountNotFeatured = 45,
  /**只有在管理员发出请求时才允许采用此操作。*/
  AdministratorOK = 46,
  /**在 Steam 协议中传输的内容版本不匹配。*/
  ContentVersion = 47,
  /**当前 CM 不能服务发出请求的用户。用户应重试。*/
  TryAnotherCM = 48,
  /**您已经在别处登录，此缓存的凭据登录已失效。*/
  PasswordRequiredToKickSession = 49,
  /**用户在别处登录。 （应换用 k_EResultLoggedInElsewhere！）*/
  AlreadyLoggedInElsewhere = 50,
  /**长时间运行的操作已中止/暂停。 （如 内容下载。)*/
  Suspended = 51,
  /**操作被取消，通常是被用户取消。 （如 内容下载。)*/
  Cancelled = 52,
  /**因数据状况欠佳或不可恢复而取消操作。*/
  DataCorruption = 53,
  /**磁盘空间不足，取消操作。*/
  DiskFull = 54,
  /**远程或 IPC 调用失败。*/
  RemoteCallFailed = 55,
  /**密码在服务器端未设置，无法验证。*/
  PasswordUnset = 56,
  /**外部帐户（PSN、Facebook 等）未链接至 Steam 帐户。*/
  ExternalAccountUnlinked = 57,
  /**PSN 票证无效。*/
  PSNTicketInvalid = 58,
  /**外部帐户（PSN、Facebook 等）已链接至其他帐户，必须先显式请求替代/删除此链接。*/
  ExternalAccountAlreadyLinked = 59,
  /**由于本地与远程文件冲突，同步无法继续。*/
  RemoteFileConflict = 60,
  /**请求的新密码不被接受。*/
  IllegalPassword = 61,
  /**新值与旧值相同。 用于安全问题及其答案。*/
  SameAsPreviousValue = 62,
  /**由于第二步验证失败，帐户登录被拒绝。*/
  AccountLogonDenied = 63,
  /**请求的新密码不受认可。*/
  CannotUseOldPassword = 64,
  /**由于验证码无效，帐户登录被拒。*/
  InvalidLoginAuthCode = 65,
  /**由于第二步验证失败，帐户登录被拒，且未发送邮件。*/
  AccountLogonDeniedNoMail = 66,
  /**用户硬件不支持英特尔身份保护技术（IPT）。*/
  HardwareNotCapableOfIPT = 67,
  /**英特尔身份保护技术（IPT）初始化失败。*/
  IPTInitError = 68,
  /**由于当前用户受家长控制限制，操作失败。*/
  ParentalControlRestricted = 69,
  /**Facebook 查询返回错误。*/
  FacebookQueryError = 70,
  /**由于验证码失效，帐户登录被拒。*/
  ExpiredLoginAuthCode = 71,
  /**由于 IP 限制，登录失败。*/
  IPLoginRestrictionFailed = 72,
  /**当前用户帐户目前被锁定，无法使用。 通常是因为帐户劫持和待处理的所有权验证。*/
  AccountLockedDown = 73,
  /**由于帐户电子邮件未能验证，登录失败。*/
  AccountLogonDeniedVerifiedEmailRequired = 74,
  /**无与提供的值匹配的 URL。*/
  NoMatchingURL = 75,
  /**由于分析失败、字段遗失等导致错误响应。*/
  BadResponse = 76,
  /**用户不能完成操作，直到重新输入密码为止。*/
  RequirePasswordReEntry = 77,
  /**输入的值在可接收范围之外。*/
  ValueOutOfRange = 78,
  /**发生了完全不曾预料的情况。*/
  UnexpectedError = 79,
  /**请求的服务已配置为不可用。*/
  Disabled = 80,
  /**提交至 CEG 服务器的文件无效。*/
  InvalidCEGSubmission = 81,
  /**使用的设备未获执行此操作的许可。*/
  RestrictedDevice = 82,
  /**由于地区限制，操作不能完成。*/
  RegionLocked = 83,
  /**超出暂时性速率极限，稍后再试，与可能为永久性的 k_EResultLimitExceeded 不同。*/
  RateLimitExceeded = 84,
  /**需要双重验证码登录。*/
  AccountLoginDeniedNeedTwoFactor = 85,
  /**我们尝试访问的物品已被删除。*/
  ItemDeleted = 86,
  /**尝试登录失败，尝试阻止对可能的攻击者响应。*/
  AccountLoginDeniedThrottle = 87,
  /**双重验证（Steam 令牌）码不正确。*/
  TwoFactorCodeMismatch = 88,
  /**双因素验证（Steam 令牌）激活码不匹配。*/
  TwoFactorActivationCodeMismatch = 89,
  /**当前帐户已与多个合作伙伴相关联。*/
  AccountAssociatedToMultiplePartners = 90,
  /**数据尚未修改。*/
  NotModified = 91,
  /**帐户无相关联的移动设备。*/
  NoMobileDevice = 92,
  /**提供的时间在范围或容差之外。*/
  TimeNotSynced = 93,
  /**短信代码失败：无匹配、无一待处理等。*/
  SmsCodeFailed = 94,
  /**访问此资源的帐户过多。*/
  AccountLimitExceeded = 95,
  /**此帐户的更改过多。*/
  AccountActivityLimitExceeded = 96,
  /**此手机号码的更改过多。*/
  PhoneActivityLimitExceeded = 97,
  /**无法退款至原支付手段，必须使用 Steam 钱包。*/
  RefundToWallet = 98,
  /**无法发送电子邮件。*/
  EmailSendFailure = 99,
  /**在支付确认前无法执行操作。*/
  NotSettled = 100,
  /**用户需提供有效的 captcha。*/
  NeedCaptcha = 101,
  /**此令牌所有者拥有的一个游戏服务器登录令牌已被封禁。*/
  GSLTDenied = 102,
  /**游戏服务器所有者因其他原因被拒，如帐户锁定、社区封禁、VAC 封禁、遗失手机等。*/
  GSOwnerDenied = 103,
  /**我们收到请求进行操作的物品类型无效。*/
  InvalidItemType = 104,
  /**IP 地址被禁止采取此操作。*/
  IPBanned = 105,
  /**此游戏服务器登录令牌（GSLT）的停用已到期，可以重置再用。*/
  GSLTExpired = 106,
  /**用户无足够钱包资金完成操作。*/
  InsufficientFunds = 107,
  /**已有过多待处理物品。*/
  TooManyPending = 108,
}

/**对话框*/
export const enum Dialog {
  /** 打开界面网页浏览器，前往指定的用户或组资料。*/
  SteamId = 'steamid',
  /** 打开与指定用户的聊天窗口，或加入组聊天。*/
  Chat = 'chat',
  /** 打开以 ISteamEconomy/StartTrade Web API 开始的 Steam 交易会话窗口。*/
  JoinTrade = 'jointrade',
  /** 打开界面网页浏览器，前往指定用户的统计。*/
  Stats = 'stats',
  /** 打开界面网页浏览器，前往指定用户的成就。*/
  Achievements = 'achievements',
  /** 以最小模式打开界面，提示用户将目标用户加为好友。*/
  FriendAdd = 'friendadd',
  /** 以最小模式打开界面，提示用户移除目标好友。*/
  FriendRemove = 'friendremove',
  /** 以最小模式打开界面，提示用户接受传入的好友邀请。*/
  FriendRequestAccept = 'friendrequestaccept',
  /** 以最小模式打开界面，提示用户忽略传入的好友邀请。*/
  FriendRequestIgnore = 'friendrequestignore',
}

/**大厅搜索筛选器选项。 这些可通过 AddRequestLobbyListStringFilter 和 AddRequestLobbyListNearValueFilter 设置。*/
export const enum LobbyComparison {
  /**大厅的值必须等于或小于此值。*/
  EqualToOrLessThan = -2,
  /**大厅的值必须小于此值。*/
  LessThan = -1,
  /**大厅的值必须与此值完全相同。*/
  Equal = 0,
  /**大厅的值必须大于此值。*/
  GreaterThan = 1,
  /**大厅的值必须等于或大于此值。*/
  EqualToOrGreaterThan = 2,
  /**大厅的值必须与此值不同。*/
  NotEqual = 3,
}

/**请求大厅列表时的大厅搜索距离筛选器。 大厅的结果按从最近到最远进行排序。 可通过 AddRequestLobbyListDistanceFilter 设置。*/
export const enum LobbyDistanceFilter {
  /**仅返回同一地区的大厅。*/
  Close = 0,
  /**仅返回同一地区或邻近地区的大厅。*/
  Default = 1,
  /**对于没有太多延迟要求的游戏，将返回距离半个地球范围内的大厅。*/
  Far = 2,
  /**不筛选。将匹配远至从印度到纽约（不建议，预计客户端之间会出现多秒延迟）的大厅。*/
  Worldwide = 3,
}

/**指定大厅类型，从 CreateLobby 和 SetLobbyType 设置。*/
export const enum LobbyType {
  /**邀请是加入大厅的唯一途径。*/
  Private = 0,
  /**好友和受邀者可加入，但不出现在大厅列表中。*/
  FriendsOnly = 1,
  /**通过搜索返回并对好友可见。*/
  Public = 2,
  /**通过搜索返回，但不对好友可见。
   如果希望一个用户同时在两个大厅中，比如将组配到一起时很有用。 用户只能加入一个普通大厅，最多可加入两个不可见大厅。*/
  Invisible = 3,
}

/**聊天成员状态变化类型*/
export const enum ChatMemberStateChange {
  /**1.进入*/
  Entered = 1,
  /**2.离开*/
  Left = 2,
  /**4.掉线*/
  Disconnected = 4,
  /**8.踢出*/
  Kicked = 8,
  /**16.封禁*/
  Banned = 16,
}

/**聊天室进入响应。*/
export const enum ChatRoomEnterResponse {
  /**  成功。*/
  Success = 1,
  /**  聊天室不存在（可能已关闭）。*/
  DoesntExist = 2,
  /**  一般性拒绝：您没有加入该聊天的权限。*/
  NotAllowed = 3,
  /**  聊天室已达到最大规模。*/
  Full = 4,
  /**  意外错误。*/
  Error = 5,
  /**  您被此聊天室封禁，不能加入。*/
  Banned = 6,
  /**  您是受限用户（帐户无值），不能加入此聊天。*/
  Limited = 7,
  /**  在组锁定或禁用时试图加入组聊天。*/
  ClanDisabled = 8,
  /**  用户在被社区封禁时试图加入聊天。*/
  CommunityBan = 9,
  /**  加入失败。此聊天室中的一个用户阻止了您的加入。*/
  MemberBlockedYou = 10,
  /**  加入失败。您阻止了一个已经加入聊天的用户。*/
  YouBlockedMember = 11,
}

/**steam事件*/
export const enum SteamEvent {
  /**当游戏覆盖层激活或隐藏时触发*/
  GameOverlayActivated = 'game-overlay-activated',
  /**当游戏链接到Steam服务器时触发*/
  GameServersConnected = 'game-servers-connected',
  /**当游戏从Steam服务器断开连接时触发*/
  GameServersDisconnected = 'game-servers-disconnected',
  /**当游戏连接Steam服务器失败时触发*/
  GameServerConnectFailure = 'game-server-connect-failure',
  /**当Steam客户端即将关闭时触发*/
  SteamShutdown = 'steam-shutdown',
  /**当一个朋友的状态改变时（与 greenworks.requestUserInformation 一起使用）触发*/
  PersonaStateChange = 'persona-state-change',
  /**如果图像还不可用，那么在上一个使用 getLargeFriendAvatar() 加载一个头像大图时触发*/
  AvatarImageLoaded = 'avatar-image-loaded',
  /**当用户收到聊天消息时触发*/
  GameConnectedFriendChatMessage = 'game-connected-friend-chat-message',
  /**当用户获得DLC的所有权，或者是安装了DLC之后触发*/
  DlcInstalled = 'dlc-installed',
  /**当用户响应微交易授权请求后触发*/
  MicroTxnAuthorizationResponse = 'micro-txn-authorization-response',
  /**在尝试创建大厅之后触发。*/
  LobbyCreated = 'lobby-created',
  /**大厅数据改变时触发*/
  LobbyDataUpdate = 'lobby-data-update',
  /**尝试进入大厅时触发。 收到这条消息后就可以使用大厅数据。*/
  LobbyEnter = 'lobby-enter',
  /**收到邀请时触发*/
  LobbyInvite = 'lobby-invite',
  /**当用户从朋友列表或被邀请后尝试加入大厅时触发。 当连接到指定大厅的时候，游戏客户端会收到这条消息。*/
  LobbyJoinRequested = 'lobby-join-requested',
  /**当用户尝试从好友列表中加入游戏时触发，其使用的是富消状态息*/
  RichPresenceJoinRequested = 'rich-presence-join-requested',
  /**在游戏已经运行时，当用户使用命令行执行steam url或查询参数（如 steam://run/<appid>//?param1=value1;param2=value2;param3=value3; ）后触发*/
  NewUrlLaunchParameters = 'new-url-launch-parameters',

  LobbyMatchList = 'lobby-match-list', //
  P2PSessionRequest = "p2p-session-request", //
  P2PSessionConnectFail = "p2p-session-connect-fail", //
  LobbyChatMsg = "lobby-chat-msg", //
  LobbyChatUpdate = "lobby-chat-update", //
}

/**游戏遮罩层*/
export const enum GameOverlayType {
  /**好友*/
  Friends = 'Friends',
  /**社区*/
  Community = 'Community',
  /**玩家*/
  Players = 'Players',
  /**设置*/
  Settings = 'Settings',
  /**官方游戏组*/
  OfficialGameGroup = 'OfficialGameGroup',
  /**统计*/
  Stats = 'Stats',
  /**成就*/
  Achievements = 'Achievements',
}

/**指定要从调用CreateQueryUserUGCRequest或CreateQueryAllUGCRequest中获取的UGC类型。*/
export const enum UGCMatchingType {
  /**小额交易物品和立即可用物品。*/
  Items = 0,
  /**小额交易物品。 （参见：鉴选型创意工坊）*/
  ItemsMtx = 1,
  /**玩家上传的普通游戏内物品。 （参见： 即用型创意工坊）*/
  ItemsReadyToUse = 2,
  /**共享 UGC 合集。*/
  Collections = 3,
  /**已共享的艺术作品。*/
  Artwork = 4,
  /**已共享的视频。*/
  Videos = 5,
  /**已共享的截图。*/
  Screenshots = 6,
  /**网页指南和综合指南。*/
  AllGuides = 7,
  /**仅在 Steam 社区推出的指南。*/
  WebGuides = 8,
  /**可以在您的游戏中使用的指南。 （比如 Dota 2 中的游戏角色指南。）*/
  IntegratedGuides = 9,
  /**立即可用物品和综合指南。*/
  UsableInGame = 10,
  /**已共享的控制器绑定。*/
  ControllerBindings = 11,
  /**游戏管理的物品（不由用户管理。）*/
  GameManagedItems = 12,
  /**  返回全部。*/
  All = 0,
}

/**与CreateQueryAllUGCRequest一起使用，指定所有可用UGC中查询的排序和筛选。*/
export const enum UGCQueryType {
  /**按一直以来的投票热度排序。*/
  RankedByVote = 0,
  /**按发布日期降序排序。*/
  RankedByPublicationDate = 1,
  /**按接受日期排序（针对 mtx 物品）。*/
  AcceptedForGameRankedByAcceptanceDate = 2,
  /**在给定的“趋势”期中按投票热度排序（通过 SetRankedByTrendDays 设置）。*/
  RankedByTrend = 3,
  /**按用户好友加入收藏的物品筛选，并按发布日期降序排序。*/
  FavoritedByFriendsRankedByPublicationDate = 4,
  /**按好友创建的物品筛选，并按发布日期降序排序。*/
  CreatedByFriendsRankedByPublicationDate = 5,
  /**按举报权重降序排列。*/
  RankedByNumTimesReported = 6,
  /**按当前用户关注的用户创建的物品筛选，并按发布日期降序排列。*/
  CreatedByFollowedUsersRankedByPublicationDate = 7,
  /**按用户的投票队列筛选。*/
  NotYetRated = 8,
  /**按总投票数升序排列（用于内部创建用户的投票队列）。*/
  RankedByTotalVotesAsc = 9,
  /**按赞的数量降序排序。 将使用“趋势”期（如有指定的话）（通过 SetRankedByTrendDays 设置）。*/
  RankedByVotesUp = 10,
  /**按关键词文本搜索相关性排列。*/
  RankedByTextSearch = 11,
  /**按不重复订阅者数总计降序排列。*/
  RankedByTotalUniqueSubscriptions = 12,
  /**按“趋势”期内总游戏时间降序排列（通过 SetRankedByTrendDays 设置）。*/
  RankedByPlaytimeTrend = 13,
  /**按总计游戏时间降序排列。*/
  RankedByTotalPlaytime = 14,
  /**按“趋势”期内平均游戏时间降序排列（通过 SetRankedByTrendDays 设置）。*/
  RankedByAveragePlaytimeTrend = 15,
  /**按总计平均游戏时间降序排列。*/
  RankedByLifetimeAveragePlaytime = 16,
  /**按“趋势”期内游戏会话数降序排列（以 SetRankedByTrendDays 设置）。*/
  RankedByPlaytimeSessionsTrend = 17,
  /**按总计游戏会话数降序排列。*/
  RankedByLifetimePlaytimeSessions = 18,
  /**按最近更新时间排列。*/
  RankedByLastUpdatedDate = 19,
}

/**单个已发布文件/UGC的详细信息。这由GetQueryUGCResult返回。*/
export interface SteamUGCDetails {
  /**此应用的开发者是否已经将此物品特别标记为被创意工坊所接受。 （参见：鉴选型创意工坊）*/
  acceptedForUse: boolean,
  /**此物品是否被封禁。*/
  banned: boolean,
  /**标签列表是否因过长而不能返回到提供的缓冲区并因此而截断。*/
  tagsTruncated: boolean,
  /**此物品的类型。*/
  fileType: WorkshopFileType,
  /**操作结果。1为成功，0为失败*/
  result: Result,
  /**此物品可见性。*/
  visibility: RemoteStoragePublishedFileVisibility,
  /**赞的数量/总票数的贝叶斯平均值，介于 0 到 1 之间。*/
  score: number,
  /**主文件的句柄。*/
  file: string,
  /**主文件的云文件名。*/
  fileName: string,
  /**主文件的文件大小。*/
  fileSize: number,
  /**预览文件的句柄。*/
  previewFile: string,
  /**预览文件的文件大小。*/
  previewFileSize: number,
  /**创建此内容的用户的 Steam ID。*/
  steamIDOwner: string,
  /**将消耗此物品的应用的 App Id。*/
  consumerAppID: number,
  /**创建此物品的应用的 App Id。*/
  creatorAppID: number,
  /**  此 UGC 的全局唯一物品句柄。*/
  publishedFileId: string,
  /**此物品的标题。*/
  title: string,
  /**此物品的说明。*/
  description: string,
  /**与此物品关联的 URL。 （用于视频或网站。）*/
  URL: string,
  /**与此物品关联的所有标签，用逗号分隔。*/
  tags: string,
  /**用户将已发布物品添加到其列表的时间（并非总是适用），以 Unix 时间戳格式提供（自 1970 年 1 月 1 日起的秒数）。*/
  timeAddedToUserList: number,
  /**所发布物品的创建时间，以 Unix 时间戳格式提供（自 1970 年 1 月 1 日起的秒数）。*/
  timeCreated: number,
  /**  所发布物品的最后更新时间，以 Unix 时间戳格式提供（自 1970 年 1 月 1 日起的秒数）。*/
  timeUpdated: number,
  /**踩的数量。*/
  votesDown: number,
  /**赞的数量。*/
  votesUp: number
}

/**与CreateQueryUserUGCRequest一起使用，为用户获取已发布UGC的不同列表。*/
export const enum UserUGCList {
  /**用户已发布的文件列表。 (相当于 http://steamcommunity.com/my/myworkshopfiles/?browsesort=myfiles)。*/
  Published = 0,
  /**用户已投票的文件列表。 包括赞和踩。*/
  VotedOn = 1,
  /**用户赞过的文件列表 （仅限于当前用户）。*/
  VotedUp = 2,
  /**用户已踩过的文件列表 （仅限于当前用户）。*/
  VotedDown = 3,
  /**已弃用。 切勿使用 （仅限于当前用户）。*/
  WillVoteLater = 4,
  /**用户已加入收藏的文件列表。 (相当于 http://steamcommunity.com/my/myworkshopfiles/?browsesort=myfavorites)。*/
  Favorited = 5,
  /**用户已订阅的文件列表 （仅限于当前用户）。 （相当于 http://steamcommunity.com/my/myworkshopfiles/?browsesort=mysubscriptions）*/
  Subscribed = 6,
  /**用户在游戏中为其花费了时间的文件列表。 （相当于 http://steamcommunity.com/my/myworkshopfiles/?browsesort=myplayedfiles）*/
  UsedOrPlayed = 7,
  /**用户关注其更新的文件列表。*/
  Followed = 8,
}

/**与CreateQueryUserUGCRequest一起使用，为用户发布的UGC列表指定排序顺序。默认为创建顺序降序。*/
export const enum UserUGCListSortOrder {
  /**按创建日期返回物品。 降序 - 从最新的物品开始。 （对应于创意工坊页面上的“sortmethod=newestfirst”）*/
  CreationOrderDesc = 0,
  /**按创建日期返回物品。 升序 - 从最旧的物品开始。 （对应于创意工坊页面上的“sortmethod=oldestfirst”）*/
  CreationOrderAsc = 1,
  /**按名称返回物品。 （对应于创意工坊页面上的“sortmethod=alpha”）*/
  TitleAsc = 2,
  /**首先返回最近更新的物品。 （对应于创意工坊页面上的“sortmethod=lastupdated”）*/
  LastUpdatedDesc = 3,
  /**首先返回最近订阅的物品。 （对应于创意工坊页面上的“sortmethod=subscriptiondate”）*/
  SubscriptionDateDesc = 4,
  /**首先返回最近分数更新的物品。 （对应于创意工坊页面上的“sortmethod=score”）*/
  VoteScoreDesc = 5,
  /**返回被举报要进行审核的物品。 （对应于创意工坊页面上的“sortmethod=formoderation”）*/
  ForModeration = 6,
}

/**指定文件状态。这些是可以组合的标志。由GetItemState返回。*/
export const enum UGCItemState {
  /**未在客户端上追踪此物品。*/
  None = 0,
  /**当前用户已订阅该物品。 不仅仅是被缓存。*/
  Subscribed = 1,
  /**此物品是使用 ISteamRemoteStorage 中的旧创意工坊函数创建的。*/
  LegacyItem = 2,
  /**该物品已安装且可用（但可能太过陈旧）。*/
  Installed = 4,
  /**此物品需要更新。 原因是尚未安装，或创建者已更新其内容。*/
  NeedsUpdate = 8,
  /**此物品更新当前正在下载。*/
  Downloading = 16,
  /**已为此物品调用 DownloadItem，直到 DownloadItemResult_t 触发后，其内容才可用。*/
  DownloadPending = 32,
}

/**共享文件将与社区共享的方式。*/
export const enum WorkshopFileType {
  /**只用于枚举。*/
  First = 0,
  /**可订阅的创意工坊常规物品。*/
  Community = 0,
  /**需要投票选择后在游戏内销售的创意工坊物品。 （参见：鉴选型创意工坊）*/
  Microtransaction = 1,
  /**一套创意工坊物品。*/
  Collection = 2,
  /**艺术作品。*/
  Art = 3,
  /**外部视频。*/
  Video = 4,
  /**截图。*/
  Screenshot = 5,
  /**未使用。过去用于参与青睐之光的游戏。*/
  Game = 6,
  /**未使用。过去用于参与青睐之光的软件。*/
  Software = 7,
  /**未使用。曾用于青睐之光相关概念。*/
  Concept = 8,
  /**Steam 网页指南。*/
  WebGuide = 9,
  /**应用程序集成指南。*/
  IntegratedGuide = 10,
  /**需要投票选择后销售的创意工坊商品。*/
  Merch = 11,
  /**Steam 控制器绑定。*/
  ControllerBinding = 12,
  /**只在 Steam 内部使用。*/
  SteamworksAccessInvite = 13,
  /**Steam 视频。*/
  SteamVideo = 14,
  /**完全由游戏管理，不由用户管理，且不显示在网页上。*/
  GameManagedItem = 15,
  /**只用于枚举。*/
  Max = 16,
}

/**创意工坊物品的可见状态种类。*/
export const enum RemoteStoragePublishedFileVisibility {
  /**所有人可见。*/
  Public = 0,
  /**仅好友可见。*/
  FriendsOnly = 1,
  /**仅物品作者可见。 若需将创意工坊物品从 API 中删除，将其设置为私有是最接近的做法。*/
  Private = 2,
  /**对所有人都可见，但不会在任何全局查询中返回。也不会在任何用户列表中返回，除非调用者是创建者或订阅者。*/
  Unlisted = 3,
}

/**工坊文件的安装信息*/
export interface ItemInstallInfo {
  /**返回创意工坊物品的字节大小。*/
  sizeOnDisk: string,
  /**通过复制返回包含内容的文件夹的绝对路径。*/
  folder: string,
  /**返回创意工坊物品上次更新的时间。*/
  timestamp: number,
}

/**DLC 元数据*/
export interface DLCData {
  /**
   * 该 DLC 的 AppID
   */
  appId: number;
  /**
   * DLC 当前是否在 Steam 商店中可用。 如果 DLC 没有可见的商店页面，则为 false。
   */
  available: boolean;
  /**
   * DLC 名称
   */
  name: string;
}

/**文件名以及大小*/
export interface FileNameAndSize {
  /**文件名*/
  name: string;
  /**文件大小*/
  size: number;
}

/**图像的大小*/
export interface ImageSize {
  /**返回图像宽度。*/
  width: number,
  /**返回图像高度。*/
  height: number,
}

/**
 * Steam 游戏在全局中的唯一标识符
 */
export interface SteamID {
  /**
   * 返回用户身份
   */
  getAccountID(): number;

  /**
   * 返回账户类型
   */
  getAccountType(): AccountType;

  /**
   * 返回当前用户为指定玩家设置的昵称 String。 如果没有为该玩家设置昵称，则为空
   */
  getNickname(): string;

  /**
   * 返回一个代表用户名字的字符串
   */
  getPersonaName(): string;

  /**
   * 返回代表SteamID的 String 值（将CSteamID转换为64位的形式）
   */
  getRawSteamID(): string;

  /**
   * 返回与用户的关系
   */
  getRelationship(): FriendRelationship;

  /**
   * 返回一个代表64位静态account key的字符串
   */
  getStaticAccountKey(): string;

  /**
   * 返回steam的等级
   */
  getSteamLevel(): number;

  /**
   * 返回是否是一个匿名账号
   */
  isAnonymous(): boolean;

  /**
   * 返回是否为匿名游戏服务器
   */
  isAnonymousGameServer(): boolean;

  /**
   * 返回是否是要填写的匿名游戏服务器登录名
   */
  isAnonymousGameServerLogin(): boolean;

  /**
   * 返回是否是匿名用户账号 （用于创建账户或重置密码）
   */
  isAnonymousUser(): boolean;

  /**
   * 返回是否是一个聊天account id
   */
  isChatAccount(): boolean;

  /**
   * 返回是否是一个小组account id
   */
  isClanAccount(): boolean;

  /**
   * 返回是否是PSN好友账号的虚拟SteamID
   */
  isConsoleUserAccount(): boolean;

  /**
   * 返回是否是一个内容服务器的account id
   */
  isContentServerAccount(): boolean;

  /**
   * 返回是否是一个游戏服务器的account id （要么是持久，要么是匿名）
   */
  isGameServerAccount(): boolean;

  /**
   * 返回是否是一个单独用户的account id
   */
  isIndividualAccount(): boolean;

  /**
   * 返回是否为大厅的聊天account id
   */
  isLobby(): boolean;

  /**
   * 返回是否是一个持久（非匿名）的游戏服务器account id
   */
  isPersistentGameServerAccount(): boolean;

  /**
   * 返回是否为有效account
   */
  isValid(): boolean;
}

/**关于朋友正在玩的游戏的信息*/
export interface FriendGameInfo {
  /**好友正在游玩的游戏ID*/
  gameID: string;
  /**好友正在游玩的服务器的IP*/
  unGameIP: number;
  /**好友正在游玩的服务器的端口*/
  usGamePort: number;
  /**好友正在游玩的服务器的查询端口*/
  usQueryPort: number;
  /**好友所在大厅的Steam ID*/
  steamIDLobby: string;
}

export type ErrorCallback = (error: Error) => void


export enum eChatEntryType {
  Invalid = 0,
  ChatMsg = 1,
  Typing = 2,
  InviteGame = 3,
  Emote = 4,
  //LobbyGameStart = 5,
  LeftConversation = 6,
  Entered = 7,
  WasKicked = 8,
  WasBanned = 9,
  Disconnected = 10,
  HistoricalChat = 11,
  //Reserved1 = 12,
  //Reserved2 = 13,
  LinkBlocked = 14,
}

export enum eChatMemberStateChange {
  /**1.进入*/
  Entered = 1,
  /**2.离开*/
  Left = 2,
  /**4.掉线*/
  Disconnected = 4,
  /**8.踢出*/
  Kicked = 8,
  /**16.封禁*/
  Banned = 16,
}