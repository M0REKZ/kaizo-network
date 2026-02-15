// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// CheckNewInput() is from Fast Input commit

#include <engine/shared/config.h>
#include <game/client/gameclient.h>
#include "controls.h"

bool CControls::CheckNewInput()
{
	bool NewInput = false;
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

			if(g_Config.m_ClSubTickAiming)
			{
				TestInput.m_TargetX = (int)m_aMousePos[Dummy].x;
				TestInput.m_TargetY = (int)m_aMousePos[Dummy].y;
			}
		}

		if(m_aFastInput[Dummy].m_Direction != TestInput.m_Direction)
			NewInput = true;
		if(m_aFastInput[Dummy].m_Hook != TestInput.m_Hook)
			NewInput = true;
		if(m_aFastInput[Dummy].m_Fire != TestInput.m_Fire)
			NewInput = true;
		if(m_aFastInput[Dummy].m_Jump != TestInput.m_Jump)
			NewInput = true;
		if(m_aFastInput[Dummy].m_NextWeapon != TestInput.m_NextWeapon)
			NewInput = true;
		if(m_aFastInput[Dummy].m_PrevWeapon != TestInput.m_PrevWeapon)
			NewInput = true;
		if(m_aFastInput[Dummy].m_WantedWeapon != TestInput.m_WantedWeapon)
			NewInput = true;

		m_aFastInput[Dummy] = TestInput;
	}

	return NewInput;
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

		if(!Shoot &&
			!GameClient()->m_Snap.m_aCharacters[GameClient()->m_aLocalIds[g_Config.m_ClDummy]].m_Active &&
			GameClient()->m_Snap.m_apPlayerInfos[GameClient()->m_aLocalIds[g_Config.m_ClDummy]] &&
			GameClient()->m_Snap.m_apPlayerInfos[GameClient()->m_aLocalIds[g_Config.m_ClDummy]]->m_Team != TEAM_SPECTATORS &&
			!GameClient()->m_aClients[GameClient()->m_aLocalIds[g_Config.m_ClDummy]].m_Paused &&
			!GameClient()->m_aClients[GameClient()->m_aLocalIds[g_Config.m_ClDummy]].m_Spec
		)
			Shoot = true;

		if(Shoot && (
			GameClient()->m_Snap.m_aCharacters[GameClient()->m_aLocalIds[g_Config.m_ClDummy]].m_Active ||
			(GameClient()->m_Snap.m_apPlayerInfos[GameClient()->m_aLocalIds[g_Config.m_ClDummy]] &&
			GameClient()->m_Snap.m_apPlayerInfos[GameClient()->m_aLocalIds[g_Config.m_ClDummy]]->m_Team == TEAM_SPECTATORS) ||
			GameClient()->m_aClients[GameClient()->m_aLocalIds[g_Config.m_ClDummy]].m_Paused ||
			GameClient()->m_aClients[GameClient()->m_aLocalIds[g_Config.m_ClDummy]].m_Spec
		))
		{
			Shoot = false;
			pMainInput->m_Fire &= ~1;
		}
			
		if(Shoot)
			pMainInput->m_Fire |= 1;
	}

	*pSecondInput = *pDummyInput;
}
