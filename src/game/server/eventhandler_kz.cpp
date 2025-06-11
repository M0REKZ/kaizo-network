/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "eventhandler.h"

#include "entity.h"
#include "gamecontext.h"

#include <base/system.h>
#include <base/vmath.h>

//////////////////////////////////////////////////
// Event handler +KZ
//////////////////////////////////////////////////

void *CEventHandler::CreateForClient(int Type, int Size, int ClientId, CClientMask Mask)
{
	void *p = Create(Type,Size,Mask);

	if(p)
	{
		m_aForClientId[m_NumEvents - 1] = ClientId;
	}

	return p;
}

void CEventHandler::ClearKZ()
{
	for(int i = 0; i < MAX_EVENTS;i++)
	{
		m_aForClientId[i] = -99;
	}
}