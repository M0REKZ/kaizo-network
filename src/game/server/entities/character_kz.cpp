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

    int Length = (int)length(m_PrevVelKZ);

    vec2 CheckingPos;
    vec2 Fraction = (m_Pos - m_PrevPos)/Length;
    int MapIndex = 0;
    int GameIndex = 0;
    int FrontIndex = 0;
    int MapCorners[4] = {0,0,0,0};
    int GameCorners[4] = {0,0,0,0};
    int FrontCorners[4] = {0,0,0,0};

    bool sametick = false;

    for(int i = 0; i <= Length; i++)
    {
        if(i == Length) //final pos
            CheckingPos = m_Pos;
        else
            CheckingPos = m_PrevPos + (Fraction * i);

        MapIndex = Collision()->GetMapIndex(CheckingPos);

        MapCorners[0] = Collision()->GetMapIndex(CheckingPos - vec2(GetProximityRadius()/3,GetProximityRadius()/3));
        MapCorners[1] = Collision()->GetMapIndex(CheckingPos + vec2(GetProximityRadius()/3,GetProximityRadius()/3));
        MapCorners[2] = Collision()->GetMapIndex(vec2(CheckingPos.x - (GetProximityRadius()/3), CheckingPos.y + (GetProximityRadius()/3)));
        MapCorners[3] = Collision()->GetMapIndex(vec2(CheckingPos.x + (GetProximityRadius()/3), CheckingPos.x - (GetProximityRadius()/3)));

        GameIndex = Collision()->GetTileIndex(MapIndex);
        FrontIndex = Collision()->GetFrontTileIndex(MapIndex);

        for(int j = 0; j < 4; j++)
        {
            GameCorners[j] = Collision()->GetTileIndex(GameCorners[j]);
            FrontCorners[j] = Collision()->GetFrontTileIndex(GameCorners[j]);
        }

        if(GameIndex == TILE_START || FrontIndex == TILE_START ||
            GameCorners[0] == TILE_START || GameCorners[1] == TILE_START || GameCorners[2] == TILE_START || GameCorners[3] == TILE_START ||
            FrontCorners[0] == TILE_START || FrontCorners[1] == TILE_START || FrontCorners[2] == TILE_START || FrontCorners[3] == TILE_START)
        {
            m_StartSubTick = i;
            m_StartDivisor = Length;
            m_StartedTickKZ = Server()->Tick();
            sametick = true;
        }

        if(m_FinishSubTick < 0 && m_StartSubTick >= 0 && (GameIndex == TILE_FINISH || FrontIndex == TILE_FINISH ||
            GameCorners[0] == TILE_FINISH || GameCorners[1] == TILE_FINISH || GameCorners[2] == TILE_FINISH || GameCorners[3] == TILE_FINISH ||
            FrontCorners[0] == TILE_FINISH || FrontCorners[1] == TILE_FINISH || FrontCorners[2] == TILE_FINISH || FrontCorners[3] == TILE_FINISH))
        {
            m_FinishSubTick = i;
            m_FinishDivisor = Length;
            m_FinishedTickKZ = Server()->Tick();
            if(sametick)
                printf("OMGGG\n");
            break;
        }
    }

    //LastCheckedTick = Server()->Tick();
}