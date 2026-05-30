// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// Just look for comments that indicate if code is from another mod...

#include "animstate.h"
#include <generated/client_data.h>

//From EClient
const CAnimState *CAnimState::GetSpec()
{
	static CAnimState s_State;
	static bool s_Init = true;

	if(s_Init)
	{
		s_State.Set(&g_pData->m_aAnimations[ANIM_BASE], 0.0f);
		s_State.Add(&g_pData->m_aAnimations[ANIM_SIT_RIGHT], 0.0f, 1.0f);
		s_Init = false;
	}

	return &s_State;
}
