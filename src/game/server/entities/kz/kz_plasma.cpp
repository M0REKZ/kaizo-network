/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */

// modified by +KZ

#include "kz_plasma.h"
#include <game/server/entities/character.h>

#include <engine/server.h>
#include <engine/shared/config.h>

#include <generated/protocol.h>
#include <game/teamscore.h>

#include <game/server/gamecontext.h>
#include <game/server/player.h>

const float PLASMA_ACCEL = 1.1f;

CKZPlasma::CKZPlasma(CGameWorld *pGameWorld, vec2 Pos, vec2 Dir, bool Damage,
	bool Explosive, int ForClientId) :
	CEntity(pGameWorld, CGameWorld::CUSTOM_ENTTYPE_KZPLASMA)
{
	m_Pos = Pos;
	m_Core = Dir;
	m_Damage = Damage;
	m_Explosive = Explosive;
	m_ForClientId = ForClientId;
	m_EvalTick = Server()->Tick();
	m_LifeTime = Server()->TickSpeed() * 1.5f;

	GameWorld()->InsertEntity(this);
}

void CKZPlasma::Tick()
{
	// A plasma bullet has only a limited lifetime
	if(m_LifeTime == 0)
	{
		Reset();
		return;
	}
	CCharacter *pTarget = GameServer()->GetPlayerChar(m_ForClientId);
	// Without a target, a plasma bullet has no reason to live
	if(!pTarget)
	{
		Reset();
		return;
	}
	m_LifeTime--;
	Move();
	HitCharacter(pTarget);
	// Plasma bullets may explode twice if they would hit both a player and an obstacle in the next move step
	HitObstacle(pTarget);
}

void CKZPlasma::Move()
{
	m_Pos += m_Core;
	m_Core *= PLASMA_ACCEL;
}

bool CKZPlasma::HitCharacter(CCharacter *pTarget)
{
	vec2 IntersectPos;
	CCharacter *pHitPlayer = GameServer()->m_World.IntersectCharacter(
		m_Pos, m_Pos + m_Core, 0.0f, IntersectPos, nullptr, m_ForClientId);
	if(!pHitPlayer)
	{
		return false;
	}

	// Super player should not be able to stop the plasma bullets
	if(pHitPlayer->Team() == TEAM_SUPER)
	{
		return false;
	}

	if(m_Damage)
		pHitPlayer->TakeDamageVanilla(normalize(m_Core)*3, m_Explosive ? g_Config.m_SvDamageTurretExplosiveDmg : g_Config.m_SvDamageTurretDmg, pHitPlayer->GetPlayer() ? pHitPlayer->GetPlayer()->GetCid() : -1, WEAPON_WORLD);

	if(m_Explosive)
	{
		// Plasma Turrets are very precise weapons only one tee gets speed from it,
		// other tees near the explosion remain unaffected
		GameServer()->CreateExplosion(
			m_Pos, m_ForClientId, WEAPON_GRENADE, true, pTarget->Team(), pTarget->TeamMask());
	}
	Reset();
	return true;
}

bool CKZPlasma::HitObstacle(CCharacter *pTarget)
{
	// Check if the plasma bullet is stopped by a solid block or a laser stopper
	int HasIntersection = GameServer()->Collision()->IntersectNoLaser(m_Pos, m_Pos + m_Core, nullptr, nullptr);
	if(HasIntersection)
	{
		if(m_Explosive)
		{
			// Even in the case of an explosion due to a collision with obstacles, only one player is affected
			GameServer()->CreateExplosion(
				m_Pos, m_ForClientId, WEAPON_GRENADE, true, pTarget->Team(), pTarget->TeamMask());
		}
		Reset();
		return true;
	}
	return false;
}

void CKZPlasma::Reset()
{
	m_MarkedForDestroy = true;
}

void CKZPlasma::Snap(int SnappingClient)
{
	// Only players who can see the targeted player can see the plasma bullet
	CCharacter *pTarget = GameServer()->GetPlayerChar(m_ForClientId);
	if(!pTarget || !pTarget->CanSnapCharacter(SnappingClient))
	{
		return;
	}

	// Only players with the plasma bullet in their field of view or who want to see everything will receive the snap
	if(NetworkClipped(SnappingClient))
		return;

	int SnappingClientVersion = GameServer()->GetClientVersion(SnappingClient);

	int Subtype = (m_Explosive ? 1 : 0) | (m_Damage ? 2 : 0);
	GameServer()->SnapLaserObject(CSnapContext(SnappingClientVersion, Server()->IsSixup(SnappingClient), SnappingClient), GetId(),
		m_Pos, m_Pos, m_EvalTick, -1, Server()->Tick()%2, Subtype, m_Number);
}

void CKZPlasma::SwapClients(int Client1, int Client2)
{
	m_ForClientId = m_ForClientId == Client1 ? Client2 : m_ForClientId == Client2 ? Client1 : m_ForClientId;
}
