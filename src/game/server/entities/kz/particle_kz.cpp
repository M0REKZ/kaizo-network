#include "particle_kz.h"
#include <game/server/gamecontext.h>

CParticleKZ::CParticleKZ(CGameWorld *pGameWorld, vec2 Pos, int LiveTicks) :
CEntity(pGameWorld, CGameWorld::KZ_ENTTYPE_PARTICLE_KZ, Pos)
{
    m_LiveTicks = LiveTicks;

    GameWorld()->InsertEntity(this);
}

void CParticleKZ::Tick()
{
    if(m_LiveTicks <=0)
        Reset();
    m_LiveTicks--;
}

void CParticleKZ::Snap(int SnappingClient)
{
    if(NetworkClipped(SnappingClient))
        return;

    {
		CNetObj_Projectile *pProj = Server()->SnapNewItem<CNetObj_Projectile>(GetId());
		if(!pProj)
		{
			return;
		}
		pProj->m_X = (int)m_Pos.x;
        pProj->m_Y = (int)m_Pos.y;
        pProj->m_VelX = 0;
        pProj->m_VelY = 0;
        pProj->m_StartTick = 0;
        pProj->m_Type = WEAPON_HAMMER;
	}
}

void CParticleKZ::Reset()
{
    m_MarkedForDestroy = true;
}
