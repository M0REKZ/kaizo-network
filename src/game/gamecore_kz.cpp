/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "gamecore.h"

#include "collision.h"
#include "mapitems.h"
#include "teamscore.h"

#include <base/system.h>
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

CPortalCore::CPortalCore(int OwnerId, vec2 Pos, vec2 Pos2, bool IsBlue)
{
	m_OwnerId = OwnerId;
	m_Pos = Pos;
	m_Pos2 = Pos2;
	m_IsBlue = IsBlue;
}