// Copyright (C) Benjamín Gajardo (also known as +KZ)
// FastIntersectLine Originally made by nheir, modified for Kaizo Network by +KZ
// FastIntersectLinePortalLaser based on FastIntersectLine by nheir
// IntersectCharacterCore based on IntersectCharacter from Teeworlds
// GetAnimationTransform m_Time OutOfRange InsideTriangle InsideQuad BarycentricCoordinates and Rotate were taken from Kaffeine's Infclass, modified by +KZ for Kaizo Network
// det() and AreLinesColliding() were taken from internet code examples

#include <base/math.h>
#include <base/system.h>
#include <base/vmath.h>

#include <antibot/antibot_data.h>

#include <cmath>
#include <vector>
#include <engine/map.h>

#include <game/collision.h>
#include <game/layers.h>
#include <game/mapitems.h>

#include <engine/shared/config.h>
#include "collision.h"
#include <game/kz/envelopeaccess.h>

int CCollision::GetCollisionAt(float x, float y, SKZColCharCoreParams *pCharCoreParams) const
{
    int i = GetTile(round_to_int(x), round_to_int(y));
    if(pCharCoreParams)
    {
        int j = CheckPointForCore(x, y, pCharCoreParams);
		if(j)
			return j;
    }
    return i;
}

int CCollision::CheckPointForCore(float x, float y, SKZColCharCoreParams *pCharCoreParams) const
{
	CCharacterCore *pCore = nullptr;
	bool IsHook = false;
	bool IsWeapon = false;

	if(pCharCoreParams && pCharCoreParams->pCore)
    {
		pCore = pCharCoreParams->pCore;
		IsHook = pCharCoreParams->IsHook;
		IsWeapon = pCharCoreParams->IsWeapon;

        if(m_pKZGame || m_pKZFront)
        {
            int Nx = std::clamp(round_to_int(x) / 32, 0, m_KZGameWidth - 1);
            int Ny = std::clamp(round_to_int(y) / 32, 0, m_KZGameHeight - 1);

            CKZTile* KZTile = GetKZGameTile(Nx, Ny);
            
            Nx = std::clamp(round_to_int(x) / 32, 0, m_KZFrontWidth - 1);
            Ny = std::clamp(round_to_int(y) / 32, 0, m_KZFrontHeight - 1);
            CKZTile* KZFrontTile = GetKZFrontTile(Nx, Ny);
            

            if(!KZTile && !KZFrontTile)
                return 0;

            if(KZTile)
            {
                if(KZTile->m_Index == KZ_TILE_SWITCHABLE && KZTile->m_Number && (KZTile->m_Value1 == TILE_SOLID || KZTile->m_Value1 == TILE_NOHOOK) && pCore->m_Id != -1 && pCore->m_pWorld && pCore->m_pTeams && !pCore->m_pWorld->m_vSwitchers.empty() && pCore->m_pWorld->m_vSwitchers[KZTile->m_Number].m_aStatus[pCore->m_pTeams->Team(pCore->m_Id)])
                {
                    pCore->m_SendCoreThisTick = true;
                    return KZTile->m_Value1;
                }

				if(!IsHook && !IsWeapon && KZTile->m_Index == KZ_GAMETILE_SWITCHABLE_TEE_ONLY && KZTile->m_Number && pCore->m_Id != -1 && pCore->m_pWorld && pCore->m_pTeams && !pCore->m_pWorld->m_vSwitchers.empty() && pCore->m_pWorld->m_vSwitchers[KZTile->m_Number].m_aStatus[pCore->m_pTeams->Team(pCore->m_Id)])
                {
                    pCore->m_SendCoreThisTick = true;
                    return TILE_NOHOOK;
                }

                if(KZTile->m_Index == KZ_TILE_SOLID_STOPPER && !IsHook && !IsWeapon)
                {
                    switch(KZTile->m_Flags)
                    {
                        case ROTATION_0:
                            {
                                if(pCore->m_Vel.y >=0)
                                {
                                    pCore->m_SendCoreThisTick = true;
                                    return TILE_SOLID;
                                }
                                break;
                            }
                        case ROTATION_90:
                            {
                                if(pCore->m_Vel.x <= 0)
                                {
                                    pCore->m_SendCoreThisTick = true;
                                    return TILE_SOLID;
                                }
                                break;
                            }
                        case ROTATION_180:
                            {
                                if(pCore->m_Vel.y <= 0)
                                {
                                    pCore->m_SendCoreThisTick = true;
                                    return TILE_SOLID;
                                }
                                break;
                            }
                        case ROTATION_270:
                            {
                                if(pCore->m_Vel.x >= 0)
                                {
                                    pCore->m_SendCoreThisTick = true;
                                    return TILE_SOLID;
                                }
                                break;
                            }
                    }
                }
            }

            if(KZFrontTile)
            {       
                if(KZFrontTile->m_Index == KZ_TILE_SWITCHABLE && KZFrontTile->m_Number && (KZFrontTile->m_Value1 == TILE_SOLID || KZFrontTile->m_Value1 == TILE_NOHOOK) && pCore->m_Id != -1 && pCore->m_pWorld && pCore->m_pTeams && !pCore->m_pWorld->m_vSwitchers.empty() && pCore->m_pWorld->m_vSwitchers[KZFrontTile->m_Number].m_aStatus[pCore->m_pTeams->Team(pCore->m_Id)])
                {
                    pCore->m_SendCoreThisTick = true;
                    return KZFrontTile->m_Value1;
                }

                if(KZFrontTile->m_Index == KZ_TILE_SOLID_STOPPER && !IsHook && !IsWeapon)
                {
                    switch(KZFrontTile->m_Flags)
                    {
                        case ROTATION_0:
                            {
                                if(pCore->m_Vel.y >=0)
                                {
                                    pCore->m_SendCoreThisTick = true;
                                    return TILE_SOLID;
                                }
                                break;
                            }
                        case ROTATION_90:
                            {
                                if(pCore->m_Vel.x <= 0)
                                {
                                    pCore->m_SendCoreThisTick = true;
                                    return TILE_SOLID;
                                }
                                break;
                            }
                        case ROTATION_180:
                            {
                                if(pCore->m_Vel.y <= 0)
                                {
                                    pCore->m_SendCoreThisTick = true;
                                    return TILE_SOLID;
                                }
                                break;
                            }
                        case ROTATION_270:
                            {
                                if(pCore->m_Vel.x >= 0)
                                {
                                    pCore->m_SendCoreThisTick = true;
                                    return TILE_SOLID;
                                }
                                break;
                            }
                    }
                }
            }
        }
    }
	return 0;
}

int CCollision::CheckPointForProjectile(vec2 Pos, SKZColProjectileParams *pProjectileParams) const
{
	if(!m_pWorldCore || !m_pTeamsCore || !pProjectileParams)
		return 0;

	if(!(pProjectileParams->pProjPos))
		return 0;

	CKZTile *pKZTile = GetKZGameTile(Pos.x, Pos.y);
	CKZTile *pKZFrontTile = GetKZFrontTile(Pos.x, Pos.y);
	if(!pKZTile && !pKZFrontTile)
		return 0;

	int OwnerId = pProjectileParams->OwnerId;
	int Weapon = pProjectileParams->Weapon;
	vec2 *pProjPos = pProjectileParams->pProjPos;

	if(pKZTile)
	{
		if(pKZTile->m_Index == KZ_GAMETILE_HITTABLE_SWITCH && pKZTile->m_Number && !m_pWorldCore->m_vSwitchers.empty() && OwnerId >= 0 && OwnerId < SERVER_MAX_CLIENTS)
		{
			bool hit = false;
			if(BitWiseAndInt64(pKZTile->m_Value3, KZ_HITTABLE_SWITCH_FLAG_GUN) && Weapon == WEAPON_GUN)
			{
				hit = true;
			}
			else if(BitWiseAndInt64(pKZTile->m_Value3, KZ_HITTABLE_SWITCH_FLAG_GRENADE) && Weapon == WEAPON_GRENADE)
			{
				hit = true;
			}
			else if(BitWiseAndInt64(pKZTile->m_Value3, KZ_HITTABLE_SWITCH_FLAG_LASER) && Weapon == WEAPON_LASER)
			{
				hit = true;
			}
			else if(BitWiseAndInt64(pKZTile->m_Value3, KZ_HITTABLE_SWITCH_FLAG_SHOTGUN) && Weapon == WEAPON_SHOTGUN)
			{
				hit = true;
			}

			if(hit)
			{
				switch(pKZTile->m_Value1) //Type
				{
					case 0: //switch deactivate
						{
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(OwnerId)] = true;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aEndTick[m_pTeamsCore->Team(OwnerId)] = 0;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aType[m_pTeamsCore->Team(OwnerId)] = TILE_SWITCHOPEN;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(OwnerId)] = m_pWorldCore->m_WorldTickKZ;
						}
						break;
					case 1: //switch timed deactivate
						{
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(OwnerId)] = true;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aEndTick[m_pTeamsCore->Team(OwnerId)] = m_pWorldCore->m_WorldTickKZ + 1 + pKZTile->m_Value2 * SERVER_TICK_SPEED;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aType[m_pTeamsCore->Team(OwnerId)] = TILE_SWITCHTIMEDOPEN;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(OwnerId)] = m_pWorldCore->m_WorldTickKZ;
						}
						break;
					case 2: //switch timed activate
						{
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(OwnerId)] = false;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aEndTick[m_pTeamsCore->Team(OwnerId)] = m_pWorldCore->m_WorldTickKZ + 1 + pKZTile->m_Value2 * SERVER_TICK_SPEED;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aType[m_pTeamsCore->Team(OwnerId)] = TILE_SWITCHTIMEDCLOSE;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(OwnerId)] = m_pWorldCore->m_WorldTickKZ;
						}
						break;
					case 3: //switch activate
						{
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(OwnerId)] = false;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aEndTick[m_pTeamsCore->Team(OwnerId)] = 0;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aType[m_pTeamsCore->Team(OwnerId)] = TILE_SWITCHCLOSE;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(OwnerId)] = m_pWorldCore->m_WorldTickKZ;
						}
						break;
					case 4: // +KZ switch toggle
						{
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(OwnerId)] = !m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(OwnerId)];
							if(m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(OwnerId)])
							{
								m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aType[m_pTeamsCore->Team(OwnerId)] = TILE_SWITCHOPEN;
							}
							else
							{
								m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aType[m_pTeamsCore->Team(OwnerId)] = TILE_SWITCHCLOSE;
							}
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aEndTick[m_pTeamsCore->Team(OwnerId)] = 0;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(OwnerId)] = m_pWorldCore->m_WorldTickKZ;
						}
						break;

					default:
						break;
				}

				return 1;
			}
		}
	}
	
	if(pKZFrontTile)
	{
		if(pKZFrontTile->m_Index == KZ_FRONTTILE_POS_SHIFTER && BitWiseAndInt64(pKZFrontTile->m_Value3, KZ_POS_SWITCHER_FLAG_PROJECTILE) && ((pKZFrontTile->m_Number && OwnerId >= 0 && OwnerId < SERVER_MAX_CLIENTS && !m_pWorldCore->m_vSwitchers.empty()) ? m_pWorldCore->m_vSwitchers[pKZFrontTile->m_Number].m_aStatus[m_pTeamsCore->Team(OwnerId)] : true))
		{
			*pProjPos += vec2(pKZFrontTile->m_Value1, pKZFrontTile->m_Value2);
			return -1;
		}
	}

	return 0;
}

int CCollision::CheckPointForLaser(vec2 Pos, SKZColLaserParams *pLaserParams) const
{
	if(!pLaserParams || !m_pWorldCore || !m_pTeamsCore)
		return 0;

	CKZTile *pKZTile = GetKZGameTile(Pos.x, Pos.y);
	CKZTile *pKZFrontTile = GetKZFrontTile(Pos.x, Pos.y);
	if(!pKZTile && !pKZFrontTile)
		return 0;

	if(pKZTile)
	{
		if(pKZTile->m_Index == KZ_GAMETILE_HITTABLE_SWITCH && pKZTile->m_Number && !m_pWorldCore->m_vSwitchers.empty() && pLaserParams->OwnerId >= 0 && pLaserParams->OwnerId < SERVER_MAX_CLIENTS)
		{
			bool hit = false;
			if(BitWiseAndInt64(pKZTile->m_Value3, KZ_HITTABLE_SWITCH_FLAG_GUN) && pLaserParams->Type == WEAPON_GUN)
			{
				hit = true;
			}
			else if(BitWiseAndInt64(pKZTile->m_Value3, KZ_HITTABLE_SWITCH_FLAG_GRENADE) && pLaserParams->Type == WEAPON_GRENADE)
			{
				hit = true;
			}
			else if(BitWiseAndInt64(pKZTile->m_Value3, KZ_HITTABLE_SWITCH_FLAG_LASER) && pLaserParams->Type == WEAPON_LASER)
			{
				hit = true;
			}
			else if(BitWiseAndInt64(pKZTile->m_Value3, KZ_HITTABLE_SWITCH_FLAG_SHOTGUN) && pLaserParams->Type == WEAPON_SHOTGUN)
			{
				hit = true;
			}

			if(hit)
			{
				switch(pKZTile->m_Value1) //Type
				{
					case 0: //switch deactivate
						{
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(pLaserParams->OwnerId)] = true;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aEndTick[m_pTeamsCore->Team(pLaserParams->OwnerId)] = 0;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aType[m_pTeamsCore->Team(pLaserParams->OwnerId)] = TILE_SWITCHOPEN;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(pLaserParams->OwnerId)] = m_pWorldCore->m_WorldTickKZ;
						}
						break;
					case 1: //switch timed deactivate
						{
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(pLaserParams->OwnerId)] = true;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aEndTick[m_pTeamsCore->Team(pLaserParams->OwnerId)] = m_pWorldCore->m_WorldTickKZ + 1 + pKZTile->m_Value2 * SERVER_TICK_SPEED;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aType[m_pTeamsCore->Team(pLaserParams->OwnerId)] = TILE_SWITCHTIMEDOPEN;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(pLaserParams->OwnerId)] = m_pWorldCore->m_WorldTickKZ;
						}
						break;
					case 2: //switch timed activate
						{
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(pLaserParams->OwnerId)] = false;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aEndTick[m_pTeamsCore->Team(pLaserParams->OwnerId)] = m_pWorldCore->m_WorldTickKZ + 1 + pKZTile->m_Value2 * SERVER_TICK_SPEED;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aType[m_pTeamsCore->Team(pLaserParams->OwnerId)] = TILE_SWITCHTIMEDCLOSE;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(pLaserParams->OwnerId)] = m_pWorldCore->m_WorldTickKZ;
						}
						break;
					case 3: //switch activate
						{
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(pLaserParams->OwnerId)] = false;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aEndTick[m_pTeamsCore->Team(pLaserParams->OwnerId)] = 0;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aType[m_pTeamsCore->Team(pLaserParams->OwnerId)] = TILE_SWITCHCLOSE;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(pLaserParams->OwnerId)] = m_pWorldCore->m_WorldTickKZ;
						}
						break;
					case 4: // +KZ switch toggle
						{
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(pLaserParams->OwnerId)] = !m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(pLaserParams->OwnerId)];
							if(m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aStatus[m_pTeamsCore->Team(pLaserParams->OwnerId)])
							{
								m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aType[m_pTeamsCore->Team(pLaserParams->OwnerId)] = TILE_SWITCHOPEN;
							}
							else
							{
								m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aType[m_pTeamsCore->Team(pLaserParams->OwnerId)] = TILE_SWITCHCLOSE;
							}
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aEndTick[m_pTeamsCore->Team(pLaserParams->OwnerId)] = 0;
							m_pWorldCore->m_vSwitchers[pKZTile->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(pLaserParams->OwnerId)] = m_pWorldCore->m_WorldTickKZ;
						}
						break;

					default:
						break;
				}

				pLaserParams->BounceNum = 21474836; //set to max value, no more bounces
				return 1;
			}
		}
	}

	return 0;
}

CKZTile *CCollision::GetKZGameTile(int Index) const
{
	return m_pKZGame ? &m_pKZGame[Index] : nullptr;
}

CKZTile *CCollision::GetKZGameTile(int x, int y) const
{
	return m_pKZGame ? &m_pKZGame[y * m_KZGameWidth + x] : nullptr;
}

CKZTile *CCollision::GetKZGameTile(float x, float y) const
{
    int Nx = std::clamp(round_to_int(x) / 32, 0, m_KZGameWidth - 1);
    int Ny = std::clamp(round_to_int(y) / 32, 0, m_KZGameHeight - 1);
    return m_pKZGame ? &m_pKZGame[Ny * m_KZGameWidth + Nx] : nullptr;
}

CKZTile *CCollision::GetKZFrontTile(int Index) const
{
	return m_pKZFront ? &m_pKZFront[Index] : nullptr;
}

CKZTile *CCollision::GetKZFrontTile(int x, int y) const
{
    return m_pKZFront ? &m_pKZFront[y * m_KZFrontWidth + x] : nullptr;
}

CKZTile *CCollision::GetKZFrontTile(float x, float y) const
{
    int Nx = std::clamp(round_to_int(x) / 32, 0, m_KZFrontWidth - 1);
    int Ny = std::clamp(round_to_int(y) / 32, 0, m_KZFrontHeight - 1);
    return m_pKZFront ? &m_pKZFront[Ny * m_KZFrontWidth + Nx] : nullptr;
}

unsigned char CCollision::GetKZGameTileIndex(float x, float y) const
{
	int Nx = std::clamp(round_to_int(x) / 32, 0, m_KZGameWidth - 1);
	int Ny = std::clamp(round_to_int(y) / 32, 0, m_KZGameHeight - 1);
	return GetKZGameTileIndex(Ny * m_KZGameWidth + Nx);
}

unsigned char CCollision::GetKZGameTileIndex(int x, int y) const
{
	return GetKZGameTileIndex(y * m_KZGameWidth + x);
}

unsigned char CCollision::GetKZGameTileIndex(int Index) const
{
	return m_pKZGame ? m_pKZGame[Index].m_Index : TILE_AIR;
}

unsigned char CCollision::GetKZFrontTileIndex(float x, float y) const
{
	int Nx = std::clamp(round_to_int(x) / 32, 0, m_KZFrontWidth - 1);
	int Ny = std::clamp(round_to_int(y) / 32, 0, m_KZFrontHeight - 1);
	return GetKZFrontTileIndex(Ny * m_KZFrontWidth + Nx);
}

unsigned char CCollision::GetKZFrontTileIndex(int x, int y) const
{
	return GetKZFrontTileIndex(y * m_KZFrontWidth + x);
}

unsigned char CCollision::GetKZFrontTileIndex(int Index) const
{
	return m_pKZFront ? m_pKZFront[Index].m_Index : TILE_AIR;
}

int CCollision::FastIntersectLine(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision, SKZColCharCoreParams *pCharCoreParams) const
{
	const int Tile0X = round_to_int(Pos0.x)/32;
	const int Tile0Y = round_to_int(Pos0.y)/32;
	const int Tile1X = round_to_int(Pos1.x)/32;
	const int Tile1Y = round_to_int(Pos1.y)/32;

	const float Ratio = (Tile0X == Tile1X) ? 1.f : (Pos1.y - Pos0.y) / (Pos1.x-Pos0.x);

	const float DetPos = Pos0.x * Pos1.y - Pos0.y * Pos1.x;

	const int DeltaTileX = (Tile0X <= Tile1X) ? 1 : -1;
	const int DeltaTileY = (Tile0Y <= Tile1Y) ? 1 : -1;

	const float DeltaError = DeltaTileY * DeltaTileX * Ratio;

	int CurTileX = Tile0X;
	int CurTileY = Tile0Y;
	vec2 Pos = Pos0;

	bool Vertical = false;

	float Error = 0;
	if(Tile0Y != Tile1Y && Tile0X != Tile1X)
	{
		Error = (CurTileX * Ratio - CurTileY - DetPos / (32*(Pos1.x-Pos0.x))) * DeltaTileY;
		if(Tile0X < Tile1X)
			Error += Ratio * DeltaTileY;
		if(Tile0Y < Tile1Y)
			Error -= DeltaTileY;
	}

	while(CurTileX != Tile1X || CurTileY != Tile1Y)
	{
		if(IsSolid(CurTileX*32,CurTileY*32)|| CheckPointForCore(CurTileX*32, CurTileY*32, pCharCoreParams))
			break;
		if(CurTileY != Tile1Y && (CurTileX == Tile1X || Error > 0))
		{
			CurTileY += DeltaTileY;
			Error -= 1;
			Vertical = false;
		}
		else
		{
			CurTileX += DeltaTileX;
			Error += DeltaError;
			Vertical = true;
		}
	}
    int kzid = 0;
	if(IsSolid(CurTileX*32,CurTileY*32)|| (kzid = CheckPointForCore(CurTileX*32, CurTileY*32, pCharCoreParams)))
	{
		if(CurTileX != Tile0X || CurTileY != Tile0Y)
		{
			if(Vertical)
			{
				Pos.x = 32 * (CurTileX + ((Tile0X < Tile1X) ? 0 : 1));
				Pos.y = (Pos.x * (Pos1.y - Pos0.y) - DetPos) / (Pos1.x - Pos0.x);
			}
			else
			{
				Pos.y = 32 * (CurTileY + ((Tile0Y < Tile1Y) ? 0 : 1));
				Pos.x = (Pos.y * (Pos1.x - Pos0.x) + DetPos) / (Pos1.y - Pos0.y);
			}
		}
		if(pOutCollision)
			*pOutCollision = Pos;
		if(pOutBeforeCollision)
		{
			vec2 Dir = normalize(Pos1-Pos0);
			if(Vertical)
				Dir *= 0.5f / absolute(Dir.x) + 1.f;
			else
				Dir *= 0.5f / absolute(Dir.y) + 1.f;
			*pOutBeforeCollision = Pos - Dir;
		}
        if(kzid)
            return kzid;
        else
		    return GetTile(CurTileX*32,CurTileY*32);
	}
	if(pOutCollision)
		*pOutCollision = Pos1;
	if(pOutBeforeCollision)
		*pOutBeforeCollision = Pos1;
	return 0;
}

int CCollision::FastIntersectLinePortalLaser(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision, CKZTile **pKZTile, int *pTeleNr, SKZColCharCoreParams *pCharCoreParams) const
{
	const int Tile0X = round_to_int(Pos0.x)/32;
	const int Tile0Y = round_to_int(Pos0.y)/32;
	const int Tile1X = round_to_int(Pos1.x)/32;
	const int Tile1Y = round_to_int(Pos1.y)/32;

	const float Ratio = (Tile0X == Tile1X) ? 1.f : (Pos1.y - Pos0.y) / (Pos1.x-Pos0.x);

	const float DetPos = Pos0.x * Pos1.y - Pos0.y * Pos1.x;

	const int DeltaTileX = (Tile0X <= Tile1X) ? 1 : -1;
	const int DeltaTileY = (Tile0Y <= Tile1Y) ? 1 : -1;

	const float DeltaError = DeltaTileY * DeltaTileX * Ratio;

	int CurTileX = Tile0X;
	int CurTileY = Tile0Y;
	vec2 Pos = Pos0;

	bool Vertical = false;

	float Error = 0;
	if(Tile0Y != Tile1Y && Tile0X != Tile1X)
	{
		Error = (CurTileX * Ratio - CurTileY - DetPos / (32*(Pos1.x-Pos0.x))) * DeltaTileY;
		if(Tile0X < Tile1X)
			Error += Ratio * DeltaTileY;
		if(Tile0Y < Tile1Y)
			Error -= DeltaTileY;
	}

    int Index = -1;
    int kzid = 0;
    CKZTile *pKZTilelocal = nullptr;
	while(CurTileX != Tile1X || CurTileY != Tile1Y)
	{
		if(IsSolid(CurTileX*32,CurTileY*32))
			break;

        Index = GetPureMapIndex(vec2(CurTileX*32,CurTileY*32));
		if(pTeleNr)
		{
			if(g_Config.m_SvOldTeleportWeapons && IsTeleport(Index))
				break;
			else if(IsTeleportWeapon(Index))
                break;
		}

        if((kzid = CheckPointForCore(CurTileX*32, CurTileY*32, pCharCoreParams)))
        {
            if(kzid == TILE_SOLID || kzid == TILE_NOHOOK)
                break;
            else
                kzid = 0;
        }

        if(pKZTile && !(g_Config.m_SvPortalMode == 2))
        {
            pKZTilelocal = GetKZGameTile(vec2(CurTileX*32,CurTileY*32));
            if(!pKZTilelocal || !(pKZTilelocal->m_Index == KZ_TILE_PORTAL_DISALLOW || pKZTilelocal->m_Index == KZ_TILE_PORTAL_RESET))
                pKZTilelocal = GetKZFrontTile(vec2(CurTileX*32,CurTileY*32));

            if(pKZTilelocal)
            {
                if(pKZTilelocal->m_Index == KZ_TILE_PORTAL_DISALLOW || pKZTilelocal->m_Index == KZ_TILE_PORTAL_RESET)
                {
                    break;
                }
                else
                {
                    pKZTilelocal = nullptr;
                }
            }
        }
        else if(g_Config.m_SvPortalMode == 2)
        {
            kzid = GetTileIndex(Index);
            if(!kzid || !(kzid == TILE_LFREEZE || kzid == TILE_LUNFREEZE))
                kzid = GetFrontTileIndex(Index);
            
            if(kzid == TILE_LFREEZE || kzid == TILE_LUNFREEZE)
                break;
            else
                kzid = 0;
        }

		if(CurTileY != Tile1Y && (CurTileX == Tile1X || Error > 0))
		{
			CurTileY += DeltaTileY;
			Error -= 1;
			Vertical = false;
		}
		else
		{
			CurTileX += DeltaTileX;
			Error += DeltaError;
			Vertical = true;
		}
	}
	if(IsSolid(CurTileX*32,CurTileY*32) || (pKZTilelocal && (pKZTilelocal->m_Index == KZ_TILE_PORTAL_DISALLOW || pKZTilelocal->m_Index == KZ_TILE_PORTAL_RESET)) || (kzid ? kzid : (kzid = CheckPointForCore(CurTileX*32, CurTileY*32, pCharCoreParams))) || (g_Config.m_SvOldTeleportWeapons ? IsTeleport(Index) : IsTeleportWeapon(Index)))
	{
		if(CurTileX != Tile0X || CurTileY != Tile0Y)
		{
			if(Vertical)
			{
				Pos.x = 32 * (CurTileX + ((Tile0X < Tile1X) ? 0 : 1));
				Pos.y = (Pos.x * (Pos1.y - Pos0.y) - DetPos) / (Pos1.x - Pos0.x);
			}
			else
			{
				Pos.y = 32 * (CurTileY + ((Tile0Y < Tile1Y) ? 0 : 1));
				Pos.x = (Pos.y * (Pos1.x - Pos0.x) + DetPos) / (Pos1.y - Pos0.y);
			}
		}
		if(pOutCollision)
			*pOutCollision = Pos;
		if(pOutBeforeCollision)
		{
			vec2 Dir = normalize(Pos1-Pos0);
			if(Vertical)
				Dir *= 0.5f / absolute(Dir.x) + 1.f;
			else
				Dir *= 0.5f / absolute(Dir.y) + 1.f;
			*pOutBeforeCollision = Pos - Dir;
		}
        if(pTeleNr)
		{
			if(g_Config.m_SvOldTeleportWeapons)
				*pTeleNr = IsTeleport(Index);
			else
				*pTeleNr = IsTeleportWeapon(Index);
		}

        if(pKZTilelocal)
        {
            if(pKZTilelocal->m_Index == KZ_TILE_PORTAL_DISALLOW || pKZTilelocal->m_Index == KZ_TILE_PORTAL_RESET)
            {
                *pKZTile = pKZTilelocal;
            }
        }
        else
        {
            pKZTilelocal = GetKZGameTile(vec2(CurTileX*32,CurTileY*32));
            if(!pKZTilelocal || !(pKZTilelocal->m_Index == KZ_TILE_PORTAL_ALLOW))
                pKZTilelocal = GetKZFrontTile(vec2(CurTileX*32,CurTileY*32));

            if(pKZTilelocal)
            {
                if(pKZTilelocal->m_Index == KZ_TILE_PORTAL_ALLOW)
                {
                    *pKZTile = pKZTilelocal;
                }
            }
        }

		if(pTeleNr && *pTeleNr)
		{
			return TILE_TELEINWEAPON;
		}
        if(kzid)
            return kzid;
        else
		    return GetTile(CurTileX*32,CurTileY*32);
	}
	if(pOutCollision)
		*pOutCollision = Pos1;
	if(pOutBeforeCollision)
		*pOutBeforeCollision = Pos1;
	return 0;
}

bool CCollision::DDNetLayerExists(int Layer)
{
	switch(Layer)
	{
	case LAYER_GAME:
		return m_pTiles;
	case LAYER_FRONT:
		return m_pFront;
	case LAYER_SWITCH:
		return m_pSwitch;
	case LAYER_TELE:
		return m_pTele;
	case LAYER_SPEEDUP:
		return m_pSpeedup;
	case LAYER_TUNE:
		return m_pTune;
	default:
		return false;
	}
}

CPortalCore *CCollision::IntersectCharacterWithPortal(vec2 Pos, CCharacterCore *pCore) const
{
    if(!pCore)
	    return nullptr;
    
    if(!pCore->m_pWorld)
        return nullptr;

	if(!pCore->m_pTeams)
		return nullptr;

    vec2 TempPos;
    vec2 SavedPos;

    SavedPos = pCore->m_Pos;
    pCore->m_Pos = Pos;
    
    for(int i = 0; i < MAX_CLIENTS;i++)
    {
        for(int j = 0; j < 2; j++)
        {
            CPortalCore *pPortal = pCore->m_pWorld->GetPortalKZ(i,j);

            if(!pPortal)
                continue;
			
			if(pPortal->m_Team != pCore->m_pTeams->Team(pCore->m_Id))
				continue;

			if(!pCore->m_pTeams->CanCollide(pPortal->m_OwnerId, pCore->m_Id))
				continue;

            if(IntersectCharacterCore(pPortal->m_Pos,pPortal->m_Pos2,0.f,TempPos,pCore))
            {
                pCore->m_Pos = SavedPos;
                return pPortal;
            }
        }
    }
    pCore->m_Pos = SavedPos;
    return nullptr;
}

CCharacterCore *CCollision::IntersectCharacterCore(vec2 Pos0, vec2 Pos1, float Radius, vec2 &NewPos, CCharacterCore *pThisOnly) const
{
	float ClosestLen = distance(Pos0, Pos1) * 100.0f;
	CCharacterCore *pClosest = nullptr;
	

	if(!pThisOnly)
		return nullptr;

	vec2 IntersectPos;
	if(closest_point_on_line(Pos0, Pos1, pThisOnly->m_Pos, IntersectPos))
	{
		float Len = distance(pThisOnly->m_Pos, IntersectPos);
		if(Len < (pThisOnly->PhysicalSize() +1) / 2 + Radius)
		{
			Len = distance(Pos0, IntersectPos);
			if(Len < ClosestLen)
			{
				NewPos = IntersectPos;
				ClosestLen = Len;
				pClosest = pThisOnly;
			}
		}
	}
	

	return pClosest;
}

bool CCollision::HandlePortalCollision(vec2 &InOutPos, vec2 &InOutVel, CCharacterCore *pCore) const
{
    if(!pCore)
        return false;
    
    if(!pCore->m_pWorld)
        return false;
    
    if(pCore->m_Id < 0 || pCore->m_Id > MAX_CLIENTS)
        return false;

    CPortalCore *pPortal = IntersectCharacterWithPortal(InOutPos,pCore);

    if(!pPortal)
    {
        pPortal = IntersectCharacterWithPortal(InOutPos + InOutVel,pCore);
    }

    if(pPortal)
    {
        CPortalCore *pOtherPortal = pCore->m_pWorld->GetPortalKZ(pPortal->m_OwnerId,!pPortal->m_IsBlue);

        if(pOtherPortal)
        {
            bool dotele = false;
            vec2 OutVel = pCore->m_Vel;
			vec2 OutPos = pCore->m_Pos;

			if((pPortal->m_Pos.x == pPortal->m_Pos2.x && pOtherPortal->m_Pos.x != pOtherPortal->m_Pos2.x)||(pPortal->m_Pos.x != pPortal->m_Pos2.x && pOtherPortal->m_Pos.x == pOtherPortal->m_Pos2.x))
			{
				vec2 temp = OutVel;
				OutVel.x = temp.y;
				OutVel.y = temp.x;
			}

			if(pOtherPortal->m_Pos.x == pOtherPortal->m_Pos2.x)
			{
				if(IsTeleportViable(vec2(pOtherPortal->m_Pos.x+32.f,pOtherPortal->m_Pos.y+32.f)))
				{
					if(OutVel.x < 0)
					{
						OutVel.x *= -1;
					}
					OutPos = vec2(pOtherPortal->m_Pos.x+32.f,pOtherPortal->m_Pos.y+32.f);
					dotele = true;
				}
				else if(IsTeleportViable(vec2(pOtherPortal->m_Pos.x-32.f,pOtherPortal->m_Pos.y+32.f)))
				{
					if(OutVel.x > 0)
					{
						OutVel.x *= -1;
					}
					OutPos = vec2(pOtherPortal->m_Pos.x-32.f,pOtherPortal->m_Pos.y+32.f);
					dotele = true;
				}
				
			}
			else
			{
				if(IsTeleportViable(vec2(pOtherPortal->m_Pos.x+32.f,pOtherPortal->m_Pos.y+32.f)))
				{
					if(OutVel.y < 0)
					{
						OutVel.y *= -1;
					}
					OutPos = vec2(pOtherPortal->m_Pos.x+32.f,pOtherPortal->m_Pos.y+32.f);
					dotele = true;
				}
				else if(IsTeleportViable(vec2(pOtherPortal->m_Pos.x+32.f,pOtherPortal->m_Pos.y-32.f)))
				{
					if(OutVel.y > 0)
					{
						OutVel.y *= -1;
					}
					OutPos = vec2(pOtherPortal->m_Pos.x+32.f,pOtherPortal->m_Pos.y-32.f);
					dotele = true;
				}
				
			}

			if(dotele)
			{
				InOutPos = OutPos;
				InOutVel = OutVel;
                return true;
			}
        }
    }
    return false;
}

bool CCollision::IsTeleportViable(vec2 Pos) const
{
	return !(CheckPoint(Pos.x + 14.0f,Pos.y + 14.0f) || CheckPoint(Pos.x - 14.0f,Pos.y + 14.0f) || CheckPoint(Pos.x + 14.0f,Pos.y - 14.0f) || CheckPoint(Pos.x - 14.0f,Pos.y - 14.0f));
}

bool CCollision::TestBoxKZ(vec2 OrigPos, vec2 *pInOutPos, vec2 *pInOutVel, vec2 Size, float ElasticityX, float ElasticityY, bool *pGrounded, CCharacterCore *pCore) const
{
	if(!pCore || !m_pWorldCore || !m_pTeamsCore)
		return false;

	Size *= 0.5f;
	CKZTile *pKZTile[4] = {nullptr,nullptr,nullptr,nullptr};
	bool collide = false;
	if(pKZTile[0] = GetKZGameTile(pInOutPos->x - Size.x, pInOutPos->y - Size.y))
		collide = true;
	if(pKZTile[1] = GetKZGameTile(pInOutPos->x + Size.x, pInOutPos->y - Size.y))
		collide = true;
	if(pKZTile[2] = GetKZGameTile(pInOutPos->x - Size.x, pInOutPos->y + Size.y))
		collide = true;
	if(pKZTile[3] = GetKZGameTile(pInOutPos->x + Size.x, pInOutPos->y + Size.y))
		collide = true;

	bool updatedpos = false;
	CKZTile *pPrevKZHITTABLESwitch = nullptr;
	if(collide)
	{
		for(int i = 0; i < 4; i++)
		{
			if(pKZTile[i])
			{
				if(pPrevKZHITTABLESwitch != pKZTile[i] && pCore->pTouchingKZTiles[0] != pKZTile[i] && pCore->pTouchingKZTiles[1] != pKZTile[i] && pCore->pTouchingKZTiles[2] != pKZTile[i]&& pCore->pTouchingKZTiles[3] != pKZTile[i] && pKZTile[i]->m_Index == KZ_GAMETILE_HITTABLE_SWITCH && BitWiseAndInt64(pKZTile[i]->m_Value3,KZ_HITTABLE_SWITCH_FLAG_NINJA) && pKZTile[i]->m_Number && !m_pWorldCore->m_vSwitchers.empty() && pCore->m_ActiveWeapon == WEAPON_NINJA && pCore->m_Ninja.m_CurrentMoveTime > 0)
				{
					switch(pKZTile[i]->m_Value1) //Type
					{
						case 0: //switch deactivate
							{
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aStatus[m_pTeamsCore->Team(pCore->m_Id)] = true;
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aEndTick[m_pTeamsCore->Team(pCore->m_Id)] = 0;
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aType[m_pTeamsCore->Team(pCore->m_Id)] = TILE_SWITCHOPEN;
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(pCore->m_Id)] = m_pWorldCore->m_WorldTickKZ;
							}
							break;
						case 1: //switch timed deactivate
							{
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aStatus[m_pTeamsCore->Team(pCore->m_Id)] = true;
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aEndTick[m_pTeamsCore->Team(pCore->m_Id)] = m_pWorldCore->m_WorldTickKZ + 1 + pKZTile[i]->m_Value2 * SERVER_TICK_SPEED;
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aType[m_pTeamsCore->Team(pCore->m_Id)] = TILE_SWITCHTIMEDOPEN;
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(pCore->m_Id)] = m_pWorldCore->m_WorldTickKZ;
							}
							break;
						case 2: //switch timed activate
							{
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aStatus[m_pTeamsCore->Team(pCore->m_Id)] = false;
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aEndTick[m_pTeamsCore->Team(pCore->m_Id)] = m_pWorldCore->m_WorldTickKZ + 1 + pKZTile[i]->m_Value2 * SERVER_TICK_SPEED;
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aType[m_pTeamsCore->Team(pCore->m_Id)] = TILE_SWITCHTIMEDCLOSE;
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(pCore->m_Id)] = m_pWorldCore->m_WorldTickKZ;
							}
							break;
						case 3: //switch activate
							{
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aStatus[m_pTeamsCore->Team(pCore->m_Id)] = false;
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aEndTick[m_pTeamsCore->Team(pCore->m_Id)] = 0;
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aType[m_pTeamsCore->Team(pCore->m_Id)] = TILE_SWITCHCLOSE;
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(pCore->m_Id)] = m_pWorldCore->m_WorldTickKZ;
							}
							break;
						case 4: // +KZ switch toggle
							{
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aStatus[m_pTeamsCore->Team(pCore->m_Id)] = !m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aStatus[m_pTeamsCore->Team(pCore->m_Id)];
								if(m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aStatus[m_pTeamsCore->Team(pCore->m_Id)])
								{
									m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aType[m_pTeamsCore->Team(pCore->m_Id)] = TILE_SWITCHOPEN;
								}
								else
								{
									m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aType[m_pTeamsCore->Team(pCore->m_Id)] = TILE_SWITCHCLOSE;
								}
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aEndTick[m_pTeamsCore->Team(pCore->m_Id)] = 0;
								m_pWorldCore->m_vSwitchers[pKZTile[i]->m_Number].m_aLastUpdateTick[m_pTeamsCore->Team(pCore->m_Id)] = m_pWorldCore->m_WorldTickKZ;
							}
							break;

						default:
							break;
					}
					pPrevKZHITTABLESwitch = pKZTile[i];
					continue;
				}

				if(pKZTile[i]->m_Index == KZ_GAMETILE_SOLID_STOPPER_V2 && pCore->m_pTeams && pCore->m_pWorld && ((pKZTile[i]->m_Number && !pCore->m_pWorld->m_vSwitchers.empty()) ? pCore->m_pWorld->m_vSwitchers[pKZTile[i]->m_Number].m_aStatus[pCore->m_pTeams->Team(pCore->m_Id)] : true))
				{
					switch(pKZTile[i]->m_Flags)
					{
						case ROTATION_0:
							{
								if(pInOutVel->y >= 0)
								{
									pInOutVel->y *= -ElasticityY;
									pInOutPos->y = OrigPos.y;
									updatedpos = true;
									pCore->m_SendCoreThisTick = true;
									if(pGrounded && ElasticityY > 0)
										*pGrounded = true;
								}
								break;
							}
						case ROTATION_90:
							{
								if(pInOutVel->x <= 0)
								{
									pInOutVel->x *= -ElasticityX;
									pInOutPos->x = OrigPos.x;
									updatedpos = true;
									pCore->m_SendCoreThisTick = true;
								}
								break;
							}
						case ROTATION_180:
							{
								if(pInOutVel->y <= 0)
								{
									pInOutVel->y *= -ElasticityY;
									pInOutPos->y = OrigPos.y;
									updatedpos = true;
									pCore->m_SendCoreThisTick = true;
								}
								break;
							}
						case ROTATION_270:
							{
								if(pInOutVel->x >= 0)
								{
									pInOutVel->x *= -ElasticityX;
									pInOutPos->x = OrigPos.x;
									updatedpos = true;
									pCore->m_SendCoreThisTick = true;
								}
								break;
							}
					}
					continue;
				}
			}
		}

		for(int i = 0; i < 4; i++)
		{
			pCore->pTouchingKZTiles[i] = pKZTile[i];
		}

		return updatedpos;
	}

	return false;
}

void CCollision::UpdateQuadCache()
{
	for(int i = 0; i < m_aKZQuads.size(); i++)
	{

		//Get moved quad pos

		vec2 Position = vec2(0,0);

		GetAnimationTransform(m_Time + (m_aKZQuads[i].m_pQuad->m_PosEnvOffset / 1000.0), m_aKZQuads[i].m_pQuad->m_PosEnv, Position, m_aKZQuads[i].m_CachedAngle);

		//Set Center first
		m_aKZQuads[i].m_CachedPos[4] = vec2(fx2f(m_aKZQuads[i].m_pQuad->m_aPoints[4].x), fx2f(m_aKZQuads[i].m_pQuad->m_aPoints[4].y)) + Position;

		//Cache corners
		for(int j = 0; j < 4; j ++)
		{
			m_aKZQuads[i].m_CachedPos[j] = Position + vec2(fx2f(m_aKZQuads[i].m_pQuad->m_aPoints[j].x), fx2f(m_aKZQuads[i].m_pQuad->m_aPoints[j].y));
		}

		//Rotate Cached corners
		if(m_aKZQuads[i].m_CachedAngle != 0)
		{
			Rotate(m_aKZQuads[i].m_CachedPos[4], &m_aKZQuads[i].m_CachedPos[0], m_aKZQuads[i].m_CachedAngle);
			Rotate(m_aKZQuads[i].m_CachedPos[4], &m_aKZQuads[i].m_CachedPos[1], m_aKZQuads[i].m_CachedAngle);
			Rotate(m_aKZQuads[i].m_CachedPos[4], &m_aKZQuads[i].m_CachedPos[2], m_aKZQuads[i].m_CachedAngle);
			Rotate(m_aKZQuads[i].m_CachedPos[4], &m_aKZQuads[i].m_CachedPos[3], m_aKZQuads[i].m_CachedAngle);
		}
	}
}

std::vector<SKZQuadData *> CCollision::GetQuadsAt(vec2 Pos)
{
	std::vector<SKZQuadData *> apQuads;

	for(int i = 0; i < m_aKZQuads.size(); i++)
	{
		if(OutOfRange(Pos.x, m_aKZQuads[i].m_CachedPos[0].x, m_aKZQuads[i].m_CachedPos[1].x, m_aKZQuads[i].m_CachedPos[2].x, m_aKZQuads[i].m_CachedPos[3].x))
			continue;
		if(OutOfRange(Pos.y, m_aKZQuads[i].m_CachedPos[0].y, m_aKZQuads[i].m_CachedPos[1].y, m_aKZQuads[i].m_CachedPos[2].y, m_aKZQuads[i].m_CachedPos[3].y))
			continue;

		if(InsideQuad(m_aKZQuads[i].m_CachedPos[0], m_aKZQuads[i].m_CachedPos[1], m_aKZQuads[i].m_CachedPos[2], m_aKZQuads[i].m_CachedPos[3], Pos))
		{
			apQuads.push_back(&m_aKZQuads[i]);
		}
	}

	return apQuads;
}

int CCollision::QuadTypeToTileId(int Type)
{
	switch (Type)
	{
	case KZQUADTYPE_FREEZE:
		return TILE_FREEZE;
	case KZQUADTYPE_UNFREEZE:
		return TILE_UNFREEZE;
	case KZQUADTYPE_HOOK:
		return TILE_SOLID;
	case KZQUADTYPE_UNHOOK:
		return TILE_NOHOOK;
	case KZQUADTYPE_STOPA:
		return TILE_STOPA;
	case KZQUADTYPE_DEATH:
		return TILE_DEATH;
	case KZQUADTYPE_CFRM:
		return TILE_TELECHECKINEVIL;
	}
	return TILE_AIR;
}

void CCollision::PushBoxOutsideQuads(vec2 *pPos, vec2 *pInOutVel, vec2 Size, CCharacterCore * pCore, bool * pGrounded)
{
	vec2 BoxCorners[4];

	Size *= 0.5f;

	//vertically first, then horizontally

	bool exit = false;
	bool horizontal = false;
	do
	{

		if(!horizontal)
		{
			BoxCorners[0].x = pPos->x - Size.x + 1;
			BoxCorners[0].y = pPos->y - Size.y + pInOutVel->y;

			BoxCorners[1].x = pPos->x + Size.x - 1;
			BoxCorners[1].y = pPos->y - Size.y + pInOutVel->y;

			BoxCorners[2].x = pPos->x - Size.x + 1;
			BoxCorners[2].y = pPos->y + Size.y + pInOutVel->y;

			BoxCorners[3].x = pPos->x + Size.x - 1;
			BoxCorners[3].y = pPos->y + Size.y + pInOutVel->y;
		}
		else
		{
			BoxCorners[0].x = pPos->x - Size.x + pInOutVel->x;
			BoxCorners[0].y = pPos->y - Size.y + pInOutVel->y + 1;

			BoxCorners[1].x = pPos->x + Size.x + pInOutVel->x;
			BoxCorners[1].y = pPos->y - Size.y + pInOutVel->y - 1;

			BoxCorners[2].x = pPos->x - Size.x + pInOutVel->x;
			BoxCorners[2].y = pPos->y + Size.y + pInOutVel->y + 1;

			BoxCorners[3].x = pPos->x + Size.x + pInOutVel->x;
			BoxCorners[3].y = pPos->y + Size.y + pInOutVel->y - 1;
		}

		bool docontinue;
		for(int i = 0; i < m_aKZQuads.size(); i++)
		{
			if(m_aKZQuads[i].m_Type != KZQUADTYPE_HOOK && m_aKZQuads[i].m_Type != KZQUADTYPE_UNHOOK && m_aKZQuads[i].m_Type != KZQUADTYPE_STOPA)
				continue;

			docontinue = true;
			for(int j = 0; j < 4; j++)
			{
				if(!OutOfRange(BoxCorners[j].x, m_aKZQuads[i].m_CachedPos[0].x, m_aKZQuads[i].m_CachedPos[1].x, m_aKZQuads[i].m_CachedPos[2].x, m_aKZQuads[i].m_CachedPos[3].x) && !OutOfRange(BoxCorners[j].y, m_aKZQuads[i].m_CachedPos[0].y, m_aKZQuads[i].m_CachedPos[1].y, m_aKZQuads[i].m_CachedPos[2].y, m_aKZQuads[i].m_CachedPos[3].y))
				{
					docontinue = false;
					break;
				}
			}

			if(docontinue)
				continue;

			
				if(!horizontal)
				{
					float altdown[4] = {-9999,-9999,-9999,-9999};
					
					//commented, always do vertical collision for all lines
					//if(std::abs(m_aKZQuads[i].m_CachedPos[0].x - m_aKZQuads[i].m_CachedPos[1].x) >= std::abs(m_aKZQuads[i].m_CachedPos[0].y - m_aKZQuads[i].m_CachedPos[1].y))
						altdown[0] = CalculateSlopeAltitude(BoxCorners[2].x, BoxCorners[3].x, m_aKZQuads[i].m_CachedPos[0], m_aKZQuads[i].m_CachedPos[1]);
					//if(std::abs(m_aKZQuads[i].m_CachedPos[1].x - m_aKZQuads[i].m_CachedPos[3].x) >= std::abs(m_aKZQuads[i].m_CachedPos[1].y - m_aKZQuads[i].m_CachedPos[3].y))
						altdown[1] = CalculateSlopeAltitude(BoxCorners[2].x, BoxCorners[3].x, m_aKZQuads[i].m_CachedPos[1], m_aKZQuads[i].m_CachedPos[3]);
					//if(std::abs(m_aKZQuads[i].m_CachedPos[3].x - m_aKZQuads[i].m_CachedPos[2].x) >= std::abs(m_aKZQuads[i].m_CachedPos[3].y - m_aKZQuads[i].m_CachedPos[2].y))
						altdown[2] = CalculateSlopeAltitude(BoxCorners[2].x, BoxCorners[3].x, m_aKZQuads[i].m_CachedPos[3], m_aKZQuads[i].m_CachedPos[2]);
					//if(std::abs(m_aKZQuads[i].m_CachedPos[2].x - m_aKZQuads[i].m_CachedPos[0].x) >= std::abs(m_aKZQuads[i].m_CachedPos[2].y - m_aKZQuads[i].m_CachedPos[0].y))
						altdown[3] = CalculateSlopeAltitude(BoxCorners[2].x, BoxCorners[3].x, m_aKZQuads[i].m_CachedPos[2], m_aKZQuads[i].m_CachedPos[0]);
					
					float finalaltdown = altdown[0];

					for(int k = 1; k < 4; k++)
					{
						if(std::abs(finalaltdown - pPos->y) > std::abs(altdown[k] - pPos->y))
							finalaltdown = altdown[k];
					}

					if (finalaltdown == -9999)
					{
						continue;
					}

					if(finalaltdown <= pPos->y + Size.y && finalaltdown > pPos->y)
					{
						pPos->y = finalaltdown - Size.y;
						if(pInOutVel->y > 0)
							pInOutVel->y = 0;

						if(pGrounded && m_aKZQuads[i].m_Type != KZQUADTYPE_STOPA) //set grounded if not stopper quad
						{
							*pGrounded = true;
						}
					}
					else if(finalaltdown >= pPos->y - Size.y && finalaltdown < pPos->y)
					{
						pPos->y = finalaltdown + Size.y;
						if(pInOutVel->y < 0)
							pInOutVel->y = 0;
					}
				}
				else
				{

					float altdown[4] = {-9999,-9999,-9999,-9999};
					
					if(std::abs(m_aKZQuads[i].m_CachedPos[0].x - m_aKZQuads[i].m_CachedPos[1].x) < std::abs(m_aKZQuads[i].m_CachedPos[0].y - m_aKZQuads[i].m_CachedPos[1].y))
						altdown[0] = CalculateSlopeAltitudeSide(BoxCorners[1].y, BoxCorners[3].y, m_aKZQuads[i].m_CachedPos[0], m_aKZQuads[i].m_CachedPos[1]);
					if(std::abs(m_aKZQuads[i].m_CachedPos[1].x - m_aKZQuads[i].m_CachedPos[3].x) < std::abs(m_aKZQuads[i].m_CachedPos[1].y - m_aKZQuads[i].m_CachedPos[3].y))
						altdown[1] = CalculateSlopeAltitudeSide(BoxCorners[1].y, BoxCorners[3].y, m_aKZQuads[i].m_CachedPos[1], m_aKZQuads[i].m_CachedPos[3]);
					if(std::abs(m_aKZQuads[i].m_CachedPos[3].x - m_aKZQuads[i].m_CachedPos[2].x) < std::abs(m_aKZQuads[i].m_CachedPos[3].y - m_aKZQuads[i].m_CachedPos[2].y))
						altdown[2] = CalculateSlopeAltitudeSide(BoxCorners[1].y, BoxCorners[3].y, m_aKZQuads[i].m_CachedPos[3], m_aKZQuads[i].m_CachedPos[2]);
					if(std::abs(m_aKZQuads[i].m_CachedPos[2].x - m_aKZQuads[i].m_CachedPos[0].x) < std::abs(m_aKZQuads[i].m_CachedPos[2].y - m_aKZQuads[i].m_CachedPos[0].y))
						altdown[3] = CalculateSlopeAltitudeSide(BoxCorners[1].y, BoxCorners[3].y, m_aKZQuads[i].m_CachedPos[2], m_aKZQuads[i].m_CachedPos[0]);
					
					float finalaltdown = altdown[0];

					for(int k = 1; k < 4; k++)
					{
						if(std::abs(finalaltdown - pPos->x) > std::abs(altdown[k] - pPos->x))
							finalaltdown = altdown[k];
					}

					if (finalaltdown == -9999)
					{
						continue;
					}

					if(finalaltdown <= pPos->x + Size.x && finalaltdown > pPos->x)
					{
						pPos->x = finalaltdown - Size.x;
						if(pInOutVel->x > 0)
							pInOutVel->x = 0;
					}
					else if(finalaltdown >= pPos->x - Size.x && finalaltdown < pPos->x)
					{
						pPos->x = finalaltdown + Size.x;
						if(pInOutVel->x < 0)
							pInOutVel->x = 0;
					}
				}

				if(pCore)
				{
					pCore->m_SendCoreThisTick = true;
				}
			
		}

		if(!horizontal)
			horizontal = true;
		else
			exit = true;
	} while(!exit);
	
}

static inline float det(float a, float b, float c, float d) {
    return a * d - b * c;
}

bool CCollision::AreLinesColliding(vec2 p1, vec2 p2, vec2 p3, vec2 p4, vec2 *interseccion) {
    float dx1 = p2.x - p1.x;
    float dy1 = p2.y - p1.y;
    float dx2 = p4.x - p3.x;
    float dy2 = p4.y - p3.y;

    float denom = det(dx1, dy1, dx2, dy2);

    if (fabsf(denom) < 1e-6f) {
        return 0;
    }

    float dx = p3.x - p1.x;
    float dy = p3.y - p1.y;

    float t = det(dx, dy, dx2, dy2) / denom;
    float u = det(dx, dy, dx1, dy1) / denom;

    if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
        if (interseccion) {
            interseccion->x = p1.x + t * dx1;
            interseccion->y = p1.y + t * dy1;
        }
        return 1;
    }

    return 0;
}

bool CCollision::IntersectQuad(vec2 From, vec2 To, vec2 *pOut, vec2 *pLineStart, vec2 *pLineEnd, vec2 pos1, vec2 pos2, vec2 pos3, vec2 pos4)
{
	vec2 intersect[4];
	bool intersected[4] = {false,false,false,false};

	intersected[0] = AreLinesColliding(From,To,pos1,pos2, &intersect[0]);
	intersected[1] = AreLinesColliding(From,To,pos2,pos4, &intersect[1]);
	intersected[2] = AreLinesColliding(From,To,pos4,pos3, &intersect[2]);
	intersected[3] = AreLinesColliding(From,To,pos3,pos1, &intersect[3]);

	if(pOut || pLineStart || pLineEnd)
	{
		bool intersectedbool = false;
		int best = -1;
		
		if(intersected[0])
		{
			best = 0;
			intersectedbool = true;
		}

		for(int i = 1; i < 4;i++)
		{
			if(!intersected[i])
				continue;

			intersectedbool = true;

			if(best == -1)
			{
				best = i;
				continue;
			}

			if(distance(From,intersect[best]) > distance(From,intersect[i]))
			{
				best = i;
			}
		}

		if(best == -1)
		{
			if(pOut)
				*pOut = To;
		}
		else
		{
			if(pOut)
				*pOut = intersect[best];
			if(pLineStart)
			{
				switch(best)
				{
					case 0:
						*pLineStart = pos1;
						break;
					case 1:
						*pLineStart = pos2;
						break;
					case 2:
						*pLineStart = pos4;
						break;
					case 3:
						*pLineStart = pos3;
						break;
				}
			}
			if(pLineEnd)
			{
				switch(best)
				{
					case 0:
						*pLineEnd = pos2;
						break;
					case 1:
						*pLineEnd = pos4;
						break;
					case 2:
						*pLineEnd = pos3;
						break;
					case 3:
						*pLineEnd = pos1;
						break;
				}
			}
		}
		return intersectedbool;
	}
	else
	{
		for(int i = 0; i < 4; i++)
		{
			if(intersected[i])
				return true;
		}
		return false;
	}
}

SKZQuadData * CCollision::IntersectQuad(vec2 From, vec2 To, vec2 *pOut, vec2 *pLineStart, vec2 *pLineEnd)
{
	SKZQuadData * pQuad = nullptr;

	for(int i = 0; i < m_aKZQuads.size(); i++)
	{
		if(m_aKZQuads[i].m_Type != KZQUADTYPE_HOOK && m_aKZQuads[i].m_Type != KZQUADTYPE_UNHOOK)
			continue;

		if(IntersectQuad(From,To,pOut,pLineStart,pLineStart,m_aKZQuads[i].m_CachedPos[0],m_aKZQuads[i].m_CachedPos[1],m_aKZQuads[i].m_CachedPos[2],m_aKZQuads[i].m_CachedPos[3]))
		{
			pQuad = &m_aKZQuads[i];
			break;
		}
	}

	return pQuad;
}

vec2 CCollision::ReflexLineOnLine(vec2 Point, vec2 Center, vec2 P1)
{
	float wallangle = atan2(Center.x - P1.x,Center.y - P1.y);

	float lineangle = atan2(Point.x - Center.x,Point.y - Center.y);

	float bounceangle = lineangle - wallangle;
	
	bounceangle = 3.14159f - bounceangle;

	bounceangle += wallangle;

	Point = vec2(sin(bounceangle),cos(bounceangle));

	return Point;
}

float CCollision::CalculateSlopeAltitude(float xleft, float xright, vec2 pos1, vec2 pos2)
{
	float linewidth = pos1.x - pos2.x;
	float lineheight = pos1.y - pos2.y;

	if(linewidth < 0) // pos1 -> pos2
	{
		if(xright < pos1.x || xleft > pos2.x)
			return -9999; //invalid

		if(lineheight < 0) // up-down
		{
			//printf("-> updown\n");
			if(xleft < pos1.x)
			{
				return pos1.y;
			}
			else
			{
				return pos1.y + (lineheight / linewidth) * (xleft - pos1.x);
			}
		}
		else if (lineheight > 0) // down-up
		{
			//printf("-> downup\n");
			if(xright > pos2.x)
			{
				return pos2.y;
			}
			else
			{
				return pos1.y + (lineheight / linewidth) * (xright - pos1.x);
			}
		}
		else
		{
			return pos1.y;
		}
		
	}
	else // pos2 -> pos1
	{
		if(xright < pos2.x || xleft > pos1.x)
			return -9999; //invalid

		if(lineheight > 0) // up-down
		{
			//printf("<- updown\n");
			if(xleft < pos2.x)
			{
				return pos2.y;
			}
			else
			{
				return pos1.y + (lineheight / linewidth) * (xleft - pos1.x);
			}
		}
		else if(lineheight < 0) // down-up
		{
			//printf("<- downup\n");
			if(xright > pos1.x)
			{
				return pos1.y;
			}
			else
			{
				return pos1.y + (lineheight / linewidth) * (xright - pos1.x);
			}
		}
		else
		{
			return pos1.y;
		}
	}
}

float CCollision::CalculateSlopeAltitudeSide(float xup, float xdown, vec2 pos1, vec2 pos2)
{
	float linewidth = pos1.x - pos2.x;
	float lineheight = pos1.y - pos2.y;

	if(lineheight < 0) // pos1 V pos 2
	{
		if(xdown < pos1.y || xup > pos2.y)
			return -9999; //invalid

		if(linewidth > 0) // left-right
		{
			//printf("V leftright\n");
			if(xdown > pos2.y)
			{
				return pos2.x;
			}
			else
			{
				return pos1.x + (linewidth / lineheight) * (xdown - pos1.y);
			}
		}
		else if(linewidth < 0)// right-left
		{
			//printf("V rightleft\n");
			if(xup < pos1.y)
			{
				return pos2.x;
			}
			else
			{
				return pos1.x + (linewidth / lineheight) * (xup - pos1.y);
			}
		}
		else
		{
			return pos1.x;
		}
		
	}
	else // pos2 V pos1
	{
		if(xdown < pos2.y || xup > pos1.y)
			return -9999; //invalid

		if(linewidth > 0) // right-left
		{
			//printf("^ rightleft\n");
			if(xup < pos2.y)
			{
				return pos2.x;
			}
			else
			{
				return pos1.x + (linewidth / lineheight) * (xup - pos1.y);
			}
		}
		else if(linewidth < 0) // left-right
		{
			//printf("^ leftright\n");
			if(xdown > pos1.y)
			{
				return pos1.x;
			}
			else
			{
				return pos1.x + (linewidth / lineheight) * (xdown - pos1.y);
			}
		}
		else
		{
			return pos1.x;
		}
	}
}

void CCollision::Rotate(const vec2 Center, vec2 * pPoint, float Rotation) const
{
	float x = pPoint->x - Center.x;
	float y = pPoint->y - Center.y;
	pPoint->x = (x * cosf(Rotation) - y * sinf(Rotation) + Center.x);
	pPoint->y = (x * sinf(Rotation) + y * cosf(Rotation) + Center.y);
}

void CCollision::GetAnimationTransform(float GlobalTime, int Env, vec2 &Position, float &Angle) const
{
	Position.x = 0.0f;
	Position.y = 0.0f;
	Angle = 0.0f;
	
	int Start, Num;
	m_pLayers->Map()->GetType(MAPITEMTYPE_ENVELOPE, &Start, &Num);
	if(Env >= Num)
		return;
	CMapItemEnvelope *pItem = (CMapItemEnvelope *)m_pLayers->Map()->GetItem(Start+Env, 0, 0);
	
	if(pItem->m_NumPoints == 0)
		return;

	CMapBasedEnvelopePointAccess EnvelopePoints(m_pLayers->Map());
	EnvelopePoints.SetPointsRange(pItem->m_StartPoint, pItem->m_NumPoints);
	
	if(EnvelopePoints.NumPoints() == 0)
		return;

	if(EnvelopePoints.NumPoints() == 1)
	{
		Position.x = fx2f(EnvelopePoints.GetPoint(0)->m_aValues[0]);
		Position.y = fx2f(EnvelopePoints.GetPoint(0)->m_aValues[1]);
		Angle = fx2f(EnvelopePoints.GetPoint(0)->m_aValues[2])/360.0f*pi*2.0f;
		return;
	}

	float Time = fmod(GlobalTime, EnvelopePoints.GetPoint(pItem->m_NumPoints-1)->m_Time.GetInternal()/1000.0f)*1000.0f;
	for(int i = 0; i < pItem->m_NumPoints-1; i++)
	{
		if(Time >= EnvelopePoints.GetPoint(i)->m_Time.GetInternal() && Time <= EnvelopePoints.GetPoint(i+1)->m_Time.GetInternal())
		{
			float Delta = EnvelopePoints.GetPoint(i+1)->m_Time.GetInternal()-EnvelopePoints.GetPoint(i)->m_Time.GetInternal();
			float a = (Time-EnvelopePoints.GetPoint(i)->m_Time.GetInternal())/Delta;
			switch (EnvelopePoints.GetPoint(i)->m_Curvetype)
			{
				case CURVETYPE_SMOOTH:
				{
					a = -2*a*a*a + 3*a*a; // second hermite basis
					break;
				}
				case CURVETYPE_SLOW:
				{
					a = a*a*a;
					break;
				}
				case CURVETYPE_FAST:
				{
					a = 1-a;
					a = 1-a*a*a;
					break;
				}
				case CURVETYPE_STEP:
				{
					a = 0;
					break;
				}
				case CURVETYPE_BEZIER:
				{
					const CEnvPointBezier *pCurrentPointBezier = EnvelopePoints.GetBezier(i);
					const CEnvPointBezier *pNextPointBezier = EnvelopePoints.GetBezier(i + 1);
					if(pCurrentPointBezier == nullptr || pNextPointBezier == nullptr)
						break; // fallback to linear
					for(size_t c = 0; c < 3; c++)
					{
						// monotonic 2d cubic bezier curve
						const vec2 p0 = vec2(EnvelopePoints.GetPoint(i)->m_Time.GetInternal(), fx2f(EnvelopePoints.GetPoint(i)->m_aValues[c]));
						const vec2 p3 = vec2(EnvelopePoints.GetPoint(i+1)->m_Time.GetInternal(), fx2f(EnvelopePoints.GetPoint(i+1)->m_aValues[c]));

						const vec2 OutTang = vec2(pCurrentPointBezier->m_aOutTangentDeltaX[c].GetInternal(), fx2f(pCurrentPointBezier->m_aOutTangentDeltaY[c]));
						const vec2 InTang = vec2(pNextPointBezier->m_aInTangentDeltaX[c].GetInternal(), fx2f(pNextPointBezier->m_aInTangentDeltaY[c]));

						vec2 p1 = p0 + OutTang;
						vec2 p2 = p3 + InTang;

						// validate bezier curve
						p1.x = std::clamp(p1.x, p0.x, p3.x);
						p2.x = std::clamp(p2.x, p0.x, p3.x);

						// solve x(a) = time for a
						a = std::clamp(SolveBezier(Time, p0.x, p1.x, p2.x, p3.x), 0.0f, 1.0f);

						// value = y(t)
						if(c == 0)
							Position.x = bezier(p0.y, p1.y, p2.y, p3.y, a);
						else if(c == 1)
							Position.y = bezier(p0.y, p1.y, p2.y, p3.y, a);
						else if(c == 2)
							Angle = bezier(p0.y, p1.y, p2.y, p3.y, a)/360.0f*pi*2.0f;
					}

					return;
				}
				default:
				{
					// linear
				}
			}
			// X
			{
				float v0 = fx2f(EnvelopePoints.GetPoint(i)->m_aValues[0]);
				float v1 = fx2f(EnvelopePoints.GetPoint(i+1)->m_aValues[0]);
				Position.x = v0 + (v1-v0) * a;
			}
			// Y
			{
				float v0 = fx2f(EnvelopePoints.GetPoint(i)->m_aValues[1]);
				float v1 = fx2f(EnvelopePoints.GetPoint(i+1)->m_aValues[1]);
				Position.y = v0 + (v1-v0) * a;
			}
			// angle
			{
				float v0 = fx2f(EnvelopePoints.GetPoint(i)->m_aValues[2]);
				float v1 = fx2f(EnvelopePoints.GetPoint(i+1)->m_aValues[2]);
				Angle = (v0 + (v1-v0) * a)/360.0f*pi*2.0f;
			}
			return;
		}
	}
	Position.x = fx2f(EnvelopePoints.GetPoint(EnvelopePoints.NumPoints()-1)->m_aValues[0]);
	Position.y = fx2f(EnvelopePoints.GetPoint(EnvelopePoints.NumPoints()-1)->m_aValues[1]);
	Angle = fx2f(EnvelopePoints.GetPoint(EnvelopePoints.NumPoints()-1)->m_aValues[2]);
	return;
}

bool CCollision::OutOfRange(double value, double q0, double q1, double q2, double q3) const
{
	if(q0 > q1)
	{
		if(q2 > q3)
		{
			const double Min = minimum(q1, q3);
			if(value < Min)
				return true;
			const double Max = maximum(q0, q2);
			if(value > Max)
				return true;
		}
		else
		{
			const double Min = minimum(q1, q2);
			if(value < Min)
				return true;
			const double Max = maximum(q0, q3);
			if(value > Max)
				return true;
		}
	}
	else
	{
		// q1 is bigger than q0
		if(q2 > q3)
		{
			const double Min = minimum(q0, q3);
			if(value < Min)
				return true;
			const double Max = maximum(q1, q2);
			if(value > Max)
				return true;
		}
		else
		{
			// q3 is bigger than q2
			const double Min = minimum(q0, q2);
			if(value < Min)
				return true;
			const double Max = maximum(q1, q3);
			if(value > Max)
				return true;
		}
	}
	return false;
}

//t0, t1 and t2 are position of triangle vertices
bool CCollision::InsideTriangle(const vec2& t0, const vec2& t1, const vec2& t2, const vec2& p) const
{
    vec3 bary = BarycentricCoordinates(t0, t1, t2, p);
    return (bary.x >= 0.0f && bary.y >= 0.0f && bary.x + bary.y < 1.0f);
}
//q0, q1, q2 and q3 are position of quad vertices
bool CCollision::InsideQuad(const vec2& q0, const vec2& q1, const vec2& q2, const vec2& q3, const vec2& p) const
{
	return InsideTriangle(q0, q1, q2, p) || InsideTriangle(q1, q2, q3, p);
}

vec3 CCollision::BarycentricCoordinates(const vec2& t0, const vec2& t1, const vec2& t2, const vec2& p) const
{
    vec2 e0 = t1 - t0;
    vec2 e1 = t2 - t0;
    vec2 e2 = p - t0;
    
    float d00 = dot(e0, e0);
    float d01 = dot(e0, e1);
    float d11 = dot(e1, e1);
    float d20 = dot(e2, e0);
    float d21 = dot(e2, e1);
    float denom = d00 * d11 - d01 * d01;
    
    vec3 bary;
    bary.x = (d11 * d20 - d01 * d21) / denom;
    bary.y = (d00 * d21 - d01 * d20) / denom;
    bary.z = 1.0f - bary.x - bary.y;
    
    return bary;
}