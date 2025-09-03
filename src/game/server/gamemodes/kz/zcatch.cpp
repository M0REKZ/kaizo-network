/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
/* Based on Race mod stuff and tweaked by GreYFoX@GTi and others to fit our DDRace needs. */
#include "zcatch.h"

#include <engine/server.h>
#include <engine/shared/config.h>
#include <game/mapitems.h>
#include <game/server/entities/character.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <game/server/score.h>
#include <game/version.h>

#define GAME_TYPE_NAME "zCatch-rw"
#define TEST_TYPE_NAME "TestzCatch"

CGameController_zCatch::CGameController_zCatch(class CGameContext *pGameServer) :
	CGameControllerLMS(pGameServer)
{
	if(m_InstagibWeapon == -1)
		m_InstagibWeapon = WEAPON_GRENADE; //+KZ Grenade default for zCatch

	m_pGameType = g_Config.m_SvTestingCommands ? TEST_TYPE_NAME : GAME_TYPE_NAME;
	
	
	m_GameFlags = protocol7::GAMEFLAG_SURVIVAL;
}

CGameController_zCatch::~CGameController_zCatch() = default;

void CGameController_zCatch::OnNewMatch()
{
	CGameControllerDM::OnNewMatch();

	for(auto pPlayer : GameServer()->m_apPlayers)
	{
		if(!pPlayer)
			continue;

		pPlayer->m_IsDead = false;
		pPlayer->m_Lives = g_Config.m_SvSurvivalLives; // +KZ LMS/LTS Lives
		pPlayer->m_Killer = -1; // +KZ zCatch Killer reset
	}
}

int CGameController_zCatch::OnCharacterDeath(CCharacter *pVictim, CPlayer *pKiller, int Weapon)
{
	if(pVictim && pVictim->GetPlayer())
	{
		pVictim->GetPlayer()->m_Killer = pKiller ? pKiller->GetCid() : -1; // +KZ zCatch Killer
		if(pVictim->GetPlayer()->m_Killer == pVictim->GetPlayer()->GetCid())
		{
			pVictim->GetPlayer()->m_Killer = -1; // reset killer if player killed himself
		}

		if(pVictim->GetPlayer()->m_Killer != -1)
		{
			char aBuf[256];
			str_format(aBuf, sizeof(aBuf), "You are caught until %s dies", Server()->ClientName(pVictim->GetPlayer()->m_Killer));
			GameServer()->SendChatTarget(pVictim->GetPlayer()->GetCid(), aBuf);
		}

		//release victims
		for(auto pPlayer : GameServer()->m_apPlayers)
		{
			if(!pPlayer)
				continue;

			if(pPlayer->m_Killer == pVictim->GetPlayer()->GetCid())
			{
				pPlayer->m_Killer = -1; // reset killer
				pPlayer->m_IsDead = false; // release player
				pPlayer->Respawn(); // respawn player if he was killed by victim
			}
		}
	}
	return CGameControllerLMS::OnCharacterDeath(pVictim,pKiller,Weapon);
}

int CGameController_zCatch::DoWinCheck()
{
	if(m_StartingMatch || m_StartingRound || m_WaitingForPlayers)
		return 0;
		
	// check score win condition
	if(CGameControllerDM::DoWinCheck())
		return 1;

	bool WinRound = false;

	// check for survival win
	CPlayer *pAlivePlayer = 0;
	int AlivePlayerCount = 0;
	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		if(GameServer()->m_apPlayers[i] && GameServer()->m_apPlayers[i]->GetTeam() != TEAM_SPECTATORS &&
			!GameServer()->m_apPlayers[i]->m_IsDead)
		{
			++AlivePlayerCount;
			pAlivePlayer = GameServer()->m_apPlayers[i];
		}
	}

	if(AlivePlayerCount == 0) // no winner
	{
		WinRound = true;
	}
	else if(AlivePlayerCount == 1) // 1 winner
	{
		WinRound = true;
	}

	if(WinRound && g_Config.m_SvScoreLimit)
	{
		GameServer()->SendChat(-1, TEAM_ALL, "Score limit not reached, all players released");
		for(auto pPlayer : GameServer()->m_apPlayers)
		{
			if(!pPlayer)
				continue;
			pPlayer->m_Killer = -1; // reset killer
			pPlayer->m_IsDead = false; // release player
			pPlayer->Respawn(); // respawn player if he was killed by victim
		}
		return 0;
	}
	else if(WinRound)
	{
		return 1;
	}

	return 0;
}

void CGameController_zCatch::OnCharacterSpawn(CCharacter *pChr)
{
	CGameControllerDM::OnCharacterSpawn(pChr);
}