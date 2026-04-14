// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include "gamecore.h"

#include "collision.h"
#include "mapitems.h"
#include "teamscore.h"

#include <engine/shared/config.h>

#include <limits>

class CPortalCore *CWorldCore::SetPortalKZ(CPortalCore *pPortal)
{
	if(!pPortal)
		return nullptr;

	if(pPortal->m_OwnerId < 0 || pPortal->m_OwnerId >= MAX_CLIENTS)
		return nullptr;

	int blue = pPortal->m_IsBlue ? 1 : 0;

	if(m_apPortals[pPortal->m_OwnerId][blue])
	{
		delete m_apPortals[pPortal->m_OwnerId][blue];
		m_apPortals[pPortal->m_OwnerId][blue] = nullptr;
	}

	m_apPortals[pPortal->m_OwnerId][blue] = pPortal;

	return m_apPortals[pPortal->m_OwnerId][blue];
}

CPortalCore *CWorldCore::GetPortalKZ(int OwnerId, bool IsBlue)
{
	if(OwnerId < 0 || OwnerId >= MAX_CLIENTS)
		return nullptr;
	
	int blue = IsBlue ? 1 : 0;
	return m_apPortals[OwnerId][blue];
}

void CWorldCore::DeletePortalKZ(int OwnerId, bool IsBlue)
{
	if(OwnerId < 0 || OwnerId >= MAX_CLIENTS)
		return;

	int blue = IsBlue ? 1 : 0;
	delete m_apPortals[OwnerId][blue];
	m_apPortals[OwnerId][blue] = nullptr;
}

CPortalCore::CPortalCore(int OwnerId, vec2 Pos, vec2 Pos2, bool IsBlue, int Team)
{
	m_OwnerId = OwnerId;
	m_Pos = Pos;
	m_Pos2 = Pos2;
	m_IsBlue = IsBlue;
	m_Team = Team;
}

bool CCharacterCore::HandleKZTileOnMoveBox(vec2 *pMoveBoxPos, vec2 *pMoveBoxVel, vec2 MoveBoxSize, vec2 MoveBoxElasticity)
{
	if(!m_pWorld || !m_pCollision || !m_pTeams)
		return false;

	CKZTile *pKZTile = nullptr;
	if(m_pCollision->KZGameFound())
		pKZTile = m_pCollision->GetKZGameTile(*pMoveBoxPos);
	CKZTile *pKZTileFront = nullptr;
	if(m_pCollision->KZFrontFound())
		pKZTileFront = m_pCollision->GetKZFrontTile(*pMoveBoxPos);

	if(!pKZTile && !pKZTileFront)
		return false;

	if(pKZTileFront)
	{
		if(pKZTileFront->m_Index == KZ_FRONTTILE_POS_SHIFTER && ((pKZTileFront->m_Number && !m_pWorld->m_vSwitchers.empty()) ? m_pWorld->m_vSwitchers[pKZTileFront->m_Number].m_aStatus[m_pTeams->Team(m_Id)] : true))
		{
			pMoveBoxPos->x += (int)pKZTileFront->m_Value1;
			pMoveBoxPos->y += (int)pKZTileFront->m_Value2;

			if(BitWiseAndInt64(pKZTileFront->m_Value3, KZ_POS_SWITCHER_FLAG_HOOK))
			{
				if(m_HookState == HOOK_GRABBED && m_HookedPlayer == -1)
				{
					m_HookPos.x += (int)pKZTileFront->m_Value1;
					m_HookPos.y += (int)pKZTileFront->m_Value2;
				}
			}

			if(BitWiseAndInt64(pKZTileFront->m_Value3, KZ_POS_SWITCHER_FLAG_INVERT_VELX))
			{
				pMoveBoxVel->x = -pMoveBoxVel->x;
			}
			if(BitWiseAndInt64(pKZTileFront->m_Value3, KZ_POS_SWITCHER_FLAG_INVERT_VELY))
			{
				pMoveBoxVel->y = -pMoveBoxVel->y;
			}

			m_SendCoreThisTick = true;
			m_DontCheckPlayerCollisionOnThisMove = true;
			m_ServerResetPrevPos = true;
		}
	}

	return false;
}

void CCharacterCore::PreTickKZ()
{
	if(m_Input.m_Fire & 1)
	{
		if(((m_pKaizoNetworkChar && m_pKaizoNetworkChar->m_RealCurrentWeapon == KZ_CUSTOM_WEAPON_ATTRACTOR_BEAM - KZ_CUSTOM_WEAPONS_START)) && m_AttractorBeamPlayer == -1)
		{
			vec2 Dir = vec2(m_Input.m_TargetX, m_Input.m_TargetY);
			if(length(Dir) > 300.f)
			{
				Dir = normalize(Dir) * 300.f;
			}

			vec2 AttractorPos = m_Pos + (normalize(vec2(m_Input.m_TargetX,m_Input.m_TargetY)) * 82);

			CCharacterCore * pCore = nullptr;

			float BestDistance = 1000.f;
			for(int i = 0; i < MAX_CLIENTS;i++)
			{
				if(!m_pWorld->m_apCharacters[i])
					continue;

				if(i == m_Id)
					continue;

				if((m_Id != -1 && !m_pTeams->CanCollide(i, m_Id)) || m_pWorld->m_apCharacters[i]->m_Solo || m_Solo)
					continue;

				vec2 TempPos;

				if(Collision()->IntersectCharacterCore(AttractorPos, m_Pos + Dir, PhysicalSize(), TempPos, m_pWorld->m_apCharacters[i]) && distance(AttractorPos, TempPos) < BestDistance)
				{
					BestDistance = distance(AttractorPos, TempPos);
					pCore = m_pWorld->m_apCharacters[i];
				}
			}

			if(pCore)
			{
				m_AttractorBeamPlayer = pCore->m_Id;
			}
		}
		else if((m_pKaizoNetworkChar && m_pKaizoNetworkChar->m_RealCurrentWeapon != KZ_CUSTOM_WEAPON_ATTRACTOR_BEAM - KZ_CUSTOM_WEAPONS_START) && m_AttractorBeamPlayer != -1)
		{
			m_AttractorBeamPlayer = -1;
		}
	}
	else
	{
		m_AttractorBeamPlayer = -1;
	}

	if(m_AttractorBeamPlayer >= 0 && m_AttractorBeamPlayer < MAX_CLIENTS)
	{
		vec2 AttractorPos = m_Pos + (normalize(vec2(m_Input.m_TargetX,m_Input.m_TargetY)) * 82);
		CCharacterCore * pCore = m_pWorld->m_apCharacters[m_AttractorBeamPlayer];
		if(pCore)
		{
			pCore->m_Vel = normalize(AttractorPos - pCore->m_Pos) * 4.f;
			pCore->m_SendCoreThisTick = true;
		}
		else
		{
			m_AttractorBeamPlayer = -1;
		}
	}
}
