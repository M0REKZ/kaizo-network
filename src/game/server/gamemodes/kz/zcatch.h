/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#ifndef GAME_SERVER_GAMEMODES_KZ_ZCATCH_H
#define GAME_SERVER_GAMEMODES_KZ_ZCATCH_H

#include <game/server/gamemodes/kz/lms.h>

class CGameController_zCatch : public CGameControllerLMS
{
public:
	CGameController_zCatch(class CGameContext *pGameServer);
	~CGameController_zCatch();

	virtual void OnNewMatch() override;

	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) override;
	virtual void OnCharacterSpawn(class CCharacter *pChr) override;
	virtual int DoWinCheck() override;


};
#endif // GAME_SERVER_GAMEMODES_DDRACE_H
