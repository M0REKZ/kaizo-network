#ifndef GAME_SERVER_ENTITIES_KZ_PROJECTILE_KZ_H
#define GAME_SERVER_ENTITIES_KZ_PROJECTILE_KZ_H

#include <game/server/entity.h>

class CProjectileKZ : public CEntity
{
public:
	CProjectileKZ(
		CGameWorld *pGameWorld,
		int Owner,
		vec2 Pos,
		vec2 Dir,
        int Type,
        int EntType = CGameWorld::KZ_ENTTYPE_PROJECTILE_KZ);

    ~CProjectileKZ();

	void Reset() override;
	void Tick() override;
	void TickPaused() override;
	void Snap(int SnappingClient) override;

protected:

    vec2 m_Dir;
	int m_LifeSpan;
	int m_Owner;
	int m_StartTick;
    int m_SnapTick;
    int m_Type;
    int m_PrevTuneZone;
    float m_Curvature = 0.f;
    float m_Speed = 0.f;
    CCharacter * m_pInsideChar;
    CCharacter * m_pOwnerChar;

	// DDRace

    int m_Bouncing; //TODO +KZ
	int m_TuneZone;
    CCharacter *m_pSoloChar;

    virtual void Move();
    virtual int Collide(vec2 PrevPos, vec2 *pPreIntersectPos = nullptr, vec2 *pIntersectPos = nullptr, int *pTeleNr = nullptr);
    virtual void OnCollide(vec2 PrevPos, int TileIndex = 0, vec2 *pPreIntersectPos = nullptr, vec2 *pIntersectPos = nullptr, int *pTeleNr = nullptr);
    virtual void OnCharacterCollide(vec2 PrevPos, CCharacter* pChar, vec2 *pIntersectPos = nullptr);

    // Kaizo-Insta projectile rollback
	// for rollback players the projectile will be forwarded
	// to match the tick they sent the fire input
	//
	// on DDNet Antiping clients it will send m_OrigStartTick on Snap so it is
	// intended to play with DDNet grenade antiping, but the best would be to find
	// a way to bypass client antiping and show what the server really has
    int m_CollidedTick = -1;
	bool m_FirstTick;
    int m_OrigStartTick; // the tick when the projectile was created
    vec2 m_OrigPos; // the position when the projectile was created
    vec2 m_OrigDir; // the direction when the projectile was created
    int m_RollbackSnapTickDifference = 0; // the difference between the snap tick and the original start tick
	bool m_FirstSnap;
	int m_aParticleIds[3]; // particles to let know others a projectile got fired, in case it was forwarded too much
    bool m_Rollback = false;

public:

    vec2 m_SnapPos;
    vec2 m_SnapVel;
    int m_SoundImpact;

    //keep those functions for CProjectile compat:

    void SwapClients(int Client1, int Client2) override;
    void SetBouncing(int Value);
    vec2 GetPos(float Time);
	void FillInfo(CNetObj_Projectile *pProj);
	bool FillExtraInfoLegacy(CNetObj_DDRaceProjectile *pProj);
	void FillExtraInfo(CNetObj_DDNetProjectile *pProj);
	int GetOwnerId() const override { return m_Owner; }
    
};

#endif