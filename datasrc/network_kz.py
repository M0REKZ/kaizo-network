# Copyright (C) Benjamín Gajardo (also known as +KZ)
#
# Includes messages, enums and values from other clients, above they have a comment from where they got taken

import datatypes

KaizoCharacterFlags = ["BLUEPORTAL", "LASERRECOVERJUMP"]
GameInfoFlagsTWPlus = ["PREDICT_PTWPLUS_TILES", "LASERJUMPS", "GUN_FULLAUTO"]

RawHeader += '''

// <FoxNet
enum
{
	COSMETIC_FLAG_ANCHORED=1,
	COSMETIC_FLAG_XFLIP=2,

	// Whether to render the Laser Head at the "from" and/or "to" position
	COSMETIC_LASER_FLAG_FROM_HEAD=4,
	COSMETIC_LASER_FLAG_TO_HEAD=8,
};
// FoxNet>

'''


Flags += [
	datatypes.Flags("KAIZOCHARACTERFLAG", KaizoCharacterFlags),
 	datatypes.Flags("GAMETWPLUSFLAG", GameInfoFlagsTWPlus),
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

	NetObjectEx("PickupCustomTWPlus", "pickup-custom@pointerstwplus", [
		NetIntAny("m_X"),
		NetIntAny("m_Y"),
		NetIntAny("m_ResourceId"),
	]),
    
    # Duck/InfClass Client
    
    NetObjectEx("RespawnTimer", "respawn-timer@ddnet-community.github.io", [
		NetIntAny("m_TicksLeft"),
	]),
    
	# <FoxNet
	NetObjectEx("CosmeticPickup", "cosmetic-pickup@foxnet-ddnet.github.io", [
		NetIntAny("m_X"),
		NetIntAny("m_Y"),
		NetIntRange("m_Type", 0, 'max_int'),
		NetIntRange("m_Subtype", 0, 'max_int'),
		NetIntRange("m_Owner", -1, 'MAX_CLIENTS-1'),
		# -1 = Whatever alpha the Rendered Owner has (e.g. if solo, its set to cl_show_others_alpha)
		# If no owner or set to 0 it gets skipped 
		NetIntRange("m_Alpha", -1, 100),
		# Rotation of the Pickup
		NetIntRange("m_Rotation", 0, 359),
		NetIntAny("m_Flags", default=3),
	]),

	NetObjectEx("CosmeticLaser", "cosmetic-laser@foxnet-ddnet.github.io", [
		NetIntAny("m_ToX"),
		NetIntAny("m_ToY"),
		NetIntAny("m_FromX"),
		NetIntAny("m_FromY"),
		# Laser "Thickness"
		NetIntRange("m_TickOffset", 0, 8),
		NetIntRange("m_Owner", -1, 'MAX_CLIENTS-1'),
		# -1 = Whatever alpha the Rendered Owner has (e.g. if solo, its set to cl_show_others_alpha)
		# If no owner or set to 0 it gets skipped 
		NetIntRange("m_Alpha", -1, 100),
		NetIntAny("m_Type"),
		NetIntAny("m_Flags", default=3),
	]),

	NetObjectEx("CosmeticProjectile", "cosmetic-projectile@foxnet-ddnet.github.io", [
		NetIntAny("m_X"),
		NetIntAny("m_Y"),
		NetIntRange("m_Type", 0, 'max_int'),
		NetIntRange("m_Owner", -1, 'MAX_CLIENTS-1'),
		# -1 = Whatever alpha the Rendered Owner has (e.g. if solo, its set to cl_show_others_alpha)
		# If no owner or set to 0 it gets skipped 
		NetIntRange("m_Alpha", -1, 100),
		NetIntRange("m_Rotation", 0, 359),
		NetIntAny("m_Flags", default=3),
	]),
	# FoxNet>
]

Messages += [
    # +KZ Kaizo Network
 
	NetMessageEx("Sv_KaizoNetworkCrown", "kaizocrown@m0rekz.github.io", [
		NetIntAny("m_ClientId"),
	]),
 
	# Duck/InfClass Client
 
	NetMessageEx("Cl_Language", "language@ddnet-community.github.io", [
		NetStringStrict("m_pLanguage"),
	]),

	# Pointer's TW+ 0.7 & Duck/InfClass Client

	NetMessageEx("Sv_ImageResourceTWPlus", "image-resource@pointerstwplus", [
		NetIntAny("m_Id"),
		NetString("m_pName"),
		NetString("m_pUnused"),
	]),

	NetMessageEx("Cl_IHaveResourceTWPlus", "i-have-resource@pointerstwplus", [
		NetIntAny("m_Id"),
	]),
]