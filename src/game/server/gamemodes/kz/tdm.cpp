/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
/* Based on Race mod stuff and tweaked by GreYFoX@GTi and others to fit our DDRace needs. */
#include "tdm.h"

#include <engine/server.h>
#include <engine/shared/config.h>
#include <game/mapitems.h>
#include <game/server/entities/character.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <game/server/score.h>
#include <game/version.h>

#define GAME_TYPE_NAME "TDM-rw"
#define TEST_TYPE_NAME "TestTDM"

CGameControllerTDM::CGameControllerTDM(class CGameContext *pGameServer) :
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

	m_GameFlags = GAMEFLAG_TEAMS;

	m_aTeamScore[TEAM_RED] = 0;
	m_aTeamScore[TEAM_BLUE] = 0;
}

CGameControllerTDM::~CGameControllerTDM() = default;

int CGameControllerTDM::DoWinCheck()
{
	if(m_StartingMatch || m_StartingRound || m_WaitingForPlayers)
		return 0;

	// check score win condition
	if((g_Config.m_SvScoreLimit > 0 && (m_aTeamScore[TEAM_RED] >= g_Config.m_SvScoreLimit || m_aTeamScore[TEAM_BLUE] >= g_Config.m_SvScoreLimit)) ||
		(g_Config.m_SvTimeLimit > 0 && (Server()->Tick() - m_RoundStartTick) >= g_Config.m_SvTimeLimit * Server()->TickSpeed() * 60))
	{
		if(m_SuddenDeath)
		{
			if(m_aTeamScore[TEAM_RED] / 100 != m_aTeamScore[TEAM_BLUE] / 100)
			{
				m_WinPauseTicks = 10 * Server()->TickSpeed();
				return 1;
			}
		}
		else
		{
			if(m_aTeamScore[TEAM_RED] != m_aTeamScore[TEAM_BLUE])
			{
				m_WinPauseTicks = 10 * Server()->TickSpeed();
				return 1;
			}
			else
				m_SuddenDeath = 1;
		}
	}
	return 0;
}

void CGameControllerTDM::OnNewMatch()
{
	m_aTeamScore[TEAM_RED] = 0;
	m_aTeamScore[TEAM_BLUE] = 0;
}

int CGameControllerTDM::OnCharacterDeath(CCharacter *pVictim, CPlayer *pKiller, int Weapon)
{
	int S = CGameControllerBasePvP::OnCharacterDeath(pVictim,pKiller,Weapon);

	if(!S || !pKiller)
		return 0;
	
	int Team = pKiller->GetTeam();

	if(Team == TEAM_RED)
		m_aTeamScore[TEAM_RED] += S > 0 ? 1 : -1;
	else if(Team == TEAM_BLUE)
		m_aTeamScore[TEAM_BLUE] += S > 0 ? 1 : -1;
	
	DoWinCheck();
}

bool CGameControllerTDM::CanJoinTeam(int Team, int NotThisId, char *pErrorReason, int ErrorReasonSize)
{
	if(Team == TEAM_RED || Team == TEAM_BLUE || Team == TEAM_SPECTATORS)
		return true;

	return CGameControllerBasePvP::CanJoinTeam(Team,NotThisId,pErrorReason,ErrorReasonSize);
}

void CGameControllerTDM::Snap(int SnappingClient)
{
	CGameControllerBasePvP::Snap(SnappingClient);

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
