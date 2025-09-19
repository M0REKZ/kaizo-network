// Copyright (C) Benjamín Gajardo (also known as +KZ)

#ifndef GAME_SERVER_ENTITIES_KZ_PORTAL_H
#define GAME_SERVER_ENTITIES_KZ_PORTAL_H

#include <game/server/entity.h>
#include <game/gamecore.h>

class CPortalKZ : public CEntity
{
public:

	CPortalKZ(CGameWorld *pGameWorld, vec2 StartPos, int Owner, bool Blue);
	~CPortalKZ();

	vec2 m_Pos2;
	int m_Owner;
	int m_Laser2ID;
	bool m_Blue;
	int m_OrigTeam;

	virtual void Reset() override;
	virtual void Tick() override;
	virtual void Snap(int SnappingClient) override;

	CPortalKZ* GetOtherPortal();
	bool IsTeleportViable(vec2 Pos);

private:

	CPortalCore *m_pCore = nullptr;

	bool CheckPosition(vec2 Pos);

};

#endif
