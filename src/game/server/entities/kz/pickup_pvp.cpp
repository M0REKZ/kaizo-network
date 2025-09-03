/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include "pickup_pvp.h"
#include "particle_kz.h"
#include <game/server/entities/character.h>
#include <engine/shared/config.h>

#include <game/generated/protocol.h>
#include <game/mapitems.h>
#include <game/teamscore.h>

#include <game/server/gamecontext.h>
#include <game/server/player.h>

static constexpr int gs_PickupPhysSize = 14;

CPickupPvP::CPickupPvP(CGameWorld *pGameWorld, int Type, int SubType, int Layer, int Number, int Flags) :
	CEntity(pGameWorld, CGameWorld::ENTTYPE_PICKUP, vec2(0, 0), gs_PickupPhysSize)
{
	m_Core = vec2(0.0f, 0.0f);
	m_Type = Type;
	m_Subtype = SubType;

	m_Layer = Layer;
	m_Number = Number;
	m_Flags = Flags;

	if(m_Type == POWERUP_NINJA)
		m_SpawnTick = Server()->Tick() + Server()->TickSpeed() * 90;

	GameWorld()->InsertEntity(this);
}

void CPickupPvP::Reset()
{
	m_MarkedForDestroy = true;
}

void CPickupPvP::Tick()
{
	Move();
	// wait for respawn
	if(m_SpawnTick > 0)
	{
		if (Server()->Tick() % 2 == 0 && g_Config.m_SvPickupParticles)
		{
			new CParticleKZ(GameWorld(), vec2(m_Pos.x - 16 + rand() % 32, m_Pos.y - 16 + rand() % 32), 10);
		}
		if(Server()->Tick() > m_SpawnTick)
		{
			// respawn
			m_SpawnTick = -1;

			if(m_Type == POWERUP_WEAPON)
				GameServer()->CreateSound(m_Pos, SOUND_WEAPON_SPAWN);
		}
		else
			return;
	}
	// Check if a player intersected us
	CCharacter *pChr = GameWorld()->ClosestCharacter(m_Pos, 20.0f, nullptr);
	if(pChr && pChr->IsAlive())
	{
		// player picked us up, is someone was hooking us, let them go
		bool Picked = false;
		switch (m_Type)
		{
			case POWERUP_HEALTH:
				if(pChr->IncreaseHealth(1))
				{
					Picked = true;
					GameServer()->CreateSound(m_Pos, SOUND_PICKUP_HEALTH);
				}
				break;

			case POWERUP_ARMOR:
				if(pChr->IncreaseArmor(1))
				{
					Picked = true;
					GameServer()->CreateSound(m_Pos, SOUND_PICKUP_ARMOR);
				}
				break;

			case POWERUP_WEAPON:
			switch (m_Subtype)
			{
				case WEAPON_GRENADE:
					if(!pChr->GetWeaponGot(WEAPON_GRENADE) || pChr->GetWeaponAmmo(WEAPON_GRENADE) < 10)
					{
						pChr->GiveWeapon(WEAPON_GRENADE);
						pChr->SetWeaponAmmo(WEAPON_GRENADE, 10);
						Picked = true;
						GameServer()->CreateSound(m_Pos, SOUND_PICKUP_GRENADE);
						if(pChr->GetPlayer())
							GameServer()->SendWeaponPickup(pChr->GetPlayer()->GetCid(), WEAPON_GRENADE);
					}
					break;
				case WEAPON_SHOTGUN:
					if(!pChr->GetWeaponGot(WEAPON_SHOTGUN) || pChr->GetWeaponAmmo(WEAPON_SHOTGUN) < 10)
					{
						pChr->GiveWeapon(WEAPON_SHOTGUN);
						pChr->SetWeaponAmmo(WEAPON_SHOTGUN, 10);
						Picked = true;
						GameServer()->CreateSound(m_Pos, SOUND_PICKUP_SHOTGUN);
						if(pChr->GetPlayer())
							GameServer()->SendWeaponPickup(pChr->GetPlayer()->GetCid(), WEAPON_SHOTGUN);
					}
					break;
				case WEAPON_LASER:
					if(!pChr->GetWeaponGot(WEAPON_LASER) || pChr->GetWeaponAmmo(WEAPON_LASER) < 10)
					{
						pChr->GiveWeapon(WEAPON_LASER);
						pChr->SetWeaponAmmo(WEAPON_LASER, 10);
						Picked = true;
						GameServer()->CreateSound(m_Pos, SOUND_PICKUP_SHOTGUN);
						if(pChr->GetPlayer())
							GameServer()->SendWeaponPickup(pChr->GetPlayer()->GetCid(), WEAPON_LASER);
					}
					break;
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
						if (pC != pChr)
							pC->SetEmote(EMOTE_SURPRISE, Server()->Tick() + Server()->TickSpeed());
					}

					pChr->SetEmote(EMOTE_ANGRY, Server()->Tick() + 1200 * Server()->TickSpeed() / 1000);
					break;
				}

			default:
				break;
		};

		if(Picked)
		{
			char aBuf[256];
			str_format(aBuf, sizeof(aBuf), "pickup player='%d:%s' item=%d",
				pChr->GetPlayer()->GetCid(), Server()->ClientName(pChr->GetPlayer()->GetCid()), m_Type);
			GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
			int RespawnTime = m_Type == POWERUP_NINJA ? 90 : 15;
			if(RespawnTime >= 0)
				m_SpawnTick = Server()->Tick() + Server()->TickSpeed() * RespawnTime;
		}
	}
}

void CPickupPvP::TickPaused()
{
	m_SpawnTick++;
}

void CPickupPvP::Snap(int SnappingClient)
{
	if(m_SpawnTick > 0)
		return;

	if(NetworkClipped(SnappingClient))
		return;

	int SnappingClientVersion = GameServer()->GetClientVersion(SnappingClient);
	bool Sixup = Server()->IsSixup(SnappingClient);

	if(SnappingClientVersion < VERSION_DDNET_ENTITY_NETOBJS)
	{
		CCharacter *pChar = GameServer()->GetPlayerChar(SnappingClient);

		if(SnappingClient != SERVER_DEMO_CLIENT && (GameServer()->m_apPlayers[SnappingClient]->GetTeam() == TEAM_SPECTATORS || GameServer()->m_apPlayers[SnappingClient]->IsPaused()) && GameServer()->m_apPlayers[SnappingClient]->m_SpectatorId != SPEC_FREEVIEW)
			pChar = GameServer()->GetPlayerChar(GameServer()->m_apPlayers[SnappingClient]->m_SpectatorId);

		int Tick = (Server()->Tick() % Server()->TickSpeed()) % 11;
		if(pChar && pChar->IsAlive() && m_Layer == LAYER_SWITCH && m_Number > 0 && !Switchers()[m_Number].m_aStatus[pChar->Team()] && !Tick)
			return;
	}

	GameServer()->SnapPickup(CSnapContext(SnappingClientVersion, Sixup, SnappingClient), GetId(), m_Pos, m_Type, m_Subtype, m_Number, m_Flags);
}

void CPickupPvP::Move()
{
	if(Server()->Tick() % (int)(Server()->TickSpeed() * 0.15f) == 0)
	{
		GameServer()->Collision()->MoverSpeed(m_Pos.x, m_Pos.y, &m_Core);
		m_Pos += m_Core;
	}
}
