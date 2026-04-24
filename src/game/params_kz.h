// Copyright (C) Benjamín Gajardo (also known as +KZ)

#ifndef GAME_PARAMS_KZ_H
#define GAME_PARAMS_KZ_H

#include <base/vmath.h>

class CWorldCore;
class CCharacterCore;

//Collision params
struct SKZColGenericParams
{
	CCharacterCore *pCore = nullptr;
	CWorldCore *m_pOriginWorld = nullptr;

	SKZColGenericParams()
	{
		m_pOriginWorld = nullptr;
	}

	SKZColGenericParams(CWorldCore * OriginWorld)
	{
		m_pOriginWorld = OriginWorld;
	}
};
struct SKZColLaserParams // TODO: Do it for Projectiles too
{
	vec2 From;
	vec2 To;
	int Type;
	int OwnerId;
	int BounceNum;
	CWorldCore *m_pOriginWorld = nullptr;

	SKZColLaserParams()
	{
		m_pOriginWorld = nullptr;
	}

	SKZColLaserParams(CWorldCore * OriginWorld)
	{
		m_pOriginWorld = OriginWorld;
	}
};

struct SKZColProjectileParams
{
	vec2 *pProjPos = nullptr;
	int OwnerId = -1;
	vec2 *pOutCollision = nullptr;
	vec2 *pOutBeforeCollision = nullptr;
	int Weapon = -1;
	bool m_IsDDraceProjectile = false;
	bool *m_pDoResetTick = nullptr;
	CWorldCore *m_pOriginWorld = nullptr;

	SKZColProjectileParams()
	{
		m_pOriginWorld = nullptr;
	}

	SKZColProjectileParams(CWorldCore * OriginWorld)
	{
		m_pOriginWorld = OriginWorld;
	}
};

struct SKZColCharCoreParams : public SKZColGenericParams
{
	bool IsHook = false;
	bool IsWeapon = false;

	SKZColCharCoreParams() : SKZColGenericParams() {}
	SKZColCharCoreParams(CWorldCore * OriginWorld) : SKZColGenericParams(OriginWorld) {}
};

struct SKZColTeleWeaponParams : public SKZColLaserParams // IntersectLineTeleWeapon
{
	SKZColCharCoreParams *pCharCoreParams = nullptr;

	SKZColTeleWeaponParams() : SKZColLaserParams() {}
	SKZColTeleWeaponParams(CWorldCore * OriginWorld) : SKZColLaserParams(OriginWorld) {}
};

struct SKZColIntersectLineParams : public SKZColCharCoreParams // IntersectLine
{
	vec2 *pProjPos = nullptr;
	int Weapon = -1;
	bool m_IsDDraceProjectile = false;
	bool m_DoResetTick = false;

	SKZColIntersectLineParams() : SKZColCharCoreParams() {}
	SKZColIntersectLineParams(CWorldCore * OriginWorld) : SKZColCharCoreParams(OriginWorld) {}
};

//Entities Params (for entity creation)

struct SKZLaserParams
{
	bool m_IsRecoverJump = false;
};

#endif