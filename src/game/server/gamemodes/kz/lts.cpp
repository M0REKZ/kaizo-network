/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
/* Based on Race mod stuff and tweaked by GreYFoX@GTi and others to fit our DDRace needs. */
#include "lts.h"

#include <engine/server.h>
#include <engine/shared/config.h>
#include <game/mapitems.h>
#include <game/server/entities/character.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <game/server/score.h>
#include <game/version.h>

#define GAME_TYPE_NAME "LTS-rw"
#define TEST_TYPE_NAME "TestLTS"

CGameControllerLTS::CGameControllerLTS(class CGameContext *pGameServer) :
	CGameControllerLMS(pGameServer)
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

	m_GameFlags = GAMEFLAG_TEAMS | protocol7::GAMEFLAG_SURVIVAL;

	m_aTeamScore[TEAM_RED] = 0;
	m_aTeamScore[TEAM_BLUE] = 0;
}

CGameControllerLTS::~CGameControllerLTS() = default;

int CGameControllerLTS::DoWinCheck()
{
	if(GameServer()->m_World.m_Paused || m_StartingMatch || m_StartingRound || m_WaitingForPlayers)
		return 0;

	// check score win condition
	if(DoWinCheckMatch())
		return 1;

	//LTS round end
	int Count[2] = {0};
	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		if(GameServer()->m_apPlayers[i] && GameServer()->m_apPlayers[i]->GetTeam() != TEAM_SPECTATORS &&
			!GameServer()->m_apPlayers[i]->m_IsDead)
			++Count[GameServer()->m_apPlayers[i]->GetTeam()];
	}

	if(Count[TEAM_RED]+Count[TEAM_BLUE] == 0 || (g_Config.m_SvTimeLimit > 0 && (Server()->Tick()-m_RoundStartTick) >= g_Config.m_SvTimeLimit*Server()->TickSpeed()*60))
	{
		++m_aTeamScore[TEAM_BLUE];
		++m_aTeamScore[TEAM_RED];
		m_WinPauseTicks = 5 * Server()->TickSpeed();
		m_IsRoundEnd = true;
		return 1;
	}
	else if(Count[TEAM_RED] == 0)
	{
		++m_aTeamScore[TEAM_BLUE];
		m_WinPauseTicks = 5 * Server()->TickSpeed();
		m_IsRoundEnd = true;
		return 1;
	}
	else if(Count[TEAM_BLUE] == 0)
	{
		++m_aTeamScore[TEAM_RED];
		m_WinPauseTicks = 5 * Server()->TickSpeed();
		m_IsRoundEnd = true;
		return 1;
	}
	return 0;
}

void CGameControllerLTS::OnNewMatch()
{
	CGameControllerDM::OnNewMatch();

	for(auto pPlayer : GameServer()->m_apPlayers)
	{
		if(!pPlayer)
			continue;

		pPlayer->m_IsDead = false; //+KZ
		pPlayer->m_Lives = g_Config.m_SvSurvivalLives; // +KZ LMS/LTS Lives
	}

	if(!m_IsRoundEnd)
	{
		m_aTeamScore[TEAM_RED] = 0;
		m_aTeamScore[TEAM_BLUE] = 0;
	}
}

bool CGameControllerLTS::CanJoinTeam(int Team, int NotThisId, char *pErrorReason, int ErrorReasonSize)
{
	if(Team == TEAM_RED || Team == TEAM_BLUE || Team == TEAM_SPECTATORS)
		return true;

	return CGameControllerLMS::CanJoinTeam(Team,NotThisId,pErrorReason,ErrorReasonSize);
}

bool CGameControllerLTS::DoWinCheckMatch()
{
	//TeamPlay match end
	if((g_Config.m_SvScoreLimit > 0 && (m_aTeamScore[TEAM_RED] >= g_Config.m_SvScoreLimit || m_aTeamScore[TEAM_BLUE] >= g_Config.m_SvScoreLimit)) ||
		(g_Config.m_SvTimeLimit > 0 && (Server()->Tick() - m_RoundStartTick) >= g_Config.m_SvTimeLimit * Server()->TickSpeed() * 60))
	{
		if(m_SuddenDeath)
		{
			if(m_aTeamScore[TEAM_RED] / 100 != m_aTeamScore[TEAM_BLUE] / 100)
			{
				m_WinPauseTicks = 10 * Server()->TickSpeed();
				return true;
			}
		}
		else
		{
			if(m_aTeamScore[TEAM_RED] != m_aTeamScore[TEAM_BLUE])
			{
				m_WinPauseTicks = 10 * Server()->TickSpeed();
				return true;
			}
			else
				m_SuddenDeath = 1;
		}
	}
	return false;
}

void CGameControllerLTS::Snap(int SnappingClient)
{
	CGameControllerLMS::Snap(SnappingClient);

	if(Server()->IsSixup(SnappingClient))
	{
		protocol7::CNetObj_GameDataTeam *pGameDataObj = Server()->SnapNewItem<protocol7::CNetObj_GameDataTeam>(0);

		if(!pGameDataObj)
			return;

		pGameDataObj->m_TeamscoreRed = m_aTeamScore[TEAM_RED];
		pGameDataObj->m_TeamscoreBlue = m_aTeamScore[TEAM_BLUE];
	}
	else
	{
		CNetObj_GameData *pGameDataObj = (CNetObj_GameData *)Server()->SnapNewItem(NETOBJTYPE_GAMEDATA, 0, sizeof(CNetObj_GameData));
		if(!pGameDataObj)
			return;

		pGameDataObj->m_TeamscoreRed = m_aTeamScore[TEAM_RED];
		pGameDataObj->m_TeamscoreBlue = m_aTeamScore[TEAM_BLUE];

		pGameDataObj->m_FlagCarrierRed = 0;
		pGameDataObj->m_FlagCarrierBlue = 0;
	}
}
