#include <engine/shared/config.h>

#include <engine/shared/protocolglue.h>
#include <game/generated/protocol.h>
#include <game/mapitems.h>
#include <game/server/score.h>
#include <game/teamscore.h>

#include "gamecontext.h"
#include "gamecontroller.h"
#include "player.h"

#include "entities/character.h"
#include "entities/door.h"
#include "entities/dragger.h"
#include "entities/gun.h"
#include "entities/light.h"
#include "entities/pickup.h"
#include "entities/projectile.h"

bool IGameController::HasEnoughPlayers() const
{
	int Red = 0;
	int Blue = 0;

	for(auto &pPlayer : GameServer()->m_apPlayers)
	{
		if(pPlayer && pPlayer->GetTeam() == TEAM_RED)
			Red++;
		else if(pPlayer && pPlayer->GetTeam() == TEAM_BLUE)
			Blue++;
	}

	return (IsTeamPlay() && Red > 0 && Blue > 0) || (!IsTeamPlay() && Red > 1);
}