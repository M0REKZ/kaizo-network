//From CMClient
#include "splits.h"

#include <engine/shared/config.h>
#include <engine/shared/json.h>
#include <engine/shared/jsonwriter.h>

#include <generated/protocol.h>

#include <game/client/gameclient.h>

#include <climits>

#include <base/io.h>
#include <base/process.h>

//+KZ
extern int g_KaizoConfig_KaizoSplits;
extern int g_KaizoConfig_KaizoSplitsComparisonSource;
extern int g_KaizoConfig_KaizoSplitsResetAfterFinishDeath;
extern int g_KaizoConfig_KaizoSplitsResetAfterDeath;
extern int g_KaizoConfig_KaizoSplitsTrackDummy;
extern int g_KaizoConfig_KaizoSplitsForceAny;
extern int g_KaizoConfig_KaizoSplitsComparisonForceCategory;

const char *CSplits::ms_pSplitsDir = "splits";

CSplits::CSplits()
{
	m_SplitsActive = false;
	OnReset();
}

void CSplits::OnReset()
{
	for(int i = 0; i < NUM_DUMMIES; i++)
	{
		for(int j = 0; j < SPLIT_TOTAL; j++)
			m_aSplitTimesCurrent[i][j] = -1;
		m_SplitCategory[i] = SPLITCATEGORY_NORANK;
		m_RaceStartTick[i] = -1, m_LastRaceStartTick[i] = -1;
		m_DummyId[i] = -1;
		m_WasTeam0Mode[i] = false, m_HasStarted[i] = false, m_HasDied[i] = false;
		m_HasQueuedSplit[i] = false;
	}
	for(int i = 0; i < MAX_CLIENTS; i++)
		m_Teams[i] = -1;
	m_CurrentDummy = 0;

	m_aSplitTimesBestRun = m_aSplitTimesBestAnyRun;
	m_aSplitTimesBestSegmentTotal = m_aSplitTimesBestAnySegmentTotal;
	m_aSplitTimesBestSegmentSegmented = m_aSplitTimesBestAnySegmentSegmented;
	m_aSplitTimesComparisonRun = m_aSplitTimesBestRun;
	m_aSplitTimesComparisonSegmentTotal = m_aSplitTimesBestSegmentTotal;
	m_aSplitTimesComparisonSegmentSegmented = m_aSplitTimesBestSegmentSegmented;
}

void CSplits::OnMapLoad()
{
	OnReset();
	m_SplitsActive = false;
	if(g_KaizoConfig_KaizoSplits)
	{
		m_MapHasCheckpoints = false;
		for(int i = 0; i < Collision()->GetWidth() * Collision()->GetHeight(); i++)
		{
			int tileG = Collision()->GetTileIndex(i), tileF = Collision()->GetFrontTileIndex(i);
			if(tileG == TILE_FINISH || tileF == TILE_FINISH)
				m_SplitsActive = true;
			else if(tileG >= TILE_TIME_CHECKPOINT_FIRST && tileG <= TILE_TIME_CHECKPOINT_LAST)
				m_MapHasCheckpoints = true;
			else if(tileF >= TILE_TIME_CHECKPOINT_FIRST && tileF <= TILE_TIME_CHECKPOINT_LAST)
				m_MapHasCheckpoints = true;
			if(m_SplitsActive && m_MapHasCheckpoints)
				break;
		}
		if(m_SplitsActive)
			LoadSplits();
	}
}

bool CSplits::RecalculateSplitOrdering(int SplitIdOrdered, int SplitId)
{
	// split #SplitIdOrdered-1 just hit checkpoint SplitId, what route gets to the finish?
	if(SplitIdOrdered >= 0)
		m_aSplitOrdering[SplitIdOrdered] = -1;
	// if this checkpoint links to the finish, this is the end of the route
	if(m_aSplitTimesBestSegmentSegmented[(SplitId + 1) * SPLIT_TOTAL + SPLIT_FINISH] >= 0)
		return true;
	// figure out what checkpoint would be next
	// assume later checkpoints will be closer to the finish
	for(int i = SPLIT_FINISH - 1; i >= 0; i--)
	{
		// skip checkpoints already hit
		bool HasVisited = false;
		for(int j = 0; j < SplitIdOrdered; j++)
		{
			if(m_aSplitOrdering[j] == i)
			{
				HasVisited = true;
				break;
			}
		}
		if(HasVisited)
			continue;
		if(SplitIdOrdered >= 0)
			m_aSplitOrdering[SplitIdOrdered] = i;
		// if the last checkpoint links to this checkpoint, add to the route and try to finish the route
		if(m_aSplitTimesBestSegmentSegmented[(SplitId + 1) * SPLIT_TOTAL + i] >= 0 && RecalculateSplitOrdering(SplitIdOrdered + 1, i))
			return true;
	}
	// this checkpoint is a dead end
	if(SplitIdOrdered >= 0)
		m_aSplitOrdering[SplitIdOrdered] = -1;
	return false;
}
bool CSplits::CommitTime(int Dummy, int RunTime, int SplitId, int SplitIdOrdered, int PrevIdOrdered)
{
	bool DoSaveSplits = false;
	int SegmentTime = PrevIdOrdered < 0 ? RunTime : RunTime - m_aSplitTimesCurrent[Dummy][PrevIdOrdered];
	int PrevId = PrevIdOrdered < 0 ? PrevIdOrdered : m_aSplitOrdering[PrevIdOrdered];
	// if the new time is better than the current time, save the new time
	if(m_aSplitTimesCurrent[Dummy][SplitIdOrdered] < 0 || RunTime < m_aSplitTimesCurrent[Dummy][SplitIdOrdered])
		m_aSplitTimesCurrent[Dummy][SplitIdOrdered] = RunTime;
	// if the new time is better than the best for this segment, save the new time
	if(m_aSplitTimesBestSegmentTotal[SplitId] < 0 || RunTime < m_aSplitTimesBestSegmentTotal[SplitId])
		m_aSplitTimesBestSegmentTotal[SplitId] = RunTime, DoSaveSplits = true;
	// if the segmented time is better than the best for this segment, save the segmented time
	if(m_aSplitTimesBestSegmentSegmented[(PrevId + 1) * SPLIT_SEGMENTEDSTRIDE + SplitId] < 0 || SegmentTime < m_aSplitTimesBestSegmentSegmented[(PrevId + 1) * SPLIT_SEGMENTEDSTRIDE + SplitId])
	{
		// if there was a time to beat, this is a gold split
		if(m_aSplitTimesBestSegmentSegmented[(PrevId + 1) * SPLIT_SEGMENTEDSTRIDE + SplitId] >= 0)
			m_aSplitTimesGold[Dummy][SplitId] = true;
		m_aSplitTimesBestSegmentSegmented[(PrevId + 1) * SPLIT_SEGMENTEDSTRIDE + SplitId] = SegmentTime, DoSaveSplits = true;
	}
	return DoSaveSplits;
}
bool CSplits::CommitTimeCheckpoint(int Dummy, int RunTime, int SplitId)
{
	int PrevIdOrdered = -1;
	while(m_aSplitOrdering[PrevIdOrdered + 1] != SplitId && m_aSplitOrdering[PrevIdOrdered + 1] >= 0 && m_aSplitTimesCurrent[Dummy][PrevIdOrdered + 1] >= 0)
		PrevIdOrdered++;
	int SplitIdOrdered = PrevIdOrdered + 1;
	if(m_aSplitOrdering[PrevIdOrdered + 1] != SplitId)
	{
		bool Recalculate = false, Reset = false;
		for(int i = 0; i < NUM_DUMMIES; i++)
		{
			if(i < Dummy && m_aSplitTimesCurrent[i][SplitIdOrdered] >= 0)
				Reset = true;
			else if(i == Dummy && !Reset)
				Recalculate = true;
			else if(i > Dummy && Recalculate)
				m_SplitCategory[i] = SPLITCATEGORY_NORANK;
		}
		if(Recalculate)
		{
			SplitIdOrdered = PrevIdOrdered + 1;
			m_aSplitOrdering[SplitIdOrdered] = SplitId;
			for(int j = SplitIdOrdered + 1; j < SPLIT_FINISH; j++)
				m_aSplitOrdering[j] = -1;
			RecalculateSplitOrdering(SplitIdOrdered + 1, SplitId);
			if(g_KaizoConfig_KaizoSplitsComparisonSource == 2)
			{
				int prevI = PrevIdOrdered >= 0 ? m_aSplitOrdering[PrevIdOrdered] : -1;
				int t = 0;
				for(int j = SplitIdOrdered; j < SPLIT_TOTAL; j++)
				{
					if(m_aSplitOrdering[j] < 0)
						continue;
					if(m_aSplitTimesComparisonSegmentSegmented[(prevI + 1) * SPLIT_SEGMENTEDSTRIDE + m_aSplitOrdering[j]] < 0)
						break;
					t += m_aSplitTimesComparisonSegmentSegmented[(prevI + 1) * SPLIT_SEGMENTEDSTRIDE + m_aSplitOrdering[j]];
					m_aSplitTimesBestDisplay[j] = t;
					prevI = m_aSplitOrdering[j];
				}
			}
			else
			{
				for(int j = SplitIdOrdered; j < SPLIT_FINISH; j++)
					m_aSplitTimesBestDisplay[j] = m_aSplitOrdering[j] >= 0 ? m_aSplitTimesComparisonSegmentTotal[m_aSplitOrdering[j]] : -1;
			}
		}
		else if(Reset)
			m_SplitCategory[Dummy] = SPLITCATEGORY_NORANK;
	}
	return CommitTime(Dummy, RunTime, SplitId, SplitIdOrdered, PrevIdOrdered);
}
void CSplits::OnUpdate()
{
	if(!SplitsActive() || Client()->State() == IClient::STATE_DEMOPLAYBACK)
		return;

	// figure out who the dummies are and which one is the current dummy
	if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
	{
		for(int i = 0; i < NUM_DUMMIES; i++)
		{
			if(m_DummyId[i] >= 0 && !GameClient()->m_aClients[m_DummyId[i]].m_Active)
				m_DummyId[i] = -1;
		}
		m_CurrentDummy = -1;
		for(int i = 0; i < NUM_DUMMIES; i++)
		{
			if(m_DummyId[i] >= 0 && GameClient()->m_Snap.m_LocalClientId == m_DummyId[i])
			{
				m_CurrentDummy = i;
				break;
			}
		}
		if(m_CurrentDummy < 0)
		{
			for(int i = 0; i < NUM_DUMMIES; i++)
			{
				if(m_DummyId[i] < 0)
				{
					m_DummyId[i] = GameClient()->m_Snap.m_LocalClientId;
					m_CurrentDummy = i;
					break;
				}
			}
		}
		if(m_CurrentDummy < 0)
			return;
	}
	else
	{
		for(int i = 0; i < NUM_DUMMIES; i++)
			m_DummyId[i] = GameClient()->m_aLocalIds[i];
		m_CurrentDummy = g_Config.m_ClDummy;
	}

	// run management
	bool ResetCurrentSplits[NUM_DUMMIES], RecalculateCategory[NUM_DUMMIES];
	for(int i = 0; i < NUM_DUMMIES; i++)
		ResetCurrentSplits[i] = false, RecalculateCategory[i] = false;
	bool DoSaveSplits = false;
	int CurRaceStartTick[NUM_DUMMIES];
	for(int i = 0; i < NUM_DUMMIES; i++)
	{
		// if a run was invalidated, reset that player's splits
		if(m_RaceStartTick[i] >= 0 && m_SplitCategory[i] == SPLITCATEGORY_NORANK)
			m_RaceStartTick[i] = 0, ResetCurrentSplits[i] = true;
		// recalculate a player's category if they haven't started a run yet
		if(m_RaceStartTick[i] < 0)
		{
			RecalculateCategory[i] = true;
			for(int j = 0; j < SPLIT_TOTAL; j++)
			{
				if(m_aSplitTimesCurrent[i][j] >= 0)
				{
					RecalculateCategory[i] = false;
					break;
				}
			}
		}
		// reset current splits if the player died and this death should reset the splits
		if(((m_HasStarted[i] && m_aSplitTimesCurrent[i][SPLIT_FINISH] >= 0) ? g_KaizoConfig_KaizoSplitsResetAfterFinishDeath : g_KaizoConfig_KaizoSplitsResetAfterDeath) && m_HasDied[i])
			ResetCurrentSplits[i] = true;
		if(m_HasDied[i])
			m_HasStarted[i] = false, m_HasQueuedSplit[i] = false;
		m_HasDied[i] = false;
		CurRaceStartTick[i] = m_RaceStartTick[i];
	}
	// handle run timer
	// the client only tracks this for the active player, and it may not be accurate if we are spectating
	if(GameClient()->m_Snap.m_LocalClientId == m_DummyId[m_CurrentDummy] && !GameClient()->m_Snap.m_SpecInfo.m_Active)
	{
		if(GameClient()->m_Snap.m_pGameInfoObj && (GameClient()->m_Snap.m_pGameInfoObj->m_GameStateFlags & GAMESTATEFLAG_RACETIME))
			m_RaceStartTick[m_CurrentDummy] = -GameClient()->m_Snap.m_pGameInfoObj->m_WarmupTimer, m_HasStarted[m_CurrentDummy] = true;
		else
			m_RaceStartTick[m_CurrentDummy] = -1;
		// if we are in a team with our dummy, then they will share our run time
		if(m_Teams[m_DummyId[m_CurrentDummy]] > 0 && !m_WasTeam0Mode[m_CurrentDummy])
		{
			for(int i = 0; i < NUM_DUMMIES; i++)
			{
				if(m_DummyId[i] >= 0 && m_Teams[m_DummyId[i]] == m_Teams[m_DummyId[m_CurrentDummy]])
					m_RaceStartTick[i] = m_RaceStartTick[m_CurrentDummy];
			}
		}
		// handle runs starting
		if(m_RaceStartTick[m_CurrentDummy] >= 0 && m_RaceStartTick[m_CurrentDummy] != m_LastRaceStartTick[m_CurrentDummy])
		{
			// splits should not be reset if a run was active, a run is still active, and it wasn't a start line that changed the race timer
			if(m_RaceStartTick[m_CurrentDummy] >= 0 && m_LastRaceStartTick[m_CurrentDummy] >= 0)
			{
				const CNetObj_Character *pCur = GameClient()->m_Snap.m_pLocalCharacter, *pPrev = GameClient()->m_Snap.m_pLocalPrevCharacter;
				if(!pPrev)
					pPrev = pCur;
				if(pCur)
				{
					vec2 CurPos = vec2(pCur->m_X, pCur->m_Y), PrevPos = vec2(pPrev->m_X, pPrev->m_Y);
					float Distance = distance(PrevPos, CurPos);
					if(Distance == 0.0f)
						Distance = 1.0f;
					bool HitStart = false;
					std::vector<int> vIndices = Collision()->GetMapIndices(PrevPos, CurPos);
					for(const int ind : vIndices)
					{
						int tileG = Collision()->GetTileIndex(ind), tileF = Collision()->GetFrontTileIndex(ind);
						if(tileG == TILE_START || tileF == TILE_START)
							HitStart = true;
						else
							continue;
						break;
					}
					if(HitStart)
						ResetCurrentSplits[m_CurrentDummy] = true;
				}
			}
			else
				ResetCurrentSplits[m_CurrentDummy] = true;
			if(ResetCurrentSplits[m_CurrentDummy] && m_Teams[m_DummyId[m_CurrentDummy]] > 0 && !m_WasTeam0Mode[m_CurrentDummy])
			{
				for(int i = 0; i < NUM_DUMMIES; i++)
				{
					if(m_DummyId[i] >= 0 && m_Teams[m_DummyId[i]] == m_Teams[m_DummyId[m_CurrentDummy]])
						ResetCurrentSplits[i] = true;
				}
			}
		}
	}

	// check for split completions
	for(int i = 0; i < (g_KaizoConfig_KaizoSplitsTrackDummy ? NUM_DUMMIES : 1); i++)
	{
		m_LastRaceStartTick[m_CurrentDummy] = m_RaceStartTick[m_CurrentDummy];
		if((!m_HasQueuedSplit[i] || !m_QueuedSplitFinish[i]) && CurRaceStartTick[i] < 0)
			continue;
		if(m_DummyId[i] < 0)
		{
			if(m_RaceStartTick[i] >= 0)
				m_RaceStartTick[i] = -1, m_LastRaceStartTick[i] = -1, ResetCurrentSplits[i] = true;
			continue;
		}
		if(m_SplitCategory[i] != m_SplitCategory[0] || m_SplitCategory[i] == SPLITCATEGORY_NORANK || !m_HasStarted[i])
			continue;
		if(CurRaceStartTick[i] >= 0)
		{
			if(m_HasQueuedSplit[i] && m_QueuedSplitFinish[i])
			{
				// the finish line was crossed
				int PrevIdOrdered = SPLIT_FINISH - 1;
				while(PrevIdOrdered >= 0 && m_aSplitTimesCurrent[i][PrevIdOrdered] < 0)
					PrevIdOrdered--;
				DoSaveSplits = CommitTime(i, m_QueuedSplitTime[i], SPLIT_FINISH, SPLIT_FINISH, PrevIdOrdered) || DoSaveSplits;
				// if the new time is better than the best run, save the current run
				if(m_aSplitTimesBestRun[SPLIT_FINISH] < 0 || m_QueuedSplitTime[i] < m_aSplitTimesBestRun[SPLIT_FINISH])
				{
					for(int j = 0; j < SPLIT_FINISH; j++)
						m_aSplitTimesBestRun[j] = -1;
					for(int j = 0; j < SPLIT_FINISH; j++)
					{
						if(m_aSplitOrdering[j] >= 0)
							m_aSplitTimesBestRun[m_aSplitOrdering[j]] = m_aSplitTimesCurrent[i][j];
					}
					m_aSplitTimesBestRun[SPLIT_FINISH] = m_QueuedSplitTime[i];
					DoSaveSplits = true;
				}
				/*
				   /timecp existing means this can't actually be trusted to be ours...
				// if the server's check time is better than the best for this segment, save the check time to the any category
				// we can't guarantee that the check time was from a team, so it is only safe to save it to any
				if (m_aSplitTimesBestAnySegmentTotal[SPLIT_FINISH] < 0 || m_QueuedSplitTime[i] - m_QueuedSplitCheck[i] < m_aSplitTimesBestAnySegmentTotal[SPLIT_FINISH])
				  m_aSplitTimesBestAnySegmentTotal[SPLIT_FINISH] = m_QueuedSplitTime[i] - m_QueuedSplitCheck[i], DoSaveSplits = true;
				*/
				ResetCurrentSplits[i] = false, RecalculateCategory[i] = false;
				// probably redundant
				/*if (m_Teams[m_DummyId[i]] > 0 && !m_WasTeam0Mode[i])
				{
				  for (int j = 0; j < NUM_DUMMIES; j++)
				  {
				    if (m_DummyId[j] >= 0 && m_Teams[m_DummyId[j]] == m_Teams[m_DummyId[i]])
				    {
				      int PrevIdOrdered = SPLIT_FINISH-1;
				      while (PrevIdOrdered >= 0 && m_aSplitTimesCurrent[j][PrevIdOrdered] < 0) PrevIdOrdered--;
				      DoSaveSplits = CommitTime(j, m_QueuedSplitTime[i], SPLIT_FINISH, SPLIT_FINISH, PrevIdOrdered) || DoSaveSplits;
				      // no need to check if this is a best run
				      ResetCurrentSplits[j] = false, RecalculateCategory[j] = false;
				    }
				  }
				}*/
				if(m_aSplitTimesCurrent[i][SPLIT_FINISH] >= 0)
				{
					// make sure the split ordering is correct for the final run
					int aTmpCurrent[SPLIT_FINISH], aTmpBest[SPLIT_FINISH];
					for(int j = 0; j < SPLIT_FINISH; j++)
						aTmpCurrent[j] = -1, aTmpBest[j] = -1;
					for(int j = 0; j < SPLIT_FINISH; j++)
					{
						if(m_aSplitOrdering[j] < 0)
							break;
						aTmpCurrent[m_aSplitOrdering[j]] = m_aSplitTimesCurrent[i][j], aTmpBest[m_aSplitOrdering[j]] = m_aSplitTimesBestDisplay[j];
						m_aSplitOrdering[j] = -1;
					}
					int currentOrdered = -1, currentOrderedI = 0;
					for(int j = 0; j < SPLIT_FINISH; j++)
					{
						int best = m_aSplitTimesCurrent[i][SPLIT_FINISH], bestI = -1;
						for(int k = 0; k < SPLIT_FINISH; k++)
						{
							if(currentOrdered < aTmpCurrent[k] && aTmpCurrent[k] < best)
								best = aTmpCurrent[k], bestI = k;
						}
						if(bestI < 0)
							break;
						m_aSplitTimesCurrent[i][currentOrderedI] = best, m_aSplitTimesBestDisplay[currentOrderedI] = aTmpBest[bestI], m_aSplitOrdering[currentOrderedI] = bestI;
						currentOrdered = best, currentOrderedI++;
					}
					for(int j = currentOrderedI; j < SPLIT_FINISH; j++)
						m_aSplitTimesCurrent[i][j] = -1, m_aSplitTimesBestDisplay[j] = -1, m_aSplitOrdering[j] = -1;
				}
			}
			else if(m_MapHasCheckpoints)
			{
				// check if we crossed a time checkpoint
				const CNetObj_Character *pCur = &GameClient()->m_Snap.m_aCharacters[m_DummyId[i]].m_Cur, *pPrev = &GameClient()->m_Snap.m_aCharacters[m_DummyId[i]].m_Prev;
				if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
					pCur = &GameClient()->m_aClients[m_DummyId[i]].m_RenderCur, pPrev = &GameClient()->m_aClients[m_DummyId[i]].m_RenderPrev;
				if(!pPrev)
					pPrev = pCur;
				if(pCur)
				{
					// the server checks if the previous tick's movement crossed a time checkpoint
					// can't just trace from previous to current because a checkpoint may have been placed in a teleport
					// without actually simulating more of the server's process, the simplest way to mimic the server is to assume the previous tick's character moved with the same velocity as the previous tick
					int ticks = pCur->m_Tick - pPrev->m_Tick;
					int Time = CurrentRunTime() - 2 * (ticks - 1); // time is in units of 1/100, ticks are in units of 1/50
					vec2 PrevPos = vec2(pPrev->m_X, pPrev->m_Y);
					for(int j = 0; j < ticks; j++)
					{
						vec2 CurPos = PrevPos + vec2(pPrev->m_VelX / 256.0f, pPrev->m_VelY / 256.0f);
						int SplitId = -1;
						std::vector<int> vIndices = Collision()->GetMapIndices(PrevPos, CurPos);
						for(const int ind : vIndices)
						{
							int tileG = Collision()->GetTileIndex(ind), tileF = Collision()->GetFrontTileIndex(ind);
							if(tileG >= TILE_TIME_CHECKPOINT_FIRST && tileG <= TILE_TIME_CHECKPOINT_LAST)
								SplitId = tileG - TILE_TIME_CHECKPOINT_FIRST;
							else if(tileF >= TILE_TIME_CHECKPOINT_FIRST && tileF <= TILE_TIME_CHECKPOINT_LAST)
								SplitId = tileF - TILE_TIME_CHECKPOINT_FIRST;
							else
								continue;
							break;
						}
						// assume we only ever cross one time checkpoint in a tick
						if(SplitId >= 0 && SplitId < SPLIT_FINISH)
						{
							DoSaveSplits = CommitTimeCheckpoint(i, (m_HasQueuedSplit[i] && !m_QueuedSplitFinish[i]) ? m_QueuedSplitTime[i] : Time, SplitId) || DoSaveSplits;
							break;
						}
						Time += 2;
						PrevPos = CurPos;
					}
				}
			}
		}
	}

	// more run management
	for(int i = 0; i < MAX_CLIENTS; i++)
		m_Teams[i] = GameClient()->m_Teams.Team(i);
	for(int i = 0; i < NUM_DUMMIES; i++)
		m_WasTeam0Mode[i] = (GameClient()->m_Snap.m_aCharacters[m_DummyId[i]].m_HasExtendedDisplayInfo && (GameClient()->m_Snap.m_aCharacters[m_DummyId[i]].m_ExtendedData.m_Flags & CHARACTERFLAG_TEAM0_MODE));
	for(int i = 0; i < NUM_DUMMIES; i++)
	{
		m_HasQueuedSplit[i] = false;
		if(ResetCurrentSplits[i])
		{
			for(int j = 0; j < SPLIT_TOTAL; j++)
				m_aSplitTimesCurrent[i][j] = -1, m_aSplitTimesGold[i][j] = false;
			RecalculateCategory[i] = true;
		}
		if(RecalculateCategory[i])
		{
			if((g_KaizoConfig_KaizoSplitsTrackDummy && i > 0) || m_DummyId[i] < 0 || GameClient()->m_Teams.Team(m_DummyId[i]) < 0 || (GameClient()->m_Snap.m_aCharacters[m_DummyId[i]].m_HasExtendedDisplayInfo && (GameClient()->m_Snap.m_aCharacters[m_DummyId[i]].m_ExtendedData.m_Flags & CHARACTERFLAG_PRACTICE_MODE)))
				m_SplitCategory[i] = SPLITCATEGORY_NORANK;
			else if(g_KaizoConfig_KaizoSplitsForceAny || !GameClient()->m_GameInfo.m_DDRaceTeam || GameClient()->m_Teams.Team(m_DummyId[i]) == 0 || (GameClient()->m_Snap.m_aCharacters[m_DummyId[i]].m_HasExtendedDisplayInfo && (GameClient()->m_Snap.m_aCharacters[m_DummyId[i]].m_ExtendedData.m_Flags & CHARACTERFLAG_TEAM0_MODE)))
				m_SplitCategory[i] = SPLITCATEGORY_ANYRANK;
			else
			{
				bool hasNonDummy = false;
				int teamMemberCount = 0;
				for(int j = 0; j < MAX_CLIENTS; j++)
				{
					if(!GameClient()->m_Snap.m_apPlayerInfos[j])
						continue;
					if(GameClient()->m_Teams.Team(GameClient()->m_Snap.m_apPlayerInfos[j]->m_ClientId) == GameClient()->m_Teams.Team(m_DummyId[i]))
					{
						bool isDummy = false;
						for(int k = 0; k < NUM_DUMMIES; k++)
						{
							if(GameClient()->m_Snap.m_apPlayerInfos[j]->m_ClientId == m_DummyId[k])
							{
								isDummy = true;
								break;
							}
						}
						if(!isDummy)
						{
							hasNonDummy = true;
							break;
						}
						teamMemberCount++;
					}
				}
				if(hasNonDummy)
					m_SplitCategory[i] = SPLITCATEGORY_TEAMRANK;
				else if(teamMemberCount > 1)
					m_SplitCategory[i] = SPLITCATEGORY_DUMMYRANK;
				else
					m_SplitCategory[i] = SPLITCATEGORY_SOLORANK;
			}
			// solo servers get forced into the solo category
			if(m_SplitCategory[i] == SPLITCATEGORY_ANYRANK && ((Config()->m_SvTeam == SV_TEAM_ALLOWED && g_Config.m_SvSoloServer) || Config()->m_SvTeam == SV_TEAM_FORCED_SOLO))
				m_SplitCategory[i] = SPLITCATEGORY_SOLORANK;
			if(i == 0)
			{
				switch(m_SplitCategory[i])
				{
				case SPLITCATEGORY_NORANK:
					m_aSplitTimesBestRun = NULL;
					m_aSplitTimesBestSegmentTotal = NULL;
					m_aSplitTimesBestSegmentSegmented = NULL;
					break;
				case SPLITCATEGORY_ANYRANK:
					m_aSplitTimesBestRun = m_aSplitTimesBestAnyRun;
					m_aSplitTimesBestSegmentTotal = m_aSplitTimesBestAnySegmentTotal;
					m_aSplitTimesBestSegmentSegmented = m_aSplitTimesBestAnySegmentSegmented;
					break;
				case SPLITCATEGORY_TEAMRANK:
					m_aSplitTimesBestRun = m_aSplitTimesBestTeamRun;
					m_aSplitTimesBestSegmentTotal = m_aSplitTimesBestTeamSegmentTotal;
					m_aSplitTimesBestSegmentSegmented = m_aSplitTimesBestTeamSegmentSegmented;
					break;
				case SPLITCATEGORY_DUMMYRANK:
					m_aSplitTimesBestRun = m_aSplitTimesBestDummyRun;
					m_aSplitTimesBestSegmentTotal = m_aSplitTimesBestDummySegmentTotal;
					m_aSplitTimesBestSegmentSegmented = m_aSplitTimesBestDummySegmentSegmented;
					break;
				case SPLITCATEGORY_SOLORANK:
					m_aSplitTimesBestRun = m_aSplitTimesBestSoloRun;
					m_aSplitTimesBestSegmentTotal = m_aSplitTimesBestSoloSegmentTotal;
					m_aSplitTimesBestSegmentSegmented = m_aSplitTimesBestSoloSegmentSegmented;
					break;
				}
				switch(g_KaizoConfig_KaizoSplitsComparisonForceCategory)
				{
				case 0:
					m_aSplitTimesComparisonRun = m_aSplitTimesBestRun;
					m_aSplitTimesComparisonSegmentTotal = m_aSplitTimesBestSegmentTotal;
					m_aSplitTimesComparisonSegmentSegmented = m_aSplitTimesBestSegmentSegmented;
					break;
				case 1:
					m_aSplitTimesComparisonRun = m_aSplitTimesBestAnyRun;
					m_aSplitTimesComparisonSegmentTotal = m_aSplitTimesBestAnySegmentTotal;
					m_aSplitTimesComparisonSegmentSegmented = m_aSplitTimesBestAnySegmentSegmented;
					break;
				case 2:
					m_aSplitTimesComparisonRun = m_aSplitTimesBestTeamRun;
					m_aSplitTimesComparisonSegmentTotal = m_aSplitTimesBestTeamSegmentTotal;
					m_aSplitTimesComparisonSegmentSegmented = m_aSplitTimesBestTeamSegmentSegmented;
					break;
				case 3:
					m_aSplitTimesComparisonRun = m_aSplitTimesBestDummyRun;
					m_aSplitTimesComparisonSegmentTotal = m_aSplitTimesBestDummySegmentTotal;
					m_aSplitTimesComparisonSegmentSegmented = m_aSplitTimesBestDummySegmentSegmented;
					break;
				case 4:
					m_aSplitTimesComparisonRun = m_aSplitTimesBestSoloRun;
					m_aSplitTimesComparisonSegmentTotal = m_aSplitTimesBestSoloSegmentTotal;
					m_aSplitTimesComparisonSegmentSegmented = m_aSplitTimesBestSoloSegmentSegmented;
					break;
				}
				if(m_SplitCategory[i] == SPLITCATEGORY_NORANK)
				{
					for(int j = 0; j < SPLIT_TOTAL; j++)
					{
						m_aSplitTimesCurrent[i][j] = -1, m_aSplitTimesGold[i][j] = false;
						m_aSplitOrdering[j] = -1, m_aSplitTimesBestDisplay[j] = -1;
					}
					m_aSplitTimesCurrent[i][SPLIT_FINISH] = -1, m_aSplitTimesGold[i][SPLIT_FINISH] = false;
					m_aSplitOrdering[SPLIT_FINISH] = SPLIT_FINISH;
				}
				else
				{
					m_aSplitOrdering[SPLIT_FINISH] = SPLIT_FINISH;
					int *source, *sourceSegmented = m_aSplitTimesComparisonSegmentSegmented;
					int sourceBest;
					if(g_KaizoConfig_KaizoSplitsComparisonSource == 0 && m_aSplitTimesComparisonRun[SPLIT_FINISH] >= 0)
						source = m_aSplitTimesComparisonRun, sourceBest = m_aSplitTimesBestDisplay[SPLIT_FINISH] = m_aSplitTimesComparisonRun[SPLIT_FINISH];
					else
					{
						source = m_aSplitTimesComparisonSegmentTotal, sourceBest = m_aSplitTimesBestDisplay[SPLIT_FINISH] = m_aSplitTimesComparisonSegmentTotal[SPLIT_FINISH];
						for(int j = 0; j < SPLIT_TOTAL; j++)
						{
							if(source[j] > sourceBest)
								sourceBest = source[j];
						}
						if(sourceBest != source[SPLIT_FINISH])
							sourceBest++;
					}
					int ordered = -1, orderedI = 0;
					int prevI = -1;
					for(int j = 0; j < SPLIT_FINISH; j++)
					{
						int best = sourceBest, bestI = -1;
						for(int k = 0; k < SPLIT_FINISH; k++)
						{
							if(ordered < source[k] && source[k] < best && sourceSegmented[(prevI + 1) * SPLIT_SEGMENTEDSTRIDE + k] >= 0)
								best = source[k], bestI = k;
						}
						if(bestI < 0)
							break;
						m_aSplitTimesBestDisplay[orderedI] = source[bestI], m_aSplitOrdering[orderedI] = bestI;
						ordered = best, orderedI++;
						prevI = bestI;
					}
					for(int j = orderedI; j < SPLIT_FINISH; j++)
						m_aSplitTimesBestDisplay[j] = -1, m_aSplitOrdering[j] = -1;
					if(g_KaizoConfig_KaizoSplitsComparisonSource == 2)
					{
						prevI = -1;
						int t = 0;
						for(int j = 0; j < SPLIT_TOTAL; j++)
						{
							if(m_aSplitOrdering[j] < 0)
								continue;
							if(sourceSegmented[(prevI + 1) * SPLIT_SEGMENTEDSTRIDE + m_aSplitOrdering[j]] < 0)
								break;
							t += sourceSegmented[(prevI + 1) * SPLIT_SEGMENTEDSTRIDE + m_aSplitOrdering[j]];
							m_aSplitTimesBestDisplay[j] = t;
							prevI = m_aSplitOrdering[j];
						}
					}
				}
			}
		}
	}
	if(DoSaveSplits)
		SaveSplits();
}

void CSplits::OnMessage(int MsgType, void *pRawMsg, int Dummy)
{
	if(!SplitsActive() || Client()->State() == IClient::STATE_DEMOPLAYBACK)
		return;

	int CurrentDummy = Client()->State() == IClient::STATE_DEMOPLAYBACK ? m_CurrentDummy : g_Config.m_ClDummy;
	if(Dummy)
		CurrentDummy = !CurrentDummy;

	if((MsgType == NETMSGTYPE_SV_DDRACETIME || MsgType == NETMSGTYPE_SV_DDRACETIMELEGACY) && m_RaceStartTick[CurrentDummy] >= 0)
	{
		CNetMsg_Sv_DDRaceTime *pMsg = (CNetMsg_Sv_DDRaceTime *)pRawMsg;
		m_HasQueuedSplit[CurrentDummy] = true;
		m_QueuedSplitTick[CurrentDummy] = Client()->GameTick(Client()->State() == IClient::STATE_DEMOPLAYBACK ? 0 : CurrentDummy);
		m_QueuedSplitTime[CurrentDummy] = pMsg->m_Time, m_QueuedSplitCheck[CurrentDummy] = pMsg->m_Check, m_QueuedSplitFinish[CurrentDummy] = pMsg->m_Finish;
	}
	else if((MsgType == NETMSGTYPE_SV_RECORD || MsgType == NETMSGTYPE_SV_RECORDLEGACY) && true)
	{
		CNetMsg_Sv_Record *pMsg = (CNetMsg_Sv_Record *)pRawMsg;
		if(MsgType == NETMSGTYPE_SV_RECORDLEGACY && GameClient()->m_GameInfo.m_DDRaceRecordMessage)
		{
			// apparently a different message (NETMSGTYPE_SV_RACETIME)?
			m_HasQueuedSplit[CurrentDummy] = true;
			m_QueuedSplitTick[CurrentDummy] = Client()->GameTick(Client()->State() == IClient::STATE_DEMOPLAYBACK ? 0 : CurrentDummy);
			m_QueuedSplitTime[CurrentDummy] = pMsg->m_ServerTimeBest, m_QueuedSplitCheck[CurrentDummy] = pMsg->m_PlayerTimeBest, m_QueuedSplitFinish[CurrentDummy] = false;
		}
		else if(MsgType == NETMSGTYPE_SV_RECORD || GameClient()->m_GameInfo.m_RaceRecordMessage)
		{
			m_HasQueuedSplit[CurrentDummy] = true;
			m_QueuedSplitTick[CurrentDummy] = Client()->GameTick(Client()->State() == IClient::STATE_DEMOPLAYBACK ? 0 : CurrentDummy);
			m_QueuedSplitTime[CurrentDummy] = pMsg->m_PlayerTimeBest, m_QueuedSplitCheck[CurrentDummy] = 0, m_QueuedSplitFinish[CurrentDummy] = true;
		}
	}
	else if(MsgType == NETMSGTYPE_SV_KILLMSG)
	{
		CNetMsg_Sv_KillMsg *pMsg = (CNetMsg_Sv_KillMsg *)pRawMsg;
		for(int i = 0; i < NUM_DUMMIES; i++)
		{
			if(m_DummyId[i] == pMsg->m_Victim)
			{
				m_HasDied[i] = true;
				break;
			}
		}
	}
	else if(MsgType == NETMSGTYPE_SV_KILLMSGTEAM)
	{
		CNetMsg_Sv_KillMsgTeam *pMsg = (CNetMsg_Sv_KillMsgTeam *)pRawMsg;
		for(int i = 0; i < NUM_DUMMIES; i++)
		{
			if(GameClient()->m_Teams.Team(m_DummyId[i]) == pMsg->m_Team)
				m_HasDied[i] = true;
		}
	}
}

void CSplits::OnConsoleInit()
{
}

void CSplits::LoadSplits()
{
	for(int i = 0; i < SPLIT_TOTAL; i++)
	{
		m_aSplitTimesBestAnyRun[i] = -1;
		m_aSplitTimesBestAnySegmentTotal[i] = -1;
		m_aSplitTimesBestTeamRun[i] = -1;
		m_aSplitTimesBestTeamSegmentTotal[i] = -1;
		m_aSplitTimesBestDummyRun[i] = -1;
		m_aSplitTimesBestDummySegmentTotal[i] = -1;
		m_aSplitTimesBestSoloRun[i] = -1;
		m_aSplitTimesBestSoloSegmentTotal[i] = -1;
		m_aSplitOrdering[i] = -1;
	}
	for(int i = 0; i < SPLIT_SEGMENTED; i++)
	{
		m_aSplitTimesBestAnySegmentSegmented[i] = -1;
		m_aSplitTimesBestTeamSegmentSegmented[i] = -1;
		m_aSplitTimesBestDummySegmentSegmented[i] = -1;
		m_aSplitTimesBestSoloSegmentSegmented[i] = -1;
	}
	m_aSplitOrdering[SPLIT_FINISH] = SPLIT_FINISH;
	if(!Storage()->FolderExists(ms_pSplitsDir, IStorage::TYPE_SAVE))
		return;
	char aBuf[256], aBufName[IO_MAX_PATH_LENGTH], aBufError[64 + 256 + IO_MAX_PATH_LENGTH];
	str_format(aBufName, sizeof(aBufName), "%s/%s.json", ms_pSplitsDir, GameClient()->Map()->BaseName());
	if(!Storage()->FileExists(aBufName, IStorage::TYPE_SAVE))
	{
		str_format(aBufName, sizeof(aBufName), "%s/%s.splits", ms_pSplitsDir, GameClient()->Map()->BaseName());
		if(Storage()->FileExists(aBufName, IStorage::TYPE_SAVE))
		{
			str_format(aBufError, sizeof(aBufError), "Attempting to convert old splits '%s' to new format", aBufName);
			Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
			LoadSplitsOld();
			SaveSplits();
		}
		return;
	}
	void *pFileData;
	unsigned FileLength;
	if(!Storage()->ReadFile(aBufName, IStorage::TYPE_SAVE, &pFileData, &FileLength))
	{
		if(Console())
		{
			str_format(aBufError, sizeof(aBufError), "Failed to read splits from '%s'", aBufName);
			Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
		}
		return;
	}
	json_settings JsonSettings{};
	char aError[256];
	json_value *pSplits = json_parse_ex(&JsonSettings, static_cast<const json_char *>(pFileData), FileLength, aError);
	if(!pSplits)
	{
		if(Console())
		{
			str_format(aBufError, sizeof(aBufError), "Failed to parse splits from '%s': %s", aBufName, aError);
			Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
		}
		return;
	}
	const json_value &Map = (*pSplits)["map"];
	if(Map.type != json_object)
		return;
	const json_value &MapName = Map["name"];
	if(MapName.type != json_string || !!strcmp(GameClient()->Map()->BaseName(), MapName.u.string.ptr))
		return;
	for(int i = 0; i < 4; i++)
	{
		int *TimesRun, *TimesSegmentTotal, *TimesSegmentSegmented;
		const char *TimesCategory;
		if(i == 0)
			TimesRun = m_aSplitTimesBestAnyRun, TimesSegmentTotal = m_aSplitTimesBestAnySegmentTotal, TimesSegmentSegmented = m_aSplitTimesBestAnySegmentSegmented, TimesCategory = "any";
		if(i == 1)
			TimesRun = m_aSplitTimesBestTeamRun, TimesSegmentTotal = m_aSplitTimesBestTeamSegmentTotal, TimesSegmentSegmented = m_aSplitTimesBestTeamSegmentSegmented, TimesCategory = "team";
		if(i == 2)
			TimesRun = m_aSplitTimesBestDummyRun, TimesSegmentTotal = m_aSplitTimesBestDummySegmentTotal, TimesSegmentSegmented = m_aSplitTimesBestDummySegmentSegmented, TimesCategory = "dummy";
		if(i == 3)
			TimesRun = m_aSplitTimesBestSoloRun, TimesSegmentTotal = m_aSplitTimesBestSoloSegmentTotal, TimesSegmentSegmented = m_aSplitTimesBestSoloSegmentSegmented, TimesCategory = "solo";
		const json_value &Category = (*pSplits)[static_cast<const json_char *>(TimesCategory)];
		if(Category.type != json_object)
			continue;
		const json_value &Run = Category["run"];
		if(Run.type == json_object)
		{
			for(int j = 0; j < SPLIT_TOTAL; j++)
			{
				str_format(aBuf, sizeof(aBuf), "%i", j);
				const json_value &Time = Run[static_cast<const json_char *>(aBuf)];
				if(Time.type == json_integer && Time.u.integer >= 0 && Time.u.integer <= INT_MAX)
					TimesRun[j] = Time.u.integer;
			}
		}
		const json_value &SegmentTotal = Category["segmenttotal"];
		if(SegmentTotal.type == json_object)
		{
			for(int j = 0; j < SPLIT_TOTAL; j++)
			{
				str_format(aBuf, sizeof(aBuf), "%i", j);
				const json_value &Time = SegmentTotal[static_cast<const json_char *>(aBuf)];
				if(Time.type == json_integer && Time.u.integer >= 0 && Time.u.integer <= INT_MAX)
					TimesSegmentTotal[j] = Time.u.integer;
			}
		}
		const json_value &SegmentSegmented = Category["segmentsegmented"];
		if(SegmentSegmented.type == json_object)
		{
			for(int j = 0; j < SPLIT_TOTAL; j++)
			{
				str_format(aBuf, sizeof(aBuf), "%i", j);
				const json_value &Segment = SegmentSegmented[static_cast<const json_char *>(aBuf)];
				if(Segment.type == json_object)
				{
					for(int k = 0; k < SPLIT_TOTAL; k++)
					{
						str_format(aBuf, sizeof(aBuf), "%i", k);
						const json_value &Time = Segment[static_cast<const json_char *>(aBuf)];
						if(Time.type == json_integer && Time.u.integer >= 0 && Time.u.integer <= INT_MAX)
							TimesSegmentSegmented[j * SPLIT_TOTAL + k] = Time.u.integer;
					}
				}
			}
		}
	}
	/*if(mapMetaMismatch)
	{
		if(Console())
			Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
		for(int i = 0; i < SPLIT_TOTAL; i++)
		{
			m_aSplitTimesBestAnyRun[i] = -1;
			m_aSplitTimesBestAnySegmentTotal[i] = -1;
			m_aSplitTimesBestTeamRun[i] = -1;
			m_aSplitTimesBestTeamSegmentTotal[i] = -1;
			m_aSplitTimesBestDummyRun[i] = -1;
			m_aSplitTimesBestDummySegmentTotal[i] = -1;
			m_aSplitTimesBestSoloRun[i] = -1;
			m_aSplitTimesBestSoloSegmentTotal[i] = -1;
			m_aSplitOrdering[i] = -1;
		}
		for(int i = 0; i < SPLIT_SEGMENTED; i++)
		{
			m_aSplitTimesBestAnySegmentSegmented[i] = -1;
			m_aSplitTimesBestTeamSegmentSegmented[i] = -1;
			m_aSplitTimesBestDummySegmentSegmented[i] = -1;
			m_aSplitTimesBestSoloSegmentSegmented[i] = -1;
		}
		return;
	}*/
	if(Console())
	{
		str_format(aBufError, sizeof(aBufError), "Read splits from '%s'", aBufName);
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
	}
	RecalculateCategories();
}

void CSplits::SaveSplits()
{
	RecalculateCategories();
	if(!Storage()->FolderExists(ms_pSplitsDir, IStorage::TYPE_SAVE))
		Storage()->CreateFolder(ms_pSplitsDir, IStorage::TYPE_SAVE);
	char aBuf[256], aBufName[IO_MAX_PATH_LENGTH], aBufNameDst[IO_MAX_PATH_LENGTH], aBufError[64 + IO_MAX_PATH_LENGTH];
	str_format(aBufName, sizeof(aBufName), "%s/%s_tmp_%d.json", ms_pSplitsDir, GameClient()->Map()->BaseName(), process_id());
	IOHANDLE SplitsFile = Storage()->OpenFile(aBufName, IOFLAG_WRITE, IStorage::TYPE_SAVE);
	if(!SplitsFile)
	{
		if(Console())
		{
			str_format(aBufError, sizeof(aBufError), "Unable to open '%s' for saving splits", aBufName);
			Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
		}
		return;
	}
	{
		CJsonFileWriter Writer(SplitsFile);
		Writer.BeginObject();
		Writer.WriteAttribute("map");
		Writer.BeginObject();
		Writer.WriteAttribute("name");
		Writer.WriteStrValue(GameClient()->Map()->BaseName());
		Writer.WriteAttribute("sha256");
		sha256_str(GameClient()->Map()->Sha256(), aBuf, sizeof(aBuf));
		Writer.WriteStrValue(aBuf);
		Writer.EndObject();
		for(int i = 0; i < 4; i++)
		{
			int *TimesRun, *TimesSegmentTotal, *TimesSegmentSegmented;
			const char *TimesCategory;
			if(i == 0)
				TimesRun = m_aSplitTimesBestAnyRun, TimesSegmentTotal = m_aSplitTimesBestAnySegmentTotal, TimesSegmentSegmented = m_aSplitTimesBestAnySegmentSegmented, TimesCategory = "any";
			if(i == 1)
				TimesRun = m_aSplitTimesBestTeamRun, TimesSegmentTotal = m_aSplitTimesBestTeamSegmentTotal, TimesSegmentSegmented = m_aSplitTimesBestTeamSegmentSegmented, TimesCategory = "team";
			if(i == 2)
				TimesRun = m_aSplitTimesBestDummyRun, TimesSegmentTotal = m_aSplitTimesBestDummySegmentTotal, TimesSegmentSegmented = m_aSplitTimesBestDummySegmentSegmented, TimesCategory = "dummy";
			if(i == 3)
				TimesRun = m_aSplitTimesBestSoloRun, TimesSegmentTotal = m_aSplitTimesBestSoloSegmentTotal, TimesSegmentSegmented = m_aSplitTimesBestSoloSegmentSegmented, TimesCategory = "solo";
			int SplitCountBestRun = 0, SplitCountBestSegmentTotal = 0, SplitCountBestSegmentSegmented = 0;
			for(int j = 0; j < SPLIT_TOTAL; j++)
			{
				if(TimesRun[j] >= 0)
					SplitCountBestRun++;
				if(TimesSegmentTotal[j] >= 0)
					SplitCountBestSegmentTotal++;
			}
			for(int j = 0; j < SPLIT_SEGMENTED; j++)
			{
				if(TimesSegmentSegmented[j] >= 0)
					SplitCountBestSegmentSegmented++;
			}
			if(SplitCountBestRun <= 0 && SplitCountBestSegmentTotal <= 0 && SplitCountBestSegmentSegmented <= 0)
				continue;
			Writer.WriteAttribute(TimesCategory);
			Writer.BeginObject();
			Writer.WriteAttribute("run");
			Writer.BeginObject();
			for(int j = 0; j < SPLIT_TOTAL; j++)
			{
				if(TimesRun[j] < 0)
					continue;
				str_format(aBuf, sizeof(aBuf), "%i", j);
				Writer.WriteAttribute(aBuf);
				Writer.WriteIntValue(TimesRun[j]);
			}
			Writer.EndObject();
			Writer.WriteAttribute("segmenttotal");
			Writer.BeginObject();
			for(int j = 0; j < SPLIT_TOTAL; j++)
			{
				if(TimesSegmentTotal[j] < 0)
					continue;
				str_format(aBuf, sizeof(aBuf), "%i", j);
				Writer.WriteAttribute(aBuf);
				Writer.WriteIntValue(TimesSegmentTotal[j]);
			}
			Writer.EndObject();
			Writer.WriteAttribute("segmentsegmented");
			Writer.BeginObject();
			int CurrentStart = -1;
			for(int j = 0; j < SPLIT_SEGMENTED; j++)
			{
				if(TimesSegmentSegmented[j] < 0)
					continue;
				int j0 = j / SPLIT_SEGMENTEDSTRIDE, j1 = j % SPLIT_SEGMENTEDSTRIDE;
				if(j0 != CurrentStart)
				{
					if(CurrentStart != -1)
						Writer.EndObject();
					str_format(aBuf, sizeof(aBuf), "%i", j0);
					Writer.WriteAttribute(aBuf);
					Writer.BeginObject();
					CurrentStart = j0;
				}
				str_format(aBuf, sizeof(aBuf), "%i", j1);
				Writer.WriteAttribute(aBuf);
				Writer.WriteIntValue(TimesSegmentSegmented[j]);
			}
			if(CurrentStart != -1)
				Writer.EndObject();
			Writer.EndObject();
			Writer.EndObject();
		}
		Writer.EndObject();
	}
	str_format(aBufNameDst, sizeof(aBufNameDst), "%s/%s.json", ms_pSplitsDir, GameClient()->Map()->BaseName());
	if(!Storage()->RenameFile(aBufName, aBufNameDst, IStorage::TYPE_SAVE))
	{
		if(Console())
		{
			str_format(aBufError, sizeof(aBufError), "Could not move splits file '%s' to '%s'.", aBufName, aBufNameDst);
			Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
		}
		return;
	}
	if(Console())
	{
		str_format(aBufError, sizeof(aBufError), "Saved splits to '%s'", aBufNameDst);
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
	}
}

void CSplits::LoadSplitsOld()
{
	char aBufLine[256], aBufName[IO_MAX_PATH_LENGTH], aBufError[64 + IO_MAX_PATH_LENGTH];
	str_format(aBufName, sizeof(aBufName), "%s/%s.splits", ms_pSplitsDir, GameClient()->Map()->BaseName());
	if(!Storage()->FileExists(aBufName, IStorage::TYPE_SAVE))
		return;
	IOHANDLE SplitsFile = Storage()->OpenFile(aBufName, IOFLAG_READ, IStorage::TYPE_SAVE);
	if(!SplitsFile)
	{
		if(Console())
		{
			str_format(aBufError, sizeof(aBufError), "Unable to open '%s' for reading splits", aBufName);
			Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
		}
		return;
	}
	enum
	{
		SPLITSSTATE_INIT,
		SPLITSSTATE_MAPMETADATA,
		SPLITSSTATE_BESTRUN,
		SPLITSSTATE_BESTSEGMENTTOTAL,
		SPLITSSTATE_BESTSEGMENTSEGMENTED,
		SPLITSSTATE_CATEGORY,
		SPLITSSTATE_SKIPLINE = -1
	} state = SPLITSSTATE_INIT,
	  lastState;
	int stateLen;
	int lineBufSize = 0;
	lineBufSize = io_read(SplitsFile, aBufLine, sizeof(aBufLine));
	bool mapMetaMismatch = false;
	int *timesRun = m_aSplitTimesBestAnyRun;
	int *timesSegmentTotal = m_aSplitTimesBestAnySegmentTotal;
	int *timesSegmentSegmented = m_aSplitTimesBestAnySegmentSegmented;
	while(!mapMetaMismatch && lineBufSize > 0)
	{
		int lineEnd = 0;
		while(lineEnd < lineBufSize && aBufLine[lineEnd] != '\0' && aBufLine[lineEnd] != '\r' && aBufLine[lineEnd] != '\n')
			lineEnd++;
		if(lineEnd < lineBufSize && aBufLine[lineEnd] == '\0')
			break;
		if(lineEnd >= sizeof(aBufLine))
			lastState = state, state = SPLITSSTATE_SKIPLINE;
		switch(state)
		{
		case SPLITSSTATE_INIT:
			if(aBufLine[0] == '#')
				break;
			if(aBufLine[0] >= '0' && aBufLine[0] <= '9')
			{
				int v = 0, p = 0;
				while(aBufLine[p] >= '0' && aBufLine[p] <= '9')
				{
					v = v * 10 + (aBufLine[p] - '0');
					p++;
				}
				if(aBufLine[p] == ':')
				{
					p++;
					if(lineEnd - p == 12 && !strncmp(&aBufLine[p], "map metadata", lineEnd - p))
						state = SPLITSSTATE_MAPMETADATA;
					if(lineEnd - p == 3 && !strncmp(&aBufLine[p], "run", lineEnd - p))
						state = SPLITSSTATE_BESTRUN;
					if(lineEnd - p == 13 && !strncmp(&aBufLine[p], "segment total", lineEnd - p))
						state = SPLITSSTATE_BESTSEGMENTTOTAL;
					if(lineEnd - p == 17 && !strncmp(&aBufLine[p], "segment segmented", lineEnd - p))
						state = SPLITSSTATE_BESTSEGMENTSEGMENTED;
					if(lineEnd - p == 8 && !strncmp(&aBufLine[p], "category", lineEnd - p))
						state = SPLITSSTATE_CATEGORY;
					if(state != SPLITSSTATE_INIT)
						stateLen = v + 1;
				}
				break;
			}
			break;
		case SPLITSSTATE_MAPMETADATA:
			if(stateLen == 1)
			{
				if(lineEnd != strlen(GameClient()->Map()->BaseName()) || !!strncmp(aBufLine, GameClient()->Map()->BaseName(), lineEnd))
				{
					str_format(aBufError, sizeof(aBufError), "Failed to read splits from '%s': map name does not match", aBufName);
					mapMetaMismatch = true;
					break;
				}
			}
			if(stateLen == 2)
			{
				SHA256_DIGEST digest;
				char aBufDigest[SHA256_MAXSTRSIZE];
				strncpy(aBufDigest, aBufLine, SHA256_MAXSTRSIZE - 1);
				if(!sha256_comp(digest, GameClient()->Map()->Sha256()))
				{
					str_format(aBufError, sizeof(aBufError), "Failed to read splits from '%s': map sha256 does not match", aBufName);
					mapMetaMismatch = true;
					break;
				}
			}
			break;
		case SPLITSSTATE_BESTRUN:
			if(!timesRun)
				break;
			if(aBufLine[0] >= '0' && aBufLine[0] <= '9')
			{
				int v0 = 0, v1 = 0, p = 0;
				while(aBufLine[p] >= '0' && aBufLine[p] <= '9')
				{
					v0 = v0 * 10 + (aBufLine[p] - '0');
					p++;
				}
				while(aBufLine[p] == ' ' || aBufLine[p] == '\t')
					p++;
				if(aBufLine[p] < '0' || aBufLine[p] > '9')
					v1 = -1;
				while(aBufLine[p] >= '0' && aBufLine[p] <= '9')
				{
					v1 = v1 * 10 + (aBufLine[p] - '0');
					p++;
				}
				if(v0 >= 0 && v0 < SPLIT_TOTAL)
					timesRun[v0] = v1;
				break;
			}
			break;
		case SPLITSSTATE_BESTSEGMENTTOTAL:
			if(!timesSegmentTotal)
				break;
			if(aBufLine[0] >= '0' && aBufLine[0] <= '9')
			{
				int v0 = 0, v1 = 0, p = 0;
				while(aBufLine[p] >= '0' && aBufLine[p] <= '9')
				{
					v0 = v0 * 10 + (aBufLine[p] - '0');
					p++;
				}
				while(aBufLine[p] == ' ' || aBufLine[p] == '\t')
					p++;
				if(aBufLine[p] < '0' || aBufLine[p] > '9')
					v1 = -1;
				while(aBufLine[p] >= '0' && aBufLine[p] <= '9')
				{
					v1 = v1 * 10 + (aBufLine[p] - '0');
					p++;
				}
				if(v0 >= 0 && v0 < SPLIT_TOTAL)
					timesSegmentTotal[v0] = v1;
				break;
			}
			break;
		case SPLITSSTATE_BESTSEGMENTSEGMENTED:
			if(!timesSegmentSegmented)
				break;
			if(aBufLine[0] >= '0' && aBufLine[0] <= '9')
			{
				int v0 = 0, v1 = 0, v2 = 0, p = 0;
				while(aBufLine[p] >= '0' && aBufLine[p] <= '9')
				{
					v0 = v0 * 10 + (aBufLine[p] - '0');
					p++;
				}
				while(aBufLine[p] == ' ' || aBufLine[p] == '\t')
					p++;
				if(aBufLine[p] < '0' || aBufLine[p] > '9')
					v1 = -1;
				while(aBufLine[p] >= '0' && aBufLine[p] <= '9')
				{
					v1 = v1 * 10 + (aBufLine[p] - '0');
					p++;
				}
				while(aBufLine[p] == ' ' || aBufLine[p] == '\t')
					p++;
				if(aBufLine[p] < '0' || aBufLine[p] > '9')
					v2 = -1;
				while(aBufLine[p] >= '0' && aBufLine[p] <= '9')
				{
					v2 = v2 * 10 + (aBufLine[p] - '0');
					p++;
				}
				if(v0 >= 0 && v0 < SPLIT_TOTAL && v1 >= 0 && v1 < SPLIT_TOTAL)
				{
					timesSegmentSegmented[v0 * SPLIT_SEGMENTEDSTRIDE + v1] = v2;
				}
				break;
			}
			break;
		case SPLITSSTATE_CATEGORY:
			if(stateLen == 1)
			{
				timesRun = NULL, timesSegmentTotal = NULL, timesSegmentSegmented = NULL;
				if(lineEnd == strlen("any") && !strncmp(aBufLine, "any", lineEnd))
					timesRun = m_aSplitTimesBestAnyRun, timesSegmentTotal = m_aSplitTimesBestAnySegmentTotal, timesSegmentSegmented = m_aSplitTimesBestAnySegmentSegmented;
				if(lineEnd == strlen("team") && !strncmp(aBufLine, "team", lineEnd))
					timesRun = m_aSplitTimesBestTeamRun, timesSegmentTotal = m_aSplitTimesBestTeamSegmentTotal, timesSegmentSegmented = m_aSplitTimesBestTeamSegmentSegmented;
				if(lineEnd == strlen("dummy") && !strncmp(aBufLine, "dummy", lineEnd))
					timesRun = m_aSplitTimesBestDummyRun, timesSegmentTotal = m_aSplitTimesBestDummySegmentTotal, timesSegmentSegmented = m_aSplitTimesBestDummySegmentSegmented;
				if(lineEnd == strlen("solo") && !strncmp(aBufLine, "solo", lineEnd))
					timesRun = m_aSplitTimesBestSoloRun, timesSegmentTotal = m_aSplitTimesBestSoloSegmentTotal, timesSegmentSegmented = m_aSplitTimesBestSoloSegmentSegmented;
			}
			break;
		default:
			break;
		}
		bool isCr = aBufLine[lineEnd] == '\r' && lineEnd + 1 >= lineBufSize;
		if(aBufLine[lineEnd] == '\r' && lineEnd + 1 < lineBufSize && aBufLine[lineEnd + 1] == '\n')
			lineEnd++;
		lineEnd++;
		for(int i = lineEnd; i < lineBufSize; i++)
			aBufLine[i - lineEnd] = aBufLine[i];
		lineBufSize -= lineEnd;
		if(isCr)
		{
			// lineBufSize will be 0
			lineBufSize = io_read(SplitsFile, aBufLine, 1);
			if(lineBufSize == 0)
				break;
			if(aBufLine[0] == '\n')
				lineBufSize = io_read(SplitsFile, aBufLine, sizeof(aBufLine));
			else
				lineBufSize = 1 + io_read(SplitsFile, &aBufLine[1], sizeof(aBufLine) - 1);
		}
		else
			lineBufSize += io_read(SplitsFile, &aBufLine[lineBufSize], sizeof(aBufLine) - lineBufSize);
		if(state == SPLITSSTATE_SKIPLINE)
			state = lastState;
		else if(state != SPLITSSTATE_INIT && --stateLen <= 0)
			state = SPLITSSTATE_INIT;
	}
	io_close(SplitsFile);
	if(mapMetaMismatch)
	{
		if(Console())
			Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
		for(int i = 0; i < SPLIT_TOTAL; i++)
		{
			m_aSplitTimesBestAnyRun[i] = -1;
			m_aSplitTimesBestAnySegmentTotal[i] = -1;
			m_aSplitTimesBestTeamRun[i] = -1;
			m_aSplitTimesBestTeamSegmentTotal[i] = -1;
			m_aSplitTimesBestDummyRun[i] = -1;
			m_aSplitTimesBestDummySegmentTotal[i] = -1;
			m_aSplitTimesBestSoloRun[i] = -1;
			m_aSplitTimesBestSoloSegmentTotal[i] = -1;
			m_aSplitOrdering[i] = -1;
		}
		for(int i = 0; i < SPLIT_SEGMENTED; i++)
		{
			m_aSplitTimesBestAnySegmentSegmented[i] = -1;
			m_aSplitTimesBestTeamSegmentSegmented[i] = -1;
			m_aSplitTimesBestDummySegmentSegmented[i] = -1;
			m_aSplitTimesBestSoloSegmentSegmented[i] = -1;
		}
		return;
	}
	if(Console())
	{
		str_format(aBufError, sizeof(aBufError), "Read splits from '%s'", aBufName);
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
	}
	RecalculateCategories();
}

/*void CSplits::SaveSplits()
{
	RecalculateCategories();
	if(!Storage()->FolderExists(ms_pSplitsDir, IStorage::TYPE_SAVE))
		Storage()->CreateFolder(ms_pSplitsDir, IStorage::TYPE_SAVE);
	char aBufLine[256], aBufName[IO_MAX_PATH_LENGTH], aBufNameDst[IO_MAX_PATH_LENGTH], aBufError[64 + IO_MAX_PATH_LENGTH];
	str_format(aBufName, sizeof(aBufName), "%s/%s_tmp_%d.splits", ms_pSplitsDir, Client()->GetCurrentMap(), pid());
	IOHANDLE SplitsFile = Storage()->OpenFile(aBufName, IOFLAG_WRITE, IStorage::TYPE_SAVE);
	if(!SplitsFile)
	{
		if(Console())
		{
			str_format(aBufError, sizeof(aBufError), "Unable to open '%s' for saving splits", aBufName);
			Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
		}
		return;
	}
	strncpy(aBufLine, "2:map metadata", sizeof(aBufLine));
	io_write(SplitsFile, aBufLine, strlen(aBufLine));
	io_write_newline(SplitsFile);
	sha256_str(Client()->GetCurrentMapSha256(), aBufLine, sizeof(aBufLine));
	io_write(SplitsFile, aBufLine, strlen(aBufLine));
	io_write_newline(SplitsFile);
	strncpy(aBufLine, Client()->GetCurrentMap(), sizeof(aBufLine));
	io_write(SplitsFile, aBufLine, strlen(aBufLine));
	io_write_newline(SplitsFile);
	for(int i = 0; i < 4; i++)
	{
		int *TimesRun, *TimesSegmentTotal, *TimesSegmentSegmented;
		const char *TimesCategory;
		if(i == 0)
			TimesRun = m_aSplitTimesBestAnyRun, TimesSegmentTotal = m_aSplitTimesBestAnySegmentTotal, TimesSegmentSegmented = m_aSplitTimesBestAnySegmentSegmented, TimesCategory = "any";
		if(i == 1)
			TimesRun = m_aSplitTimesBestTeamRun, TimesSegmentTotal = m_aSplitTimesBestTeamSegmentTotal, TimesSegmentSegmented = m_aSplitTimesBestTeamSegmentSegmented, TimesCategory = "team";
		if(i == 2)
			TimesRun = m_aSplitTimesBestDummyRun, TimesSegmentTotal = m_aSplitTimesBestDummySegmentTotal, TimesSegmentSegmented = m_aSplitTimesBestDummySegmentSegmented, TimesCategory = "dummy";
		if(i == 3)
			TimesRun = m_aSplitTimesBestSoloRun, TimesSegmentTotal = m_aSplitTimesBestSoloSegmentTotal, TimesSegmentSegmented = m_aSplitTimesBestSoloSegmentSegmented, TimesCategory = "solo";
		int SplitCountBestRun = 0, SplitCountBestSegmentTotal = 0, SplitCountBestSegmentSegmented = 0;
		for(int j = 0; j < SPLIT_TOTAL; j++)
		{
			if(TimesRun[j] >= 0)
				SplitCountBestRun++;
			if(TimesSegmentTotal[j] >= 0)
				SplitCountBestSegmentTotal++;
		}
		for(int j = 0; j < SPLIT_SEGMENTED; j++)
		{
			if(TimesSegmentSegmented[j] >= 0)
				SplitCountBestSegmentSegmented++;
		}
		if(SplitCountBestRun <= 0 && SplitCountBestSegmentTotal <= 0 && SplitCountBestSegmentSegmented <= 0)
			continue;
		strncpy(aBufLine, "1:category", sizeof(aBufLine));
		io_write(SplitsFile, aBufLine, strlen(aBufLine));
		io_write_newline(SplitsFile);
		strncpy(aBufLine, TimesCategory, sizeof(aBufLine));
		io_write(SplitsFile, aBufLine, strlen(aBufLine));
		io_write_newline(SplitsFile);
		str_format(aBufLine, sizeof(aBufLine), "%i:run", SplitCountBestRun);
		io_write(SplitsFile, aBufLine, strlen(aBufLine));
		io_write_newline(SplitsFile);
		for(int j = 0; j < SPLIT_TOTAL; j++)
		{
			if(TimesRun[j] < 0)
				continue;
			str_format(aBufLine, sizeof(aBufLine), "%i %i", j, TimesRun[j]);
			io_write(SplitsFile, aBufLine, strlen(aBufLine));
			io_write_newline(SplitsFile);
		}
		str_format(aBufLine, sizeof(aBufLine), "%i:segment total", SplitCountBestSegmentTotal);
		io_write(SplitsFile, aBufLine, strlen(aBufLine));
		io_write_newline(SplitsFile);
		for(int j = 0; j < SPLIT_TOTAL; j++)
		{
			if(TimesSegmentTotal[j] < 0)
				continue;
			str_format(aBufLine, sizeof(aBufLine), "%i %i", j, TimesSegmentTotal[j]);
			io_write(SplitsFile, aBufLine, strlen(aBufLine));
			io_write_newline(SplitsFile);
		}
		str_format(aBufLine, sizeof(aBufLine), "%i:segment segmented", SplitCountBestSegmentSegmented);
		io_write(SplitsFile, aBufLine, strlen(aBufLine));
		io_write_newline(SplitsFile);
		for(int j = 0; j < SPLIT_SEGMENTED; j++)
		{
			if(TimesSegmentSegmented[j] < 0)
				continue;
			str_format(aBufLine, sizeof(aBufLine), "%i %i %i", j / SPLIT_SEGMENTEDSTRIDE, j % SPLIT_SEGMENTEDSTRIDE, TimesSegmentSegmented[j]);
			io_write(SplitsFile, aBufLine, strlen(aBufLine));
			io_write_newline(SplitsFile);
		}
	}
	io_close(SplitsFile);
	str_format(aBufNameDst, sizeof(aBufNameDst), "%s/%s.splits", ms_pSplitsDir, Client()->GetCurrentMap());
	if(!Storage()->RenameFile(aBufName, aBufNameDst, IStorage::TYPE_SAVE))
	{
		if(Console())
		{
			str_format(aBufError, sizeof(aBufError), "Could not move splits file '%s' to '%s'.", aBufName, aBufNameDst);
			Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
		}
		return;
	}
	if(Console())
	{
		str_format(aBufError, sizeof(aBufError), "Saved splits to '%s'", aBufNameDst);
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "splits", aBufError, ColorRGBA(0.75f, 0.7f, 0.7f, 1.0f));
	}
}*/

void CSplits::RecalculateCategories()
{
	for(int i = 0; i < 3; i++)
	{
		int *ParentRun, *ParentSegmentTotal, *ParentSegmentSegmented;
		int *ChildRun, *ChildSegmentTotal, *ChildSegmentSegmented;
		if(i == 0)
		{
			ParentRun = m_aSplitTimesBestTeamRun, ParentSegmentTotal = m_aSplitTimesBestTeamSegmentTotal, ParentSegmentSegmented = m_aSplitTimesBestTeamSegmentSegmented;
			ChildRun = m_aSplitTimesBestDummyRun, ChildSegmentTotal = m_aSplitTimesBestDummySegmentTotal, ChildSegmentSegmented = m_aSplitTimesBestDummySegmentSegmented;
		}
		if(i == 1)
		{
			ParentRun = m_aSplitTimesBestAnyRun, ParentSegmentTotal = m_aSplitTimesBestAnySegmentTotal, ParentSegmentSegmented = m_aSplitTimesBestAnySegmentSegmented;
			ChildRun = m_aSplitTimesBestTeamRun, ChildSegmentTotal = m_aSplitTimesBestTeamSegmentTotal, ChildSegmentSegmented = m_aSplitTimesBestTeamSegmentSegmented;
		}
		if(i == 2)
		{
			ParentRun = m_aSplitTimesBestAnyRun, ParentSegmentTotal = m_aSplitTimesBestAnySegmentTotal, ParentSegmentSegmented = m_aSplitTimesBestAnySegmentSegmented;
			ChildRun = m_aSplitTimesBestSoloRun, ChildSegmentTotal = m_aSplitTimesBestSoloSegmentTotal, ChildSegmentSegmented = m_aSplitTimesBestSoloSegmentSegmented;
		}
		if(ChildRun[SPLIT_FINISH] >= 0 && (ParentRun[SPLIT_FINISH] < 0 || ChildRun[SPLIT_FINISH] < ParentRun[SPLIT_FINISH]))
		{
			for(int j = 0; j < SPLIT_TOTAL; j++)
				ParentRun[j] = ChildRun[j];
		}
		for(int j = 0; j < SPLIT_TOTAL; j++)
		{
			if(ChildSegmentTotal[j] >= 0 && (ParentSegmentTotal[j] < 0 || ChildSegmentTotal[j] < ParentSegmentTotal[j]))
				ParentSegmentTotal[j] = ChildSegmentTotal[j];
		}
		for(int j = 0; j < SPLIT_SEGMENTED; j++)
		{
			if(ChildSegmentSegmented[j] >= 0 && (ParentSegmentSegmented[j] < 0 || ChildSegmentSegmented[j] < ParentSegmentSegmented[j]))
				ParentSegmentSegmented[j] = ChildSegmentSegmented[j];
		}
	}
}

bool CSplits::SplitsActive()
{
	return m_SplitsActive && GameClient()->m_GameInfo.m_Race && GameClient()->m_GameInfo.m_TimeScore;
}

bool CSplits::VisualIgnoreDummy()
{
	if(!g_KaizoConfig_KaizoSplitsTrackDummy)
		return true;
	if(m_SplitCategory[0] == SPLITCATEGORY_NORANK)
		return false;
	for(int i = 1; i < NUM_DUMMIES; i++)
	{
		if(m_SplitCategory[i] == SPLITCATEGORY_NORANK)
			continue;
		if(m_SplitCategory[i] != m_SplitCategory[0])
			return true;
	}
	return false;
}

int CSplits::CurrentRunTime()
{
	int dummy = VisualIgnoreDummy() ? 0 : m_CurrentDummy;
	if(m_RaceStartTick[dummy] < 0)
		return -1;
	return ((int64_t)(Client()->GameTick(Client()->State() == IClient::STATE_DEMOPLAYBACK ? 0 : dummy) - m_RaceStartTick[dummy]) * (int64_t)100) / Client()->GameTickSpeed();
}

int CSplits::CurrentSplitTime(int Split)
{
	if(Split < 0 || Split >= SPLIT_TOTAL)
		return -1;
	int SplitTime = -1;
	for(int i = 0; i < (VisualIgnoreDummy() ? 1 : NUM_DUMMIES); i++)
	{
		if(m_aSplitTimesCurrent[i][Split] < 0)
			continue;
		if(SplitTime < 0 || m_aSplitTimesCurrent[i][Split] < SplitTime)
			SplitTime = m_aSplitTimesCurrent[i][Split];
	}
	return SplitTime;
}

int CSplits::BestRunSplitTime(int Split)
{
	if(Split < 0 || Split >= SPLIT_TOTAL)
		return -1;
	return m_aSplitTimesBestDisplay[Split];
}

int CSplits::CurrentSegment()
{
	int Split = SPLIT_FINISH;
	while(Split >= 0)
	{
		bool ThisSegment = false;
		for(int i = 0; i < (VisualIgnoreDummy() ? 1 : NUM_DUMMIES); i++)
		{
			if(m_aSplitTimesCurrent[i][Split] < 0)
				continue;
			ThisSegment = true;
			break;
		}
		if(ThisSegment)
			break;
		Split--;
	}
	return Split;
}

int CSplits::FurthestSegment()
{
	int Split = SPLIT_FINISH;
	while(Split >= 0)
	{
		bool hasSplit = m_aSplitTimesBestDisplay[Split] >= 0;
		for(int i = 0; i < (VisualIgnoreDummy() ? 1 : NUM_DUMMIES); i++)
			hasSplit = hasSplit || m_aSplitTimesCurrent[i][Split] >= 0;
		if(hasSplit)
			break;
		Split--;
	}
	return Split;
}

int CSplits::SplitCheck(int Split)
{
	if(Split < 0 || Split >= SPLIT_TOTAL)
		return 0;
	if(CurrentSplitTime(Split) < 0 || m_aSplitTimesBestDisplay[Split] < 0)
		return 0;
	return CurrentSplitTime(Split) - m_aSplitTimesBestDisplay[Split];
}

int CSplits::CurrentSplitCheck()
{
	int Split = NextSplitId(CurrentSegment());
	if(m_aSplitTimesBestDisplay[Split] < 0)
		return 0; //std::INT_MIN;
	return CurrentRunTime() - m_aSplitTimesBestDisplay[Split];
}

int CSplits::PrevSplitId(int Split)
{
	Split--;
	if(Split < 0 || Split >= SPLIT_TOTAL)
		return -1;
	while(Split >= 0)
	{
		bool hasSplit = m_aSplitTimesBestDisplay[Split] >= 0;
		for(int i = 0; i < (VisualIgnoreDummy() ? 1 : NUM_DUMMIES); i++)
			hasSplit = hasSplit || m_aSplitTimesCurrent[i][Split] >= 0;
		if(hasSplit)
			break;
		Split--;
	}
	return Split;
}

int CSplits::NextSplitId(int Split)
{
	Split++;
	if(Split < 0 || Split >= SPLIT_TOTAL)
		return -1;
	while(Split < SPLIT_FINISH)
	{
		bool hasSplit = m_aSplitTimesBestDisplay[Split] >= 0;
		for(int i = 0; i < (VisualIgnoreDummy() ? 1 : NUM_DUMMIES); i++)
			hasSplit = hasSplit || m_aSplitTimesCurrent[i][Split] >= 0;
		if(hasSplit)
			break;
		Split++;
	}
	return Split;
}

int CSplits::SplitIsGold(int Split)
{
	if(Split < 0 || Split >= SPLIT_TOTAL)
		return -1;
	int SplitTime = -1;
	bool SplitGold = false;
	for(int i = 0; i < (VisualIgnoreDummy() ? 1 : NUM_DUMMIES); i++)
	{
		if(m_aSplitTimesCurrent[i][Split] < 0)
			continue;
		if(SplitTime < 0 || m_aSplitTimesCurrent[i][Split] < SplitTime)
			SplitTime = m_aSplitTimesCurrent[i][Split], SplitGold = m_aSplitTimesGold[i][Split];
	}
	return SplitGold;
}

void CSplits::SplitName(int Split, char *Name, int MaxNameSize)
{
	if(Split < -1 || Split > SPLIT_FINISH)
	{
		strncpy(Name, "!", MaxNameSize);
		return;
	}
	if(Split == -1)
	{
		strncpy(Name, "S", MaxNameSize);
		return;
	}
	if(Split == SPLIT_FINISH)
	{
		strncpy(Name, "F", MaxNameSize);
		return;
	}
	if(m_aSplitOrdering[Split] < 0)
	{
		strncpy(Name, "?", MaxNameSize);
		return;
	}
	str_format(Name, MaxNameSize, "%i", m_aSplitOrdering[Split] + 1);
}

void CSplits::SplitCategoryName(char *Name, int MaxNameSize)
{
	static const char *categoryStrs[] = {
		"No Rank",
		"Any Rank",
		"Team Rank",
		"Dummy Rank",
		"Solo Rank"};
	strncpy(Name, categoryStrs[m_SplitCategory[0]], MaxNameSize);
}

int CSplits::SumOfBestSegments()
{
	if(!m_aSplitTimesComparisonSegmentSegmented)
		return 0;
	int Time = 0;
	for(int i = NextSplitId(-1); i >= 0 && i < SPLIT_TOTAL; i = NextSplitId(i))
	{
		int i0 = m_aSplitOrdering[i];
		int i1 = PrevSplitId(i) < 0 ? -1 : m_aSplitOrdering[PrevSplitId(i)];
		if(m_aSplitTimesComparisonSegmentSegmented[(i1 + 1) * SPLIT_SEGMENTEDSTRIDE + i0] >= 0)
			Time += m_aSplitTimesComparisonSegmentSegmented[(i1 + 1) * SPLIT_SEGMENTEDSTRIDE + i0];
	}
	return Time;
}

bool CSplits::SplitsAreInOrder()
{
	if(!m_aSplitTimesComparisonSegmentSegmented)
		return false;
	int SplitId = -1;
	for(int i = 0; i < SPLIT_FINISH; i++)
	{
		int Next = -1;
		for(int j = 0; j < SPLIT_TOTAL; j++)
		{
			if(m_aSplitTimesComparisonSegmentSegmented[(SplitId + 1) * SPLIT_SEGMENTEDSTRIDE + j] >= 0)
			{
				if(Next >= 0)
					return false;
				Next = j;
			}
		}
		if(Next == -1 || Next == SPLIT_FINISH)
			return true;
		SplitId = Next;
	}
	return true;
}

int CSplits::CurrentPace()
{
	if(!m_aSplitTimesComparisonRun || !m_aSplitTimesComparisonSegmentTotal || !m_aSplitTimesComparisonSegmentSegmented)
		return 0;
	int dummy = VisualIgnoreDummy() ? 0 : m_CurrentDummy;
	int Split = SPLIT_FINISH;
	while(Split >= 0)
	{
		if(m_aSplitTimesCurrent[dummy][Split] >= 0)
			break;
		Split--;
	}
	int Time = Split >= 0 ? m_aSplitTimesCurrent[dummy][Split] : 0;
	// predict the finish time using the segment times from the comparison run
	// the comparison run times might not make sense if a map has multiple routes, so the best segment time is used if it is greater than the comparison segment time
	while(Split < SPLIT_FINISH)
	{
		int Next = NextSplitId(Split);
		if(Next < 0)
			return Time;
		int Delta = m_aSplitTimesComparisonSegmentSegmented[((Split < 0 ? -1 : m_aSplitOrdering[Split]) + 1) * SPLIT_SEGMENTEDSTRIDE + m_aSplitOrdering[Next]];
		if(Delta < 0)
			return Time;
		int TimePrev = Split >= 0 ? m_aSplitTimesComparisonRun[m_aSplitOrdering[Split]] : 0;
		if(TimePrev < 0)
			TimePrev = Split >= 0 ? m_aSplitTimesComparisonSegmentTotal[m_aSplitOrdering[Split]] : 0;
		int TimeNext = m_aSplitTimesComparisonRun[m_aSplitOrdering[Next]];
		if(TimeNext < 0)
			TimeNext = m_aSplitTimesComparisonSegmentTotal[m_aSplitOrdering[Next]];
		if(TimePrev >= 0 && TimeNext >= 0)
		{
			int DeltaDisplay = TimeNext - TimePrev;
			if(DeltaDisplay > Delta)
				Delta = DeltaDisplay;
		}
		Time += Delta;
		Split = Next;
	}
	return Time;
}

int CSplits::BestPossibleTime()
{
	if(!m_aSplitTimesComparisonSegmentSegmented)
		return 0;
	int dummy = VisualIgnoreDummy() ? 0 : m_CurrentDummy;
	int Split = SPLIT_FINISH;
	while(Split >= 0)
	{
		if(m_aSplitTimesCurrent[dummy][Split] >= 0)
			break;
		Split--;
	}
	int Time = Split >= 0 ? m_aSplitTimesCurrent[dummy][Split] : 0;
	while(Split < SPLIT_FINISH)
	{
		int Next = NextSplitId(Split);
		if(Next < 0)
			return Time;
		int Delta = m_aSplitTimesComparisonSegmentSegmented[((Split < 0 ? -1 : m_aSplitOrdering[Split]) + 1) * SPLIT_SEGMENTEDSTRIDE + m_aSplitOrdering[Next]];
		if(Delta < 0)
			return Time;
		Time += Delta;
		Split = Next;
	}
	return Time;
}
