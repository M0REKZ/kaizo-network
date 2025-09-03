/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#ifndef GAME_SERVER_GAMEMODES_KZ_LMS_H
#define GAME_SERVER_GAMEMODES_KZ_LMS_H

#include <game/server/gamemodes/kz/dm.h>

class CGameControllerLMS : public CGameControllerDM
{
public:
	CGameControllerLMS(class CGameContext *pGameServer);
	~CGameControllerLMS();

	virtual void OnNewMatch() override;

	virtual int DoWinCheck() override;
	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) override;
	virtual void OnCharacterSpawn(class CCharacter *pChr) override;
	virtual bool OnEntity(int Index, int x, int y, int Layer, int Flags, bool Initial, int Number = 0) override;

};
#endif // GAME_SERVER_GAMEMODES_DDRACE_H
