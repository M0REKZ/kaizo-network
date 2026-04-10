// UUID(name_in_code, name)
//
// When adding your own extended net messages, choose the name (third
// parameter) as `<name>@<domain>` where `<name>` is a name you can choose
// freely and `<domain>` is a domain you own. If you don't own a domain, try
// choosing a string that is not a domain and uniquely identifies you, e.g. use
// the name of the client/server you develop.
//
// Example:
//
// 1) `i-unfreeze-you@ddnet.org`
// 2) `creeper@minetee`
//
// The first example applies if you own the `ddnet.org` domain, that is, if you
// are adding this message on behalf of the DDNet team.
//
// The second example shows how you could add a message if you don't own a
// domain, but need a message for your minetee client/server.

// This file can be included several times.

#ifndef UUID
// This helps IDEs properly syntax highlight the uses of the macro below.
#define UUID(Id, Name)
#endif

UUID(NETMSG_WHATIS, "what-is@ddnet.tw")
UUID(NETMSG_ITIS, "it-is@ddnet.tw")
UUID(NETMSG_IDONTKNOW, "i-dont-know@ddnet.tw")

UUID(NETMSG_RCONTYPE, "rcon-type@ddnet.tw")
UUID(NETMSG_MAP_DETAILS, "map-details@ddnet.tw")
UUID(NETMSG_CAPABILITIES, "capabilities@ddnet.tw")
UUID(NETMSG_CLIENTVER, "clientver@ddnet.tw")
UUID(NETMSG_PINGEX, "ping@ddnet.tw")
UUID(NETMSG_PONGEX, "pong@ddnet.tw")
UUID(NETMSG_CHECKSUM_REQUEST, "checksum-request@ddnet.tw")
UUID(NETMSG_CHECKSUM_RESPONSE, "checksum-response@ddnet.tw")
UUID(NETMSG_CHECKSUM_ERROR, "checksum-error@ddnet.tw")
UUID(NETMSG_REDIRECT, "redirect@ddnet.org")
UUID(NETMSG_RCON_CMD_GROUP_START, "rcon-cmd-group-start@ddnet.org")
UUID(NETMSG_RCON_CMD_GROUP_END, "rcon-cmd-group-end@ddnet.org")
UUID(NETMSG_MAP_RELOAD, "map-reload@ddnet.org")
UUID(NETMSG_RECONNECT, "reconnect@ddnet.org")
UUID(NETMSG_MAPLIST_ADD, "sv-maplist-add@ddnet.org")
UUID(NETMSG_MAPLIST_GROUP_START, "sv-maplist-start@ddnet.org")
UUID(NETMSG_MAPLIST_GROUP_END, "sv-maplist-end@ddnet.org")

//+KZ

//Kaizo Network
UUID(NETMSG_KZ_KAIZO_NETWORK_VERSION, "kaizoversion@m0rekz.github.io")

//Other Clients
UUID(NETMSG_CLIENTVER_INFCLASS, "clientver@infclass") // InfClass
UUID(NETMSG_IAM_KAIZO_CLIENT, "i-am-kaizo-client-pluskz@m0rekz.github.io") // Kaizo Client
UUID(NETMSG_IAM_DUCKCLIENT, "i-am-duckclient@pointer31.github.io") // DuckInfclass Client
UUID(NETMSG_IAMTATER, "iamtater@sjrc6.github.io") // T-Client
UUID(NETMSG_IAMQXD, "i-am-qxd@qxdFox.github.io") // E-Client
UUID(NETMSG_IAMCHILLERBOT, "i-am-chillerbot@chillerbot.github.io") // Chillerbot-UX
UUID(NETMSG_IAMSTA, "i-am-StA@stormaxs.github.io/sta.github.io") // StA Client
UUID(NETMSG_IAMALLTHEHAXX, "i-am-allthehaxx@allthehaxx.github.io") // AllTheHaxx
UUID(NETMSG_IAMPULSE, "i-am-pulse@pulse-client.xyz") // Pulse Client
UUID(NETMSG_IAM_CACTUS, "iamcactus@denchik.top") // Cactus Client
UUID(NETMSG_IAM_AIODOB, "i-am-aiodob@qxdFox.github.io") // Aiodob Client
UUID(NETMSG_IAM_FEX, "i-am-fex@faffa81.github.io") // FeX Client
UUID(NETMSG_IAMRUSHIE, "i-am-rushie@1voix1.github.io") // RClient
UUID(NETMSG_IAM_SCLIENT, "i-am-sclient@sclient.site") // S-Client
