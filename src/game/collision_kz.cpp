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

int CCollision::FastIntersectLine(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision, CCharacterCore *pCore, bool IsHook, bool IsWeapon) const
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
		if(IsSolid(CurTileX*32,CurTileY*32)|| CheckPointForCore(CurTileX*32, CurTileY*32, pCore, IsHook, IsWeapon))
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
	if(IsSolid(CurTileX*32,CurTileY*32)|| (kzid = CheckPointForCore(CurTileX*32, CurTileY*32, pCore, IsHook, IsWeapon)))
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

int CCollision::FastIntersectLinePortalLaser(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision, CKZTile **pKZTile, int *pTeleNr, CCharacterCore *pCore, bool IsHook, bool IsWeapon) const
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

        if((kzid = CheckPointForCore(CurTileX*32, CurTileY*32, pCore, IsHook, IsWeapon)))
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
	if(IsSolid(CurTileX*32,CurTileY*32) || (pKZTilelocal->m_Index == KZ_TILE_PORTAL_DISALLOW || pKZTilelocal->m_Index == KZ_TILE_PORTAL_RESET) || (kzid ? kzid : (kzid = CheckPointForCore(CurTileX*32, CurTileY*32, pCore, IsHook, IsWeapon))) || (g_Config.m_SvOldTeleportWeapons ? IsTeleport(Index) : IsTeleportWeapon(Index)))
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
            pKZTilelocal = GetKZGameTile(Pos);
            if(!pKZTilelocal || !(pKZTilelocal->m_Index == KZ_TILE_PORTAL_ALLOW))
                pKZTilelocal = GetKZFrontTile(Pos);

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
