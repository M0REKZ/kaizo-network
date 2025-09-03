/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#ifndef GAME_SERVER_GAMEMODES_KZ_TDM_H
#define GAME_SERVER_GAMEMODES_KZ_TDM_H

#include <game/server/gamemodes/kz/base_pvp.h>

class CGameControllerTDM : public CGameControllerBasePvP
{
public:
	CGameControllerTDM(class CGameContext *pGameServer);
	~CGameControllerTDM();

	virtual int DoWinCheck() override;
	virtual void OnNewMatch() override;

	virtual void Snap(int SnappingClient) override;
	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) override;
	virtual bool CanJoinTeam(int Team, int NotThisId, char *pErrorReason, int ErrorReasonSize) override;

	int m_aTeamScore[2];

};
#endif // GAME_SERVER_GAMEMODES_DDRACE_H
