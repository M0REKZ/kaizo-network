// Copyright (C) Benjamín Gajardo (also known as +KZ)

// OnGameTile() has code from Pointer's TW+

#include <game/collision.h>
#include "gameworld.h"
#include "entities/character.h"
#include "entities/door.h"
#include "entities/dragger.h"
#include "entities/laser.h"
#include "entities/pickup.h"
#include "entities/plasma.h"
#include "entities/projectile.h"
#include "entity.h"

void CGameWorld::OnCopyWorld(CGameWorld *pFrom)
{
	for(int i = 0; i < 4; i++)
	{
		m_PointerTelePositions[i].m_X = pFrom->m_PointerTelePositions[i].m_X;
		m_PointerTelePositions[i].m_Y = pFrom->m_PointerTelePositions[i].m_Y;
		m_PointerTelePositions[i].m_Exists = pFrom->m_PointerTelePositions[i].m_Exists;
	}

	//so i dont need to copypaste this along all gameclient.cpp
	m_pPredController = pFrom->m_pPredController;
}

void CGameWorld::OnGameTile(int X, int Y, const CTile *pTile)
{
    if(!pTile)
        return;

    int Index = pTile->m_Index;

	if(Index > 128)
		return;

	switch(Index)
	{
	case POINTER_TILE_TELEONE:
		if(!m_PointerTelePositions[0].m_Exists)
		{
			m_PointerTelePositions[0].m_X = X;
			m_PointerTelePositions[0].m_Y = Y;
			m_PointerTelePositions[0].m_Exists = true;
		}
		break;
	case POINTER_TILE_TELETWO:
		if(!m_PointerTelePositions[1].m_Exists)
		{
			m_PointerTelePositions[1].m_X = X;
			m_PointerTelePositions[1].m_Y = Y;
			m_PointerTelePositions[1].m_Exists = true;
		}
		break;
	case POINTER_TILE_TELETHREE:
		if(!m_PointerTelePositions[2].m_Exists)
		{
			m_PointerTelePositions[2].m_X = X;
			m_PointerTelePositions[2].m_Y = Y;
			m_PointerTelePositions[2].m_Exists = true;
		}
		break;
	case POINTER_TILE_TELEFOUR:
		if(!m_PointerTelePositions[3].m_Exists)
		{
			m_PointerTelePositions[3].m_X = X;
			m_PointerTelePositions[3].m_Y = Y;
			m_PointerTelePositions[3].m_Exists = true;
		}
		break;
	}
}

void CGameWorld::OnConnected()
{
	for(int i = 0; i < 4; i++)
	{
		m_PointerTelePositions[i].m_X = 0;
		m_PointerTelePositions[i].m_Y = 0;
		m_PointerTelePositions[i].m_Exists = false;
	}

	const CTile *pTiles = m_pCollision->GameLayer();
	for(int y = 0; y < m_pCollision->GetHeight(); y++)
	{
		for(int x = 0; x < m_pCollision->GetWidth(); x++)
		{
			OnGameTile(x, y, &pTiles[y * m_pCollision->GetWidth() + x]);
		}
	}
}

void CGameWorld::CopyWorldClean(CGameWorld *pFrom) //TClient
{
	if(pFrom == this || !pFrom)
		return;
	m_IsValidCopy = false;

	m_GameTick = pFrom->m_GameTick;
	m_pCollision = pFrom->m_pCollision;
	m_WorldConfig = pFrom->m_WorldConfig;
	m_pTuningList = pFrom->m_pTuningList;
	m_Teams = pFrom->m_Teams;
	m_Core.m_vSwitchers = pFrom->m_Core.m_vSwitchers;
	// delete the previous entities
	Clear();
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		m_apCharacters[i] = 0;
		m_Core.m_apCharacters[i] = 0;
	}
	// copy and add the new entities
	for(int Type = 0; Type < NUM_ENTTYPES; Type++)
	{
		for(CEntity *pEnt = pFrom->FindLast(Type); pEnt; pEnt = pEnt->TypePrev())
		{
			CEntity *pCopy = 0;
			if(Type == ENTTYPE_PROJECTILE)
				pCopy = new CProjectile(*((CProjectile *)pEnt));
			else if(Type == ENTTYPE_LASER)
				pCopy = new CLaser(*((CLaser *)pEnt));
			else if(Type == ENTTYPE_DRAGGER)
				pCopy = new CDragger(*((CDragger *)pEnt));
			else if(Type == ENTTYPE_CHARACTER)
				pCopy = new CCharacter(*((CCharacter *)pEnt));
			else if(Type == ENTTYPE_PICKUP)
				pCopy = new CPickup(*((CPickup *)pEnt));
			if(pCopy)
			{
				pCopy->m_pParent = nullptr;
				pCopy->m_pChild = nullptr;
				this->InsertEntity(pCopy);
			}
		}
	}

	//+KZ
	OnCopyWorld(pFrom);
}
