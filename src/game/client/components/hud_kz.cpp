
#include <base/time.h>
#include "hud.h"
#include <engine/shared/config.h>
#include <game/client/gameclient.h>

//+KZ
extern int g_KaizoConfig_KaizoShowhudSplits;
extern int g_KaizoConfig_KaizoSplitsMaxRowsAfterRun;
extern int g_KaizoConfig_KaizoSplitsMaxRowsBeforeRun;
extern int g_KaizoConfig_KaizoSplitsMaxRowsInRun;
extern int g_KaizoConfig_KaizoSplitsAlwaysShowFinalSplit;
extern int g_KaizoConfig_KaizoSplitsNameFormat;
extern int g_KaizoConfig_KaizoSplitsShowSplits;
extern int g_KaizoConfig_KaizoSplitsDisplaySegmentTime;
extern int g_KaizoConfig_KaizoSplitsMaximumCurrentCheck;
extern int g_KaizoConfig_KaizoSplitsShowMap;
extern int g_KaizoConfig_KaizoSplitsShowCategory;
extern int g_KaizoConfig_KaizoSplitsShowTimer;
extern int g_KaizoConfig_KaizoSplitsShowLastSegment;
extern int g_KaizoConfig_KaizoSplitsShowFurthest;
extern int g_KaizoConfig_KaizoSplitsShowSumOfBest;
extern int g_KaizoConfig_KaizoSplitsShowCurrentPace;
extern int g_KaizoConfig_KaizoSplitsShowBestPossible;
extern int g_KaizoConfig_KaizoSplitsLabels;
extern int g_KaizoConfig_KaizoSplitsX;

//From CMClient
void CHud::RenderSplits()
{
	if((g_KaizoConfig_KaizoShowhudSplits != 2 && (g_KaizoConfig_KaizoShowhudSplits != 1 || !GameClient()->m_Splits.MapHasCheckpoints())) || !GameClient()->m_Splits.SplitsActive())
	{
		m_SplitsLastWidth = 0.0f;
		return;
	}
	char aBuf[32];
	float FontSize = 5.0f;
	static float s_TextWidthM = TextRender()->TextWidth(FontSize, "00:00.00", -1, -1.0f);
	static float s_TextWidthH = TextRender()->TextWidth(FontSize, "00:00:00.00", -1, -1.0f);
	static float s_TextWidth0D = TextRender()->TextWidth(FontSize, "0d 00:00:00.00", -1, -1.0f);
	static float s_TextWidth00D = TextRender()->TextWidth(FontSize, "00d 00:00:00.00", -1, -1.0f);
	static float s_TextWidth000D = TextRender()->TextWidth(FontSize, "000d 00:00:00.00", -1, -1.0f);
	static float s_TextWidthDeltaS = TextRender()->TextWidth(FontSize, "+00.00", -1, -1.0f);
	static float s_TextWidthDeltaM = TextRender()->TextWidth(FontSize, "+00:00.00", -1, -1.0f);
	static float s_TextWidthDelta00H = TextRender()->TextWidth(FontSize, "+00:00:00.00", -1, -1.0f);
	static float s_TextWidthDelta000H = TextRender()->TextWidth(FontSize, "+000:00:00.00", -1, -1.0f);
	static float s_TextWidthDelta0000H = TextRender()->TextWidth(FontSize, "+0000:00:00.00", -1, -1.0f);
	//static float s_TextWidthDelta00000H = TextRender()->TextWidth(FontSize, "+00000:00:00.00", -1, -1.0f);
	static float s_TextWidthSplit = TextRender()->TextWidth(FontSize, "00", -1, -1.0f);
	static float s_TextWidthArrow = TextRender()->TextWidth(FontSize, "→", -1, -1.0f);
	int displayedSplits[CSplits::SPLIT_TOTAL];
	int currentSplit = GameClient()->m_Splits.NextSplitId(GameClient()->m_Splits.CurrentSegment());
	if(GameClient()->m_Splits.CurrentRunTime() < 0)
		currentSplit = GameClient()->m_Splits.PrevSplitId(CSplits::SPLIT_FINISH + 1);
	else if(GameClient()->m_Splits.CurrentSplitTime(currentSplit) < 0 && GameClient()->m_Splits.BestRunSplitTime(currentSplit) < 0)
		currentSplit = GameClient()->m_Splits.PrevSplitId(currentSplit);
	int splitLimit = GameClient()->m_Splits.CurrentRunTime() < 0 ? (GameClient()->m_Splits.CurrentSplitTime(CSplits::SPLIT_FINISH) >= 0 ? g_KaizoConfig_KaizoSplitsMaxRowsAfterRun : g_KaizoConfig_KaizoSplitsMaxRowsBeforeRun) : g_KaizoConfig_KaizoSplitsMaxRowsInRun;
	int splitCount = 0, finalSplit = -1;
	if(splitLimit > 0)
	{
		splitCount = 1;
		displayedSplits[0] = currentSplit;
		finalSplit = GameClient()->m_Splits.PrevSplitId(CSplits::SPLIT_FINISH + 1);
		bool finalIsFinish = false;
		for(int i = 1; i < splitLimit - (g_KaizoConfig_KaizoSplitsAlwaysShowFinalSplit && displayedSplits[0] != finalSplit ? 1 : 0); i++)
		{
			displayedSplits[i] = GameClient()->m_Splits.PrevSplitId(displayedSplits[i - 1]);
			if(displayedSplits[i] < 0)
				break;
			splitCount++;
		}
		for(int i = 0; i < splitCount / 2; i++)
		{
			int tmp = displayedSplits[i];
			displayedSplits[i] = displayedSplits[splitCount - 1 - i];
			displayedSplits[splitCount - 1 - i] = tmp;
		}
		while(splitCount < splitLimit)
		{
			int nextSplit = GameClient()->m_Splits.NextSplitId(displayedSplits[splitCount - 1]);
			if(GameClient()->m_Splits.CurrentSplitTime(nextSplit) < 0 && GameClient()->m_Splits.BestRunSplitTime(nextSplit) < 0)
				break;
			displayedSplits[splitCount++] = nextSplit;
			if(nextSplit == finalSplit)
			{
				finalIsFinish = true;
				break;
			}
		}
		if(splitCount > 1 && g_KaizoConfig_KaizoSplitsAlwaysShowFinalSplit && !finalIsFinish)
			displayedSplits[splitCount - 1] = finalSplit;
		if(GameClient()->m_Splits.CurrentRunTime() < 0 || GameClient()->m_Splits.CurrentSplitTime(currentSplit) >= 0)
			currentSplit = -1;
	}
	int SplitNameFormat = g_KaizoConfig_KaizoSplitsNameFormat;
	if(SplitNameFormat == 3)
		SplitNameFormat = GameClient()->m_Splits.SplitsAreInOrder() ? 1 : 0;
	if(SplitNameFormat == 4)
		SplitNameFormat = GameClient()->m_Splits.SplitsAreInOrder() ? 2 : 0;

	float y = 3.75f;
	float maxWT = s_TextWidthM, maxWD = s_TextWidthDeltaS;
	if(g_KaizoConfig_KaizoSplitsShowSplits >= 0 && splitCount > 0)
	{
		y += 1.0f;
		int LastTimeC = 0, LastTimeBR = 0;
		for(int i = 0; i < splitCount; i++)
		{
			int TimeC = GameClient()->m_Splits.CurrentSplitTime(displayedSplits[i]), TimeBR = GameClient()->m_Splits.BestRunSplitTime(displayedSplits[i]);
			if(TimeC < 0 && TimeBR < 0)
				continue;
			int TimeCRun = TimeC, TimeBRRun = TimeBR;
			if(g_KaizoConfig_KaizoSplitsDisplaySegmentTime)
			{
				if(TimeC >= 0)
				{
					TimeC -= LastTimeC;
					LastTimeC += TimeC;
				}
				TimeBR -= LastTimeBR;
				LastTimeBR += TimeBR;
			}
			int Time = (g_KaizoConfig_KaizoSplitsDisplaySegmentTime & 2) ? (TimeC >= 0 ? TimeC : TimeBR) : (TimeCRun >= 0 ? TimeCRun : TimeBRRun);
			int w = Time >= 3600 * 24 * 100 * 100 ? s_TextWidth000D : Time >= 3600 * 24 * 10 * 100 ? s_TextWidth00D :
									  Time >= 3600 * 24 * 100              ? s_TextWidth0D :
									  Time >= 3600 * 100                   ? s_TextWidthH :
														 s_TextWidthM;
			if(w > maxWT)
				maxWT = w;
			if((displayedSplits[i] == currentSplit || TimeC >= 0) && TimeBR >= 0)
			{
				int Delta = (displayedSplits[i] == currentSplit ? (GameClient()->m_Splits.CurrentRunTime() - ((g_KaizoConfig_KaizoSplitsDisplaySegmentTime & 1) ? LastTimeBR - TimeBR : 0)) : ((g_KaizoConfig_KaizoSplitsDisplaySegmentTime & 1) ? TimeC : TimeCRun)) - ((g_KaizoConfig_KaizoSplitsDisplaySegmentTime & 1) ? TimeBR : TimeBRRun); //displayedSplits[i] == currentSplit ? GameClient()->m_Splits.CurrentSplitCheck() : GameClient()->m_Splits.SplitCheck(displayedSplits[i]);
				int ADelta = Delta < 0 ? -Delta : Delta;
				if(displayedSplits[i] != currentSplit || g_KaizoConfig_KaizoSplitsMaximumCurrentCheck < 0 || (g_KaizoConfig_KaizoSplitsMaximumCurrentCheck > 0 && ADelta <= g_KaizoConfig_KaizoSplitsMaximumCurrentCheck * 100))
				{
					w = ADelta >= 3600 * 1000 * 100 ? s_TextWidthDelta0000H : ADelta >= 3600 * 100 * 100 ? s_TextWidthDelta000H :
											  ADelta >= 3600 * 100               ? s_TextWidthDelta00H :
											  ADelta >= 60 * 100                 ? s_TextWidthDeltaM :
															       s_TextWidthDeltaS;
					if(w > maxWD)
						maxWD = w;
				}
			}
			y += FontSize + 2.0f;
		}
	}
	char aBuf2[8][2][256];
	int displayPriorities[8];
	ColorRGBA displayColors[8];
	float displayWidths[8];
	for(int i = 0; i < 8; i++)
		aBuf2[i][0][0] = 0.0f, aBuf2[i][1][0] = 0.0f, displayPriorities[i] = -1, displayColors[i] = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f), displayWidths[i] = 0.0f;
	if(g_KaizoConfig_KaizoSplitsShowMap > 0)
		strncpy(aBuf2[0][1], GameClient()->Map()->BaseName(), sizeof(aBuf2[0][1]));
	displayPriorities[0] = g_KaizoConfig_KaizoSplitsShowMap - 1;
	if(g_KaizoConfig_KaizoSplitsShowCategory > 0)
		GameClient()->m_Splits.SplitCategoryName(aBuf2[1][1], sizeof(aBuf2[1][1]));
	displayPriorities[1] = g_KaizoConfig_KaizoSplitsShowCategory - 1;
	int RunTimer = GameClient()->m_Splits.CurrentSplitTime(CSplits::SPLIT_FINISH);
	if(RunTimer < 0)
		RunTimer = GameClient()->m_Splits.CurrentRunTime();
	if(g_KaizoConfig_KaizoSplitsShowTimer > 0)
	{
		int DRunTimer = RunTimer < 0 ? 0 : RunTimer;
		str_time((int64_t)DRunTimer, ETimeFormat::DAYS, aBuf2[2][1], sizeof(aBuf2[2][1]));
		str_format(&aBuf2[2][1][strlen(aBuf2[2][1])], sizeof(aBuf2[2][1]) - strlen(aBuf2[2][1]), ".%02i", DRunTimer % 100);
		if(RunTimer < 0)
			displayColors[2] = ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f);
		else if(RunTimer == GameClient()->m_Splits.CurrentRunTime())
			displayColors[2] = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
		else
		{
			int ComparisonTime = GameClient()->m_Splits.BestRunSplitTime(CSplits::SPLIT_FINISH);
			int Delta = RunTimer - ComparisonTime;
			if(ComparisonTime < 0 || Delta == 0)
				displayColors[2] = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
			else if(Delta > 0)
				displayColors[2] = ColorRGBA(1.0f, 0.5f, 0.5f, 1.0f);
			else
				displayColors[2] = ColorRGBA(0.5f, 1.0f, 0.5f, 1.0f);
		}
		char aWidthBuf[256];
		for(int i = 0; i < 256; i++)
		{
			if(aBuf2[2][1][i] >= '0' && aBuf2[2][1][i] <= '9')
				aWidthBuf[i] = '0';
			else
				aWidthBuf[i] = aBuf2[2][1][i];
			if(aWidthBuf[i] == 0)
				break;
		}
		displayWidths[2] = TextRender()->TextWidth(FontSize * 6.0f / 5.0f, aWidthBuf, -1, -1.0f);
	}
	displayPriorities[2] = g_KaizoConfig_KaizoSplitsShowTimer - 1;
	int SegmentTime = 0;
	if(g_KaizoConfig_KaizoSplitsShowLastSegment > 0 && RunTimer >= 0)
	{
		strncpy(aBuf2[3][0], "Last Segment: ", sizeof(aBuf2[3][0]));
		strncpy(aBuf2[3][1], "---", sizeof(aBuf2[3][1]));
		int End = GameClient()->m_Splits.PrevSplitId(currentSplit);
		if(End < 0 && GameClient()->m_Splits.CurrentSplitTime(CSplits::SPLIT_FINISH) >= 0)
			End = CSplits::SPLIT_FINISH;
		int EndTime = GameClient()->m_Splits.CurrentSplitTime(End);
		int Start = GameClient()->m_Splits.PrevSplitId(End);
		int StartTime = Start < 0 ? 0 : GameClient()->m_Splits.CurrentSplitTime(Start);
		if(StartTime >= 0 && EndTime >= 0)
			SegmentTime = EndTime - StartTime;
		if(End < 0)
			displayColors[3] = ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f);
		else
		{
			int ComparisonEndTime = GameClient()->m_Splits.BestRunSplitTime(End);
			int ComparisonStartTime = Start < 0 ? 0 : GameClient()->m_Splits.BestRunSplitTime(Start);
			if(ComparisonEndTime >= 0 && ComparisonStartTime >= 0)
			{
				int ComparisonSegmentTime = ComparisonEndTime - ComparisonStartTime;
				int Delta = SegmentTime - ComparisonSegmentTime;
				int ADelta = Delta < 0 ? -Delta : Delta;
				str_time((int64_t)ADelta, ETimeFormat::HOURS_CENTISECS, &aBuf2[3][1][1], sizeof(aBuf2[3][1]) - sizeof(aBuf2[3][1][0]));
				aBuf2[3][1][0] = Delta < 0 ? '-' : '+';
				if(GameClient()->m_Splits.SplitIsGold(End))
					displayColors[3] = ColorRGBA(1.0f, 1.0f, 0.5f, 1.0f);
				else if(SegmentTime == ComparisonSegmentTime)
					displayColors[3] = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
				else if(SegmentTime > ComparisonSegmentTime)
					displayColors[3] = ColorRGBA(1.0f, 0.5f, 0.5f, 1.0f);
				else
					displayColors[3] = ColorRGBA(0.5f, 1.0f, 0.5f, 1.0f);
			}
		}
	}
	displayPriorities[3] = g_KaizoConfig_KaizoSplitsShowLastSegment - 1;
	int furthest = GameClient()->m_Splits.FurthestSegment();
	if(g_KaizoConfig_KaizoSplitsShowFurthest > 0 && furthest != CSplits::SPLIT_FINISH)
	{
		strncpy(aBuf2[4][0], "Furthest: ", sizeof(aBuf2[4][0]));
		GameClient()->m_Splits.SplitName(furthest, aBuf2[4][1], sizeof(aBuf2[4][1]));
	}
	displayPriorities[4] = g_KaizoConfig_KaizoSplitsShowFurthest - 1;
	if(g_KaizoConfig_KaizoSplitsShowSumOfBest > 0 && furthest == CSplits::SPLIT_FINISH)
	{
		strncpy(aBuf2[5][0], "Sum of Best: ", sizeof(aBuf2[5][0]));
		int Time = GameClient()->m_Splits.SumOfBestSegments();
		str_time((int64_t)Time, ETimeFormat::DAYS, aBuf2[5][1], sizeof(aBuf2[5][1]));
		str_format(&aBuf2[5][1][strlen(aBuf2[5][1])], sizeof(aBuf2[5][1]) - strlen(aBuf2[5][1]), ".%02i", Time % 100);
	}
	displayPriorities[5] = g_KaizoConfig_KaizoSplitsShowSumOfBest - 1;
	if(g_KaizoConfig_KaizoSplitsShowCurrentPace > 0 && furthest == CSplits::SPLIT_FINISH && GameClient()->m_Splits.CurrentRunTime() >= 0)
	{
		strncpy(aBuf2[6][0], "Current Pace: ", sizeof(aBuf2[6][0]));
		int Time = GameClient()->m_Splits.CurrentPace();
		str_time((int64_t)Time, ETimeFormat::DAYS, aBuf2[6][1], sizeof(aBuf2[6][1]));
		str_format(&aBuf2[6][1][strlen(aBuf2[6][1])], sizeof(aBuf2[6][1]) - strlen(aBuf2[6][1]), ".%02i", Time % 100);
	}
	displayPriorities[6] = g_KaizoConfig_KaizoSplitsShowCurrentPace - 1;
	if(g_KaizoConfig_KaizoSplitsShowBestPossible > 0 && furthest == CSplits::SPLIT_FINISH && GameClient()->m_Splits.CurrentRunTime() >= 0)
	{
		strncpy(aBuf2[7][0], "Best Possible Time: ", sizeof(aBuf2[7][0]));
		int Time = GameClient()->m_Splits.BestPossibleTime();
		str_time((int64_t)Time, ETimeFormat::DAYS, aBuf2[7][1], sizeof(aBuf2[7][1]));
		str_format(&aBuf2[7][1][strlen(aBuf2[7][1])], sizeof(aBuf2[7][1]) - strlen(aBuf2[7][1]), ".%02i", Time % 100);
	}
	displayPriorities[7] = g_KaizoConfig_KaizoSplitsShowBestPossible - 1;

	float maxW = 0.0f;
	for(int i = 0; i < 8; i++)
	{
		if(!aBuf2[i][0][0] && !aBuf2[i][1][0])
			continue;
		if(displayWidths[i] == 0.0f)
			displayWidths[i] = aBuf2[i][1][0] ? TextRender()->TextWidth(FontSize * (i == 2 ? 6.0f / 5.0f : 1.0f), aBuf2[i][1], -1, -1.0f) : 0.0f;
		float w = ((g_KaizoConfig_KaizoSplitsLabels && aBuf2[i][0][0]) ? TextRender()->TextWidth(g_KaizoConfig_KaizoSplitsLabels == 1 ? FontSize * 4.0f / 5.0f : FontSize, aBuf2[i][0], -1, -1.0f) : 0.0f) + displayWidths[i];
		if(w > maxW)
			maxW = w;
		y += FontSize * (i == 2 ? 6.0f / 5.0f : 1.0f) + 2.0f;
	}
	if(y != 3.75f)
	{
		y += 1.75f;
		if(g_KaizoConfig_KaizoSplitsShowSplits >= 0)
		{
			maxWT += 5.0f;
			maxWD += 5.0f;
			if(maxWT + maxWD + s_TextWidthSplit + (SplitNameFormat == 0 ? s_TextWidthArrow + s_TextWidthSplit : 0) > maxW)
				maxW = maxWT + maxWD + s_TextWidthSplit + (SplitNameFormat == 0 ? s_TextWidthArrow + s_TextWidthSplit : 0);
		}
		m_SplitsLastWidth = maxW + 7.5f;
		float x = (m_Width - m_SplitsLastWidth) * (float)g_KaizoConfig_KaizoSplitsX / 100.0f;
		float h = y;
		y = 0.0f;
		if(g_KaizoConfig_KaizoSplitsX == 100)
		{
			y = 285.0f - h - 4; // 4 units distance to the next display;
			if(g_Config.m_ClShowhudSpectatorCount)
				y -= 14.0f + 4.0f;
			if(g_Config.m_ClShowhudPlayerPosition || g_Config.m_ClShowhudPlayerSpeed || g_Config.m_ClShowhudPlayerAngle)
			{
				y -= 4;
			}
			y -= GetMovementInformationBoxHeight();
			if(g_Config.m_ClShowhudScore)
			{
				y -= 56.0f; //GetScoreHudHeight(); //+KZ i dont have GetScoreHudHeight for now, use default 56 value
			}
			if(g_Config.m_ClShowhudDummyActions && !(GameClient()->m_Snap.m_pGameInfoObj /* +KZ added this because could call this function without snap received */ && GameClient()->m_Snap.m_pGameInfoObj->m_GameStateFlags & GAMESTATEFLAG_GAMEOVER) && Client()->DummyConnected())
			{
				y = y - 29.0f - 4; // dummy actions height and padding
			}
		}
		Graphics()->DrawRect(x, y, m_SplitsLastWidth, h, ColorRGBA(0.0f, 0.0f, 0.0f, 0.4f), g_KaizoConfig_KaizoSplitsX == 0 ? IGraphics::CORNER_BR : g_KaizoConfig_KaizoSplitsX == 100 ? IGraphics::CORNER_L :
																						     IGraphics::CORNER_B,
			3.75f);
		y += 3.75f;

		for(int i = 0; i < 9; i++)
		{
			if(displayPriorities[0] == i && (aBuf2[0][0][0] || aBuf2[0][1][0]))
			{
				float w = TextRender()->TextWidth(FontSize, aBuf2[0][1], -1, -1.0f);
				TextRender()->Text(x + 3.75f + maxW - w, y, FontSize, aBuf2[0][1], -1.0f);
				y += FontSize + 2.0f;
			}
			if(g_KaizoConfig_KaizoSplitsShowSplits == i && splitCount > 0)
			{
				y += 1.0f;
				int startY = y;
				int LastTimeC = 0, LastTimeBR = 0;
				for(int j = 0; j < splitCount; j++)
				{
					int TimeC = GameClient()->m_Splits.CurrentSplitTime(displayedSplits[j]), TimeBR = GameClient()->m_Splits.BestRunSplitTime(displayedSplits[j]);
					if(TimeC < 0 && TimeBR < 0)
						continue;
					int TimeCRun = TimeC, TimeBRRun = TimeBR;
					if(g_KaizoConfig_KaizoSplitsDisplaySegmentTime)
					{
						if(TimeC >= 0)
						{
							TimeC -= LastTimeC;
							LastTimeC += TimeC;
						}
						TimeBR -= LastTimeBR;
						LastTimeBR += TimeBR;
					}
					int Time = (g_KaizoConfig_KaizoSplitsDisplaySegmentTime & 2) ? (TimeC >= 0 ? TimeC : TimeBR) : (TimeCRun >= 0 ? TimeCRun : TimeBRRun);
					str_time((int64_t)Time, ETimeFormat::DAYS, aBuf, sizeof(aBuf));
					str_format(&aBuf[strlen(aBuf)], sizeof(aBuf) - strlen(aBuf), ".%02i", Time % 100);
					float w = TextRender()->TextWidth(FontSize, aBuf, -1, -1.0f); //float w = Time >= 3600 * 24 * 100 * 100 ? s_TextWidth000D : Time >= 3600 * 24 * 10 * 100 ? s_TextWidth00D : Time >= 3600 * 24 * 100 ? s_TextWidth0D : Time >= 3600 * 100 ? s_TextWidthH : s_TextWidthM;
					if(displayedSplits[j] == currentSplit)
						TextRender()->TextColor(0.8f, 0.8f, 0.8f, 1.0f);
					else if(TimeC >= 0)
						TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
					else
						TextRender()->TextColor(0.5f, 0.5f, 0.5f, 1.0f);
					TextRender()->Text(x + 3.75f + maxW - w, y, FontSize, aBuf, -1.0f);
					if((displayedSplits[j] == currentSplit || TimeC >= 0) && TimeBR >= 0)
					{
						int Delta = (displayedSplits[j] == currentSplit ? (GameClient()->m_Splits.CurrentRunTime() - ((g_KaizoConfig_KaizoSplitsDisplaySegmentTime & 1) ? LastTimeBR - TimeBR : 0)) : ((g_KaizoConfig_KaizoSplitsDisplaySegmentTime & 1) ? TimeC : TimeCRun)) - ((g_KaizoConfig_KaizoSplitsDisplaySegmentTime & 1) ? TimeBR : TimeBRRun); //displayedSplits[j] == currentSplit ? GameClient()->m_Splits.CurrentSplitCheck() : GameClient()->m_Splits.SplitCheck(displayedSplits[j]);
						int ADelta = Delta < 0 ? -Delta : Delta;
						if(displayedSplits[j] != currentSplit || g_KaizoConfig_KaizoSplitsMaximumCurrentCheck < 0 || (g_KaizoConfig_KaizoSplitsMaximumCurrentCheck > 0 && ADelta <= g_KaizoConfig_KaizoSplitsMaximumCurrentCheck * 100))
						{
							str_time((int64_t)ADelta, ETimeFormat::HOURS_CENTISECS, &aBuf[1], sizeof(aBuf) - sizeof(aBuf[0]));
							aBuf[0] = Delta < 0 ? '-' : '+';
							w = TextRender()->TextWidth(FontSize, aBuf, -1, -1.0f); //w = ADelta >= 3600 * 1000 * 100 ? s_TextWidthDelta0000H : ADelta >= 3600 * 100 * 100 ? s_TextWidthDelta000H : ADelta >= 3600 * 100 ? s_TextWidthDelta00H : ADelta >= 60 * 100 ? s_TextWidthDeltaM : s_TextWidthDeltaS;
							if(displayedSplits[j] == currentSplit)
								TextRender()->TextColor(0.5f, 0.5f, 0.5f, 1.0f);
							else if(GameClient()->m_Splits.SplitIsGold(displayedSplits[j]))
								TextRender()->TextColor(1.0f, 1.0f, 0.5f, 1.0f);
							else if(Delta == 0)
								TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
							else if(Delta > 0)
								TextRender()->TextColor(1.0f, 0.5f, 0.5f, 1.0f);
							else
								TextRender()->TextColor(0.5f, 1.0f, 0.5f, 1.0f);
							TextRender()->Text(x + 3.75f + maxW - maxWT - w, y, FontSize, aBuf, -1.0f);
						}
					}
					if(displayedSplits[j] == currentSplit)
						TextRender()->TextColor(0.8f, 0.8f, 0.8f, 1.0f);
					else if(TimeC >= 0)
						TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
					else
						TextRender()->TextColor(0.5f, 0.5f, 0.5f, 1.0f);
					if(SplitNameFormat == 0)
					{
						GameClient()->m_Splits.SplitName(displayedSplits[j], aBuf, sizeof(aBuf));
						w = TextRender()->TextWidth(FontSize, aBuf, -1, -1.0f);
						TextRender()->Text(x + 3.75f + maxW - maxWT - maxWD - w, y, FontSize, aBuf, -1.0f);
						TextRender()->Text(x + 3.75f + maxW - maxWT - maxWD - s_TextWidthSplit - s_TextWidthArrow, y, FontSize, "→", -1.0f);
						GameClient()->m_Splits.SplitName(GameClient()->m_Splits.PrevSplitId(displayedSplits[j]), aBuf, sizeof(aBuf));
						w = TextRender()->TextWidth(FontSize, aBuf, -1, -1.0f);
						TextRender()->Text(x + 3.75f + maxW - maxWT - maxWD - s_TextWidthSplit - s_TextWidthArrow - w, y, FontSize, aBuf, -1.0f);
					}
					else
					{
						GameClient()->m_Splits.SplitName(SplitNameFormat == 2 ? displayedSplits[j] : GameClient()->m_Splits.PrevSplitId(displayedSplits[j]), aBuf, sizeof(aBuf));
						w = TextRender()->TextWidth(FontSize, aBuf, -1, -1.0f);
						TextRender()->Text(x + 3.75f + maxW - maxWT - maxWD - w, y, FontSize, aBuf, -1.0f);
					}
					y += FontSize + 2.0f;
				}
				if(GameClient()->m_Splits.PrevSplitId(displayedSplits[0]) >= 0)
					Graphics()->DrawRect(x + 3.75f, startY - 2.0f + 1.0f * 0.5f, maxW, 1.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.4f), IGraphics::CORNER_ALL, 1.0f * 0.5f);
				if(displayedSplits[splitCount - 1] != finalSplit)
					Graphics()->DrawRect(x + 3.75f, y - 2.0f + 1.0f * 0.5f, maxW, 1.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.4f), IGraphics::CORNER_ALL, 1.0f * 0.5f);
				if(splitCount >= 2 && displayedSplits[splitCount - 1] == finalSplit && displayedSplits[splitCount - 2] != GameClient()->m_Splits.PrevSplitId(finalSplit))
					Graphics()->DrawRect(x + 3.75f, y - FontSize - 2.0f - 2.0f + 1.0f * 0.5f, maxW, 1.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.4f), IGraphics::CORNER_ALL, 1.0f * 0.5f);
			}
			for(int j = 1; j < 8; j++)
			{
				if(displayPriorities[j] == i && (aBuf2[j][0][0] || aBuf2[j][1][0]))
				{
					if(g_KaizoConfig_KaizoSplitsLabels && aBuf2[j][0][0])
					{
						TextRender()->TextColor(0.8f, 0.8f, 0.8f, 1.0f);
						TextRender()->Text(x + 3.75f, y + (FontSize / 5.0f) / 2.0f, g_KaizoConfig_KaizoSplitsLabels == 1 ? FontSize * 4.0f / 5.0f : FontSize, aBuf2[j][0], -1.0f);
					}
					if(aBuf2[j][1][0])
					{
						TextRender()->TextColor(displayColors[j].WithAlpha(1.0f));
						TextRender()->Text(x + 3.75f + maxW - displayWidths[j], y, FontSize * (j == 2 ? 6.0f / 5.0f : 1.0f), aBuf2[j][1], -1.0f);
					}
					y += FontSize * (j == 2 ? 6.0f / 5.0f : 1.0f) + 2.0f;
				}
			}
		}
	}
	else
		m_SplitsLastWidth = 0.0f;
	TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
}

