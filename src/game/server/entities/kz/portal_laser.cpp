/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "portal_laser.h"
#include <game/server/entities/character.h>

#include <engine/shared/config.h>

#include <game/generated/protocol.h>
#include <game/mapitems.h>

#include <game/server/gamecontext.h>
#include <game/server/gamemodes/DDRace.h>

#include "portal.h"

CPortalLaser::CPortalLaser(CGameWorld *pGameWorld, vec2 Pos, vec2 Direction, float StartEnergy, int Owner, bool BluePortal) :
	CEntity(pGameWorld, CGameWorld::CUSTOM_ENTTYPE_PORTAL_LASER)
{
	m_Pos = Pos;
	m_Owner = Owner;
	m_Energy = StartEnergy;
	m_Dir = Direction;
	m_Bounces = 0;
	m_EvalTick = 0;
	m_TelePos = vec2(0, 0);
	m_WasTele = false;
	// m_Type = Type;
	m_TeleportCancelled = false;
	// m_IsBlueTeleport = false;
	m_ZeroEnergyBounceInLastTick = false;
	m_TuneZone = GameServer()->Collision()->IsTune(GameServer()->Collision()->GetMapIndex(m_Pos));
	CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	m_TeamMask = pOwnerChar ? pOwnerChar->TeamMask() : CClientMask();
	m_BelongsToPracticeTeam = pOwnerChar && pOwnerChar->Teams()->IsPractice(pOwnerChar->Team());

	m_BluePortal = BluePortal;

	GameWorld()->InsertEntity(this);
	DoBounce();
}

void CPortalLaser::DoBounce()
{
	if(m_DestroyNextTick)
	{
		Reset();
		return;
	}

	// KZ
	CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	CCharacterCore *pOwnerCore = nullptr;

	if(pOwnerChar)
	{
		pOwnerCore = (CCharacterCore *)pOwnerChar->Core();
	}
	// End KZ

	m_EvalTick = Server()->Tick();

	if(m_Energy < 0)
	{
		m_MarkedForDestroy = true;
		return;
	}
	m_PrevPos = m_Pos;
	vec2 Coltile;

	int Res;
	int z;

	if(m_WasTele)
	{
		m_PrevPos = m_TelePos;
		m_Pos = m_TelePos;
		m_TelePos = vec2(0, 0);
	}

	vec2 To = m_Pos + m_Dir * m_Energy;
	CKZTile *pKZTile = nullptr;
	SKZColCharCoreParams ParamsKZ;
	ParamsKZ.pCore = pOwnerCore;

	Res = GameServer()->Collision()->FastIntersectLinePortalLaser(m_Pos, To, &Coltile, &To, &pKZTile, &z, &ParamsKZ); // KZ added pOwnerCore

	//+KZ Portal Start
	{
		if(pKZTile)
		{
			if(pKZTile->m_Index == KZ_TILE_PORTAL_RESET || pKZTile->m_Index == KZ_TILE_PORTAL_DISALLOW || (g_Config.m_SvPortalMode == 2 && Res == TILE_LUNFREEZE))
			{
				m_From = m_Pos;
				m_Pos = To;
				m_DestroyNextTick = true;
				return;
			}

		}
	}
	//+KZ Portal End

	if(Res)
	{
		// intersected
		m_From = m_Pos;
		m_Pos = To;

		vec2 TempPos = m_Pos;
		vec2 TempDir = m_Dir * 4.0f;

		//+KZ Portal Start
		{
			if((Res == TILE_SOLID || Res == TILE_NOHOOK) && !(Res == TILE_TELEINWEAPON))
			{
				if(g_Config.m_SvPortalMode == 0) // default
				{
					new CPortalKZ(GameWorld(), To, m_Owner, m_BluePortal);
					GameServer()->CreateSound(m_Pos, SOUND_LASER_BOUNCE, m_TeamMask);
				}
				else if(g_Config.m_SvPortalMode == 1 && pKZTile) // only certain tiles
				{
					if(pKZTile->m_Index == KZ_TILE_PORTAL_ALLOW)
					{
						new CPortalKZ(GameWorld(), To, m_Owner, m_BluePortal);
						GameServer()->CreateSound(m_Pos, SOUND_LASER_BOUNCE, m_TeamMask);
					}
				}
				else if(g_Config.m_SvPortalMode == 2) // pprace compat
				{
					int TileIndex = Collision()->GetFrontTileIndex(Collision()->GetMapIndex(Coltile));
					if(TileIndex == TILE_LFREEZE)
					{
						new CPortalKZ(GameWorld(), To, m_Owner, m_BluePortal);
						GameServer()->CreateSound(m_Pos, SOUND_LASER_BOUNCE, m_TeamMask);
					}
				}
				m_DestroyNextTick = true;
				return;
			}
		}
		//+KZ Portal End

		int f = 0;
		if(Res == -1)
		{
			f = GameServer()->Collision()->GetTile(round_to_int(Coltile.x), round_to_int(Coltile.y));
			GameServer()->Collision()->SetCollisionAt(round_to_int(Coltile.x), round_to_int(Coltile.y), TILE_SOLID);
		}
		GameServer()->Collision()->MovePoint(&TempPos, &TempDir, 1.0f, nullptr, &ParamsKZ); // KZ added pOwnerCore
		if(Res == -1)
		{
			GameServer()->Collision()->SetCollisionAt(round_to_int(Coltile.x), round_to_int(Coltile.y), f);
		}
		m_Pos = TempPos;
		m_Dir = normalize(TempDir);

		const float Distance = distance(m_From, m_Pos);
		// Prevent infinite bounces
		if(Distance == 0.0f && m_ZeroEnergyBounceInLastTick)
		{
			m_Energy = -1;
		}
		else if(!m_TuneZone)
		{
			m_Energy -= Distance + Tuning()->m_LaserBounceCost;
		}
		else
		{
			m_Energy -= distance(m_From, m_Pos) + GameServer()->TuningList()[m_TuneZone].m_LaserBounceCost;
		}
		m_ZeroEnergyBounceInLastTick = Distance == 0.0f;

		if(Res == TILE_TELEINWEAPON && !GameServer()->Collision()->TeleOuts(z - 1).empty())
		{
			int TeleOut = GameServer()->m_World.m_Core.RandomOr0(GameServer()->Collision()->TeleOuts(z - 1).size());
			m_TelePos = GameServer()->Collision()->TeleOuts(z - 1)[TeleOut];
			m_WasTele = true;
		}
		else
		{
			m_Bounces++;
			m_WasTele = false;
		}

		int BounceNum = Tuning()->m_LaserBounceNum;
		if(m_TuneZone)
			BounceNum = TuningList()[m_TuneZone].m_LaserBounceNum;

		if(m_Bounces > BounceNum)
			m_Energy = -1;

		GameServer()->CreateSound(m_Pos, SOUND_LASER_BOUNCE, m_TeamMask);
	}
	else
	{
		m_From = m_Pos;
		m_Pos = To;
		m_Energy = -1;
	}

	if(m_Owner >= 0)
	{
		int MapIndex = GameServer()->Collision()->GetPureMapIndex(Coltile);
		int TileFIndex = GameServer()->Collision()->GetFrontTileIndex(MapIndex);
	}
}

void CPortalLaser::Reset()
{
	m_MarkedForDestroy = true;
}

void CPortalLaser::Tick()
{
	if((g_Config.m_SvDestroyLasersOnDeath || m_BelongsToPracticeTeam) && m_Owner >= 0)
	{
		CCharacter *pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
		if(!(pOwnerChar && pOwnerChar->IsAlive()))
		{
			Reset();
		}
	}

	float Delay;
	if(m_TuneZone)
		Delay = TuningList()[m_TuneZone].m_LaserBounceDelay;
	else
		Delay = Tuning()->m_LaserBounceDelay;

	if((Server()->Tick() - m_EvalTick) > (Server()->TickSpeed() * Delay / 1000.0f))
		DoBounce();
}

void CPortalLaser::TickPaused()
{
	++m_EvalTick;
}

void CPortalLaser::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient) && NetworkClipped(SnappingClient, m_From))
		return;
	CCharacter *pOwnerChar = nullptr;
	if(m_Owner >= 0)
		pOwnerChar = GameServer()->GetPlayerChar(m_Owner);
	if(!pOwnerChar)
		return;

	pOwnerChar = nullptr;
	CClientMask TeamMask = CClientMask().set();

	if(m_Owner >= 0)
		pOwnerChar = GameServer()->GetPlayerChar(m_Owner);

	if(pOwnerChar && pOwnerChar->IsAlive())
		TeamMask = pOwnerChar->TeamMask();

	if(SnappingClient != SERVER_DEMO_CLIENT && !TeamMask.test(SnappingClient))
		return;

	int SnappingClientVersion = GameServer()->GetClientVersion(SnappingClient);
	int LaserType = m_BluePortal ? LASERTYPE_RIFLE : LASERTYPE_SHOTGUN;

	GameServer()->SnapLaserObject(CSnapContext(SnappingClientVersion, Server()->IsSixup(SnappingClient), SnappingClient), GetId(),
		m_Pos, m_From, Server()->Tick() - 4, m_Owner, LaserType, 0, m_Number);
}

void CPortalLaser::SwapClients(int Client1, int Client2)
{
	m_Owner = m_Owner == Client1 ? Client2 : m_Owner == Client2 ? Client1 :
								      m_Owner;
}
