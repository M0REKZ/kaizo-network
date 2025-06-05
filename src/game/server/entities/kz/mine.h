/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_MINE_H
#define GAME_SERVER_ENTITIES_MINE_H

#include <game/server/entity.h>

class CMine : public CEntity
{
public:
	CMine(CGameWorld *pGameWorld, vec2 Pos, int Number);
	void Tick() override;
	void Snap(int SnappingClient) override;
	void Reset() override;
	
	//virtual int GetOwnerId() const override { return m_Owner; }
private:
	//int m_Owner;
	int m_RespawnTicks[NUM_DDRACE_TEAMS];
	
	// DDRace

	void Move();
	vec2 m_Core;
};

#endif
