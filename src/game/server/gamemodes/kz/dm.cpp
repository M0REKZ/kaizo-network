/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
/* Based on Race mod stuff and tweaked by GreYFoX@GTi and others to fit our DDRace needs. */
#include "dm.h"

#include <engine/server.h>
#include <engine/shared/config.h>
#include <game/mapitems.h>
#include <game/server/entities/character.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <game/server/score.h>
#include <game/version.h>

#define GAME_TYPE_NAME "DM-rw"
#define TEST_TYPE_NAME "TestDM"

CGameControllerDM::CGameControllerDM(class CGameContext *pGameServer) :
	CGameControllerBasePvP(pGameServer)
{
	if(m_InstagibWeapon != -1)
	{
		if(g_Config.m_SvTestingCommands)
		{
			if(m_InstagibWeapon == WEAPON_GRENADE)
			{
				m_pGameType = "g" TEST_TYPE_NAME;
			}
			else if(m_InstagibWeapon == WEAPON_LASER)
			{
				m_pGameType = "i" TEST_TYPE_NAME;
			}
		}
		else
		{
			if(m_InstagibWeapon == WEAPON_GRENADE)
			{
				m_pGameType = "g" GAME_TYPE_NAME;
			}
			else if(m_InstagibWeapon == WEAPON_LASER)
			{
				m_pGameType = "i" GAME_TYPE_NAME;
			}
		}
	}
	else
	{
		m_pGameType = g_Config.m_SvTestingCommands ? TEST_TYPE_NAME : GAME_TYPE_NAME;
	}

	m_GameFlags = 0;
}

CGameControllerDM::~CGameControllerDM() = default;

int CGameControllerDM::DoWinCheck()
{
	if(m_StartingMatch || m_StartingRound || m_WaitingForPlayers)
		return 0;

	if(!g_Config.m_SvScoreLimit)
		return 0;

	int HighScore = -1;
	int BestPlayer = -1;
	bool Draw = false;

	for(auto pPlayer : GameServer()->m_apPlayers)
	{
		if(!pPlayer)
			continue;

		if(pPlayer->m_ScoreKZ > HighScore)
		{
			BestPlayer = pPlayer->GetCid();
			HighScore = pPlayer->m_ScoreKZ;
			Draw = false;
		}
		else if(pPlayer->m_ScoreKZ == HighScore)
		{
			Draw = true;
		}
	}

	if(m_SuddenDeath ? !Draw : HighScore >= g_Config.m_SvScoreLimit)
	{
		m_WinPauseTicks = 10 * Server()->TickSpeed();
		return 1;
	}

	return 0;
}

int CGameControllerDM::OnCharacterDeath(CCharacter *pVictim, CPlayer *pKiller, int Weapon)
{
	int res = CGameControllerBasePvP::OnCharacterDeath(pVictim,pKiller,Weapon);
	DoWinCheck();
	return res;
}
