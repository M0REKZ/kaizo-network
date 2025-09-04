/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
/* Based on Race mod stuff and tweaked by GreYFoX@GTi and others to fit our DDRace needs. */
#include "lms.h"

#include <engine/server.h>
#include <engine/shared/config.h>
#include <game/mapitems.h>
#include <game/server/entities/character.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <game/server/score.h>
#include <game/version.h>

#define GAME_TYPE_NAME "LMS-rw"
#define TEST_TYPE_NAME "TestLMS"

CGameControllerLMS::CGameControllerLMS(class CGameContext *pGameServer) :
	CGameControllerDM(pGameServer)
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
	
	m_GameFlags = protocol7::GAMEFLAG_SURVIVAL;
}

CGameControllerLMS::~CGameControllerLMS() = default;

void CGameControllerLMS::OnNewMatch()
{
	CGameControllerDM::OnNewMatch();

	for(auto pPlayer : GameServer()->m_apPlayers)
	{
		if(!pPlayer)
			continue;

		pPlayer->m_IsDead = false;
		pPlayer->m_Lives = g_Config.m_SvSurvivalLives; // +KZ LMS/LTS Lives
	}
}

int CGameControllerLMS::DoWinCheck()
{
	if(GameServer()->m_World.m_Paused || m_StartingMatch || m_StartingRound || m_WaitingForPlayers)
		return 0;
		
	// check score win condition
	if(CGameControllerDM::DoWinCheck())
		return 1;

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
		m_WinPauseTicks = 5 * Server()->TickSpeed();
		m_IsRoundEnd = true;
		return 1;
	}
	else if(AlivePlayerCount == 1) // 1 winner
	{
		pAlivePlayer->m_ScoreKZ++;
		m_WinPauseTicks = 5 * Server()->TickSpeed();
		m_IsRoundEnd = true;
		return 1;
	}

	return 0;
}

int CGameControllerLMS::OnCharacterDeath(CCharacter *pVictim, CPlayer *pKiller, int Weapon)
{
	if(pVictim && pVictim->GetPlayer())
	{
		if(!pVictim->GetPlayer()->m_Lives)
			pVictim->GetPlayer()->m_IsDead = true;
		else
		{
			pVictim->GetPlayer()->m_Lives--;
			char aBuf[64];
			str_format(aBuf, sizeof(aBuf), "Lives left: %d", pVictim->GetPlayer()->m_Lives);
			GameServer()->SendBroadcast(aBuf, pVictim->GetPlayer()->GetCid());
		}
	}
	return CGameControllerDM::OnCharacterDeath(pVictim,pKiller,Weapon);
}

void CGameControllerLMS::OnCharacterSpawn(CCharacter *pChr)
{
	CGameControllerDM::OnCharacterSpawn(pChr);

	if(pChr && m_InstagibWeapon == -1)
	{
		// give start equipment
		pChr->IncreaseArmor(5);
		pChr->GiveWeapon(WEAPON_SHOTGUN);
		pChr->SetWeaponAmmo(WEAPON_SHOTGUN, 10);
		pChr->GiveWeapon(WEAPON_GRENADE);
		pChr->SetWeaponAmmo(WEAPON_GRENADE, 5);
		pChr->GiveWeapon(WEAPON_LASER);
		pChr->SetWeaponAmmo(WEAPON_LASER, 5);
	}
}

bool CGameControllerLMS::OnEntity(int Index, int x, int y, int Layer, int Flags, bool Initial, int Number)
{
	if(Index == ENTITY_ARMOR_1 || Index == ENTITY_HEALTH_1 || Index == ENTITY_WEAPON_GRENADE || Index == ENTITY_WEAPON_LASER || Index == ENTITY_WEAPON_SHOTGUN || Index == ENTITY_POWERUP_NINJA)
		return false;

	return CGameControllerDM::OnEntity(Index, x, y, Layer, Flags, Initial, Number);
}