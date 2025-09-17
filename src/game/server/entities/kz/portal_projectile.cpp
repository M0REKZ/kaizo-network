#include <game/server/gamecontext.h>
#include "fdd_stable_projectile.h"
#include "portal_projectile.h"
#include "portal.h"
#include <game/server/teams.h>
#include <game/server/player.h>
#include <game/server/entities/character.h>
#include <engine/shared/config.h>

CPortalProjectile::CPortalProjectile(CGameWorld *pGameWorld, int Owner, vec2 Pos, vec2 Dir, bool BluePortal, bool HideOnSpec, bool OnlyShowOwner)
: CStableProjectile(pGameWorld,WEAPON_SHOTGUN,Owner,Pos,HideOnSpec,OnlyShowOwner)
{
	m_ObjType = CGameWorld::CUSTOM_ENTTYPE_PORTAL_PROJECTILE;
	m_Dir = Dir;
	m_BluePortal = BluePortal;

	m_ParticleID[0] = Server()->SnapNewId();
	m_ParticleID[1] = Server()->SnapNewId();

	GameWorld()->InsertEntity(this);
}

CPortalProjectile::~CPortalProjectile()
{
	Server()->SnapFreeId(m_ParticleID[0]);
	Server()->SnapFreeId(m_ParticleID[1]);
}

void CPortalProjectile::Tick()
{
	vec2 oldpos = m_Pos;
	m_Pos += m_Dir * 15;
	CStableProjectile::Tick();

	if(Collision()->GetKZFrontTileIndex(m_Pos) == KZ_TILE_PORTAL_RESET || Collision()->GetKZGameTileIndex(m_Pos) == KZ_TILE_PORTAL_RESET)
	{
		Reset();
		return;
	}

	if(g_Config.m_SvPortalMode == 2)
	{
		int TileIndex = Collision()->GetTileIndex(Collision()->GetMapIndex(m_Pos));
		int TileIndex2 = Collision()->GetFrontTileIndex(Collision()->GetMapIndex(m_Pos));
		if(TileIndex == TILE_LUNFREEZE || TileIndex2 == TILE_LUNFREEZE)
		{
			Reset();
			return;
		}
	}

	if(Collision()->CheckPoint(m_Pos))
	{
		if(!(g_Config.m_SvPortalMode == 2))
		{
			if(Collision()->GetKZFrontTileIndex(m_Pos) == KZ_TILE_PORTAL_DISALLOW || Collision()->GetKZFrontTileIndex(oldpos) == KZ_TILE_PORTAL_DISALLOW || Collision()->GetKZGameTileIndex(m_Pos) == KZ_TILE_PORTAL_DISALLOW || Collision()->GetKZGameTileIndex(oldpos) == KZ_TILE_PORTAL_DISALLOW)
			{
				Reset();
				return;
			}
		}

		if(g_Config.m_SvPortalMode == 0) //default
		{
			new CPortalKZ(GameWorld(),oldpos,m_Owner,m_BluePortal);
			GameServer()->CreateSound(m_Pos,SOUND_LASER_BOUNCE,m_TeamMask);
		}
		else if(g_Config.m_SvPortalMode == 1) //only certain tiles
		{
			if(Collision()->GetKZFrontTileIndex(m_Pos) == KZ_TILE_PORTAL_ALLOW)
			{
				new CPortalKZ(GameWorld(),oldpos,m_Owner,m_BluePortal);
				GameServer()->CreateSound(m_Pos,SOUND_LASER_BOUNCE,m_TeamMask);
			}
			else if(Collision()->GetKZFrontTileIndex(oldpos) == KZ_TILE_PORTAL_ALLOW)
			{
				new CPortalKZ(GameWorld(),oldpos,m_Owner,m_BluePortal);
				GameServer()->CreateSound(m_Pos,SOUND_LASER_BOUNCE,m_TeamMask);
			}
			else if(Collision()->GetKZGameTileIndex(m_Pos) == KZ_TILE_PORTAL_ALLOW)
			{
				new CPortalKZ(GameWorld(),oldpos,m_Owner,m_BluePortal);
				GameServer()->CreateSound(m_Pos,SOUND_LASER_BOUNCE,m_TeamMask);
			}
			else if(Collision()->GetKZGameTileIndex(oldpos) == KZ_TILE_PORTAL_ALLOW)
			{
				new CPortalKZ(GameWorld(),oldpos,m_Owner,m_BluePortal);
				GameServer()->CreateSound(m_Pos,SOUND_LASER_BOUNCE,m_TeamMask);
			}
		}
		else if(g_Config.m_SvPortalMode == 2) //pprace compat
		{
			int TileIndex = Collision()->GetFrontTileIndex(Collision()->GetMapIndex(m_Pos));
			if(TileIndex == TILE_LFREEZE)
			{
				new CPortalKZ(GameWorld(),oldpos,m_Owner,m_BluePortal);
				GameServer()->CreateSound(m_Pos,SOUND_LASER_BOUNCE,m_TeamMask);
			}
		}
		Reset();
	}
}

void CPortalProjectile::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	if (m_OnlyShowOwner && SnappingClient != m_Owner)
		return;

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
	if (m_HideOnSpec && pOwner && pOwner->IsPaused())
		return;

	int SnappingClientVersion = GameServer()->GetClientVersion(SnappingClient);
	bool Sixup = Server()->IsSixup(SnappingClient);

	CStableProjectile::Snap(SnappingClient);

	{
		vec2 postemp;
				
		postemp.x = m_Pos.x + m_Dir.x * 15 + 32*sin((float)Server()->Tick() / 25.0);
		postemp.y = m_Pos.y + m_Dir.y * 15 + 32*cos((float)Server()->Tick() / 25.0);

		GameServer()->SnapLaserObject(CSnapContext(SnappingClientVersion, Sixup, SnappingClient),m_ParticleID[0],postemp,postemp,Server()->Tick(),m_Owner,m_BluePortal ? LASERTYPE_PLASMA : LASERTYPE_SHOTGUN);

		postemp.x = m_Pos.x + -32*sin((float)Server()->Tick() / 25.0);

		GameServer()->SnapLaserObject(CSnapContext(SnappingClientVersion, Sixup, SnappingClient),m_ParticleID[1],postemp,postemp,Server()->Tick(),m_Owner,m_BluePortal ? LASERTYPE_PLASMA : LASERTYPE_SHOTGUN);
	}
}

void CPortalProjectile::Reset()
{
	CStableProjectile::Reset();
}
