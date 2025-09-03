/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
/* Based on Race mod stuff and tweaked by GreYFoX@GTi and others to fit our DDRace needs. */
#include "base_pvp.h"

#include <engine/server.h>
#include <engine/shared/config.h>
#include <game/mapitems.h>
#include <game/server/entities/character.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <game/server/score.h>
#include <game/version.h>

#include <game/server/entities/projectile.h>
#include <game/server/entities/laser.h>
#include <game/server/entities/kz/pickup_pvp.h>

#define GAME_TYPE_NAME "PVP"
#define TEST_TYPE_NAME "TestPVP"

CGameControllerBasePvP::CGameControllerBasePvP(class CGameContext *pGameServer) :
	CGameControllerBaseKZ(pGameServer)
{
	m_pGameType = g_Config.m_SvTestingCommands ? TEST_TYPE_NAME : GAME_TYPE_NAME;
	m_GameFlags = 0;

	if(g_Config.m_SvTuneReset)
	{
		GameServer()->Tuning()->Set("gun_speed", 2200.0f);
		GameServer()->Tuning()->Set("gun_curvature", 1.25f);
		GameServer()->Tuning()->Set("shotgun_speed", 2750.0f);
		GameServer()->Tuning()->Set("shotgun_speeddiff", 0.8f);
		GameServer()->Tuning()->Set("shotgun_curvature", 1.25f);
	}

	if(g_Config.m_SvInstagibWeapon[0] == 'l' || g_Config.m_SvInstagibWeapon[0] == 'L')
		m_InstagibWeapon = WEAPON_LASER;
	else if(g_Config.m_SvInstagibWeapon[0] == 'g' || g_Config.m_SvInstagibWeapon[0] == 'G')
		m_InstagibWeapon = WEAPON_GRENADE;
}

CGameControllerBasePvP::~CGameControllerBasePvP() = default;

bool CGameControllerBasePvP::OnCharacterTakeDamage(CCharacter *pChar, vec2 Force, int Dmg, int From, int Weapon)
{
	if(!pChar)
		return false;

	pChar->GetCoreKZ().m_Vel += Force;

	if(GameServer()->m_pController->IsFriendlyFire(pChar->GetPlayer()->GetCid(), From) && !g_Config.m_SvTeamdamage)
		return false;

	if(m_InstagibWeapon != -1)
	{
		if(pChar->GetPlayer()->GetCid() == From)
			return false;

		if(Dmg < g_Config.m_SvInstagibMinDamage)
			return false;
		
		pChar->Die(From, Weapon);

		return false;
	}

	// m_pPlayer only inflicts half damage on self
	if(From == pChar->GetPlayer()->GetCid())
		Dmg = std::max(1, Dmg/2);

	pChar->GetDamageTakenKZ()++;

	// create healthmod indicator
	if(Server()->Tick() < pChar->GetDamageTakenTickKZ()+25)
	{
		// make sure that the damage indicators doesn't group together
		GameServer()->CreateDamageInd(pChar->m_Pos, pChar->GetDamageTakenKZ()*0.25f, Dmg);
	}
	else
	{
		pChar->GetDamageTakenKZ() = 0;
		GameServer()->CreateDamageInd(pChar->m_Pos, 0, Dmg);
	}

	if(Dmg)
	{
		if(pChar->GetArmor())
		{
			if(Dmg > 1)
			{
				pChar->GetHealthKZ()--;
				Dmg--;
			}

			if(Dmg > pChar->GetArmor())
			{
				Dmg -= pChar->GetArmor();
				pChar->SetArmor(0);
			}
			else
			{
				pChar->SetArmor(pChar->GetArmor() - Dmg);
				Dmg = 0;
			}
		}

		pChar->GetHealthKZ() -= Dmg;
	}

	pChar->GetDamageTakenTickKZ() = Server()->Tick();

	// do damage Hit sound
	if(From >= 0 && From != pChar->GetPlayer()->GetCid() && GameServer()->m_apPlayers[From] && GameServer()->m_apPlayers[From]->GetCharacter())
	{
		GameServer()->CreateSoundGlobal(SOUND_HIT, From);
	}

	// check for death
	if(pChar->GetHealthKZ() <= 0)
	{
		pChar->Die(From, Weapon);

		// set attacker's face to happy (taunt!)
		if (From >= 0 && From != pChar->GetPlayer()->GetCid() && GameServer()->m_apPlayers[From])
		{
			CCharacter *pChr = GameServer()->m_apPlayers[From]->GetCharacter();
			if (pChr)
			{
				pChar->SetEmote(EMOTE_HAPPY, Server()->Tick() + Server()->TickSpeed());
			}
		}

		return false;
	}

	if (Dmg > 2)
		GameServer()->CreateSound(pChar->m_Pos, SOUND_PLAYER_PAIN_LONG);
	else
		GameServer()->CreateSound(pChar->m_Pos, SOUND_PLAYER_PAIN_SHORT);

	pChar->SetEmote(EMOTE_PAIN, Server()->Tick() + 500 * Server()->TickSpeed() / 1000);

	return true;
}

bool CGameControllerBasePvP::CharacterFireWeapon(CCharacter *pChar)
{
	if(pChar->GetActiveWeapon() == WEAPON_NINJA)
		return false;

	if(pChar->GetReloadTimerKZ() != 0)
		return true;

	pChar->DoWeaponSwitch();
	vec2 MouseTarget = vec2(pChar->GetLatestInputKZ().m_TargetX, pChar->GetLatestInputKZ().m_TargetY);
	vec2 Direction = normalize(MouseTarget);

	bool FullAuto = false;
	if(pChar->GetActiveWeapon() == WEAPON_GRENADE || pChar->GetActiveWeapon() == WEAPON_SHOTGUN || pChar->GetActiveWeapon() == WEAPON_LASER)
		FullAuto = true;


	// check if we gonna fire
	bool WillFire = false;
	if(CountInput(pChar->GetLatestPrevInputKZ().m_Fire, pChar->GetLatestInputKZ().m_Fire).m_Presses)
		WillFire = true;

	if(FullAuto && (pChar->GetLatestInputKZ().m_Fire&1) && pChar->Core()->m_aWeapons[pChar->GetActiveWeapon()].m_Ammo)
		WillFire = true;

	if(!WillFire)
		return true;

	// check for ammo
	if(!pChar->Core()->m_aWeapons[pChar->GetActiveWeapon()].m_Ammo)
	{
		// 125ms is a magical limit of how fast a human can click
		pChar->GetReloadTimerKZ() = 125 * Server()->TickSpeed() / 1000;
		if(pChar->GetLastNoAmmoSoundKZ()+Server()->TickSpeed() <= Server()->Tick())
		{
			GameServer()->CreateSound(pChar->m_Pos, SOUND_WEAPON_NOAMMO);
			pChar->GetLastNoAmmoSoundKZ() = Server()->Tick();
		}
		return true;
	}

	vec2 ProjStartPos = pChar->m_Pos+Direction*pChar->GetProximityRadius()*0.75f;

	if(Config()->m_Debug)
	{
		char aBuf[256];
		str_format(aBuf, sizeof(aBuf), "shot player='%d:%s' team=%d weapon=%d", pChar->GetPlayer()->GetCid(), Server()->ClientName(pChar->GetPlayer()->GetCid()), pChar->GetPlayer()->GetTeam(), pChar->GetActiveWeapon());
		GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
	}

	switch(pChar->GetActiveWeapon())
	{
		case WEAPON_HAMMER:
		{
			GameServer()->CreateSound(pChar->m_Pos, SOUND_HAMMER_FIRE);

			CEntity *apEnts[MAX_CLIENTS];
			int Hits = 0;
			int Num = GameServer()->m_World.FindEntities(ProjStartPos, pChar->GetProximityRadius() * 0.5f, apEnts, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);

			for(int i = 0; i < Num; ++i)
			{
				CCharacter *pTarget = (CCharacter *)apEnts[i];

				if(!pTarget->CanCollide(pChar->GetPlayer()->GetCid()))
					continue;

				if((pTarget == pChar) || GameServer()->Collision()->IntersectLine(ProjStartPos, pTarget->m_Pos, NULL, NULL))
					continue;

				// set his velocity to fast upward (for now)
				if(length(pTarget->m_Pos-ProjStartPos) > 0.0f)
					GameServer()->CreateHammerHit(pTarget->m_Pos-normalize(pTarget->m_Pos-ProjStartPos)*pChar->GetProximityRadius()*0.5f);
				else
					GameServer()->CreateHammerHit(ProjStartPos);

				vec2 Dir;
				if(length(pTarget->m_Pos - pChar->m_Pos) > 0.0f)
					Dir = normalize(pTarget->m_Pos - pChar->m_Pos);
				else
					Dir = vec2(0.f, -1.f);

				pTarget->TakeDamage(vec2(0.f, -1.f) + normalize(Dir + vec2(0.f, -1.1f)) * 10.0f, 3,
					pChar->GetPlayer()->GetCid(), pChar->GetActiveWeapon());
				Hits++;
			}

			// if we Hit anything, we have to wait for the reload
			if(Hits)
				pChar->GetReloadTimerKZ() = Server()->TickSpeed()/3;

		} break;

		case WEAPON_GUN:
		{
			int Lifetime = (int)(Server()->TickSpeed() * pChar->GetTuning(pChar->m_TuneZone)->m_GunLifetime);

			new CProjectile(
				&GameServer()->m_World,
				WEAPON_GUN, //Type
				pChar->GetPlayer()->GetCid(), //Owner
				ProjStartPos, //Pos
				Direction, //Dir
				Lifetime, //Span
				false, //Freeze
				false, //Explosive
				-1, //SoundImpact
				MouseTarget //InitDir
				);

			GameServer()->CreateSound(pChar->m_Pos, SOUND_GUN_FIRE);
		} break;

		case WEAPON_SHOTGUN:
		{
			int ShotSpread = 2;
			int Lifetime = (int)(Server()->TickSpeed() * pChar->GetTuning(pChar->m_TuneZone)->m_ShotgunLifetime);

			for(int i = -ShotSpread; i <= ShotSpread; ++i)
			{
				float Spreading[] = {-0.185f, -0.070f, 0, 0.070f, 0.185f};
				float Angle = angle(Direction);
				Angle += Spreading[i + 2];
				float v = 1 - (absolute(i) / (float)ShotSpread);
				float Speed = mix((float)GameServer()->Tuning()->m_ShotgunSpeeddiff, 1.0f, v);
				
				new CProjectile(
				&GameServer()->m_World,
				WEAPON_SHOTGUN, //Type
				pChar->GetPlayer()->GetCid(), //Owner
				ProjStartPos, //Pos
				direction(Angle)*Speed, //Dir
				Lifetime, //Span
				false, //Freeze
				false, //Explosive
				-1, //SoundImpact
				MouseTarget //InitDir
				);
			}

			GameServer()->CreateSound(pChar->m_Pos, SOUND_SHOTGUN_FIRE);
		} break;

		case WEAPON_GRENADE:
		{
			int Lifetime = (int)(Server()->TickSpeed() * pChar->GetTuning(pChar->m_TuneZone)->m_GrenadeLifetime);

			new CProjectile(
				&GameServer()->m_World,
				WEAPON_GRENADE, //Type
				pChar->GetPlayer()->GetCid(), //Owner
				ProjStartPos, //Pos
				Direction, //Dir
				Lifetime, //Span
				false, //Freeze
				true, //Explosive
				SOUND_GRENADE_EXPLODE, //SoundImpact
				MouseTarget // MouseTarget
			);

			GameServer()->CreateSound(pChar->m_Pos, SOUND_GRENADE_FIRE);
		} break;

		case WEAPON_LASER:
		{
			float LaserReach = pChar->GetTuning(pChar->m_TuneZone)->m_LaserReach;

			new CLaser(&GameServer()->m_World, pChar->m_Pos, Direction, LaserReach, pChar->GetPlayer()->GetCid(), WEAPON_LASER);
			GameServer()->CreateSound(pChar->m_Pos, SOUND_LASER_FIRE, pChar->TeamMask());
		} break;

	}

	pChar->GetAttackTickKZ() = Server()->Tick();

	if(pChar->GetCoreKZ().m_aWeapons[pChar->GetActiveWeapon()].m_Ammo > 0) // -1 == unlimited
		pChar->GetCoreKZ().m_aWeapons[pChar->GetActiveWeapon()].m_Ammo--;

	if(!pChar->GetReloadTimerKZ())
	{
		float FireDelay;
		pChar->GetTuning(pChar->m_TuneZone)->Get(offsetof(CTuningParams, m_HammerFireDelay) / sizeof(CTuneParam) + pChar->GetCoreKZ().m_ActiveWeapon, &FireDelay);
		pChar->GetReloadTimerKZ() = FireDelay * Server()->TickSpeed() / 1000;
	}

	return true;
}

int CGameControllerBasePvP::OnCharacterDeath(CCharacter *pVictim, CPlayer *pKiller, int Weapon)
{
	if(!pVictim || !pKiller)
		return 0;

	CPlayer *pPlayer = pVictim->GetPlayer();

	if(!pPlayer)
		return 0;

	if(pPlayer == pKiller)
	{
		pKiller->m_ScoreKZ--;
		return -1;
	}
	else
	{
		pKiller->m_ScoreKZ++;
		return 1;
	}

	return 0;
}

bool CGameControllerBasePvP::OnEntity(int Index, int x, int y, int Layer, int Flags, bool Initial, int Number)
{

	if(Index == ENTITY_ARMOR_1 || Index == ENTITY_HEALTH_1 || Index == ENTITY_WEAPON_GRENADE || Index == ENTITY_WEAPON_LASER || Index == ENTITY_WEAPON_SHOTGUN || Index == ENTITY_POWERUP_NINJA)
	{
		if(m_InstagibWeapon != -1)
		        return false;
	}
	else
	{
		return CGameControllerBaseKZ::OnEntity(Index, x, y, Layer, Flags, Initial, Number);
	}

	const vec2 Pos(x * 32.0f + 16.0f, y * 32.0f + 16.0f);

	int Type = -1;
	int SubType = 0;

	if(Index == ENTITY_ARMOR_1)
		Type = POWERUP_ARMOR;
	else if(Index == ENTITY_HEALTH_1)
		Type = POWERUP_HEALTH;
	else if(Index == ENTITY_WEAPON_SHOTGUN)
	{
		Type = POWERUP_WEAPON;
		SubType = WEAPON_SHOTGUN;
	}
	else if(Index == ENTITY_WEAPON_GRENADE)
	{
		Type = POWERUP_WEAPON;
		SubType = WEAPON_GRENADE;
	}
	else if(Index == ENTITY_WEAPON_LASER)
	{
		Type = POWERUP_WEAPON;
		SubType = WEAPON_LASER;
	}
	else if(Index == ENTITY_POWERUP_NINJA)
	{
		Type = POWERUP_NINJA;
		SubType = WEAPON_NINJA;
	}

	if(Type != -1)
	{
		int PickupFlags = TileFlagsToPickupFlags(Flags);
		CPickupPvP *pPickup = new CPickupPvP(&GameServer()->m_World, Type, SubType, Layer, Number, PickupFlags);
		pPickup->m_Pos = Pos;
		return true;
	}

	return false;
}

void CGameControllerBasePvP::OnCharacterSpawn(CCharacter *pChr)
{
	CGameControllerBaseKZ::OnCharacterSpawn(pChr);

	if(!pChr)
		return;

	pChr->SetWeaponAmmo(WEAPON_GUN, 10);

	if(m_InstagibWeapon == -1)
		return;

	pChr->GiveWeapon(WEAPON_HAMMER,true);
	pChr->GiveWeapon(WEAPON_GUN,true);

	pChr->GiveWeapon(m_InstagibWeapon);
	pChr->SetActiveWeapon(m_InstagibWeapon);

}
