// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// CheckNewInput() is from Fast Input commit

#include <engine/shared/config.h>
#include <game/client/gameclient.h>
#include "controls.h"

bool CControls::CheckNewInput()
{
	CNetObj_PlayerInput TestInput = m_aInputData[g_Config.m_ClDummy];
	TestInput.m_Direction = 0;
	if(m_aInputDirectionLeft[g_Config.m_ClDummy] && !m_aInputDirectionRight[g_Config.m_ClDummy])
		TestInput.m_Direction = -1;
	if(!m_aInputDirectionLeft[g_Config.m_ClDummy] && m_aInputDirectionRight[g_Config.m_ClDummy])
		TestInput.m_Direction = 1;

	bool NewInput = false;
	if(m_FastInput.m_Direction != TestInput.m_Direction)
		NewInput = true;
	if(m_FastInput.m_Hook != TestInput.m_Hook)
		NewInput = true;
	if(m_FastInput.m_Fire != TestInput.m_Fire)
		NewInput = true;
	if(m_FastInput.m_Jump != TestInput.m_Jump)
		NewInput = true;

	if(m_FastInput.m_WantedWeapon != TestInput.m_WantedWeapon)
		NewInput = true;
	if(m_FastInput.m_NextWeapon != TestInput.m_NextWeapon)
		NewInput = true;
	if(m_FastInput.m_PrevWeapon != TestInput.m_PrevWeapon)
		NewInput = true;

	if(m_FastInput.m_PlayerFlags != TestInput.m_PlayerFlags)
		NewInput = true;

	if(g_Config.m_ClSubTickAiming)
	{
		TestInput.m_TargetX = (int)m_aMousePos[g_Config.m_ClDummy].x;
		TestInput.m_TargetY = (int)m_aMousePos[g_Config.m_ClDummy].y;
		TestInput.m_TargetX *= GameClient()->m_Camera.m_Zoom;
		TestInput.m_TargetY *= GameClient()->m_Camera.m_Zoom;
	}

	if(m_FastInput.m_TargetX != TestInput.m_TargetX)
		NewInput = true;
	if(m_FastInput.m_TargetY != TestInput.m_TargetY)
		NewInput = true;

	if (NewInput)
	{
		m_FastInput = TestInput;
		return true;
	}
	return false;
}

void CControls::HandleKaizoInput(CNetObj_PlayerInput *pMainInput, CNetObj_PlayerInput *pSecondInput)
{
	CNetObj_PlayerInput *pDummyInput = &(GameClient()->m_DummyInput);

	//this is so we always receive ping from other players
	//exclude F-DDrace since some features requires pressing/releasing scoreboard key
	if(g_Config.m_KaizoPingCircles && !GameClient()->m_GameInfo.m_EntitiesFDDrace)
	{
		pMainInput->m_PlayerFlags |= PLAYERFLAG_SCOREBOARD;
	}

	if(g_Config.m_KaizoHideChatBubble)
	{
		pMainInput->m_PlayerFlags &= ~(PLAYERFLAG_CHATTING);
		pDummyInput->m_PlayerFlags &= ~(PLAYERFLAG_CHATTING);
	}

	if(g_Config.m_KaizoHideInMenuStatus)
	{
		//PLAYERFLAG_IN_MENU & PLAYERFLAG_PLAYING are about in-menu state
		pMainInput->m_PlayerFlags &= ~(PLAYERFLAG_IN_MENU);
		pMainInput->m_PlayerFlags &= ~(PLAYERFLAG_PLAYING);
		pDummyInput->m_PlayerFlags &= ~(PLAYERFLAG_IN_MENU);
		pDummyInput->m_PlayerFlags &= ~(PLAYERFLAG_PLAYING);
	}

	*pSecondInput = *pDummyInput;
}
