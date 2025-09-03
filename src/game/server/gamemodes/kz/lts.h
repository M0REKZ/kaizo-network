/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#ifndef GAME_SERVER_GAMEMODES_KZ_LTS_H
#define GAME_SERVER_GAMEMODES_KZ_LTS_H

#include <game/server/gamemodes/kz/lms.h>

class CGameControllerLTS : public CGameControllerLMS
{
public:
	CGameControllerLTS(class CGameContext *pGameServer);
	~CGameControllerLTS();

	virtual int DoWinCheck() override;
	virtual void OnNewMatch() override;
	virtual void Snap(int SnappingClient) override;
	virtual bool CanJoinTeam(int Team, int NotThisId, char *pErrorReason, int ErrorReasonSize) override;

	bool DoWinCheckMatch();
	int m_aTeamScore[2];

};
#endif // GAME_SERVER_GAMEMODES_DDRACE_H
