#ifndef GAME_SERVER_ENTITIES_KZ_PARTICLE_KZ_H
#define GAME_SERVER_ENTITIES_KZ_PARTICLE_KZ_H

#include <game/server/entity.h>

class CParticleKZ : public CEntity
{
public:
    CParticleKZ(CGameWorld *pGameWorld, vec2 Pos, int LiveTicks);
    
    void Tick() override;
    void Snap(int SnappingClient) override;
    void Reset() override;

    int m_LiveTicks;
};

#endif