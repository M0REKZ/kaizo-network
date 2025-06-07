/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_KZ_PICKUP_H
#define GAME_SERVER_ENTITIES_KZ_PICKUP_H

#include <game/server/entity.h>
#include <game/teamscore.h>

class CKZPickup : public CEntity
{
public:
	static const int ms_CollisionExtraSize = 6;

	CKZPickup(CGameWorld *pGameWorld, int Type, int SubType = 0, int Layer = 0, int Number = 0, int Flags = 0);
	CKZPickup(CGameWorld *pGameWorld, int Objtype, vec2 Pos = vec2(0, 0), int ProximityRadius = 0);
	virtual ~CKZPickup();

	void Reset() override;
	void Tick() override;
	void TickPaused() override;
	void Snap(int SnappingClient) override;

	int m_SpawnTickTeam[NUM_DDRACE_TEAMS];
	
	int m_Id2; //+KZ from Pointer tw+
	bool m_Dropped = false;
	int m_DropTick = 0;
	vec2 m_Vel = vec2(0,0);
	int m_ThisTeamOnly = -1;
	bool m_IsGrounded = false;
	int m_Ammo = -2;

	int Type() const { return m_Type; }
	int Subtype() const { return m_Subtype; }

	int GetSpawnTick(int Team = 0) { return m_SpawnTickTeam[Team]; } //+KZ

protected:
	int m_Type;
	int m_Subtype;

	// DDRace

	void Move();
	vec2 m_Core;
	int m_Flags;
};

#endif
