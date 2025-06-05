// (c) +KZ

#ifndef GAME_SERVER_GAMEMODES_KZ_KZ_H
#define GAME_SERVER_GAMEMODES_KZ_KZ_H

#include <game/server/gamemodes/DDRace.h>
#include <game/server/entities/kz/flag.h>

class CGameControllerKZ : public CGameControllerDDRace
{
public:
	CGameControllerKZ(class CGameContext *pGameServer);
	~CGameControllerKZ();

	CScore *Score();

	void HandleCharacterTiles(class CCharacter *pChr, int MapIndex) override;
	void SetArmorProgress(CCharacter *pCharacter, int Progress) override;

	void OnPlayerConnect(class CPlayer *pPlayer) override;
	void OnPlayerDisconnect(class CPlayer *pPlayer, const char *pReason) override;

	void OnCharacterSpawn(class CCharacter *pChr) override;

	void OnReset() override;

	void Tick() override;

	void DoTeamChange(class CPlayer *pPlayer, int Team, bool DoChatMsg = true) override;

	//+KZ

	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) override;
	bool OnEntityKZ(int Index, int x, int y, int Layer, int Flags, bool Initial, unsigned char Number = 0, int64_t Value1 = 0, int64_t Value2 = 0, int64_t Value3 = 0) override;
	void OnNewRecordKZ(int ClientId, float Time, float PrevTime) override;

	bool OnEntity(int Index, int x, int y, int Layer, int Flags, bool Initial, int Number = 0) override;
	virtual void Snap(int SnappingClient) override;

	class CFlag *m_apFlags[2];

	int m_flagstand_temp_i_0; //+KZ from tw_plus
	int m_flagstand_temp_i_1; //same

	void FlagTick();
	void DoCrown();
};
#endif // GAME_SERVER_GAMEMODES_DDRACE_H