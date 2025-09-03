/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#ifndef GAME_SERVER_GAMEMODES_KZ_DM_H
#define GAME_SERVER_GAMEMODES_KZ_DM_H

#include <game/server/gamemodes/kz/base_pvp.h>

class CGameControllerDM : public CGameControllerBasePvP
{
public:
	CGameControllerDM(class CGameContext *pGameServer);
	~CGameControllerDM();

	virtual int DoWinCheck() override;
	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) override;

};
#endif // GAME_SERVER_GAMEMODES_DDRACE_H
