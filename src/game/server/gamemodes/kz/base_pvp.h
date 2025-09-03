/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#ifndef GAME_SERVER_GAMEMODES_KZ_BASE_PVP_H
#define GAME_SERVER_GAMEMODES_KZ_BASE_PVP_H

#include <game/server/gamemodes/kz/base_kz.h>

class CGameControllerBasePvP : public CGameControllerBaseKZ
{
public:
	CGameControllerBasePvP(class CGameContext *pGameServer);
	~CGameControllerBasePvP();

	virtual bool OnCharacterTakeDamage(CCharacter *pChar, vec2 Force, int Dmg, int From, int Weapon) override;
	virtual bool CharacterFireWeapon(CCharacter *pChar) override;
	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) override;
	virtual bool OnEntity(int Index, int x, int y, int Layer, int Flags, bool Initial, int Number = 0) override;
	virtual void OnCharacterSpawn(class CCharacter *pChr) override;

	int m_InstagibWeapon = -1;

};
#endif // GAME_SERVER_GAMEMODES_DDRACE_H
