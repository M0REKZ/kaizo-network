// (c) +KZ

#include <base/math.h>
#include <base/system.h>
#include <base/vmath.h>

#include <antibot/antibot_data.h>

#include <cmath>
#include <engine/map.h>

#include <game/collision.h>
#include <game/layers.h>
#include <game/mapitems.h>

#include <engine/shared/config.h>
#include "collision.h"

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
