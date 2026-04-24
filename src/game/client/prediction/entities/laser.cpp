/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "laser.h"

#include "character.h"

#include <engine/shared/config.h>

#include <generated/protocol.h>

#include <game/client/laser_data.h>
#include <game/collision.h>
#include <game/mapitems.h>

CLaser::CLaser(CGameWorld *pGameWorld, vec2 Pos, vec2 Direction, float StartEnergy, int Owner, int Type, SKZLaserParams *pParams) :
	CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
	m_Pos = Pos;
	m_Owner = Owner;
	m_Energy = StartEnergy;
	if(pGameWorld->m_WorldConfig.m_IsFNG && m_Energy < 10.f)
		m_Energy = 800.0f;
	m_Dir = Direction;
	m_Bounces = 0;
	m_EvalTick = 0;
	m_Type = Type;
	m_ZeroEnergyBounceInLastTick = false;
	m_TuneZone = GameWorld()->m_WorldConfig.m_UseTuneZones ? Collision()->IsTune(Collision()->GetMapIndex(m_Pos)) : 0;

	//+KZ
	if(pParams)
	{
		m_IsRecoverJump = pParams->m_IsRecoverJump;
	}

	GameWorld()->InsertEntity(this);
	DoBounce();
}

bool CLaser::HitCharacter(vec2 From, vec2 To)
{
	static const vec2 StackedLaserShotgunBugSpeed = vec2(-2147483648.0f, -2147483648.0f);
	vec2 At;
	CCharacter *pOwnerChar = GameWorld()->GetCharacterById(m_Owner);
	CCharacter *pHit;
	bool DontHitSelf = (g_Config.m_SvOldLaser || !GameWorld()->m_WorldConfig.m_IsDDRace) || (m_Bounces == 0);

	if(pOwnerChar ? (!pOwnerChar->LaserHitDisabled() && m_Type == WEAPON_LASER) || (!pOwnerChar->ShotgunHitDisabled() && m_Type == WEAPON_SHOTGUN) : g_Config.m_SvHit)
		pHit = GameWorld()->IntersectCharacter(m_Pos, To, 0.f, At, DontHitSelf ? pOwnerChar : nullptr, m_Owner);
	else
		pHit = GameWorld()->IntersectCharacter(m_Pos, To, 0.f, At, DontHitSelf ? pOwnerChar : nullptr, m_Owner, pOwnerChar);

	if(!pHit || (pHit == pOwnerChar && g_Config.m_SvOldLaser) || (pHit != pOwnerChar && pOwnerChar ? (pOwnerChar->LaserHitDisabled() && m_Type == WEAPON_LASER) || (pOwnerChar->ShotgunHitDisabled() && m_Type == WEAPON_SHOTGUN) : !g_Config.m_SvHit))
		return false;
	m_From = From;
	m_Pos = At;
	m_Energy = -1;
	m_pLaserLastHit = pHit; //DuckDDNet addition
	if(m_Type == WEAPON_SHOTGUN)
	{
		float Strength = TuningList()[m_TuneZone].m_ShotgunStrength;

		const vec2 &HitPos = pHit->Core()->m_Pos;
		if(!g_Config.m_SvOldLaser)
		{
			if(m_PrevPos != HitPos)
			{
				pHit->AddVelocity(normalize(m_PrevPos - HitPos) * Strength);
			}
			else
			{
				pHit->SetRawVelocity(StackedLaserShotgunBugSpeed);
			}
		}
		else if(g_Config.m_SvOldLaser && pOwnerChar)
		{
			if(pOwnerChar->Core()->m_Pos != HitPos)
			{
				pHit->AddVelocity(normalize(pOwnerChar->Core()->m_Pos - HitPos) * Strength);
			}
			else
			{
				pHit->SetRawVelocity(StackedLaserShotgunBugSpeed);
			}
		}
		else
		{
			// Re-apply move restrictions as a part of 'shotgun bug' reproduction
			pHit->ApplyMoveRestrictions();
		}
	}
	else if(m_Type == WEAPON_LASER)
	{
		pHit->Unfreeze();
	}

	//+KZ Recover jump

	if(m_IsRecoverJump)
	{
		pHit->GetCoreKZ().m_Jumped = 0;
		pHit->GetCoreKZ().m_JumpedTotal = 0;
	}

	return true;
}

void CLaser::DoBounce()
{
	// KZ
	CCharacter *pOwnerChar = GameWorld()->GetCharacterById(m_Owner); 
	CCharacterCore *pOwnerCore = nullptr;

	if(pOwnerChar)
	{
		pOwnerCore = (CCharacterCore *)pOwnerChar->Core();
	}
	// End KZ

	m_EvalTick = GameWorld()->GameTick();

	if(m_Energy < 0)
	{
		m_MarkedForDestroy = true;
		return;
	}
	m_PrevPos = m_Pos;
	vec2 Coltile;

	int Res;
	vec2 To = m_Pos + m_Dir * m_Energy;

	//+KZ
	SKZColTeleWeaponParams ParamsKZ(&GameWorld()->m_Core);
	ParamsKZ.From = m_Pos;
	ParamsKZ.To = To;
	ParamsKZ.Type = m_Type;
	ParamsKZ.OwnerId = m_Owner;
	ParamsKZ.BounceNum = m_Bounces;
	SKZColCharCoreParams ParamsKZ2(&GameWorld()->m_Core);
	ParamsKZ.pCharCoreParams = &ParamsKZ2;
	ParamsKZ2.pCore = pOwnerCore;

	SKZQuadData *pQuadData = nullptr;
	vec2 QuadColPos;
	vec2 LineStart;

	if(Collision()->m_IsKaizoServer && g_Config.m_SvGoresQuadsEnable) // this is only for Kaizo Network
		pQuadData = Collision()->IntersectQuadTeleWeapon(&GameWorld()->m_Core ,m_Pos ,To, &QuadColPos, &LineStart);
	Res = Collision()->IntersectLineTeleWeapon(m_Pos, To, &Coltile, &To, nullptr, &ParamsKZ); //+KZ added ParamsKZ

	bool quadbounce = false;

	if(pQuadData && Res)
	{
		if(distance(m_Pos, QuadColPos) < distance(m_Pos, To))
		{
			quadbounce = true;
		}
	}
	else if(pQuadData)
	{
		quadbounce = true;
	}

	if(quadbounce)
	{
		m_KZQuadBounces++;
		if(m_KZQuadBounces > 50)
		{
			//something is wrong, kill the laser before he can kill us
			//(Kaizo Network infinite laser bouncing in quads)
			m_MarkedForDestroy = true;
			return;
		}

		int Index = Collision()->QuadTypeToTileId(pQuadData);

		if(Index == -1) //Kaizo-Insta Quad
		{
			Index = pQuadData->m_pQuad->m_ColorEnvOffset;
		}

		//bool WasTele = false;

		if(pQuadData->m_pQuad && (g_Config.m_SvOldTeleportWeapons ? (Index == TILE_TELEIN) : (Index == TILE_TELEINWEAPON)) && !Collision()->TeleOuts(pQuadData->m_pQuad->m_aColors[0].r - 1).empty())
		{
			//WasTele = true;
			m_MarkedForDestroy = true;
		}

		Res = 0;
		vec2 OldDir = m_Dir;
		m_Dir = normalize(Collision()->ReflexLineOnLine(m_Pos, QuadColPos, LineStart));
		//if(WasTele)
		//	m_Dir = OldDir;
		m_From = m_Pos;
		m_Pos = QuadColPos + normalize(m_Pos - QuadColPos);

		const float Distance = distance(m_From, m_Pos);
		// Prevent infinite bounces
		if(Distance == 0.0f && m_ZeroEnergyBounceInLastTick)
		{
			m_Energy = -1;
		}
		/*else if(!m_TuneZone)
		{
			m_Energy -= Distance + GetTuning(0)->m_LaserBounceCost;
		}*/ //duplicated code
		else
		{
			m_Energy -= Distance + GetTuning(m_TuneZone)->m_LaserBounceCost;
		}

		int BounceNum = GetTuning(m_TuneZone)->m_LaserBounceNum;
		
		if(m_Bounces > BounceNum)
			m_Energy = -1;

		m_ZeroEnergyBounceInLastTick = Distance == 0.0f;

		GameWorld()->CreatePredictedSound(m_Pos, SOUND_LASER_BOUNCE, m_Id);
		return;
	}

	if(m_Bounces != ParamsKZ.BounceNum)
	{
		m_Bounces = ParamsKZ.BounceNum;

		int BounceNum = GetTuning(m_TuneZone)->m_LaserBounceNum;
		if(m_Bounces > BounceNum)
		{
			m_Energy = -1;
			m_From = m_Pos;
			m_Pos = To;
		}
		GameWorld()->CreatePredictedSound(m_Pos, SOUND_LASER_BOUNCE, m_Id);
		return;
	}

	if(Res)
	{
		if(!HitCharacter(m_Pos, To))
		{
			// intersected
			m_From = m_Pos;
			m_Pos = To;

			vec2 TempPos = m_Pos;
			vec2 TempDir = m_Dir * 4.0f;

			int f = 0;
			if(Res == -1)
			{
				f = Collision()->GetTile(round_to_int(Coltile.x), round_to_int(Coltile.y));
				Collision()->SetCollisionAt(round_to_int(Coltile.x), round_to_int(Coltile.y), TILE_SOLID);
			}
			Collision()->MovePoint(&TempPos, &TempDir, 1.0f, nullptr, &ParamsKZ2); //+KZ modified
			if(Res == -1)
			{
				Collision()->SetCollisionAt(round_to_int(Coltile.x), round_to_int(Coltile.y), f);
			}
			m_Pos = TempPos;
			m_Dir = normalize(TempDir);

			const float Distance = distance(m_From, m_Pos);
			// Prevent infinite bounces
			if(Distance == 0.0f && m_ZeroEnergyBounceInLastTick)
			{
				m_Energy = -1;
			}
			else
			{
				m_Energy -= Distance + GetTuning(m_TuneZone)->m_LaserBounceCost;
			}
			m_ZeroEnergyBounceInLastTick = Distance == 0.0f;

			m_Bounces++;

			int BounceNum = GetTuning(m_TuneZone)->m_LaserBounceNum;

			//From Pointer's TW+
			if(GameWorld()->m_WorldConfig.m_HasLaserJump && distance(m_From, m_Pos) < 96 && m_Bounces <= 1)
			{
				GameWorld()->CreateExplosion(To, m_Owner, 4, 0, pOwnerChar ? pOwnerChar->Team() : -1, CClientMask().set());
				m_Energy = -1;
			}

			if(m_Bounces > BounceNum)
				m_Energy = -1;

			GameWorld()->CreatePredictedSound(m_Pos, SOUND_LASER_BOUNCE, m_Id);
		}
	}
	else
	{
		if(!HitCharacter(m_Pos, To))
		{
			m_From = m_Pos;
			m_Pos = To;
			m_Energy = -1;
		}
	}
}

void CLaser::Tick()
{
	float Delay = GetTuning(m_TuneZone)->m_LaserBounceDelay;

	if(GameWorld()->m_WorldConfig.m_IsVanilla) // predict old physics on vanilla 0.6 servers
	{
		if(GameWorld()->GameTick() > m_EvalTick + (GameWorld()->GameTickSpeed() * Delay / 1000.0f))
			DoBounce();
	}
	else
	{
		if((GameWorld()->GameTick() - m_EvalTick) > (GameWorld()->GameTickSpeed() * Delay / 1000.0f))
			DoBounce();
	}
}

CLaser::CLaser(CGameWorld *pGameWorld, int Id, CLaserData *pLaser) :
	CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
	m_Pos = pLaser->m_To;
	m_From = pLaser->m_From;
	m_EvalTick = pLaser->m_StartTick;
	m_TuneZone = GameWorld()->m_WorldConfig.m_UseTuneZones ? Collision()->IsTune(Collision()->GetMapIndex(m_Pos)) : 0;
	m_Owner = -2;
	m_Energy = GetTuning(m_TuneZone)->m_LaserReach;
	if(pGameWorld->m_WorldConfig.m_IsFNG && m_Energy < 10.f)
		m_Energy = 800.0f;

	m_Dir = m_Pos - m_From;
	if(length(m_Pos - m_From) > 0.001f)
		m_Dir = normalize(m_Dir);
	else
		m_Energy = 0;
	m_Type = pLaser->m_Type == LASERTYPE_SHOTGUN ? WEAPON_SHOTGUN : WEAPON_LASER;
	m_PrevPos = m_From;
	m_Id = Id;
}

bool CLaser::Match(CLaser *pLaser)
{
	if(pLaser->m_EvalTick != m_EvalTick)
		return false;
	if(distance(pLaser->m_From, m_From) > 2.f)
		return false;
	const vec2 ThisDiff = m_Pos - m_From;
	const vec2 OtherDiff = pLaser->m_Pos - pLaser->m_From;
	const float DirError = distance(normalize(OtherDiff) * length(ThisDiff), ThisDiff);
	return DirError <= 2.f;
}

CLaserData CLaser::GetData() const
{
	CLaserData Result;
	Result.m_From.x = m_From.x;
	Result.m_From.y = m_From.y;
	Result.m_To.x = m_Pos.x;
	Result.m_To.y = m_Pos.y;
	Result.m_StartTick = m_EvalTick;
	Result.m_ExtraInfo = true;
	Result.m_Owner = m_Owner;
	Result.m_Type = m_Type == WEAPON_SHOTGUN ? LASERTYPE_SHOTGUN : LASERTYPE_RIFLE;
	Result.m_Subtype = -1;
	Result.m_TuneZone = m_TuneZone;
	Result.m_SwitchNumber = m_Number;
	return Result;
}
