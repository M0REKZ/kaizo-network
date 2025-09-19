// Copyright (C) Benjamín Gajardo (also known as +KZ)

// Move() taken from DDNet and is not under +KZ copyright
// m_Core taken from DDNet and is not under +KZ copyright

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
	
private:
	int m_RespawnTicks[NUM_DDRACE_TEAMS];
	
	// DDRace

	void Move();
	vec2 m_Core;
};

#endif
