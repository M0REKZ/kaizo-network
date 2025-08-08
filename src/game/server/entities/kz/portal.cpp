/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "portal.h"

#include <game/generated/protocol.h>
#include <game/mapitems.h>
#include <game/teamscore.h>

#include <game/server/gamecontext.h>
#include <game/server/player.h>

#include <engine/shared/config.h>
//dirty:
#include <game/server/gamecontroller.h>

#include <game/server/entities/character.h>
#include <game/mapitems.h>

CPortalKZ::CPortalKZ(CGameWorld *pGameWorld, vec2 StartPos, int Owner,bool Blue) :
CEntity(pGameWorld,CGameWorld::CUSTOM_ENTTYPE_PORTAL)
{
	m_Owner = Owner;
	m_Blue = Blue;
	m_Laser2ID = Server()->SnapNewId();

	GameWorld()->InsertEntity(this);
	
	if(CheckPosition(StartPos))
		Reset();

	if(!GameServer()->m_apPlayers[m_Owner])
		Reset();

	if(!GameServer()->GetPlayerChar(m_Owner))
		Reset();

	if(!m_MarkedForDestroy)
	{
		m_OrigTeam = GameServer()->GetPlayerChar(m_Owner)->Team();

		CPortalKZ* p = (CPortalKZ*)GameWorld()->FindFirst(CGameWorld::CUSTOM_ENTTYPE_PORTAL);

		for(;p;p = (CPortalKZ*)p->TypeNext())
		{
			if(p == this)
				continue;

			if(p->m_Owner==m_Owner && p->m_Blue == m_Blue)
				p->Reset();
		}

		m_pCore = new CPortalCore(m_Owner, m_Pos, m_Pos2, m_Blue, m_OrigTeam);
		if(m_pCore)
		{
			GameWorld()->m_Core.SetPortalKZ(m_pCore);
		}
		else
		{
			Reset();
		}
	}
}

CPortalKZ::~CPortalKZ()
{
	Server()->SnapFreeId(m_Laser2ID);
}

void CPortalKZ::Reset()
{
	m_MarkedForDestroy = true;
	GameWorld()->m_Core.DeletePortalKZ(m_Owner,m_Blue);
}

void CPortalKZ::Tick()
{
	if(!GameWorld()->m_Core.GetPortalKZ(m_Owner,m_Blue))
	{
		Reset();
		return;
	}

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!pOwner || (pOwner && !pOwner->IsAlive()))
	{
		Reset();
		return;
	}

	if(pOwner->Team() != m_OrigTeam)
	{
		Reset();
		return;
	}

	//Portal collision handling is on collision_kz.cpp
}

void CPortalKZ::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!pOwner || (pOwner && !pOwner->IsAlive()))
		return;

	CCharacter* pChr = GameServer()->GetPlayerChar(SnappingClient);

	int Team = pOwner->Team();

	if(pChr)
	{
		Team = pChr->Team();
	}

	if(pOwner->Team() != TEAM_SUPER && pOwner->Team() != Team)
		return;

	int SnappingClientVersion = GameServer()->GetClientVersion(SnappingClient);
	bool Sixup = Server()->IsSixup(SnappingClient);

	GameServer()->SnapLaserObject(CSnapContext(SnappingClientVersion, Sixup, SnappingClient),GetId(),m_Pos,m_Pos2,Server()->Tick(),m_Owner,m_Blue ? LASERTYPE_PLASMA : LASERTYPE_SHOTGUN);
	GameServer()->SnapLaserObject(CSnapContext(SnappingClientVersion, Sixup, SnappingClient),m_Laser2ID,m_Pos2,m_Pos,Server()->Tick(),m_Owner,m_Blue ? LASERTYPE_PLASMA : LASERTYPE_SHOTGUN);
}

CPortalKZ *CPortalKZ::GetOtherPortal()
{
	for(CPortalKZ* p = (CPortalKZ*)GameWorld()->FindFirst(CGameWorld::CUSTOM_ENTTYPE_PORTAL);p;p = (CPortalKZ*)p->TypeNext())
	{
		if(p == this)
			continue;

		if(!p->m_MarkedForDestroy && p->m_Owner==m_Owner && p->m_Blue != m_Blue)
			return p;
	}

	return nullptr;
}

bool CPortalKZ::IsTeleportViable(vec2 Pos)
{
	return !(Collision()->CheckPoint(Pos.x + 14.0f,Pos.y + 14.0f) || Collision()->CheckPoint(Pos.x - 14.0f,Pos.y + 14.0f) || Collision()->CheckPoint(Pos.x + 14.0f,Pos.y - 14.0f) || Collision()->CheckPoint(Pos.x - 14.0f,Pos.y - 14.0f));
}

bool CPortalKZ::CheckPosition(vec2 Pos)
{
	bool Col[4] = {false,false,false,false};

	if(Collision()->CheckPoint(vec2(Pos.x-32,Pos.y))) //left
	{
		Col[0] =true;
	}
	if(Collision()->CheckPoint(vec2(Pos.x+32,Pos.y))) //right
	{
		Col[1] =true;
	}
	if(Collision()->CheckPoint(vec2(Pos.x,Pos.y-32))) //up
	{
		Col[2] =true;
	}
	if(Collision()->CheckPoint(vec2(Pos.x,Pos.y+32))) //down
	{
		Col[3] =true;
	}

	if((Col[0] && Col[1]) || (Col[2] && Col[3]))
		return true;
	
	if(Col[0] || Col[1])
	{
		m_Pos = vec2(Pos.x,Pos.y-32);
		m_Pos2 = vec2(Pos.x,Pos.y+32);
	}
	else
	{
		m_Pos = vec2(Pos.x-32,Pos.y);
		m_Pos2 = vec2(Pos.x+32,Pos.y);
	}

	//Check if able to teleport

	bool dotele = false;

	if(m_Pos.x == m_Pos2.x)
	{
		if(IsTeleportViable(vec2(m_Pos.x + 32.f, m_Pos.y + 32.f)))
		{
			dotele = true;
		}
		else if(IsTeleportViable(vec2(m_Pos.x - 32.f, m_Pos.y + 32.f)))
		{
			dotele = true;
		}
	}
	else
	{
		if(IsTeleportViable(vec2(m_Pos.x + 32.f, m_Pos.y + 32.f)))
		{
			dotele = true;
		}
		else if(IsTeleportViable(vec2(m_Pos.x + 32.f, m_Pos.y - 32.f)))
		{
			dotele = true;
		}
	}

	if(!dotele)
		return true;

	return false;
}
