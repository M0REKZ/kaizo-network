/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#ifndef GAME_SERVER_GAMEMODES_KZ_BASE_KZ_H
#define GAME_SERVER_GAMEMODES_KZ_BASE_KZ_H

#include <game/server/gamemodes/DDRace.h>

class CGameControllerBaseKZ : public CGameControllerDDRace
{
public:
	CGameControllerBaseKZ(class CGameContext *pGameServer);
	~CGameControllerBaseKZ();

	void HandleCharacterTiles(class CCharacter *pChr, int MapIndex) override;
	void SetArmorProgress(CCharacter *pCharacter, int Progress) override;
	void OnPlayerConnect(class CPlayer *pPlayer) override;
	void OnPlayerDisconnect(class CPlayer *pPlayer, const char *pReason) override;
	void OnReset() override;
	void Tick() override;
	void DoTeamChange(class CPlayer *pPlayer, int Team, bool DoChatMsg = true) override;

	//+KZ

	virtual int DoWinCheck();

	void EndMatch(int Ticks);
	virtual bool IsFriendlyFire(int ClientID1, int ClientID2) override;
};
#endif // GAME_SERVER_GAMEMODES_DDRACE_H
