/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "mine.h"

#include <game/server/entities/character.h>

#include <game/generated/protocol.h>
#include <game/mapitems.h>
#include <game/teamscore.h>

#include <game/server/gamecontext.h>
#include <game/server/player.h>

#include <engine/shared/config.h>
#include <game/server/entities/projectile.h>
#include <game/server/gamecontroller.h>

CMine::CMine(CGameWorld *pGameWorld, vec2 Pos, int Number) :
	CEntity(pGameWorld, CGameWorld::CUSTOM_ENTTYPE_MINE)
{
	m_Pos = Pos;
	m_Number = Number;

	for(int i = 0; i < NUM_DDRACE_TEAMS; i++)
	{
		m_RespawnTicks[i] = 0;
	}

	GameWorld()->InsertEntity(this);
}

void CMine::Tick()
{
	Move();

	bool Explode = false;

	for(int i = 0; i < NUM_DDRACE_TEAMS; i++)
	{
		Explode = false;

		if(m_RespawnTicks[i])
		{
			m_RespawnTicks[i]--;
		}
		else if(m_Number > 0 ? Switchers()[m_Number].m_aStatus[i] : true)
		{
			CCharacter *apCloseChars[MAX_CLIENTS];
			CCharacter *pChr = nullptr;
			int Num = GameServer()->m_World.FindEntities(m_Pos, 10, (CEntity **)apCloseChars, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
			for(int j = 0; j < Num; ++j)
			{
				if(apCloseChars[j] && apCloseChars[j]->IsAlive() && apCloseChars[j]->Team() == i)
				{
					pChr = apCloseChars[j];
					Explode = true;
					break;
				}
			}

			CEntity *pProj = nullptr;
			CEntity *p = GameServer()->m_World.FindFirst(CGameWorld::ENTTYPE_PROJECTILE);
			float Len = 0.0f;
			for(; p; p = p->TypeNext())
			{
				Len = 0.0f;
				CProjectile *pProjectile = (CProjectile *)p;

				if(pProjectile->GetDDraceTeam() != i)
					continue;

				Len = distance(m_Pos, pProjectile->GetPos((Server()->Tick() - pProjectile->GetStartTick()) / (float)Server()->TickSpeed()));

				if(Len < 50.0f)
				{
					p->Reset();
					pProj = p;
					Explode = true;
					break;
				}
			}

			if(Explode)
			{
				if(pChr)
				{
					GameServer()->CreateExplosion(m_Pos, pChr->GetPlayer()->GetCid(), WEAPON_GRENADE, true, pChr->Team(), pChr->TeamMask());
					pChr->TakeDamageVanilla(vec2(0, 0), g_Config.m_SvDamageMineDmg, pChr->GetPlayer()->GetCid(), WEAPON_GRENADE);
				}
				else if(pProj)
				{
					GameServer()->CreateExplosion(m_Pos, pProj->GetOwnerId(), WEAPON_GRENADE, true, -1, CClientMask().set());
				}

				GameServer()->CreateSound(m_Pos, SOUND_GRENADE_EXPLODE);
				Explode = false;
				m_RespawnTicks[i] = Server()->TickSpeed() * 20;
			}
		}
	}
}

void CMine::Snap(int SnappingClient)
{
	int team = -1;

	if(SnappingClient >= 0 && SnappingClient < SERVER_MAX_CLIENTS)
		team = GameServer()->GetDDRaceTeam(SnappingClient);

	if((team >= 0 && team < NUM_DDRACE_TEAMS) ? m_RespawnTicks[team] : true)
		return;

	if(m_Number > 0 ? (Switchers()[m_Number].m_aStatus[team] ? false : !((Server()->Tick() % Server()->TickSpeed()) % 11)) : false)
		return;

	if(NetworkClipped(SnappingClient))
		return;

	CNetObj_Projectile *pObj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, GetId(), sizeof(CNetObj_Projectile)));
	if(!pObj)
		return;

	pObj->m_X = (int)m_Pos.x;
	pObj->m_Y = (int)m_Pos.y + 8 * sin((float)Server()->Tick() / 25.0); // do animation like a normal pickup
	pObj->m_VelX = 1;
	pObj->m_VelY = 1;
	pObj->m_Type = WEAPON_LASER;
}

void CMine::Reset()
{
	m_MarkedForDestroy = true;
}

void CMine::Move()
{
	if(Server()->Tick() % (int)(Server()->TickSpeed() * 0.15f) == 0)
	{
		Collision()->MoverSpeed(m_Pos.x, m_Pos.y, &m_Core);
		m_Pos += m_Core;
		if(GameLayerClipped(m_Pos))
			Reset();
	}
}
