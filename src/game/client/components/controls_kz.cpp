// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// CheckNewInput() is from Fast Input commit

#include <engine/shared/config.h>
#include <game/client/gameclient.h>
#include "controls.h"

bool CControls::CheckNewInput()
{
	bool NewInput[2] = {};
	for(int Dummy = 0; Dummy < NUM_DUMMIES; Dummy++)
	{
		CNetObj_PlayerInput TestInput = m_aInputData[Dummy];
		if(Dummy == g_Config.m_ClDummy)
		{
			TestInput.m_Direction = 0;
			if(m_aInputDirectionLeft[Dummy] && !m_aInputDirectionRight[Dummy])
				TestInput.m_Direction = -1;
			if(!m_aInputDirectionLeft[Dummy] && m_aInputDirectionRight[Dummy])
				TestInput.m_Direction = 1;
		}

		if(m_aFastInput[Dummy].m_Direction != TestInput.m_Direction)
			NewInput[Dummy] = true;
		if(m_aFastInput[Dummy].m_Hook != TestInput.m_Hook)
			NewInput[Dummy] = true;
		if(m_aFastInput[Dummy].m_Fire != TestInput.m_Fire)
			NewInput[Dummy] = true;
		if(m_aFastInput[Dummy].m_Jump != TestInput.m_Jump)
			NewInput[Dummy] = true;
		if(m_aFastInput[Dummy].m_NextWeapon != TestInput.m_NextWeapon)
			NewInput[Dummy] = true;
		if(m_aFastInput[Dummy].m_PrevWeapon != TestInput.m_PrevWeapon)
			NewInput[Dummy] = true;
		if(m_aFastInput[Dummy].m_WantedWeapon != TestInput.m_WantedWeapon)
			NewInput[Dummy] = true;

		bool SetMousePos = false;
		// We need to be careful about how we manage the mouse position to avoid mispredicted hooks and fires
		// on the first tick that they activate before we know what mouse position we actually sent to the server
		if(Dummy == g_Config.m_ClDummy)
		{
			if(m_aFastInput[Dummy].m_Hook == 0 && TestInput.m_Hook == 1)
			{
				m_FastInputHookAction = true;
				SetMousePos = true;
			}
			if(m_aFastInput[Dummy].m_Fire != TestInput.m_Fire && TestInput.m_Fire % 2 == 1)
			{
				m_FastInputFireAction = true;
				SetMousePos = true;
			}
			if(!m_FastInputHookAction && !m_FastInputFireAction)
			{
				SetMousePos = true;
			}
		}

		if(SetMousePos)
		{
			TestInput.m_TargetX = (int)m_aMousePos[Dummy].x;
			TestInput.m_TargetY = (int)m_aMousePos[Dummy].y;
		}
		else
		{
			TestInput.m_TargetX = m_aFastInput[Dummy].m_TargetX;
			TestInput.m_TargetY = m_aFastInput[Dummy].m_TargetY;
		}

		m_aFastInput[Dummy] = TestInput;
	}

	if(NewInput[0] || NewInput[1])
		return true;
	else
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
		pMainInput->m_PlayerFlags |= PLAYERFLAG_PLAYING;
		pDummyInput->m_PlayerFlags &= ~(PLAYERFLAG_IN_MENU);
		pDummyInput->m_PlayerFlags |= PLAYERFLAG_PLAYING;
	}

	if(g_Config.m_KaizoForceChatBubble)
	{
		pMainInput->m_PlayerFlags |= PLAYERFLAG_CHATTING;
		pDummyInput->m_PlayerFlags |= PLAYERFLAG_CHATTING;
	}

	if(g_Config.m_KaizoForceInMenuStatus)
	{
		//PLAYERFLAG_IN_MENU & PLAYERFLAG_PLAYING are about in-menu state
		pMainInput->m_PlayerFlags |= PLAYERFLAG_IN_MENU;
		pMainInput->m_PlayerFlags &= ~(PLAYERFLAG_PLAYING);
		pDummyInput->m_PlayerFlags |= PLAYERFLAG_IN_MENU;
		pDummyInput->m_PlayerFlags &= ~(PLAYERFLAG_PLAYING);
	}

	if(g_Config.m_KaizoFastRespawn)
	{
		static bool Shoot = false;
		bool PrevShoot = Shoot;

		const CNetObj_GameInfo *pGameInfoObj = GameClient()->m_Snap.m_pGameInfoObj;
		if(GameClient()->m_Snap.m_pLocalInfo && !GameClient()->m_Snap.m_SpecInfo.m_Active && 
			!GameClient()->m_Snap.m_pLocalCharacter && g_Config.m_ClScoreboardOnDeath &&
			!(pGameInfoObj && pGameInfoObj->m_GameStateFlags & GAMESTATEFLAG_PAUSED))
		{
			Shoot = true;
		}
		else
		{
			Shoot = false;
		}

		if(Shoot && !PrevShoot)
			pMainInput->m_Fire |= 1;
	}

	*pSecondInput = *pDummyInput;
}
