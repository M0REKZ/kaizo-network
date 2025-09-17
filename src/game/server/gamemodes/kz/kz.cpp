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

#include <game/server/entities/kz/kz_pickup.h>
#include <game/server/entities/kz/kz_gun.h>
#include <game/server/entities/kz/kz_light.h>
#include <game/server/entities/kz/mine.h>

#define GAME_TYPE_NAME "DDraceNetwork"
#define TEST_TYPE_NAME "TestDDraceNetwork"

CGameControllerKZ::CGameControllerKZ(class CGameContext *pGameServer) :
	CGameControllerDDRace(pGameServer)
{
	m_pGameType = g_Config.m_SvTestingCommands ? TEST_TYPE_NAME : GAME_TYPE_NAME;
	m_GameFlags = protocol7::GAMEFLAG_RACE | protocol7::GAMEFLAG_FLAGS;

	m_apFlags[0] = 0;
	m_apFlags[1] = 0;

	m_flagstand_temp_i_0 = 0;
	m_flagstand_temp_i_1 = 0; 
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
	IGameController::OnPlayerConnect(pPlayer);
	int ClientId = pPlayer->GetCid();

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
		GameServer()->SendChat(-1, TEAM_ALL, aBuf, -1, CGameContext::FLAG_SIX);
		GameServer()->SendDiscordChatMessage(-1,aBuf); //+KZ

		GameServer()->SendChatTarget(ClientId, "Kaizo Network mod by +KZ based on DDNet Version: " GAME_VERSION);
		GameServer()->SendChatTarget(ClientId, "visit m0rekz.github.io and make sure to read our /rules");
	}

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

	FlagTick();

	DoCrown();
}

void CGameControllerKZ::DoTeamChange(class CPlayer *pPlayer, int Team, bool DoChatMsg)
{
	CGameControllerDDRace::DoTeamChange(pPlayer, Team, DoChatMsg);
}

bool CGameControllerKZ::OnEntity(int Index, int x, int y, int Layer, int Flags, bool Initial, int Number)
{
	CGameControllerDDRace::OnEntity(Index, x, y, Layer, Flags, Initial, Number);

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
		return CGameControllerDDRace::OnEntity(Index, x, y, Layer, Flags, Initial, Number);;
	}

	return true;
}

void CGameControllerKZ::Snap(int SnappingClient)
{
	CGameControllerDDRace::Snap(SnappingClient);

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
	}
	else
	{
		CNetObj_GameData *pGameDataObj = Server()->SnapNewItem<CNetObj_GameData>(0);
		if(!pGameDataObj)
			return;

		pGameDataObj->m_FlagCarrierRed = FlagCarrierRed;
		pGameDataObj->m_FlagCarrierBlue = FlagCarrierBlue;

		pGameDataObj->m_TeamscoreRed = 0;
		pGameDataObj->m_TeamscoreBlue = 0;
	}
}

int CGameControllerKZ::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon)
{
	CGameControllerDDRace::OnCharacterDeath(pVictim, pKiller, Weapon);
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

void CGameControllerKZ::FlagTick()
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
			if(!g_Config.m_SvSoloServer && GameServer()->GetDDRaceTeam(pFlag->GetCarrier()->GetPlayer()->GetCid()))
			{
				GameServer()->CreateSoundGlobal(SOUND_CTF_DROP);
				GameServer()->SendGameMsg(protocol7::GAMEMSG_CTF_DROP, -1);
				pFlag->Drop();
				continue;
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
				if(!g_Config.m_SvSoloServer && GameServer()->GetDDRaceTeam(apCloseCCharacters[i]->GetPlayer()->GetCid()))
					continue;

				if(pFlag->GetOtherFlag() && pFlag->GetOtherFlag()->m_pCarrier == apCloseCCharacters[i]) //+KZ dont grab flag if he is already with a flag
					continue;

				// cooldown for recollect after dropping the flag
				if(pFlag->m_pLastCarrier == apCloseCCharacters[i] && (pFlag->m_DropTick + Server()->TickSpeed()) > Server()->Tick())
					continue;

				{
					// take the flag

					pFlag->Grab(apCloseCCharacters[i]);

					char aBuf[256];
					str_format(aBuf, sizeof(aBuf), "flag_grab player='%d:%s' team=%d",
						pFlag->GetCarrier()->GetPlayer()->GetCid(),
						Server()->ClientName(pFlag->GetCarrier()->GetPlayer()->GetCid()),
						pFlag->GetCarrier()->GetPlayer()->GetTeam());
					GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
					GameServer()->SendGameMsg(protocol7::GAMEMSG_CTF_GRAB, FlagColor, -1);
					break;
				}
			}
		}
	}
}

void CGameControllerKZ::DoCrown()
{
	float besttime = -1.0f;
	int playercount = 0;

	for(int i = 0; i < SERVER_MAX_CLIENTS;i++)
	{
		
		if(!GameServer()->m_apPlayers[i])
			continue;

		playercount++;

		if(!GameServer()->GetPlayerChar(i))
			continue;

		GameServer()->GetPlayerChar(i)->m_EnableCrown = false;
		
		CPlayerData *pData = GameServer()->Score()->PlayerData(i);

		if(!pData)
			continue;
		
		if(!pData->m_BestTime)
			continue;

		if(besttime == -1.0f)
		{
			besttime = pData->m_BestTime;
		}
		if(besttime >= pData->m_BestTime)
		{
			besttime = pData->m_BestTime;
		}
		
	}

	if(playercount > 1)
	{
		for(int i = 0; i < SERVER_MAX_CLIENTS;i++)
		{
			
			if(!GameServer()->m_apPlayers[i])
				continue;

			if(!GameServer()->GetPlayerChar(i))
				continue;

			GameServer()->GetPlayerChar(i)->m_EnableCrown = false;
			
			CPlayerData *pData = GameServer()->Score()->PlayerData(i);

			if(!pData)
				continue;

			if(besttime == pData->m_BestTime)
			{
				GameServer()->GetPlayerChar(i)->m_EnableCrown = true;
			}
			
		}
	}
}

bool CGameControllerKZ::OnEntityKZ(int Index, int x, int y, int Layer, int Flags, bool Initial, unsigned char Number, int64_t Value1, int64_t Value2, int64_t Value3)
{
	int PickupType = -1;
	int PickupSubtype = -1;

	int aSides[8] = {0,0,0,0,0,0,0,0};
	if(GameServer()->Collision()->DDNetLayerExists(Layer))
	{
		aSides[0] = GameServer()->Collision()->Entity(x, y + 1, Layer);
		aSides[1] = GameServer()->Collision()->Entity(x + 1, y + 1, Layer);
		aSides[2] = GameServer()->Collision()->Entity(x + 1, y, Layer);
		aSides[3] = GameServer()->Collision()->Entity(x + 1, y - 1, Layer);
		aSides[4] = GameServer()->Collision()->Entity(x, y - 1, Layer);
		aSides[5] = GameServer()->Collision()->Entity(x - 1, y - 1, Layer);
		aSides[6] = GameServer()->Collision()->Entity(x - 1, y, Layer);
		aSides[7] = GameServer()->Collision()->Entity(x - 1, y + 1, Layer);
	}

	if(Index == KZ_TILE_PORTAL_GUN)
	{
		PickupType = POWERUP_WEAPON;
		PickupSubtype = KZ_CUSTOM_WEAPON_PORTAL_GUN;
	}

	const vec2 Pos(x * 32.0f + 16.0f, y * 32.0f + 16.0f);

	if(Index == KZ_TILE_TURRET)
	{
		new CKZGun(&GameServer()->m_World,Pos,true,false,Layer,Number);
		m_ShowHealth = true;
		return true;
	}

	if(Index == KZ_TILE_TURRET_EXPLOSIVE)
	{
		new CKZGun(&GameServer()->m_World,Pos,true,true,Layer,Number);
		m_ShowHealth = true;
		return true;
	}

	if(Index == KZ_TILE_DAMAGE_LASER)
	{
		int aSides2[8] = {0,0,0,0,0,0,0,0};
		if(GameServer()->Collision()->DDNetLayerExists(Layer))
		{
			aSides2[0] = GameServer()->Collision()->Entity(x, y + 2, Layer);
			aSides2[1] = GameServer()->Collision()->Entity(x + 2, y + 2, Layer);
			aSides2[2] = GameServer()->Collision()->Entity(x + 2, y, Layer);
			aSides2[3] = GameServer()->Collision()->Entity(x + 2, y - 2, Layer);
			aSides2[4] = GameServer()->Collision()->Entity(x, y - 2, Layer);
			aSides2[5] = GameServer()->Collision()->Entity(x - 2, y - 2, Layer);
			aSides2[6] = GameServer()->Collision()->Entity(x - 2, y, Layer);
			aSides2[7] = GameServer()->Collision()->Entity(x - 2, y + 2, Layer);
		}

		float AngularSpeed = 0.0f;
		
		AngularSpeed = (pi / 360) * Value1;

		for(int i = 0; i < 8; i++)
		{
			if(aSides[i] >= ENTITY_LASER_SHORT && aSides[i] <= ENTITY_LASER_LONG)
			{
				CKZLight *pLight = new CKZLight(&GameServer()->m_World, Pos, pi / 4 * i, 32 * 3 + 32 * (aSides[i] - ENTITY_LASER_SHORT) * 3, Layer, Number);
				pLight->m_AngularSpeed = AngularSpeed;
				m_ShowHealth = true;
				if(aSides2[i] >= ENTITY_LASER_C_SLOW && aSides2[i] <= ENTITY_LASER_C_FAST)
				{
					pLight->m_Speed = 1 + (aSides2[i] - ENTITY_LASER_C_SLOW) * 2;
					pLight->m_CurveLength = pLight->m_Length;
				}
				else if(aSides2[i] >= ENTITY_LASER_O_SLOW && aSides2[i] <= ENTITY_LASER_O_FAST)
				{
					pLight->m_Speed = 1 + (aSides2[i] - ENTITY_LASER_O_SLOW) * 2;
					pLight->m_CurveLength = 0;
				}
				else
					pLight->m_CurveLength = pLight->m_Length;
			}
		}
	}

	if(Index == KZ_TILE_MINE)
	{
		new CMine(&GameServer()->m_World, Pos, Number);
		m_ShowHealth = true;
	}

	if(Index == KZ_TILE_DAMAGE_ZONE || Index == KZ_TILE_HEALTH_ZONE)
	{
		m_ShowHealth = true;
	}

	if(PickupType != -1)
	{
		if(PickupSubtype != -1)
		{
			switch (PickupSubtype)
			{
				case KZ_CUSTOM_WEAPON_PORTAL_GUN:
				{
					CKZPickup *pPickup = new CKZPickup(&GameServer()->m_World, PickupType, PickupSubtype, Layer, (int)Number, Flags);
					pPickup->m_Pos = Pos;
					return true;
				}
				break;
			

			}
		}
	}

	return false;
}

void CGameControllerKZ::OnNewRecordKZ(int ClientId, float Time, float PrevTime)
{
	if(GameServer()->Console()->Cheated())
		return;

	if(Time < PrevTime || !PrevTime)
	{
		GameServer()->SendDiscordRecordMessage(ClientId,Time,PrevTime);
	}
}
