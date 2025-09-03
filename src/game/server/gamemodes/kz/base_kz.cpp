/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
/* Based on Race mod stuff and tweaked by GreYFoX@GTi and others to fit our DDRace needs. */
#include "base_kz.h"

#include <engine/server.h>
#include <engine/shared/config.h>
#include <game/mapitems.h>
#include <game/server/entities/character.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <game/server/score.h>
#include <game/version.h>

#define GAME_TYPE_NAME "None"
#define TEST_TYPE_NAME "TestNone"

CGameControllerBaseKZ::CGameControllerBaseKZ(class CGameContext *pGameServer) :
	CGameControllerDDRace(pGameServer)
{
	m_pGameType = g_Config.m_SvTestingCommands ? TEST_TYPE_NAME : GAME_TYPE_NAME;
	m_GameFlags = 0;
}

CGameControllerBaseKZ::~CGameControllerBaseKZ() = default;

void CGameControllerBaseKZ::HandleCharacterTiles(CCharacter *pChr, int MapIndex)
{
	CGameControllerDDRace::HandleCharacterTiles(pChr, MapIndex);
}

void CGameControllerBaseKZ::SetArmorProgress(CCharacter *pCharacter, int Progress)
{
	//Dont
}

void CGameControllerBaseKZ::OnPlayerConnect(CPlayer *pPlayer)
{
	int ClientId = pPlayer->GetCid();
	pPlayer->Respawn();

	if(!Server()->ClientPrevIngame(ClientId))
	{
		char aBuf[128];
		str_format(aBuf, sizeof(aBuf), "team_join player='%d:%s' team=%d", ClientId, Server()->ClientName(ClientId), pPlayer->GetTeam());
		GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
	}

	if(Server()->IsSixup(ClientId))
	{
		{
			protocol7::CNetMsg_Sv_GameInfo Msg;
			Msg.m_GameFlags = m_GameFlags;
			Msg.m_MatchCurrent = 1;
			Msg.m_MatchNum = 0;
			Msg.m_ScoreLimit = g_Config.m_SvScoreLimit;
			Msg.m_TimeLimit = g_Config.m_SvTimeLimit;
			Server()->SendPackMsg(&Msg, MSGFLAG_VITAL | MSGFLAG_NORECORD, ClientId);
		}

		// /team is essential
		{
			protocol7::CNetMsg_Sv_CommandInfoRemove Msg;
			Msg.m_pName = "team";
			Server()->SendPackMsg(&Msg, MSGFLAG_VITAL | MSGFLAG_NORECORD, ClientId);
		}
	}

	// init the player
	Score()->PlayerData(ClientId)->Reset();

	// Can't set score here as LoadScore() is threaded, run it in
	// LoadScoreThreaded() instead
	Score()->LoadPlayerData(ClientId);

	if(!Server()->ClientPrevIngame(ClientId))
	{
		char aBuf[512];
		char aClientName[64];
		GameServer()->IdentifyClientName(ClientId, aClientName, sizeof(aClientName));
		str_format(aBuf, sizeof(aBuf), "'%s' entered and joined the %s using %s", Server()->ClientName(ClientId), GetTeamName(pPlayer->GetTeam()), aClientName);
		GameServer()->SendChat(-1, TEAM_ALL, aBuf, -1);
		GameServer()->SendDiscordChatMessage(-1,aBuf); //+KZ

		GameServer()->SendChatTarget(ClientId, "Kaizo-PvP Mod (ALPHA). DDNet Version: " GAME_VERSION);
		GameServer()->SendChatTarget(ClientId, "please visit m0rekz.github.io or say /info and make sure to read our /rules");
	}

	//balance team
	if(IsTeamPlay() && pPlayer->GetTeam() != TEAM_SPECTATORS)
	{
		int Red = 0,Blue = 0;

		for(auto pPl : GameServer()->m_apPlayers)
		{
			if(!pPl)
				continue;

			if(pPl == pPlayer)
				continue;

			if(pPl->GetTeam() == TEAM_RED)
				Red++;
			else if(pPl->GetTeam() == TEAM_BLUE)
				Blue++;
		}

		if(Blue < Red)
		{
			pPlayer->SetTeam(TEAM_BLUE);
		}
		else
		{
			pPlayer->SetTeam(TEAM_RED);
		}
	}
}

void CGameControllerBaseKZ::OnPlayerDisconnect(CPlayer *pPlayer, const char *pReason)
{
	CGameControllerDDRace::OnPlayerDisconnect(pPlayer, pReason);
}

void CGameControllerBaseKZ::OnReset()
{
	CGameControllerDDRace::OnReset();
}

void CGameControllerBaseKZ::Tick()
{
	if(GameServer()->m_World.m_Paused)
		m_RoundStartTick++;

	if(!HasEnoughPlayers())
	{
		if(!(Server()->Tick() % (Server()->TickSpeed() * 3)))
		{
			GameServer()->SendBroadcast("Waiting for players...", -1);
		}
		m_WaitingForPlayers = true;
	}
	else if(m_WaitingForPlayers)
	{
		GameServer()->SendBroadcast("Match starting...", -1);
		m_WaitingForPlayers = false;
		m_StartingMatch = true;
		m_Warmup = g_Config.m_SvWarmup * Server()->TickSpeed();
		m_IsRoundEnd = false;
		m_GameOverTick = -1;
		m_RoundStartTick = Server()->Tick();
		m_SuddenDeath = 0;
		for(auto pPlayer : GameServer()->m_apPlayers)
		{
			if(!pPlayer)
				continue;

			pPlayer->m_IsDead = false;

			if(CCharacter *pChar = pPlayer->GetCharacter())
			{
				pChar->Reset();
			}
			pPlayer->m_ScoreKZ = 0;
			pPlayer->Respawn();
		}
		OnNewMatch();
	}

	if(m_PausedTicks > 0)
	{
		m_PausedTicks--;
		return;
	}
	else if(m_PausedTicks == 0)
	{
		m_PausedTicks = -1;
		GameServer()->m_World.m_Paused = false;
		m_GameOverTick = -1;
		m_RoundStartTick = Server()->Tick();
		m_SuddenDeath = 0;
		if(!m_IsRoundEnd)
			m_StartingMatch = true;
		else
			m_StartingRound = true;
		for(auto pPlayer : GameServer()->m_apPlayers)
		{
			if(!pPlayer)
				continue;

			pPlayer->m_IsDead = false;

			if(CCharacter *pChar = pPlayer->GetCharacter())
			{
				pChar->Reset();
			}
			if(!m_IsRoundEnd)
				pPlayer->m_ScoreKZ = 0;
			pPlayer->Respawn();
		}
		OnNewMatch();
		m_IsRoundEnd = false;
		return;
	}

	CGameControllerDDRace::Tick();

	if(!m_Warmup)
	{
		m_StartingMatch = false;
		m_StartingRound = false;

		if(g_Config.m_SvTimeLimit && (((g_Config.m_SvTimeLimit * Server()->TickSpeed() * 60) + m_RoundStartTick) <= Server()->Tick()))
			m_SuddenDeath = 1;

		if(m_WinPauseTicks)
		{
			EndMatch(m_WinPauseTicks);
			m_WinPauseTicks = 0;
		}
	}
}

void CGameControllerBaseKZ::DoTeamChange(class CPlayer *pPlayer, int Team, bool DoChatMsg)
{
	Team = ClampTeam(Team);
	if(Team == pPlayer->GetTeam())
		return;

	CCharacter *pCharacter = pPlayer->GetCharacter();

	if(Team == TEAM_SPECTATORS)
	{
		if(g_Config.m_SvTeam != SV_TEAM_FORCED_SOLO && pCharacter)
		{
			// Joining spectators should not kill a locked team, but should still
			// check if the team finished by you leaving it.
			int DDRTeam = pCharacter->Team();
			Teams().SetForceCharacterTeam(pPlayer->GetCid(), TEAM_FLOCK);
			Teams().CheckTeamFinished(DDRTeam);
		}
	}

	IGameController::DoTeamChange(pPlayer, Team, DoChatMsg);
}

int CGameControllerBaseKZ::DoWinCheck()
{
	return 0;
}

void CGameControllerBaseKZ::EndMatch(int Ticks)
{
	GameServer()->m_World.m_Paused = true;
	m_PausedTicks = Ticks;
	m_GameOverTick = Server()->Tick();
}

bool CGameControllerBaseKZ::IsFriendlyFire(int ClientID1, int ClientID2)
{
	if(ClientID1 == ClientID2)
		return false;

	if(IsTeamPlay())
	{
		if(!GameServer()->m_apPlayers[ClientID1] || !GameServer()->m_apPlayers[ClientID2])
			return false;

		if(GameServer()->m_apPlayers[ClientID1]->GetTeam() == GameServer()->m_apPlayers[ClientID2]->GetTeam())
			return true;
	}

	return false;
}