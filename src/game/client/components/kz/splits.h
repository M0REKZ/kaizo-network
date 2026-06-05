//From CMClient
#ifndef GAME_CLIENT_COMPONENTS_SPLITS_H
#define GAME_CLIENT_COMPONENTS_SPLITS_H
#include <engine/client/enums.h>
#include <engine/shared/protocol.h>

#include <game/client/component.h>

class CSplits : public CComponent
{
public:
	static const int SPLIT_TOTAL = 26;
	static const int SPLIT_FINISH = 25;
	static const int SPLIT_SEGMENTED = 26 * 26;
	static const int SPLIT_SEGMENTEDSTRIDE = 26;
	static const char *ms_pSplitsDir;

private:
	bool m_SplitsActive, m_MapHasCheckpoints;

	int m_aSplitTimesCurrent[NUM_DUMMIES][SPLIT_TOTAL];
	bool m_aSplitTimesGold[NUM_DUMMIES][SPLIT_TOTAL];
	int m_aSplitTimesBestDisplay[SPLIT_TOTAL];
	int m_aSplitOrdering[SPLIT_TOTAL];

	enum
	{
		SPLITCATEGORY_NORANK = 0,
		SPLITCATEGORY_ANYRANK,
		SPLITCATEGORY_TEAMRANK,
		SPLITCATEGORY_DUMMYRANK,
		SPLITCATEGORY_SOLORANK
	} m_SplitCategory[NUM_DUMMIES];

	int *m_aSplitTimesBestRun;
	int *m_aSplitTimesBestSegmentTotal;
	int *m_aSplitTimesBestSegmentSegmented;
	int *m_aSplitTimesComparisonRun;
	int *m_aSplitTimesComparisonSegmentTotal;
	int *m_aSplitTimesComparisonSegmentSegmented;

	int m_aSplitTimesBestAnyRun[SPLIT_TOTAL];
	int m_aSplitTimesBestAnySegmentTotal[SPLIT_TOTAL];
	int m_aSplitTimesBestAnySegmentSegmented[SPLIT_SEGMENTED];

	int m_aSplitTimesBestTeamRun[SPLIT_TOTAL];
	int m_aSplitTimesBestTeamSegmentTotal[SPLIT_TOTAL];
	int m_aSplitTimesBestTeamSegmentSegmented[SPLIT_SEGMENTED];

	int m_aSplitTimesBestDummyRun[SPLIT_TOTAL];
	int m_aSplitTimesBestDummySegmentTotal[SPLIT_TOTAL];
	int m_aSplitTimesBestDummySegmentSegmented[SPLIT_SEGMENTED];

	int m_aSplitTimesBestSoloRun[SPLIT_TOTAL];
	int m_aSplitTimesBestSoloSegmentTotal[SPLIT_TOTAL];
	int m_aSplitTimesBestSoloSegmentSegmented[SPLIT_SEGMENTED];

	int m_RaceStartTick[NUM_DUMMIES], m_LastRaceStartTick[NUM_DUMMIES];
	int m_DummyId[NUM_DUMMIES];
	int m_CurrentDummy;
	int m_Teams[MAX_CLIENTS];
	bool m_WasTeam0Mode[NUM_DUMMIES], m_HasStarted[NUM_DUMMIES], m_HasDied[NUM_DUMMIES];

	bool m_HasQueuedSplit[NUM_DUMMIES];
	int64_t m_QueuedSplitTick[NUM_DUMMIES];
	int m_QueuedSplitTime[NUM_DUMMIES], m_QueuedSplitCheck[NUM_DUMMIES], m_QueuedSplitFinish[NUM_DUMMIES];

	bool RecalculateSplitOrdering(int SplitIdOrdered, int SplitId);
	bool CommitTime(int Dummy, int RunTime, int SplitId, int SplitIdOrdered, int PrevIdOrdered);
	bool CommitTimeCheckpoint(int Dummy, int RunTime, int SplitId);

	void LoadSplits();
	void SaveSplits();
	void LoadSplitsOld();
	void RecalculateCategories();

	bool VisualIgnoreDummy();

public:
	CSplits();
	int Sizeof() const override { return sizeof(*this); }
	void OnConsoleInit() override;
	void OnReset() override;
	void OnUpdate() override;
	void OnMapLoad() override;
	void OnMessage(int MsgType, void *pRawMsg, int Dummy);
	void OnMessage(int MsgType, void *pRawMsg) override { OnMessage(MsgType, pRawMsg, 0); }

	bool MapHasCheckpoints() { return m_MapHasCheckpoints; }
	bool SplitsActive();
	int CurrentRunTime();
	int CurrentSplitTime(int Split);
	int BestRunSplitTime(int Split);
	int CurrentSegment();
	int FurthestSegment();
	int SplitCheck(int Split);
	int CurrentSplitCheck();
	int PrevSplitId(int Split);
	int NextSplitId(int Split);
	int SplitIsGold(int Split);
	void SplitName(int Split, char *Name, int MaxNameSize);
	void SplitCategoryName(char *Name, int MaxNameSize);
	int SumOfBestSegments();
	bool SplitsAreInOrder();
	int CurrentPace();
	int BestPossibleTime();
};

#endif
