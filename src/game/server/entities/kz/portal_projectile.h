#ifndef GAME_SERVER_ENTITIES_PORTAL_PROJECTILE_H
#define GAME_SERVER_ENTITIES_PORTAL_PROJECTILE_H

#include <game/server/entity.h>
#include "fdd_stable_projectile.h"

class CPortalProjectile : public CStableProjectile
{
	int m_ParticleID[2];
	vec2 m_Dir;
	bool m_BluePortal = false;
public:
	CPortalProjectile(CGameWorld *pGameWorld, int Owner, vec2 Pos, vec2 Dir, bool BluePortal, bool HideOnSpec = false, bool OnlyShowOwner = false);
	~CPortalProjectile();

	virtual void Tick() override;
	virtual void Snap(int SnappingClient) override;
	virtual void Reset() override;
};

#endif