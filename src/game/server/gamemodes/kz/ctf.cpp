/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
/* Based on Race mod stuff and tweaked by GreYFoX@GTi and others to fit our DDRace needs. */
#include "ctf.h"

#include <engine/server.h>
#include <engine/shared/config.h>
#include <game/mapitems.h>
#include <game/server/entities/character.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <game/server/score.h>
#include <game/version.h>

#define GAME_TYPE_NAME "CTF-rw"
#define TEST_TYPE_NAME "TestCTF"

CGameControllerCTF::CGameControllerCTF(class CGameContext *pGameServer) :
	CGameControllerTDM(pGameServer)
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

	m_apFlags[TEAM_RED] = nullptr;
	m_apFlags[TEAM_BLUE] = nullptr;

	m_aTeamScore[TEAM_RED] = 0;
	m_aTeamScore[TEAM_BLUE] = 0;

	m_flagstand_temp_i_0 = 0;
	m_flagstand_temp_i_1 = 0; 
}

CGameControllerCTF::~CGameControllerCTF() = default;

int CGameControllerCTF::DoWinCheck()
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

void CGameControllerCTF::OnNewMatch()
{
	m_aTeamScore[TEAM_RED] = 0;
	m_aTeamScore[TEAM_BLUE] = 0;
}

int CGameControllerCTF::OnCharacterDeath(CCharacter *pVictim, CPlayer *pKiller, int Weapon)
{
	CGameControllerBasePvP::OnCharacterDeath(pVictim,pKiller,Weapon);
	int HadFlag = 0;

	// drop flags
	for(CFlag *pFlag : m_apFlags)
	{
		if(pFlag && pKiller && pKiller->GetCharacter() && pFlag->GetCarrier() == pKiller->GetCharacter())
			HadFlag |= 2;
		if(pFlag && pFlag->GetCarrier() == pVictim)
		{
			GameServer()->CreateSoundGlobal(SOUND_CTF_DROP);
			GameServer()->SendGameMsg(protocol7::GAMEMSG_CTF_DROP, -1);
			pFlag->Drop();
			// https://github.com/ddnet-insta/ddnet-insta/issues/156
			pFlag->m_pLastCarrier = nullptr;

			HadFlag |= 1;
		}
		if(pFlag && pFlag->GetCarrier() == pVictim)
			pFlag->SetCarrier(0);
	}

	return HadFlag;
}

bool CGameControllerCTF::OnEntity(int Index, int x, int y, int Layer, int Flags, bool Initial, int Number)
{
	const vec2 Pos(x * 32.0f + 16.0f, y * 32.0f + 16.0f);
	int Team = -1;
	if(Index == ENTITY_FLAGSTAND_RED)
		Team = TEAM_RED;
	if(Index == ENTITY_FLAGSTAND_BLUE)
		Team = TEAM_BLUE;

	//twplus begin +KZ
	if(!(Team == -1 || m_apFlags[Team]))
	{
		CFlag *F = new CFlag(&GameServer()->m_World, Team);
		//F->m_StandPos = Pos;
		F->m_Pos = Pos;
		m_apFlags[Team] = F;
		GameServer()->m_World.InsertEntity(F);
	}
	
	if (Team == TEAM_RED && m_flagstand_temp_i_0 < 10) {
		//m_flagstands_0[m_flagstand_temp_i_0] = Pos;
		m_apFlags[Team]->m_StandPositions[m_flagstand_temp_i_0] = Pos;
		m_flagstand_temp_i_0++;
		m_apFlags[Team]->m_no_stands = m_flagstand_temp_i_0;
	}
	if (Team == TEAM_BLUE && m_flagstand_temp_i_1 < 10) {
		//m_flagstands_1[m_flagstand_temp_i_1] = Pos;
		m_apFlags[Team]->m_StandPositions[m_flagstand_temp_i_1] = Pos;
		m_flagstand_temp_i_1++;
		m_apFlags[Team]->m_no_stands = m_flagstand_temp_i_1;
	}
	if (Team == -1)
	{
		return CGameControllerTDM::OnEntity(Index, x, y, Layer, Flags, Initial, Number);;
	}

	return true;
}

void CGameControllerCTF::OnCharacterDestroy(const CCharacter *pChar)
{
	// drop flags
	for(CFlag *pFlag : m_apFlags)
	{
		if(pFlag && pFlag->GetCarrier() == pChar)
			pFlag->SetCarrier(0);
	}
}

void CGameControllerCTF::HandleFlag()
{
	if(GameServer()->m_World.m_ResetRequested || GameServer()->m_World.m_Paused)
		return;

	for(int FlagColor = 0; FlagColor < 2; FlagColor++)
	{
		CFlag *pFlag = m_apFlags[FlagColor];

		if(!pFlag)
			continue;

		//
		if(pFlag->GetCarrier())
		{
			// forbid holding flags in ddrace teams
			if(GameServer()->GetDDRaceTeam(pFlag->GetCarrier()->GetPlayer()->GetCid()))
			{
				GameServer()->CreateSoundGlobal(SOUND_CTF_DROP);
				GameServer()->SendGameMsg(protocol7::GAMEMSG_CTF_DROP, -1);
				pFlag->Drop();
				continue;
			}

			if(m_apFlags[FlagColor ^ 1] && m_apFlags[FlagColor ^ 1]->IsAtStand())
			{
				if(distance(pFlag->GetPos(), m_apFlags[FlagColor ^ 1]->GetPos()) < CFlag::ms_PhysSize + CCharacterCore::PhysicalSize())
				{
					// CAPTURE! \o/
					m_aTeamScore[FlagColor ^ 1] += 100;
					pFlag->GetCarrier()->GetPlayer()->m_ScoreKZ += 5;
					float Diff = Server()->Tick() - pFlag->GetGrabTick();

					char aBuf[64];
					str_format(aBuf, sizeof(aBuf), "flag_capture player='%d:%s' team=%d time=%.2f",
						pFlag->GetCarrier()->GetPlayer()->GetCid(),
						Server()->ClientName(pFlag->GetCarrier()->GetPlayer()->GetCid()),
						pFlag->GetCarrier()->GetPlayer()->GetTeam(),
						Diff / (float)Server()->TickSpeed());
					GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);

					float CaptureTime = Diff / (float)Server()->TickSpeed();
					if(CaptureTime <= 60)
						str_format(aBuf,
							sizeof(aBuf),
							"The %s flag was captured by '%s' (%d.%s%d seconds)", FlagColor ? "blue" : "red",
							Server()->ClientName(pFlag->GetCarrier()->GetPlayer()->GetCid()), (int)CaptureTime % 60, ((int)(CaptureTime * 100) % 100) < 10 ? "0" : "", (int)(CaptureTime * 100) % 100);
					else
						str_format(
							aBuf,
							sizeof(aBuf),
							"The %s flag was captured by '%s'", FlagColor ? "blue" : "red",
							Server()->ClientName(pFlag->GetCarrier()->GetPlayer()->GetCid()));
					for(auto &pPlayer : GameServer()->m_apPlayers)
					{
						if(!pPlayer)
							continue;
						if(Server()->IsSixup(pPlayer->GetCid()))
							continue;

						GameServer()->SendChatTarget(pPlayer->GetCid(), aBuf);
					}
					GameServer()->SendGameMsg(protocol7::GAMEMSG_CTF_CAPTURE, FlagColor, pFlag->GetCarrier()->GetPlayer()->GetCid(), Diff, -1);
					GameServer()->CreateSoundGlobal(SOUND_CTF_CAPTURE);
					for(CFlag *pF : m_apFlags)
						pF->Reset();
					// do a win check(capture could trigger win condition)
					if(DoWinCheck())
						return;
				}
			}
		}
		else
		{
			CCharacter *apCloseCCharacters[MAX_CLIENTS];
			int Num = GameServer()->m_World.FindEntities(pFlag->GetPos(), CFlag::ms_PhysSize, (CEntity **)apCloseCCharacters, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
			for(int i = 0; i < Num; i++)
			{
				if(!apCloseCCharacters[i]->IsAlive() || apCloseCCharacters[i]->GetPlayer()->GetTeam() == TEAM_SPECTATORS || GameServer()->Collision()->IntersectLine(pFlag->GetPos(), apCloseCCharacters[i]->GetPos(), NULL, NULL))
					continue;

				// only allow flag grabs in team 0
				if(GameServer()->GetDDRaceTeam(apCloseCCharacters[i]->GetPlayer()->GetCid()))
					continue;

				// cooldown for recollect after dropping the flag
				if(pFlag->m_pLastCarrier == apCloseCCharacters[i] && (pFlag->m_DropTick + Server()->TickSpeed()) > Server()->Tick())
					continue;

				if(apCloseCCharacters[i]->GetPlayer()->GetTeam() == pFlag->GetTeam())
				{
					// return the flag
					if(!pFlag->IsAtStand())
					{
						CCharacter *pChr = apCloseCCharacters[i];
						pChr->GetPlayer()->m_ScoreKZ++;

						char aBuf[256];
						str_format(aBuf, sizeof(aBuf), "flag_return player='%d:%s' team=%d",
							pChr->GetPlayer()->GetCid(),
							Server()->ClientName(pChr->GetPlayer()->GetCid()),
							pChr->GetPlayer()->GetTeam());
						GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
						GameServer()->SendGameMsg(protocol7::GAMEMSG_CTF_RETURN, -1);
						GameServer()->CreateSoundGlobal(SOUND_CTF_RETURN);
						pFlag->Reset();
					}
				}
				else
				{
					// take the flag
					if(pFlag->IsAtStand())
						m_aTeamScore[FlagColor ^ 1]++;

					pFlag->Grab(apCloseCCharacters[i]);

					pFlag->GetCarrier()->GetPlayer()->m_ScoreKZ++;

					char aBuf[256];
					str_format(aBuf, sizeof(aBuf), "flag_grab player='%d:%s' team=%d",
						pFlag->GetCarrier()->GetPlayer()->GetCid(),
						Server()->ClientName(pFlag->GetCarrier()->GetPlayer()->GetCid()),
						pFlag->GetCarrier()->GetPlayer()->GetTeam());
					GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
					GameServer()->SendGameMsg(protocol7::GAMEMSG_CTF_GRAB, FlagColor, -1);
					DoWinCheck();
					break;
				}
			}
		}
	}
}

void CGameControllerCTF::Tick()
{
	CGameControllerTDM::Tick();
	HandleFlag();
}

void CGameControllerCTF::Snap(int SnappingClient)
{
	CGameControllerBasePvP::Snap(SnappingClient);

	int FlagCarrierRed = FLAG_MISSING;
	if(m_apFlags[TEAM_RED])
	{
		if(m_apFlags[TEAM_RED]->m_AtStand)
			FlagCarrierRed = FLAG_ATSTAND;
		else if(m_apFlags[TEAM_RED]->GetCarrier() && m_apFlags[TEAM_RED]->GetCarrier()->GetPlayer())
			FlagCarrierRed = m_apFlags[TEAM_RED]->GetCarrier()->GetPlayer()->GetCid();
		else
			FlagCarrierRed = FLAG_TAKEN;
	}

	int FlagCarrierBlue = FLAG_MISSING;
	if(m_apFlags[TEAM_BLUE])
	{
		if(m_apFlags[TEAM_BLUE]->m_AtStand)
			FlagCarrierBlue = FLAG_ATSTAND;
		else if(m_apFlags[TEAM_BLUE]->GetCarrier() && m_apFlags[TEAM_BLUE]->GetCarrier()->GetPlayer())
			FlagCarrierBlue = m_apFlags[TEAM_BLUE]->GetCarrier()->GetPlayer()->GetCid();
		else
			FlagCarrierBlue = FLAG_TAKEN;
	}

	if(Server()->IsSixup(SnappingClient))
	{
		protocol7::CNetObj_GameDataFlag *pGameDataObj = Server()->SnapNewItem<protocol7::CNetObj_GameDataFlag>(0);
		if(!pGameDataObj)
			return;

		pGameDataObj->m_FlagCarrierRed = FlagCarrierRed;
		pGameDataObj->m_FlagCarrierBlue = FlagCarrierBlue;

		protocol7::CNetObj_GameDataTeam *pGameDataObj2 = Server()->SnapNewItem<protocol7::CNetObj_GameDataTeam>(0);

		if(!pGameDataObj2)
			return;

		pGameDataObj2->m_TeamscoreRed = m_aTeamScore[TEAM_RED];
		pGameDataObj2->m_TeamscoreBlue = m_aTeamScore[TEAM_BLUE];
	}
	else
	{
		CNetObj_GameData *pGameDataObj = Server()->SnapNewItem<CNetObj_GameData>(0);
		if(!pGameDataObj)
			return;

		pGameDataObj->m_FlagCarrierRed = FlagCarrierRed;
		pGameDataObj->m_FlagCarrierBlue = FlagCarrierBlue;

		pGameDataObj->m_TeamscoreRed = m_aTeamScore[TEAM_RED];
		pGameDataObj->m_TeamscoreBlue = m_aTeamScore[TEAM_BLUE];
	}
}
