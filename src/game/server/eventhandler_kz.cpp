// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include "eventhandler.h"

#include "entity.h"
#include "gamecontext.h"

#include <base/system.h>
#include <base/vmath.h>

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