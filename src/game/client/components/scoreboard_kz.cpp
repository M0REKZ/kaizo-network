// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// RenderRespawnTimer is code from DuckInfClass Client wrapped as a function

#include "scoreboard.h"
#include <base/str.h>
#include <generated/protocol.h>
#include <game/client/gameclient.h>

int * pLinesUsedSpectatorKZ = nullptr;

void CScoreboard::RenderRespawnTimer(const CUIRect &Screen, CUIRect &Spectators, const float ScoreboardSmallWidth)
{
    if(!pLinesUsedSpectatorKZ)
        return;

    const CNetObj_RespawnTimer *pRespawnTimer = GameClient()->m_Snap.m_pRespawnTimer;
	if(pRespawnTimer)
	{
		CUIRect RespawnTimer = {(Screen.w - ScoreboardSmallWidth) / 2.0f, Spectators.y + *pLinesUsedSpectatorKZ*11.0f + 5.0f, ScoreboardSmallWidth, 20.0f};
		
		char aBuf[128];
		float TimeLeft = (float)pRespawnTimer->m_TicksLeft / (float)Client()->GameTickSpeed();
		if (TimeLeft > 0)
			str_format(aBuf, sizeof(aBuf), "Respawn in %.1fs", TimeLeft);
		else if (TimeLeft == 0)
			str_copy(aBuf, "You can respawn");
		else
			str_copy(aBuf, "You cannot respawn right now");
		Ui()->DoLabel(&RespawnTimer, aBuf, 12.0f, TEXTALIGN_MC);
	}
}
