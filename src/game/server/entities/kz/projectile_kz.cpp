#include "projectile_kz.h"
#include <game/collision.h>
#include <game/mapitems.h>
#include <game/server/entities/character.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>

CProjectileKZ::CProjectileKZ(CGameWorld *pGameWorld, int Owner, vec2 Pos, vec2 Dir, int Type, int EntType) :
	CEntity(pGameWorld, EntType)
{
	m_OrigDir = m_SnapVel = m_Dir = normalize(Dir);
	m_OrigPos = m_SnapPos = m_Pos = Pos;
	m_Owner = Owner;
	m_Type = Type;

    m_SnapVel *= 50;
	
    m_pOwnerChar = m_pInsideChar = m_pSoloChar = GameServer()->GetPlayerChar(m_Owner);

    if(!(m_pSoloChar && m_pSoloChar->Core()->m_Solo))
        m_pSoloChar = nullptr;

	m_PrevTuneZone = -1;
	m_TuneZone = GameServer()->Collision()->IsTune(GameServer()->Collision()->GetMapIndex(m_Pos));

	switch(Type)
	{
	case WEAPON_GUN:
        if(m_TuneZone > 0)
		    m_LifeSpan = (int)(Server()->TickSpeed() * GetTuning(m_TuneZone)->m_GunLifetime);
        else
            m_LifeSpan = (int)(Server()->TickSpeed() * Tuning()->m_GunLifetime);
		break;
	case WEAPON_SHOTGUN:
		if(m_TuneZone > 0)
		    m_LifeSpan = (int)(Server()->TickSpeed() * GetTuning(m_TuneZone)->m_ShotgunLifetime);
        else
            m_LifeSpan = (int)(Server()->TickSpeed() * Tuning()->m_ShotgunLifetime);
		break;
	case WEAPON_GRENADE:
		if(m_TuneZone > 0)
		    m_LifeSpan = (int)(Server()->TickSpeed() * GetTuning(m_TuneZone)->m_GrenadeLifetime);
        else
            m_LifeSpan = (int)(Server()->TickSpeed() * Tuning()->m_GrenadeLifetime);
		break;
	default: // unknown weapon
		m_LifeSpan = 0;
		break;
	}

	m_OrigStartTick = m_StartTick = m_SnapTick = Server()->Tick();
	
	//rollback
	m_FirstTick = true;
	m_FirstSnap = true;

	for(int &ParticleId : m_aParticleIds)
	{
		ParticleId = Server()->SnapNewId();
	}

	GameWorld()->InsertEntity(this);
}

CProjectileKZ::~CProjectileKZ()
{
	for(int &ParticleId : m_aParticleIds)
	{
		Server()->SnapFreeId(ParticleId);
	}
}

void CProjectileKZ::Reset()
{
	m_MarkedForDestroy = true;
}

void CProjectileKZ::Tick()
{
	if(GameWorld()->m_Paused)
		return;

	vec2 CharacterIntersectPos;
    vec2 TileIntersectPos;
    vec2 TilePreIntersectPos;
    int TeleNr = 0;
	vec2 PrevPos;
	CCharacter* pChar = nullptr;
	int Collided = 0;
	m_CollidedTick = Server()->Tick();

	if(m_FirstTick && m_Owner >= 0 && m_Owner < MAX_CLIENTS && GameServer()->m_apPlayers[m_Owner] && GameServer()->m_apPlayers[m_Owner]->m_RollbackEnabled && GameServer()->m_apPlayers[m_Owner]->GetCharacter())
	{
		m_Rollback = true;
		int Tick = GameServer()->m_apPlayers[m_Owner]->m_LastAckedSnapshot;

		//Collide with wall and tee
		m_SnapTick = m_StartTick = m_CollidedTick = Tick;
		m_RollbackSnapTickDifference = Tick - m_OrigStartTick;
		for(; m_CollidedTick <= m_OrigStartTick; m_CollidedTick++)
		{

			if(m_pInsideChar && distance(m_Pos, m_pInsideChar->m_Pos) > 28.0f)
				m_pInsideChar = nullptr;

			if(m_LifeSpan > -1)
				m_LifeSpan--;

			PrevPos = m_Pos;

			Move();

			pChar = GameServer()->m_Rollback.IntersectCharacterOnTick(PrevPos, m_Pos, 0, CharacterIntersectPos, m_pInsideChar, m_Owner, m_pSoloChar, m_pOwnerChar, m_CollidedTick);
			Collided = Collide(PrevPos, &TilePreIntersectPos, &TileIntersectPos, &TeleNr);

			if(pChar || Collided)
				break;
		}
	}
	else
	{
		if(m_pInsideChar && distance(m_Pos,m_pInsideChar->m_Pos) > 28.0f)
			m_pInsideChar = nullptr;

		if(m_LifeSpan > -1)
			m_LifeSpan--;

		PrevPos = m_Pos;

		Move();

		pChar = GameWorld()->IntersectCharacter(PrevPos, m_Pos, 0, CharacterIntersectPos, m_pInsideChar, m_Owner, m_pSoloChar);
		Collided = Collide(PrevPos, &TilePreIntersectPos, &TileIntersectPos, &TeleNr);
	}

    if(pChar && Collided)
    {
        float lengthchar = distance(PrevPos,CharacterIntersectPos);
        float lengthtile = distance(PrevPos,TileIntersectPos);

        if(lengthtile < lengthchar)
        {
            OnCollide(PrevPos, Collided, &TilePreIntersectPos, &TileIntersectPos, &TeleNr);
        }
        else
        {
            OnCharacterCollide(PrevPos, pChar, &CharacterIntersectPos);
        }
    }
    else if(pChar)
    {
        OnCharacterCollide(PrevPos, pChar, &CharacterIntersectPos);
    }
    else if(Collided)
    {
        OnCollide(PrevPos, Collided, &TilePreIntersectPos, &TileIntersectPos, &TeleNr);
    }

    if(m_LifeSpan == -1)
        Reset();

	m_FirstTick = false;
}

void CProjectileKZ::TickPaused()
{
	m_SnapTick++;
}

void CProjectileKZ::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient, m_Pos))
		return;

	//Kaizo-Insta projectile rollback particles
	if(m_FirstSnap && m_Owner >= 0 && m_Owner < MAX_CLIENTS && GameServer()->m_apPlayers[m_Owner] && GameServer()->m_apPlayers[m_Owner]->m_RollbackEnabled)
	{
		float Curvature = 0;
		float Speed = 0;

		switch(m_Type)
		{
		case WEAPON_GRENADE:
			if(!m_TuneZone)
			{
				Curvature = Tuning()->m_GrenadeCurvature;
				Speed = Tuning()->m_GrenadeSpeed;
			}
			else
			{
				Curvature = TuningList()[m_TuneZone].m_GrenadeCurvature;
				Speed = TuningList()[m_TuneZone].m_GrenadeSpeed;
			}

			break;

		case WEAPON_SHOTGUN:
			if(!m_TuneZone)
			{
				Curvature = Tuning()->m_ShotgunCurvature;
				Speed = Tuning()->m_ShotgunSpeed;
			}
			else
			{
				Curvature = TuningList()[m_TuneZone].m_ShotgunCurvature;
				Speed = TuningList()[m_TuneZone].m_ShotgunSpeed;
			}

			break;

		case WEAPON_GUN:
			if(!m_TuneZone)
			{
				Curvature = Tuning()->m_GunCurvature;
				Speed = Tuning()->m_GunSpeed;
			}
			else
			{
				Curvature = TuningList()[m_TuneZone].m_GunCurvature;
				Speed = TuningList()[m_TuneZone].m_GunSpeed;
			}
			break;
		}

		vec2 SnapPos;

		for(int i = 0; i < 3; i++)
		{
			{
				CNetObj_Projectile *pProj = Server()->SnapNewItem<CNetObj_Projectile>(m_aParticleIds[i]);
				if(!pProj)
				{
					continue;
				}
				
				//Calculate original shoot position
				SnapPos = CalcPos(m_OrigPos, m_OrigDir, Curvature, Speed, (i * 2 + 3) / (float)Server()->TickSpeed());

				pProj->m_X = SnapPos.x;
				pProj->m_Y = SnapPos.y;
				pProj->m_VelX = 0;
				pProj->m_VelY = 0;
				pProj->m_StartTick = Server()->Tick();
				pProj->m_Type = WEAPON_HAMMER;
			}
		}
	}

	int SnappingClientVersion = GameServer()->GetClientVersion(SnappingClient);
	if(SnappingClientVersion < VERSION_DDNET_ENTITY_NETOBJS)
	{
		CCharacter *pSnapChar = GameServer()->GetPlayerChar(SnappingClient);
		int Tick = (Server()->Tick() % Server()->TickSpeed()) % 20;
		if(pSnapChar && pSnapChar->IsAlive() && (m_Layer == LAYER_SWITCH && m_Number > 0 && !Switchers()[m_Number].m_aStatus[pSnapChar->Team()] && (!Tick)))
			return;
	}

	CCharacter *pOwnerChar = nullptr;
	CClientMask TeamMask = CClientMask().set();

	if(m_Owner >= 0)
		pOwnerChar = GameServer()->GetPlayerChar(m_Owner);

	if(pOwnerChar && pOwnerChar->IsAlive())
		TeamMask = pOwnerChar->TeamMask();

	if(SnappingClient != SERVER_DEMO_CLIENT && m_Owner != -1 && !TeamMask.test(SnappingClient))
		return;

	CNetObj_DDRaceProjectile DDRaceProjectile;

	if(SnappingClientVersion >= VERSION_DDNET_ENTITY_NETOBJS)
	{
		CNetObj_DDNetProjectile *pDDNetProjectile = static_cast<CNetObj_DDNetProjectile *>(Server()->SnapNewItem(NETOBJTYPE_DDNETPROJECTILE, GetId(), sizeof(CNetObj_DDNetProjectile)));
		if(!pDDNetProjectile)
		{
			return;
		}
		FillExtraInfo(pDDNetProjectile);
		if(m_Owner == SnappingClient)
			pDDNetProjectile->m_StartTick -= m_RollbackSnapTickDifference; //rollback
	}
	else if(SnappingClientVersion >= VERSION_DDNET_ANTIPING_PROJECTILE && FillExtraInfoLegacy(&DDRaceProjectile))
	{
		int Type = SnappingClientVersion < VERSION_DDNET_MSG_LEGACY ? (int)NETOBJTYPE_PROJECTILE : NETOBJTYPE_DDRACEPROJECTILE;
		void *pProj = Server()->SnapNewItem(Type, GetId(), sizeof(DDRaceProjectile));
		if(!pProj)
		{
			return;
		}
		if(m_Owner == SnappingClient)
			DDRaceProjectile.m_StartTick -= m_RollbackSnapTickDifference; //rollback
		mem_copy(pProj, &DDRaceProjectile, sizeof(DDRaceProjectile));
	}
	else
	{
		CNetObj_Projectile *pProj = Server()->SnapNewItem<CNetObj_Projectile>(GetId());
		if(!pProj)
		{
			return;
		}
		FillInfo(pProj);
	}
	m_FirstSnap = false;
}

void CProjectileKZ::Move()
{
	// Set m_Vel to tunezone speed and curvature
	bool UpdateSpeed = false;

	m_TuneZone = GameServer()->Collision()->IsTune(GameServer()->Collision()->GetMapIndex(m_Pos));

	if(m_PrevTuneZone != m_TuneZone)
	{
		switch(m_Type)
		{
		case WEAPON_GUN:
            if(m_TuneZone == 0)
			    m_Speed = Tuning()->m_GunSpeed;
            else
                m_Speed = GetTuning(m_TuneZone)->m_GunSpeed;
			UpdateSpeed = true;
			break;
		case WEAPON_SHOTGUN:
			if(m_TuneZone == 0)
			    m_Speed = Tuning()->m_ShotgunSpeed;
            else
                m_Speed = GetTuning(m_TuneZone)->m_ShotgunSpeed;
			UpdateSpeed = true;
			break;
		case WEAPON_GRENADE:
			if(m_TuneZone == 0)
			    m_Speed = Tuning()->m_GrenadeSpeed;
            else
                m_Speed = GetTuning(m_TuneZone)->m_GrenadeSpeed;
			UpdateSpeed = true;
			break;
		}

        switch(m_Type)
        {
        case WEAPON_GUN:
            if(m_TuneZone == 0)
                m_Curvature = Tuning()->m_GunCurvature;
            else
                m_Curvature = GetTuning(m_TuneZone)->m_GunCurvature;
            break;
        case WEAPON_SHOTGUN:
            if(m_TuneZone == 0)
                m_Curvature = Tuning()->m_ShotgunCurvature;
            else
                m_Curvature = GetTuning(m_TuneZone)->m_ShotgunCurvature;
            break;
        case WEAPON_GRENADE:
            if(m_TuneZone == 0)
                m_Curvature = Tuning()->m_GrenadeCurvature;
            else
                m_Curvature = GetTuning(m_TuneZone)->m_GrenadeCurvature;
            break;
        }
	}

    //Apply curvature
	if(UpdateSpeed)
	{
        vec2 TempPos; //TempPos to change m_Dir, following the original projectile path
        float Speed = 0.f, Curvature = 0.f;

        switch(m_Type)
        {
        case WEAPON_GUN:
            if(m_PrevTuneZone == 0)
                Speed = Tuning()->m_GunSpeed;
            else
                Speed = GetTuning(m_PrevTuneZone)->m_GunSpeed;
            UpdateSpeed = true;
            break;
        case WEAPON_SHOTGUN:
            if(m_PrevTuneZone == 0)
                Speed = Tuning()->m_ShotgunSpeed;
            else
                Speed = GetTuning(m_PrevTuneZone)->m_ShotgunSpeed;
            UpdateSpeed = true;
            break;
        case WEAPON_GRENADE:
            if(m_PrevTuneZone == 0)
                Speed = Tuning()->m_GrenadeSpeed;
            else
                Speed = GetTuning(m_PrevTuneZone)->m_GrenadeSpeed;
            UpdateSpeed = true;
            break;
        }

        switch(m_Type)
        {
        case WEAPON_GUN:
            if(m_PrevTuneZone == 0)
                Curvature = Tuning()->m_GunCurvature;
            else
                Curvature = GetTuning(m_PrevTuneZone)->m_GunCurvature;
            break;
        case WEAPON_SHOTGUN:
            if(m_PrevTuneZone == 0)
                Curvature = Tuning()->m_ShotgunCurvature;
            else
                Curvature = GetTuning(m_PrevTuneZone)->m_ShotgunCurvature;
            break;
        case WEAPON_GRENADE:
            if(m_PrevTuneZone == 0)
                Curvature = Tuning()->m_GrenadeCurvature;
            else
                Curvature = GetTuning(m_PrevTuneZone)->m_GrenadeCurvature;
            break;
        }

	float Time = ((m_CollidedTick - (m_StartTick - 1))/(float)Server()->TickSpeed());
        Time *= Speed;

        TempPos.x = m_SnapPos.x + m_Dir.x * Time;
        TempPos.y = m_SnapPos.y + m_Dir.y * Time + Curvature / 10000 * (Time * Time);

		m_SnapPos = m_Pos;
        m_StartTick = m_SnapTick = m_CollidedTick - 1;
        m_SnapVel = m_Dir = normalize(TempPos - m_Pos);

        Time = (1/(float)Server()->TickSpeed());
        Time *= m_Speed;

        m_Pos.x = m_SnapPos.x + m_Dir.x * Time;
        m_Pos.y = m_SnapPos.y + m_Dir.y * Time + m_Curvature / 10000 * (Time * Time);

        m_SnapVel *= 50;
        UpdateSpeed = false;
        m_PrevTuneZone = m_TuneZone;
	}
    else
    {
        float Time = ((m_CollidedTick - (float)m_StartTick)/(float)Server()->TickSpeed());
        Time *= m_Speed;

        m_Pos.x = m_SnapPos.x + m_Dir.x * Time;
        m_Pos.y = m_SnapPos.y + m_Dir.y * Time + m_Curvature / 10000 * (Time * Time);
    }
}

int CProjectileKZ::Collide(vec2 PrevPos, vec2 *pPreIntersectPos, vec2 *pIntersectPos, int *pTeleNr)
{
	SKZColFastIntersectLineProjectileParams Params;
	Params.pTeleNr = pTeleNr;

	return Collision()->FastIntersectLine(PrevPos, m_Pos, pIntersectPos, pPreIntersectPos, &Params);
}

void CProjectileKZ::OnCollide(vec2 PrevPos, int TileIndex, vec2 *pPreIntersectPos, vec2 *pIntersectPos, int *pTeleNr)
{
    if(TileIndex == TILE_TELEINWEAPON && pTeleNr && *pTeleNr)
	{
		int TeleOut = GameWorld()->m_Core.RandomOr0(Collision()->TeleOuts(*pTeleNr - 1).size());
		m_Pos = Collision()->TeleOuts(*pTeleNr - 1)[TeleOut];
	}
	else
	{
		Reset();
	}
}

void CProjectileKZ::OnCharacterCollide(vec2 PrevPos, CCharacter *pChar, vec2 *pIntersectPos)
{
    if(pChar->GetPlayer() && pChar->GetPlayer()->GetCid() != m_Owner)
        Reset();
}

void CProjectileKZ::FillInfo(CNetObj_Projectile *pProj)
{
	pProj->m_X = (int)m_SnapPos.x;
	pProj->m_Y = (int)m_SnapPos.y;
	pProj->m_VelX = (int)(m_SnapVel.x * 2);
	pProj->m_VelY = (int)(m_SnapVel.y * 2);
	pProj->m_StartTick = m_SnapTick;
	pProj->m_Type = m_Type;
}

bool CProjectileKZ::FillExtraInfoLegacy(CNetObj_DDRaceProjectile *pProj)
{
	const int MaxPos = 0x7fffffff / 100;
	if(absolute((int)m_SnapPos.y) + 1 >= MaxPos || absolute((int)m_SnapPos.x) + 1 >= MaxPos)
	{
		// If the modified data would be too large to fit in an integer, send normal data instead
		return false;
	}
	// Send additional/modified info, by modifying the fields of the netobj
	float Angle = -std::atan2(m_SnapVel.x, m_SnapVel.y);

	int Data = 0;
	Data |= (absolute(m_Owner) & 255) << 0;
	if(m_Owner < 0)
		Data |= LEGACYPROJECTILEFLAG_NO_OWNER;
	// This bit tells the client to use the extra info
	Data |= LEGACYPROJECTILEFLAG_IS_DDNET;
	// LEGACYPROJECTILEFLAG_BOUNCE_HORIZONTAL, LEGACYPROJECTILEFLAG_BOUNCE_VERTICAL
	Data |= (m_Bouncing & 3) << 10;
	// if(m_Explosive)
	//	Data |= LEGACYPROJECTILEFLAG_EXPLOSIVE;
	// if(m_Freeze)
	//	Data |= LEGACYPROJECTILEFLAG_FREEZE;

	pProj->m_X = (int)(m_SnapPos.x * 100.0f);
	pProj->m_Y = (int)(m_SnapPos.y * 100.0f);
	pProj->m_Angle = (int)(Angle * 1000000.0f);
	pProj->m_Data = Data;
	pProj->m_StartTick = m_SnapTick;
	pProj->m_Type = m_Type;
	return true;
}

void CProjectileKZ::FillExtraInfo(CNetObj_DDNetProjectile *pProj)
{
	int Flags = 0;
	if(m_Bouncing & 1)
	{
		Flags |= PROJECTILEFLAG_BOUNCE_HORIZONTAL;
	}
	if(m_Bouncing & 2)
	{
		Flags |= PROJECTILEFLAG_BOUNCE_VERTICAL;
	}
	/*if(m_Explosive)
	{
		Flags |= PROJECTILEFLAG_EXPLOSIVE;
	}
	if(m_Freeze)
	{
		Flags |= PROJECTILEFLAG_FREEZE;
	}*/

	if(m_Owner < 0)
	{
		pProj->m_VelX = round_to_int(m_Dir.x * 1e6f);
		pProj->m_VelY = round_to_int(m_Dir.y * 1e6f);
	}
	else
	{
		pProj->m_VelX = round_to_int(m_SnapVel.x);
		pProj->m_VelY = round_to_int(m_SnapVel.y);
		Flags |= PROJECTILEFLAG_NORMALIZE_VEL;
	}

	pProj->m_X = round_to_int(m_SnapPos.x * 100.0f);
	pProj->m_Y = round_to_int(m_SnapPos.y * 100.0f);
	pProj->m_Type = m_Type;
	pProj->m_StartTick = m_SnapTick;
	pProj->m_Owner = m_Owner;
	pProj->m_Flags = Flags;
	pProj->m_SwitchNumber = m_Number;
	pProj->m_TuneZone = m_TuneZone;
}

vec2 CProjectileKZ::GetPos(float Time)
{
	float Curvature = 0;
	float Speed = 0;

	switch(m_Type)
	{
	case WEAPON_GRENADE:
		if(!m_TuneZone)
		{
			Curvature = Tuning()->m_GrenadeCurvature;
			Speed = Tuning()->m_GrenadeSpeed;
		}
		else
		{
			Curvature = TuningList()[m_TuneZone].m_GrenadeCurvature;
			Speed = TuningList()[m_TuneZone].m_GrenadeSpeed;
		}

		break;

	case WEAPON_SHOTGUN:
		if(!m_TuneZone)
		{
			Curvature = Tuning()->m_ShotgunCurvature;
			Speed = Tuning()->m_ShotgunSpeed;
		}
		else
		{
			Curvature = TuningList()[m_TuneZone].m_ShotgunCurvature;
			Speed = TuningList()[m_TuneZone].m_ShotgunSpeed;
		}

		break;

	case WEAPON_GUN:
		if(!m_TuneZone)
		{
			Curvature = Tuning()->m_GunCurvature;
			Speed = Tuning()->m_GunSpeed;
		}
		else
		{
			Curvature = TuningList()[m_TuneZone].m_GunCurvature;
			Speed = TuningList()[m_TuneZone].m_GunSpeed;
		}
		break;
	}

	return CalcPos(m_Pos, m_Dir, Curvature, Speed, Time);
}

void CProjectileKZ::SwapClients(int Client1, int Client2)
{
	m_Owner = m_Owner == Client1 ? Client2 : m_Owner == Client2 ? Client1 :
								      m_Owner;
}