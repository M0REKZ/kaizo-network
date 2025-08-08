/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include <game/generated/protocol.h>
#include <game/mapitems.h>
#include <game/teamscore.h>

#include <game/server/entities/character.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <game/server/gamecontroller.h>

#include "kz_pickup.h"
#include <game/mapitems_kz.h>

static constexpr int gs_PickupPhysSize = 14;

CKZPickup::CKZPickup(CGameWorld *pGameWorld, int Type, int SubType, int Layer, int Number, int Flags) :
CEntity(pGameWorld,CGameWorld::CUSTOM_ENTTYPE_KZPICKUP,vec2(0,0),gs_PickupPhysSize)
{
	m_Type = Type;
	m_Subtype = SubType;

	m_Layer = Layer;
	m_Number = Number;
	m_Flags = Flags;

	int SpawnDelay = m_Type == POWERUP_NINJA ? 90 : 0;

	m_Id2 = Server()->SnapNewId();

	for(int i=0;i < NUM_DDRACE_TEAMS;i++)
	{
		if(SpawnDelay > 0)
			m_SpawnTickTeam[i] = Server()->Tick() + Server()->TickSpeed() * SpawnDelay;
		else
			m_SpawnTickTeam[i] = -1;
	}

	GameWorld()->InsertEntity(this);
}

CKZPickup::CKZPickup(CGameWorld *pGameWorld, int Objtype, vec2 Pos, int ProximityRadius) :
CEntity(pGameWorld,Objtype,Pos,ProximityRadius)
{
}

CKZPickup::~CKZPickup()
{
	if(m_Id2 != -1)
		Server()->SnapFreeId(m_Id2);
}

void CKZPickup::Reset()
{
	m_MarkedForDestroy = true;
}

void CKZPickup::Tick()
{
	Move();

	if(m_MarkedForDestroy)
		return;

	if(m_Dropped && m_DropTick + Server()->TickSpeed() > Server()->Tick())
		return;

	if(m_Dropped && m_DropTick + Server()->TickSpeed() * 30 < Server()->Tick()) //if dropped dont stay forever
	{
		Reset();
		return;
	}
	
	for(int i=0;i < NUM_DDRACE_TEAMS;i++)
	{
		// wait for respawn
		if(m_SpawnTickTeam[i] > 0)
		{
			if(Server()->Tick() > m_SpawnTickTeam[i])
			{
				// respawn
				m_SpawnTickTeam[i] = -1;

				if(m_Type == POWERUP_WEAPON)
					GameServer()->CreateSound(m_Pos, SOUND_WEAPON_SPAWN, GameServer()->m_pController->Teams().TeamMask(i));
			}
			else
				continue;
		}
	}

	// Check if a player intersected us
	CCharacter *pChr = (CCharacter*)GameWorld()->FindFirst(CGameWorld::ENTTYPE_CHARACTER);//GameWorld()->ClosestCharacter(m_Pos, GetProximityRadius() + ms_CollisionExtraSize, 0);
	for(;pChr;pChr = (CCharacter*)pChr->TypeNext())
	{
		if(pChr && pChr->IsAlive())
		{
			if(m_ThisTeamOnly != -1 && pChr->Team() != m_ThisTeamOnly)
				continue;

			if(pChr->Team() < 0 || pChr->Team() >= NUM_DDRACE_TEAMS)
				continue;

			if(m_SpawnTickTeam[pChr->Team()] > 0)
				continue;

			if(distance(m_Pos,pChr->m_Pos) > (GetProximityRadius() + ms_CollisionExtraSize + pChr->GetProximityRadius()))
				continue;

			if(m_Number > 0 && !Switchers()[m_Number].m_aStatus[pChr->Team()])
				continue;

			bool Picked = false;
			// player picked us up, is someone was hooking us, let them go
			switch(m_Type)
			{
			case POWERUP_HEALTH:
				if(m_Subtype == 1 ? pChr->IncreaseHealth(5) : pChr->IncreaseHealth(1))
				{
					Picked = true;
					GameServer()->CreateSound(m_Pos, SOUND_PICKUP_HEALTH, pChr->TeamMask());
				}
				break;

			case POWERUP_ARMOR:
				if(m_Subtype == 1 ? pChr->IncreaseArmor(5) : pChr->IncreaseArmor(1))
				{
					Picked = true;
					GameServer()->CreateSound(m_Pos, SOUND_PICKUP_ARMOR, pChr->TeamMask());
				}
				break;

			case POWERUP_WEAPON:

				if(m_Subtype >= 0 && m_Subtype < KZ_NUM_CUSTOM_WEAPONS && (!pChr->GetWeaponGot(m_Subtype) || pChr->GetWeaponAmmo(m_Subtype) != -1))
				{

						if((m_Subtype == WEAPON_GUN || m_Subtype == WEAPON_HAMMER) && !pChr->GetWeaponGot(m_Subtype))
						{
							GameServer()->CreateSound(m_Pos, SOUND_PICKUP_ARMOR, pChr->TeamMask());
							pChr->GiveWeapon(m_Subtype, false);
						}
						else if(m_Subtype == KZ_CUSTOM_WEAPON_PORTAL_GUN && !pChr->GetWeaponGot(m_Subtype))
						{
							GameServer()->CreateSound(m_Pos, SOUND_PICKUP_GRENADE, pChr->TeamMask());
							if(m_Ammo == -2)
								pChr->GiveWeapon(m_Subtype, false);
							else
								pChr->GiveWeapon(m_Subtype, false);
						}

						if(pChr->GetPlayer())
							GameServer()->SendWeaponPickup(pChr->GetPlayer()->GetCid(), m_Subtype);
						Picked = true;
				}
				break;

			case POWERUP_NINJA:
			{
				Picked = true;

				// activate ninja on target player
				pChr->GiveNinja();

				// loop through all players, setting their emotes
				CCharacter *pC = static_cast<CCharacter *>(GameWorld()->FindFirst(CGameWorld::ENTTYPE_CHARACTER));
				for(; pC; pC = (CCharacter *)pC->TypeNext())
				{
					if(pC != pChr)
						pC->SetEmote(EMOTE_SURPRISE, Server()->Tick() + Server()->TickSpeed());
				}

				pChr->SetEmote(EMOTE_ANGRY, Server()->Tick() + 1200 * Server()->TickSpeed() / 1000);
				break;
			}
			default:
				break;
			};

			if(!m_Dropped && Picked && (m_Type != POWERUP_WEAPON))
			{
				char aBuf[256];
				str_format(aBuf, sizeof(aBuf), "pickup player='%d:%s' item=%d",
					pChr->GetPlayer()->GetCid(), Server()->ClientName(pChr->GetPlayer()->GetCid()), m_Type);
				GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
				int RespawnTime = m_Type == POWERUP_NINJA ? 90 : 15;
				if(RespawnTime >= 0)
					m_SpawnTickTeam[pChr->Team()] = Server()->Tick() + Server()->TickSpeed() * RespawnTime;
			}
			else if(m_Dropped && Picked)
			{
				char aBuf[256];
				str_format(aBuf, sizeof(aBuf), "pickup player='%d:%s' item=%d",
					pChr->GetPlayer()->GetCid(), Server()->ClientName(pChr->GetPlayer()->GetCid()), m_Type);
				GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
				Reset();
			}
		}
	}
}

void CKZPickup::TickPaused()
{
	for(int i = 0;i<NUM_DDRACE_TEAMS;i++)
	{
		if(m_SpawnTickTeam[i] != -1)
			++m_SpawnTickTeam[i];
	}
}

void CKZPickup::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CCharacter *pChar = GameServer()->GetPlayerChar(SnappingClient);

	int Team;

	if(m_ThisTeamOnly == -1)
	{
		if(!pChar)
			Team = 0;
		else
			Team = pChar->Team();
	}
	else
	{
		if(!pChar)
			Team = m_ThisTeamOnly;
		else
			Team = pChar->Team();
	}

	if(m_ThisTeamOnly != -1 && Team != m_ThisTeamOnly)
		return;

	if(Team < 0 || Team >= NUM_DDRACE_TEAMS)
		return;

	if(!(m_SpawnTickTeam[Team] == -1))
		return;

	int SnappingClientVersion = GameServer()->GetClientVersion(SnappingClient);
	bool Sixup = Server()->IsSixup(SnappingClient);

	if(SnappingClientVersion < VERSION_DDNET_ENTITY_NETOBJS)
	{

		if(SnappingClient != SERVER_DEMO_CLIENT && (GameServer()->m_apPlayers[SnappingClient]->GetTeam() == TEAM_SPECTATORS || GameServer()->m_apPlayers[SnappingClient]->IsPaused()) && GameServer()->m_apPlayers[SnappingClient]->SpectatorId() != SPEC_FREEVIEW)
			pChar = GameServer()->GetPlayerChar(GameServer()->m_apPlayers[SnappingClient]->SpectatorId());

		int Tick = (Server()->Tick() % Server()->TickSpeed()) % 11;
		if(pChar && pChar->IsAlive() && m_Number > 0 && !Switchers()[m_Number].m_aStatus[pChar->Team()] && !Tick)
			return;
	}

	if ((m_Type == POWERUP_HEALTH || m_Type == POWERUP_ARMOR) && m_Subtype == 1 && m_Id2 != -1)
	{
		vec2 pos1, pos2;
		
		pos1.x = (int)m_Pos.x + 16*sin((float)Server()->Tick() / 25.0);
		pos1.y = (int)m_Pos.y + 16*sin((float)Server()->Tick() / 25.0);
		
		pos2.x = (int)m_Pos.x + 16*cos((float)Server()->Tick() / 25.0);
		pos2.y = (int)m_Pos.y + -16*cos((float)Server()->Tick() / 25.0);
		
		GameServer()->SnapPickup(CSnapContext(SnappingClientVersion, Sixup, SnappingClient), GetId(), pos1, m_Type, 0, m_Number,m_Flags);
		GameServer()->SnapPickup(CSnapContext(SnappingClientVersion, Sixup, SnappingClient), m_Id2, pos2, m_Type, 0, m_Number,m_Flags);
	}
	else if(m_Subtype >=0 && m_Subtype < NUM_WEAPONS)
	{
		vec2 postemp;
				
		postemp.x = m_Pos.x + 32*sin((float)Server()->Tick() / 25.0);
		postemp.y = m_Pos.y + 32*cos((float)Server()->Tick() / 25.0);

		CNetObj_Projectile *pProj = Server()->SnapNewItem<CNetObj_Projectile>(m_Id2);
		if(!pProj)
		{
			return;
		}
		pProj->m_X = postemp.x;
		pProj->m_Y = postemp.y;
		pProj->m_VelX = 0;
		pProj->m_VelY = 0;
		pProj->m_StartTick = Server()->Tick();
		pProj->m_Type = WEAPON_HAMMER;
		GameServer()->SnapPickup(CSnapContext(SnappingClientVersion, Sixup, SnappingClient), GetId(), m_Pos, m_Type, m_Subtype, m_Number, m_Flags);
	}
	else
	{
		if(m_Subtype == KZ_CUSTOM_WEAPON_PORTAL_GUN)
		{
			vec2 postemp;
					
			postemp.x = m_Pos.x + 32*sin((float)Server()->Tick() / 25.0);
			postemp.y = m_Pos.y + 32*cos((float)Server()->Tick() / 25.0);

			GameServer()->SnapLaserObject(CSnapContext(SnappingClientVersion, Sixup, SnappingClient),m_Id2,postemp,postemp,Server()->Tick(),-1,Server()->Tick() % 3);
			GameServer()->SnapPickup(CSnapContext(SnappingClientVersion, Sixup, SnappingClient), GetId(), m_Pos, m_Type, WEAPON_LASER, m_Number, m_Flags);
		}
	}
}

void CKZPickup::Move()
{
	if(!m_Dropped)
	{
		if(Server()->Tick() % (int)(Server()->TickSpeed() * 0.15f) == 0)
		{
			Collision()->MoverSpeed(m_Pos.x, m_Pos.y, &m_Core);
			m_Pos += m_Core;
		}
	}
	else
	{
		//flag physics

		if((GameServer()->Collision()->GetCollisionAt(m_Pos.x, m_Pos.y) == TILE_DEATH) ||
			(GameServer()->Collision()->GetFrontCollisionAt(m_Pos.x, m_Pos.y) == TILE_DEATH) ||
			GameLayerClipped(m_Pos))
		{
			Reset();
			return;
		}

		
			if(Server()->Tick() > m_DropTick + Server()->TickSpeed() * 30)
			{
				Reset();
				return;
			}
			else
			{
				// Friction
				m_IsGrounded = false;
				if(GameServer()->Collision()->CheckPoint(m_Pos.x + gs_PickupPhysSize / 2, m_Pos.y + gs_PickupPhysSize / 2 + 5))
					m_IsGrounded = true;
				if(GameServer()->Collision()->CheckPoint(m_Pos.x - gs_PickupPhysSize / 2, m_Pos.y + gs_PickupPhysSize / 2 + 5))
					m_IsGrounded = true;

				if(m_IsGrounded)
				{
					m_Vel.x *= 0.75f;
				}
				else
				{
					m_Vel.x *= 0.98f;
				}

				// Gravity
				m_Vel.y += GameWorld()->m_Core.m_aTuning[0].m_Gravity;
				GameServer()->Collision()->MoveBox(&m_Pos, &m_Vel, vec2(gs_PickupPhysSize, gs_PickupPhysSize), vec2(0.5, 0.5));

				//ICTFX Flag teleport
				int index = GameServer()->Collision()->GetMapIndex(m_Pos);
				//CCollision * col = GameServer()->Collision();
				int tele = GameServer()->Collision()->IsTeleport(index);
				if(!tele)
					tele = GameServer()->Collision()->IsEvilTeleport(index);
				if(!tele)
					tele = GameServer()->Collision()->IsCheckTeleport(index);
				if(!tele)
					tele = GameServer()->Collision()->IsCheckEvilTeleport(index);

				if(tele)
				{
					int size = GameServer()->Collision()->TeleOuts(tele-1).size();
					if(size)
					{
						int RandomOut = rand() % size;
						m_Pos = GameServer()->Collision()->TeleOuts(tele-1)[RandomOut];
					}
				}
			}
		
	}
}