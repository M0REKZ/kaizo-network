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

int CCollision::GetCollisionAt(float x, float y, CCharacterCore *pCore, bool IsHook, bool IsWeapon) const
{
    int i = GetTile(round_to_int(x), round_to_int(y));
    if(pCore && !i)
    {
        return CheckPointForCore(x, y, pCore, IsHook, IsWeapon);
    }
    return i;
}

int CCollision::CheckPointForCore(float x, float y, CCharacterCore *pCore, bool IsHook, bool IsWeapon) const
{
    if(pCore)
    {
        if(m_pKZGame || m_pKZFront)
        {
            int Nx = clamp(round_to_int(x) / 32, 0, m_KZGameWidth - 1);
            int Ny = clamp(round_to_int(y) / 32, 0, m_KZGameHeight - 1);

            CKZTile* KZTile = GetKZGameTile(Nx, Ny);
            
            Nx = clamp(round_to_int(x) / 32, 0, m_KZFrontWidth - 1);
            Ny = clamp(round_to_int(y) / 32, 0, m_KZFrontHeight - 1);
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
    int Nx = clamp(round_to_int(x) / 32, 0, m_KZGameWidth - 1);
    int Ny = clamp(round_to_int(y) / 32, 0, m_KZGameHeight - 1);
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
    int Nx = clamp(round_to_int(x) / 32, 0, m_KZFrontWidth - 1);
    int Ny = clamp(round_to_int(y) / 32, 0, m_KZFrontHeight - 1);
    return m_pKZFront ? &m_pKZFront[Ny * m_KZFrontWidth + Nx] : nullptr;
}

unsigned char CCollision::GetKZGameTileIndex(float x, float y) const
{
	int Nx = clamp(round_to_int(x) / 32, 0, m_KZGameWidth - 1);
	int Ny = clamp(round_to_int(y) / 32, 0, m_KZGameHeight - 1);
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
	int Nx = clamp(round_to_int(x) / 32, 0, m_KZFrontWidth - 1);
	int Ny = clamp(round_to_int(y) / 32, 0, m_KZFrontHeight - 1);
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

