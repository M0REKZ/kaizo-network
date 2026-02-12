# Copyright (C) Benjamín Gajardo (also known as +KZ)

import datatypes

KaizoCharacterFlags = ["BLUEPORTAL", "LASERRECOVERJUMP"]
GameInfoFlagsTWPlus = datatypes.Flags("GAMETWPLUSFLAG", ["PREDICT_PTWPLUS_TILES", "LASERJUMPS", "GUN_FULLAUTO"])

Flags += [
	datatypes.Flags("KAIZOCHARACTERFLAG", KaizoCharacterFlags),
 	GameInfoFlagsTWPlus
]

Objects += [
    # +KZ Kaizo Network
 
	NetObjectEx("KaizoNetworkTurret", "kaizoturret@m0rekz.github.io", [
		NetIntAny("m_X"),
		NetIntAny("m_Y"),
		NetIntRange("m_Type", 0, 'max_int'),
	]),
 
	NetObjectEx("KaizoNetworkMine", "kaizomine@m0rekz.github.io", [
		NetIntAny("m_X"),
		NetIntAny("m_Y"),
		NetIntRange("m_Type", 0, 'max_int'),
	]),

    NetObjectEx("KaizoNetworkCharacter", "kaizocharacter@m0rekz.github.io", [
		NetIntAny("m_Flags", default=0),
        NetIntAny("m_RealCurrentWeapon", default=-1),
        NetIntAny("m_Tick", default=0),
	], validate_size=False),
    
    NetObjectEx("KaizoNetworkPickup", "kaizopickup@m0rekz.github.io", [
		NetIntAny("m_X", default=0),
        NetIntAny("m_Y", default=0),
        NetIntAny("m_Type", default=0),
        NetIntAny("m_Switch", default=0),
	], validate_size=False),
    
    NetObjectEx("KaizoNetworkPlayerPing", "kaizoplayerping@m0rekz.github.io", [
		NetIntAny("m_Ping", default=0),
	], validate_size=False),
    
    # Pointer's TW+ 0.7
    
    NetObjectEx("GameInfoTWPlus", "gameinfo-twplus@pointerstwplus", [
        NetIntAny("m_Flags", default=0),
    ], validate_size=False),
]

Messages += [
    # +KZ Kaizo Network
 
	NetMessageEx("Sv_KaizoNetworkCrown", "kaizocrown@m0rekz.github.io", [
		NetIntAny("m_ClientId"),
	]),
]