#ifndef GAME_PARAMS_KZ_H
#define GAME_PARAMS_KZ_H

#include <base/vmath.h>

class CCharacterCore;

struct SKZColGenericParams
{
	CCharacterCore *pCore = nullptr;
};
struct SKZColLaserParams // TODO: Do it for Projectiles too
{
	vec2 From;
	vec2 To;
	int Type;
	int OwnerId;
	int BounceNum;
};

struct SKZColProjectileParams
{
	vec2 *pProjPos = nullptr;
	int OwnerId = -1;
	vec2 *pOutCollision = nullptr;
	vec2 *pOutBeforeCollision = nullptr;
	int Weapon = -1;
};

struct SKZColCharCoreParams : public SKZColGenericParams
{
	bool IsHook = false;
	bool IsWeapon = false;
};

struct SKZColTeleWeaponParams : public SKZColLaserParams // IntersectLineTeleWeapon
{
	SKZColCharCoreParams *pCharCoreParams = nullptr;
};

struct SKZColIntersectLineParams : public SKZColCharCoreParams // IntersectLine
{
	vec2 *pProjPos = nullptr;
	int Weapon = -1;
};

#endif