/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_COLLISION_H
#define GAME_COLLISION_H

#include <base/vmath.h>
#include <engine/shared/protocol.h>

#include <map>
#include <vector>

#include <game/mapitems.h> // KZ
#include <game/gamecore.h> // KZ
#include <game/params_kz.h> // KZ

class CTile;
class CLayers;
class CTeleTile;
class CSpeedupTile;
class CSwitchTile;
class CTuneTile;
class CDoorTile;

enum
{
	CANTMOVE_LEFT = 1 << 0,
	CANTMOVE_RIGHT = 1 << 1,
	CANTMOVE_UP = 1 << 2,
	CANTMOVE_DOWN = 1 << 3,
};

vec2 ClampVel(int MoveRestriction, vec2 Vel);

typedef bool (*CALLBACK_SWITCHACTIVE)(int Number, void *pUser);
struct CAntibotMapData;

class CCollision
{
public:
	CCollision();
	~CCollision();

	// KZ

	int CheckPointForCore(float x, float y, SKZColCharCoreParams *pCharCoreParams) const;
	int CheckPointForProjectile(vec2 Pos, SKZColProjectileParams *pProjectileParams) const;
	int CheckPointForLaser(vec2 Pos, SKZColLaserParams *pLaserParams) const;
	int FastIntersectLine(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision, SKZColCharCoreParams *pCharCoreParams) const;
	int FastIntersectLinePortalLaser(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision, CKZTile **pKZTile = nullptr, int *pTeleNr = nullptr, SKZColCharCoreParams *pCharCoreParams = nullptr) const;
	bool DDNetLayerExists(int Layer);
	CPortalCore *IntersectCharacterWithPortal(vec2 Pos, CCharacterCore *pCore)const;
	CCharacterCore *IntersectCharacterCore(vec2 Pos0, vec2 Pos1, float Radius, vec2 &NewPos, CCharacterCore *pThisOnly = nullptr) const;
	bool HandlePortalCollision(vec2 &InOutPos, vec2 &InOutVel, CCharacterCore *pCore) const;
	bool IsTeleportViable(vec2 Pos) const;
	bool TestBoxKZ(vec2 OrigPos, vec2 *pInOutPos, vec2 *pInOutVel, vec2 Size, float ElasticityX, float ElasticityY, bool *pGrounded = nullptr, CCharacterCore *pCore = nullptr) const;

	bool KZGameFound() const { return m_pKZGame != 0; }
	bool KZFrontFound() const { return m_pKZFront != 0; }

	int GetKZGameWidth() const { return m_KZGameWidth; }
	int GetKZGameHeight() const { return m_KZGameHeight; }
	int GetKZFrontWidth() const { return m_KZFrontWidth; }
	int GetKZFrontHeight() const { return m_KZFrontHeight; }

	CKZTile *GetKZGameTile(int Index) const;
	CKZTile *GetKZGameTile(int x, int y) const;
	CKZTile *GetKZGameTile(float x, float y) const;
	CKZTile *GetKZGameTile(vec2 Pos) const { return GetKZGameTile(Pos.x,Pos.y); }
	CKZTile *GetKZFrontTile(int Index) const;
	CKZTile *GetKZFrontTile(int x, int y) const;
	CKZTile *GetKZFrontTile(float x, float y) const;
	CKZTile *GetKZFrontTile(vec2 Pos) const { return GetKZFrontTile(Pos.x,Pos.y); }

	unsigned char GetKZGameTileIndex(float x, float y) const;
	unsigned char GetKZGameTileIndex(int x, int y) const;
	unsigned char GetKZGameTileIndex(vec2 Pos) const { return GetKZGameTileIndex(Pos.x,Pos.y); }
	unsigned char GetKZGameTileIndex(int Index) const;

	unsigned char GetKZFrontTileIndex(float x, float y) const;
	unsigned char GetKZFrontTileIndex(int x, int y) const;
	unsigned char GetKZFrontTileIndex(vec2 Pos) const { return GetKZFrontTileIndex(Pos.x,Pos.y); }
	unsigned char GetKZFrontTileIndex(int Index) const;

	// End KZ

	void Init(CLayers *pLayers);
	void Unload();
	void FillAntibot(CAntibotMapData *pMapData) const;

	bool CheckPoint(float x, float y, SKZColCharCoreParams *pCharCoreParams = nullptr) const { return IsSolid(round_to_int(x), round_to_int(y)) || CheckPointForCore(x, y, pCharCoreParams); } // KZ: modified
	bool CheckPoint(vec2 Pos, SKZColCharCoreParams *pCharCoreParams = nullptr) const { return CheckPoint(Pos.x, Pos.y, pCharCoreParams); } // KZ: modified
	int GetCollisionAt(float x, float y, SKZColCharCoreParams *pCharCoreParams = nullptr) const; // KZ: modified
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	int IntersectLine(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision, SKZColIntersectLineParams *pIntersectLineParams = nullptr) const; // KZ modified
	int IntersectLineTeleWeapon(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision, int *pTeleNr = nullptr, SKZColTeleWeaponParams *pTeleWeaponParams = nullptr) const; //KZ modified
	int IntersectLineTeleHook(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision, int *pTeleNr = nullptr, SKZColGenericParams *pGenericParams = nullptr) const; // KZ modified
	void MovePoint(vec2 *pInoutPos, vec2 *pInoutVel, float Elasticity, int *pBounces, SKZColCharCoreParams *pCharCoreParams = nullptr) const; // KZ modified
	void MoveBox(vec2 *pInoutPos, vec2 *pInoutVel, vec2 Size, vec2 Elasticity, bool *pGrounded = nullptr, SKZColCharCoreParams *pCharCoreParams = nullptr) const; // KZ modified
	bool TestBox(vec2 Pos, vec2 Size, SKZColCharCoreParams *pCharCoreParams = nullptr) const; // KZ modified

	// DDRace
	void SetCollisionAt(float x, float y, int Index);
	void SetDoorCollisionAt(float x, float y, int Type, int Flags, int Number);
	void GetDoorTile(int Index, CDoorTile *pDoorTile) const;
	int GetFrontCollisionAt(float x, float y) const { return GetFrontTile(round_to_int(x), round_to_int(y)); }
	int IntersectNoLaser(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision) const;
	int IntersectNoLaserNoWalls(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision) const;
	int IntersectAir(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision) const;
	int GetIndex(int x, int y) const;
	int GetIndex(vec2 PrevPos, vec2 Pos) const;
	int GetFrontIndex(int x, int y) const;

	int GetMoveRestrictions(CALLBACK_SWITCHACTIVE pfnSwitchActive, void *pUser, vec2 Pos, float Distance = 18.0f, int OverrideCenterTileIndex = -1) const;
	int GetMoveRestrictions(vec2 Pos, float Distance = 18.0f) const
	{
		return GetMoveRestrictions(nullptr, nullptr, Pos, Distance);
	}

	int GetTile(int x, int y) const;
	int GetFrontTile(int x, int y) const;
	int Entity(int x, int y, int Layer) const;
	int GetPureMapIndex(float x, float y) const;
	int GetPureMapIndex(vec2 Pos) const { return GetPureMapIndex(Pos.x, Pos.y); }
	std::vector<int> GetMapIndices(vec2 PrevPos, vec2 Pos, unsigned MaxIndices = 0) const;
	int GetMapIndex(vec2 Pos) const;
	bool TileExists(int Index) const;
	bool TileExistsNext(int Index) const;
	vec2 GetPos(int Index) const;
	int GetTileIndex(int Index) const;
	int GetFrontTileIndex(int Index) const;
	int GetTileFlags(int Index) const;
	int GetFrontTileFlags(int Index) const;
	int IsTeleport(int Index) const;
	int IsEvilTeleport(int Index) const;
	bool IsCheckTeleport(int Index) const;
	bool IsCheckEvilTeleport(int Index) const;
	int IsTeleportWeapon(int Index) const;
	int IsTeleportHook(int Index) const;
	int IsTeleCheckpoint(int Index) const;
	int IsSpeedup(int Index) const;
	int IsTune(int Index) const;
	void GetSpeedup(int Index, vec2 *pDir, int *pForce, int *pMaxSpeed, int *pType) const;
	int GetSwitchType(int Index) const;
	int GetSwitchNumber(int Index) const;
	int GetSwitchDelay(int Index) const;

	int IsSolid(int x, int y) const;
	bool IsThrough(int x, int y, int OffsetX, int OffsetY, vec2 Pos0, vec2 Pos1) const;
	bool IsHookBlocker(int x, int y, vec2 Pos0, vec2 Pos1) const;
	int IsWallJump(int Index) const;
	int IsNoLaser(int x, int y) const;
	int IsFrontNoLaser(int x, int y) const;

	int IsTimeCheckpoint(int Index) const;
	int IsFrontTimeCheckpoint(int Index) const;

	int MoverSpeed(int x, int y, vec2 *pSpeed) const;

	const CLayers *Layers() const { return m_pLayers; }
	const CTile *GameLayer() const { return m_pTiles; }
	const CTeleTile *TeleLayer() const { return m_pTele; }
	const CSpeedupTile *SpeedupLayer() const { return m_pSpeedup; }
	const CTile *FrontLayer() const { return m_pFront; }
	const CSwitchTile *SwitchLayer() const { return m_pSwitch; }
	const CTuneTile *TuneLayer() const { return m_pTune; }

	int m_HighestSwitchNumber;

	/**
	 * Index all teleporter types (in, out and checkpoints)
	 * as one consecutive list.
	 *
	 * @param Number is the teleporter number (one less than what is shown in game)
	 * @param Offset picks the n'th occurence of that teleporter in the map
	 *
	 * @return The coordinates of the teleporter in the map
	 *         or (-1, -1) if not found
	 */
	vec2 TeleAllGet(int Number, size_t Offset);

	/**
	 * @param Number is the teleporter number (one less than what is shown in game)
	 * @return The amount of occurences of that teleporter across all types (in, out, checkpoint)
	 */
	size_t TeleAllSize(int Number);

	const std::vector<vec2> &TeleIns(int Number) { return m_TeleIns[Number]; }
	const std::vector<vec2> &TeleOuts(int Number) { return m_TeleOuts[Number]; }
	const std::vector<vec2> &TeleCheckOuts(int Number) { return m_TeleCheckOuts[Number]; }
	const std::vector<vec2> &TeleOthers(int Number) { return m_TeleOthers[Number]; }

private:
	CLayers *m_pLayers;

	int m_Width;
	int m_Height;

	CTile *m_pTiles;
	CTeleTile *m_pTele;
	CSpeedupTile *m_pSpeedup;
	CTile *m_pFront;
	CSwitchTile *m_pSwitch;
	CTuneTile *m_pTune;
	CDoorTile *m_pDoor;

	// TILE_TELEIN
	std::map<int, std::vector<vec2>> m_TeleIns;
	// TILE_TELEOUT
	std::map<int, std::vector<vec2>> m_TeleOuts;
	// TILE_TELECHECKOUT
	std::map<int, std::vector<vec2>> m_TeleCheckOuts;
	// TILE_TELEINEVIL, TILE_TELECHECK, TILE_TELECHECKIN, TILE_TELECHECKINEVIL
	std::map<int, std::vector<vec2>> m_TeleOthers;

	// KZ
public:

	int m_KZGameWidth;
	int m_KZGameHeight;
	int m_KZFrontWidth;
	int m_KZFrontHeight;
	CKZTile *m_pKZGame;
	CKZTile *m_pKZFront;

	CWorldCore *m_pWorldCore;
	CTeamsCore *m_pTeamsCore;
};

void ThroughOffset(vec2 Pos0, vec2 Pos1, int *pOffsetX, int *pOffsetY);
#endif
