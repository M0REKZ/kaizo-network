#include "projectile_pvp.h"
#include <game/server/entities/character.h>
#include <game/collision.h>
#include <game/mapitems.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>

CProjectilePvP::CProjectilePvP(CGameWorld *pGameWorld, int Owner, vec2 Pos, vec2 Dir, int Type) :
CProjectileKZ(pGameWorld, Owner, Pos, Dir, Type, CGameWorld::KZ_ENTTYPE_PROJECTILE_PVP)
{
    //GameWorld()->InsertEntity(this); Dont, this is already done is base class
}

void CProjectilePvP::Tick()
{
	vec2 PrevPos = m_Pos;

	CProjectileKZ::Tick();
	
	if(m_LifeSpan == -1 && m_Type == WEAPON_GRENADE && !m_Exploded)
	{
		if(!m_Rollback)
			GameServer()->CreateExplosion(m_Pos, m_Owner, m_Type, false, -1);
		else
			GameServer()->m_Rollback.CreateExplosionOnTick(m_Pos, m_Owner, m_Type, false, -1, m_CollidedTick);
		GameServer()->CreateSound(m_Pos, SOUND_GRENADE_EXPLODE);
		m_Exploded = true;
	}
}

void CProjectilePvP::OnCollide(vec2 PrevPos, int TileIndex, vec2 *pPreIntersectPos, vec2 *pIntersectPos, int *pTeleNr)
{
	CProjectileKZ::OnCollide(PrevPos, TileIndex, pPreIntersectPos, pIntersectPos, pTeleNr);

	if(m_MarkedForDestroy) //Reset was called on base class
	{
		if(m_Type == WEAPON_GRENADE && !m_Exploded)
		{
			if(!m_Rollback)
				GameServer()->CreateExplosion(*pPreIntersectPos, m_Owner, m_Type, false, -1);
			else
				GameServer()->m_Rollback.CreateExplosionOnTick(*pPreIntersectPos, m_Owner, m_Type, false, -1, m_CollidedTick);
			GameServer()->CreateSound(*pPreIntersectPos, SOUND_GRENADE_EXPLODE);
			m_Exploded = true;
		}

		if(TileIndex == TILE_NOHOOK) //make metallic sound because yes
		{
			GameServer()->CreateSound(*pPreIntersectPos, SOUND_HOOK_NOATTACH);
		}
	}
}

void CProjectilePvP::OnCharacterCollide(vec2 PrevPos, CCharacter *pChar, vec2 *pIntersectPos)
{
	switch (m_Type)
	{
		case WEAPON_GRENADE:
			if(!m_Exploded)
			{
				if(!m_Rollback)
					GameServer()->CreateExplosion(*pIntersectPos, m_Owner, m_Type, false, -1);
				else
					GameServer()->m_Rollback.CreateExplosionOnTick(*pIntersectPos, m_Owner, m_Type, false, -1, m_CollidedTick);
				GameServer()->CreateSound(*pIntersectPos, SOUND_GRENADE_EXPLODE);
				m_Exploded = true;
			}
			break;
		case WEAPON_SHOTGUN:
			pChar->TakeDamage(normalize(*pIntersectPos - PrevPos),1,m_Owner,WEAPON_SHOTGUN);
			break;
		case WEAPON_GUN:
			pChar->TakeDamage(vec2(0,0),1,m_Owner,WEAPON_GUN);
			break;
	}

	GameServer()->CreateSound(*pIntersectPos, SOUND_HOOK_ATTACH_PLAYER); //some weird realistic sound

	Reset();
}
