//+KZ code taken from score.cpp
#include "score.h"

#include "player.h"
#include "save.h"
#include "scoreworker.h"

#include <base/system.h>

#include <engine/server/databases/connection_pool.h>
#include <engine/shared/config.h>
#include <engine/shared/console.h>
#include <engine/shared/linereader.h>
#include <engine/storage.h>

#include <generated/wordlist.h>

#include <game/server/gamemodes/DDRace.h>
#include <game/team_state.h>

#include <memory>

void CScore::SaveScoreFloat(int ClientId, float Time, const char *pTimestamp, const float aTimeCp[NUM_CHECKPOINTS], bool NotEligible)
{
	CConsole *pCon = (CConsole *)GameServer()->Console();
	if(pCon->Cheated() || NotEligible)
		return;

	GameServer()->TeehistorianRecordPlayerFinish(ClientId, (int)(Time*Server()->TickSpeed()));

	CPlayer *pCurPlayer = GameServer()->m_apPlayers[ClientId];
	if(pCurPlayer->m_ScoreFinishResult != nullptr)
		dbg_msg("sql", "WARNING: previous save score result didn't complete, overwriting it now");
	pCurPlayer->m_ScoreFinishResult = std::make_shared<CScorePlayerResult>();
	auto Tmp = std::make_unique<CSqlScoreData>(pCurPlayer->m_ScoreFinishResult);
	str_copy(Tmp->m_aMap, Server()->GetMapName(), sizeof(Tmp->m_aMap));
	FormatUuid(GameServer()->GameUuid(), Tmp->m_aGameUuid, sizeof(Tmp->m_aGameUuid));
	Tmp->m_ClientId = ClientId;
	str_copy(Tmp->m_aName, Server()->ClientName(ClientId), sizeof(Tmp->m_aName));
	Tmp->m_Time = Time;
	str_copy(Tmp->m_aTimestamp, pTimestamp, sizeof(Tmp->m_aTimestamp));
	for(int i = 0; i < NUM_CHECKPOINTS; i++)
		Tmp->m_aCurrentTimeCp[i] = aTimeCp[i];

	m_pPool->ExecuteWrite(CScoreWorker::SaveScore, std::move(Tmp), "save score");
}

void CScore::SaveTeamScoreFloat(int Team, int *pClientIds, unsigned int Size, float Time, const char *pTimestamp)
{
	CConsole *pCon = (CConsole *)GameServer()->Console();
	if(pCon->Cheated())
		return;
	for(unsigned int i = 0; i < Size; i++)
	{
		if(GameServer()->m_apPlayers[pClientIds[i]]->m_NotEligibleForFinish)
			return;
	}

	GameServer()->TeehistorianRecordTeamFinish(Team, (int)(Time*Server()->TickSpeed()));

	auto Tmp = std::make_unique<CSqlTeamScoreData>();
	for(unsigned int i = 0; i < Size; i++)
		str_copy(Tmp->m_aaNames[i], Server()->ClientName(pClientIds[i]), sizeof(Tmp->m_aaNames[i]));
	Tmp->m_Size = Size;
	Tmp->m_Time = Time;
	str_copy(Tmp->m_aTimestamp, pTimestamp, sizeof(Tmp->m_aTimestamp));
	FormatUuid(GameServer()->GameUuid(), Tmp->m_aGameUuid, sizeof(Tmp->m_aGameUuid));
	str_copy(Tmp->m_aMap, Server()->GetMapName(), sizeof(Tmp->m_aMap));
	Tmp->m_TeamrankUuid = RandomUuid();

	m_pPool->ExecuteWrite(CScoreWorker::SaveTeamScore, std::move(Tmp), "save team score");
}