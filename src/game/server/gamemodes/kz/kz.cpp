// (c) +KZ

#include "kz.h"

#include <engine/server.h>
#include <engine/shared/config.h>
#include <game/mapitems.h>
#include <game/server/entities/character.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <game/server/score.h>
#include <game/version.h>

#define GAME_TYPE_NAME "DDraceNetwork"
#define TEST_TYPE_NAME "TestDDraceNetwork"

CGameControllerKZ::CGameControllerKZ(class CGameContext *pGameServer) :
	CGameControllerDDRace(pGameServer)
{
	m_pGameType = g_Config.m_SvTestingCommands ? TEST_TYPE_NAME : GAME_TYPE_NAME;
	m_GameFlags = protocol7::GAMEFLAG_RACE;
}

CGameControllerKZ::~CGameControllerKZ() = default;

CScore *CGameControllerKZ::Score()
{
	return CGameControllerDDRace::Score();
}

void CGameControllerKZ::HandleCharacterTiles(CCharacter *pChr, int MapIndex)
{
	CGameControllerDDRace::HandleCharacterTiles(pChr, MapIndex);
}

void CGameControllerKZ::SetArmorProgress(CCharacter *pCharacter, int Progress)
{
	CGameControllerDDRace::SetArmorProgress(pCharacter, Progress);
}

void CGameControllerKZ::OnPlayerConnect(CPlayer *pPlayer)
{
	CGameControllerDDRace::OnPlayerConnect(pPlayer);

	if(pPlayer)
		pPlayer->m_SentKZWelcomeMsg = false;
}

void CGameControllerKZ::OnPlayerDisconnect(CPlayer *pPlayer, const char *pReason)
{
	CGameControllerDDRace::OnPlayerDisconnect(pPlayer, pReason);
}

void CGameControllerKZ::OnCharacterSpawn(class CCharacter *pChr)
{
	CGameControllerDDRace::OnCharacterSpawn(pChr);

	if(pChr && pChr->GetPlayer() && !pChr->GetPlayer()->m_SentKZWelcomeMsg)
	{
		pChr->GetPlayer()->m_SentKZWelcomeMsg = true;
		GameServer()->SendBroadcast("--- Welcome to Kaizo Network! ---", pChr->GetPlayer()->GetCid());
	}
}

void CGameControllerKZ::OnReset()
{
	CGameControllerDDRace::OnReset();
}

void CGameControllerKZ::Tick()
{
	CGameControllerDDRace::Tick();
}

void CGameControllerKZ::DoTeamChange(class CPlayer *pPlayer, int Team, bool DoChatMsg)
{
	CGameControllerDDRace::DoTeamChange(pPlayer, Team, DoChatMsg);
}
