//+KZ has code from character.cpp
#include "character.h"

#include "laser.h"
#include "pickup.h"
#include "projectile.h"

#include <antibot/antibot_data.h>

#include <base/log.h>
#include <base/vmath.h>

#include <engine/antibot.h>
#include <engine/shared/config.h>

#include <generated/protocol.h>
#include <generated/server_data.h>

#include <game/mapitems.h>
#include <game/version.h>

#include <game/params_kz.h> //+KZ

#include <game/server/gamecontext.h>
#include <game/server/gamecontroller.h>
#include <game/server/player.h>
#include <game/server/score.h>
#include <game/server/teams.h>
#include <game/team_state.h>

#include <game/server/entities/kz/portal_projectile.h>
#include <game/server/entities/kz/portal_laser.h>
#include <game/server/entities/kz/portal.h>

void CCharacter::HandleSubTickStartFinish()
{
    //static int LastCheckedTick = -1;

    int Length = (int)length(m_Core.m_Vel);

    vec2 CheckingPos;
    vec2 Fraction = (m_Pos - m_PrevPos)/Length;
    int MapIndex = 0;
    int GameIndex = 0;
    int FrontIndex = 0;

    for(int i = 0; i < Length; i++)
    {
        CheckingPos = m_PrevPos + (Fraction * i);

        MapIndex = Collision()->GetMapIndex(CheckingPos);

        GameIndex = Collision()->GetTileIndex(MapIndex);
        FrontIndex = Collision()->GetFrontTileIndex(MapIndex);

        if(GameIndex == TILE_START || FrontIndex == TILE_START)
        {
            m_StartSubTick = i;
            m_StartDivisor = Length;
            m_StartedTickKZ = Server()->Tick();
        }

        if(m_StartSubTick >= 0 && (GameIndex == TILE_FINISH || FrontIndex == TILE_FINISH))
        {
            m_FinishSubTick = i;
            m_FinishDivisor = Length;
            m_FinishedTickKZ = Server()->Tick();
            break;
        }
    }

    //LastCheckedTick = Server()->Tick();
}