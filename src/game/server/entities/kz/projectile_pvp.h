#ifndef GAME_SERVER_ENTITIES_KZ_PROJECTILE_PVP_H
#define GAME_SERVER_ENTITIES_KZ_PROJECTILE_PVP_H

#include "projectile_kz.h"

class CProjectilePvP : public CProjectileKZ
{
public:
	CProjectilePvP(
		CGameWorld *pGameWorld,
		int Owner,
		vec2 Pos,
		vec2 Dir,
        int Type);

		virtual void Tick() override;

protected:

		virtual void OnCollide(vec2 PrevPos, int TileIndex = 0, vec2 *pPreIntersectPos = nullptr, vec2 *pIntersectPos = nullptr, int *pTeleNr = nullptr) override;
    	virtual void OnCharacterCollide(vec2 PrevPos, CCharacter* pChar, vec2 *pIntersectPos = nullptr) override;

};

#endif