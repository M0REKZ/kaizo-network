/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#ifndef GAME_SERVER_GAMEMODES_KZ_CTF_H
#define GAME_SERVER_GAMEMODES_KZ_CTF_H

#include <game/server/gamemodes/kz/tdm.h>
#include <game/server/entities/kz/flag.h>

class CGameControllerCTF : public CGameControllerTDM
{
public:
	CGameControllerCTF(class CGameContext *pGameServer);
	~CGameControllerCTF();

	virtual int DoWinCheck() override;
	virtual void OnNewMatch() override;

	virtual void Tick() override;
	virtual void Snap(int SnappingClient) override;
	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) override;
	virtual bool OnEntity(int Index, int x, int y, int Layer, int Flags, bool Initial, int Number = 0) override;
	virtual void OnCharacterDestroy(const CCharacter *pChar) override;

	void HandleFlag();

	class CFlag *m_apFlags[2];

	int m_flagstand_temp_i_0; //+KZ from tw_plus
	int m_flagstand_temp_i_1; //same

};
#endif // GAME_SERVER_GAMEMODES_DDRACE_H
