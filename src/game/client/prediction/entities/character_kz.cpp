// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// KaizoPredictNormalTiles has code from character.cpp (server version)
// KaizoPredictFireWeapon has code from character.cpp

#include <engine/shared/config.h>
#include <game/collision.h>
#include "character.h"
#include <generated/client_data.h>
#include "projectile.h"

void CCharacter::KaizoPredictNormalTiles(int Index)
{
    int MapIndex = Index;

    if(g_Config.m_KaizoPredictDDNetTeleport && GameWorld()->m_WorldConfig.m_IsDDRace)
    {
        int z = Collision()->IsTeleport(MapIndex);

        if(!g_Config.m_SvOldTeleportHook && !g_Config.m_SvOldTeleportWeapons)
        {
            if(g_Config.m_KaizoPredictTeleToDeath && z)
            {
                //check if all tele-outs lead to death tiles
                bool AllLeadToDeath = true;
                if(Collision()->TeleOuts(z - 1).size() > 0)
                {
                    for(vec2 TeleOutPos : Collision()->TeleOuts(z - 1))
                    {
                        if(Collision()->GetTile(round_to_int(TeleOutPos.x),round_to_int(TeleOutPos.y)) != TILE_DEATH &&
                            Collision()->GetFrontTile(round_to_int(TeleOutPos.x),round_to_int(TeleOutPos.y)) != TILE_DEATH)
                        {
                            AllLeadToDeath = false;
                            break;
                        }
                    }

                    if(AllLeadToDeath)
                    {
                        m_IsInDeathTile = true;
                        return;
                    }
                }
            }
            if(z && Collision()->TeleOuts(z - 1).size() == 1)
            {
                if(m_Core.m_Super || m_Core.m_Invincible)
                    return;
                int TeleOut = GameWorld()->m_Core.RandomOr0(Collision()->TeleOuts(z - 1).size());
                m_Core.m_Pos = Collision()->TeleOuts(z - 1)[TeleOut];
                m_DontMixPredictedPos = true; //+KZ added this
                if(!g_Config.m_SvTeleportHoldHook)
                {
                    ResetHook();
                }
                if(g_Config.m_SvTeleportLoseWeapons)
                    ResetPickups();
                return;
            }
        }
        int evilz = Collision()->IsEvilTeleport(MapIndex);
        if(g_Config.m_KaizoPredictTeleToDeath && evilz)
        {
            // check if all tele-outs lead to death tiles
            bool AllLeadToDeath = true;
            if(Collision()->TeleOuts(evilz - 1).size() > 0)
            {
                for(vec2 TeleOutPos : Collision()->TeleOuts(evilz - 1))
                {
                    if(Collision()->GetTile(round_to_int(TeleOutPos.x),round_to_int(TeleOutPos.y)) != TILE_DEATH &&
                        Collision()->GetFrontTile(round_to_int(TeleOutPos.x),round_to_int(TeleOutPos.y)) != TILE_DEATH)
                    {
                        AllLeadToDeath = false;
                        break;
                    }
                }

                if(AllLeadToDeath)
                {
                    m_IsInDeathTile = true;
                    return;
                }
            }
        }
	    if(evilz && Collision()->TeleOuts(evilz - 1).size() == 1)
        {
            if(m_Core.m_Super || m_Core.m_Invincible)
                return;
            int TeleOut = GameWorld()->m_Core.RandomOr0(Collision()->TeleOuts(evilz - 1).size());
            m_Core.m_Pos = Collision()->TeleOuts(evilz - 1)[TeleOut];
            m_DontMixPredictedPos = true; //+KZ added this
            if(!g_Config.m_SvOldTeleportHook && !g_Config.m_SvOldTeleportWeapons)
            {
                m_Core.m_Vel = vec2(0, 0);

                if(!g_Config.m_SvTeleportHoldHook)
                {
                    ResetHook();
                    GameWorld()->ReleaseHooked(GetCid());
                }
                if(g_Config.m_SvTeleportLoseWeapons)
                {
                    ResetPickups();
                }
            }
            return;
        }
        if(Collision()->IsCheckEvilTeleport(MapIndex))
        {
            if(m_Core.m_Super || m_Core.m_Invincible)
                return;
            // first check if there is a TeleCheckOut for the current recorded checkpoint, if not check previous checkpoints
            for(int k = m_TeleCheckpoint - 1; k >= 0; k--)
            {
                if(Collision()->TeleCheckOuts(k).size() == 1)
                {
                    int TeleOut = GameWorld()->m_Core.RandomOr0(Collision()->TeleCheckOuts(k).size());

                    vec2 TeleOutPos = Collision()->TeleCheckOuts(k)[TeleOut];
                    if(g_Config.m_KaizoPredictTeleToDeath && (Collision()->GetTile(round_to_int(TeleOutPos.x),round_to_int(TeleOutPos.y)) == TILE_DEATH ||
                        Collision()->GetFrontTile(round_to_int(TeleOutPos.x),round_to_int(TeleOutPos.y)) == TILE_DEATH))
                    {
                        m_IsInDeathTile = true;
                        return;
                    }

                    m_Core.m_Pos = Collision()->TeleCheckOuts(k)[TeleOut];
                    m_Core.m_Vel = vec2(0, 0);
                    m_DontMixPredictedPos = true; //+KZ added this

                    if(!g_Config.m_SvTeleportHoldHook)
                    {
                        ResetHook();
                        GameWorld()->ReleaseHooked(GetCid());
                    }

                    return;
                }
            }
            // if no checkpointout have been found (or if there no recorded checkpoint), teleport to start
            return;
        }
        if(Collision()->IsCheckTeleport(MapIndex))
        {
            if(m_Core.m_Super || m_Core.m_Invincible)
                return;
            // first check if there is a TeleCheckOut for the current recorded checkpoint, if not check previous checkpoints
            for(int k = m_TeleCheckpoint - 1; k >= 0; k--)
            {
                if(Collision()->TeleCheckOuts(k).size() == 1)
                {
                    int TeleOut = GameWorld()->m_Core.RandomOr0(Collision()->TeleCheckOuts(k).size());

                    vec2 TeleOutPos = Collision()->TeleCheckOuts(k)[TeleOut];
                    if(g_Config.m_KaizoPredictTeleToDeath && (Collision()->GetTile(round_to_int(TeleOutPos.x),round_to_int(TeleOutPos.y)) == TILE_DEATH ||
                        Collision()->GetFrontTile(round_to_int(TeleOutPos.x),round_to_int(TeleOutPos.y)) == TILE_DEATH))
                    {
                        m_IsInDeathTile = true;
                        return;
                    }

                    m_Core.m_Pos = Collision()->TeleCheckOuts(k)[TeleOut];
                    m_DontMixPredictedPos = true; //+KZ added this

                    if(!g_Config.m_SvTeleportHoldHook)
                    {
                        ResetHook();
                    }

                    return;
                }
            }
            // if no checkpointout have been found (or if there no recorded checkpoint), teleport to start
            return;
        }
    }
    // From Pointer's TW+
    if(g_Config.m_KaizoPredictPointerTWPlus)
    {
        if(GameWorld()->m_WorldConfig.m_IsPointerTWPlus)
        {
            int CornersTileIds[4];

            CornersTileIds[0] = Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x + m_ProximityRadius / 3.f, m_Pos.y - m_ProximityRadius / 3.f)));
            CornersTileIds[1] = Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x + m_ProximityRadius / 3.f, m_Pos.y + m_ProximityRadius / 3.f)));
            CornersTileIds[2] = Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x - m_ProximityRadius / 3.f, m_Pos.y - m_ProximityRadius / 3.f)));
            CornersTileIds[3] = Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x - m_ProximityRadius / 3.f, m_Pos.y + m_ProximityRadius / 3.f)));

            // speedup
            if((CornersTileIds[0] == POINTER_TILE_SPEEDUPFAST ||
                    CornersTileIds[1] == POINTER_TILE_SPEEDUPFAST ||
                    CornersTileIds[2] == POINTER_TILE_SPEEDUPFAST ||
                    CornersTileIds[3] == POINTER_TILE_SPEEDUPFAST))
            {
                m_Core.m_Vel += {0, -5};
            }
            
            if(m_Core.m_TriggeredEvents & COREEVENT_GROUND_JUMP)
            {
                if (Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x - m_ProximityRadius / 3.f, m_Pos.y + 2*32))) == POINTER_TILE_SPEEDUPFAST ||
                    Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x + m_ProximityRadius / 3.f, m_Pos.y + 2*32))) == POINTER_TILE_SPEEDUPFAST)
                    m_Core.m_Vel = {m_Core.m_Vel.x, m_Core.m_Vel.y * 2};
            }

            // teleports
            if (CornersTileIds[0] == POINTER_TILE_TELEONE ||
                CornersTileIds[1] == POINTER_TILE_TELEONE ||
                CornersTileIds[2] == POINTER_TILE_TELEONE ||
                CornersTileIds[3] == POINTER_TILE_TELEONE)
            {
                if (!m_InPointerTele) {
                    m_InPointerTele = true;
                    int x = GameWorld()->m_PointerTelePositions[0].m_X;
                    int y = GameWorld()->m_PointerTelePositions[0].m_Y;
                    int tx = GameWorld()->m_PointerTelePositions[1].m_X;
                    int ty = GameWorld()->m_PointerTelePositions[1].m_Y;
                    vec2 start = {(float)x, (float)y};
                    vec2 end = {(float)tx, (float)ty};
                    m_Pos = m_Pos - start * 32 + end * 32;
                    m_Core.m_Pos = m_Pos;
                }
            }
            else if (CornersTileIds[0] == POINTER_TILE_TELETWO ||
                    CornersTileIds[1] == POINTER_TILE_TELETWO ||
                    CornersTileIds[2] == POINTER_TILE_TELETWO ||
                    CornersTileIds[3] == POINTER_TILE_TELETWO)
            {
                if (!m_InPointerTele) {
                    m_InPointerTele = true;
                    int x = GameWorld()->m_PointerTelePositions[1].m_X;
                    int y = GameWorld()->m_PointerTelePositions[1].m_Y;
                    int tx = GameWorld()->m_PointerTelePositions[0].m_X;
                    int ty = GameWorld()->m_PointerTelePositions[0].m_Y;
                    vec2 start = {(float)x, (float)y};
                    vec2 end = {(float)tx, (float)ty};
                    m_Pos = m_Pos - start * 32 + end * 32;
                    m_Core.m_Pos = m_Pos;
                }
            }
            else if (CornersTileIds[0] == POINTER_TILE_TELETHREE ||
                    CornersTileIds[1] == POINTER_TILE_TELETHREE ||
                    CornersTileIds[2] == POINTER_TILE_TELETHREE ||
                    CornersTileIds[3] == POINTER_TILE_TELETHREE)
            {
                if (!m_InPointerTele) {
                    m_InPointerTele = true;
                    int x = GameWorld()->m_PointerTelePositions[2].m_X;
                    int y = GameWorld()->m_PointerTelePositions[2].m_Y;
                    int tx = GameWorld()->m_PointerTelePositions[3].m_X;
                    int ty = GameWorld()->m_PointerTelePositions[3].m_Y;
                    vec2 start = {(float)x, (float)y};
                    vec2 end = {(float)tx, (float)ty};
                    m_Pos = m_Pos - start * 32 + end * 32;
                    m_Core.m_Pos = m_Pos;
                }
            }
            else if (CornersTileIds[0] == POINTER_TILE_TELEFOUR ||
                    CornersTileIds[1] == POINTER_TILE_TELEFOUR ||
                    CornersTileIds[2] == POINTER_TILE_TELEFOUR ||
                    CornersTileIds[3] == POINTER_TILE_TELEFOUR)
            {
                if (!m_InPointerTele) {
                    m_InPointerTele = true;
                    int x = GameWorld()->m_PointerTelePositions[3].m_X;
                    int y = GameWorld()->m_PointerTelePositions[3].m_Y;
                    int tx = GameWorld()->m_PointerTelePositions[2].m_X;
                    int ty = GameWorld()->m_PointerTelePositions[2].m_Y;
                    vec2 start = {(float)x, (float)y};
                    vec2 end = {(float)tx, (float)ty};
                    m_Pos = m_Pos - start * 32 + end * 32;
                    m_Core.m_Pos = m_Pos;
                }
            }
            else
            {
                // you are not not in a teleport
                m_InPointerTele = false;
            }
        }
        else if(GameWorld()->m_WorldConfig.m_HasPointerTiles) // TW+ 0.7 version
        {

            int CornersTileIds[4];

            CornersTileIds[0] = Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x + m_ProximityRadius / 3.f, m_Pos.y - m_ProximityRadius / 3.f)));
            CornersTileIds[1] = Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x + m_ProximityRadius / 3.f, m_Pos.y + m_ProximityRadius / 3.f)));
            CornersTileIds[2] = Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x - m_ProximityRadius / 3.f, m_Pos.y - m_ProximityRadius / 3.f)));
            CornersTileIds[3] = Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x - m_ProximityRadius / 3.f, m_Pos.y + m_ProximityRadius / 3.f)));

            // speedup
            if((CornersTileIds[0] == POINTER_TILE_SPEEDUPFAST ||
                    CornersTileIds[1] == POINTER_TILE_SPEEDUPFAST ||
                    CornersTileIds[2] == POINTER_TILE_SPEEDUPFAST ||
                    CornersTileIds[3] == POINTER_TILE_SPEEDUPFAST))
            {
                m_Core.m_Vel += {0, -5};
            }
            
            if(m_Core.m_TriggeredEvents & COREEVENT_GROUND_JUMP)
            {
                if (Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x - m_ProximityRadius / 3.f, m_Pos.y + 2*32))) == POINTER_TILE_SPEEDUPFAST ||
                    Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x + m_ProximityRadius / 3.f, m_Pos.y + 2*32))) == POINTER_TILE_SPEEDUPFAST)
                    m_Core.m_Vel = {m_Core.m_Vel.x, m_Core.m_Vel.y * 2};
            }

            // teleports
            int TeleId = -1;
            if (Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x, m_Pos.y))) >= POINTER07_TILE_TELE_START &&
                Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x, m_Pos.y))) < POINTER07_TILE_TELE_START + POINTER07_NUM_TILE_TELE)
                TeleId = Collision()->GetTileIndex(Collision()->GetMapIndex(vec2(m_Pos.x, m_Pos.y))) - POINTER07_TILE_TELE_START;
                
            if (TeleId >= 0)
            {
                if (!m_InPointerTele) {
                    m_InPointerTele = true;
                    int TeleIdEnd = TeleId % 2 == 0 ? TeleId+1  : TeleId-1;
                    int x = GameWorld()->m_PointerTelePositions[TeleId].m_X;
                    int y = GameWorld()->m_PointerTelePositions[TeleId].m_Y;
                    int tx = GameWorld()->m_PointerTelePositions[TeleIdEnd].m_X;
                    int ty = GameWorld()->m_PointerTelePositions[TeleIdEnd].m_Y;
                    vec2 start = {(float)x, (float)y};
                    vec2 end = {(float)tx, (float)ty};
                    m_Pos = m_Pos - start * 32 + end * 32;
                    m_Core.m_Pos = m_Pos;
                }
            }
            else
            {
                m_InPointerTele = false;
            }
        }
    }
}

bool CCharacter::KaizoPredictFireWeapon()
{
    if(m_ReloadTimer != 0)
		return false;

    vec2 Direction = normalize(vec2(m_LatestInput.m_TargetX, m_LatestInput.m_TargetY));

    bool FullAuto = false;
    if(m_FrozenLastTick)
		FullAuto = true;

    // check if we gonna fire
	bool WillFire = false;
	if(CountInput(m_LatestPrevInput.m_Fire, m_LatestInput.m_Fire).m_Presses)
		WillFire = true;

	if(FullAuto && (m_LatestInput.m_Fire & 1) && m_Core.m_aWeapons[m_Core.m_ActiveWeapon].m_Ammo)
		WillFire = true;

	if(!WillFire)
		return false;

	// check for ammo
	if(!m_Core.m_aWeapons[m_Core.m_ActiveWeapon].m_Ammo || m_FreezeTime)
	{
		return false;
	}

    vec2 ProjStartPos = m_Pos + Direction * m_ProximityRadius * 0.75f;

    bool Fired = false;

    // From Pointer's TW+
    if(g_Config.m_KaizoPredictPointerTWPlus && GameWorld()->m_WorldConfig.m_IsPointerTWPlus)
    {
        switch(m_Core.m_ActiveWeapon)
        {
            case WEAPON_NINJA:
            {
                // reset Hit objects
                m_NumObjectsHit = 0;

                m_Core.m_Ninja.m_ActivationDir = Direction;
                m_Core.m_Ninja.m_CurrentMoveTime = g_pData->m_Weapons.m_Ninja.m_Movetime * GameWorld()->GameTickSpeed() / 1000;
                m_Core.m_Ninja.m_OldVelAmount = 10; //vel is constant in Pointer TW+ through a server config, but we will assume 10

                Fired = true;
            }
            break;
        }
    }
    else if(g_Config.m_KaizoPredictGoresGrenadeTele && g_Config.m_SvGoresGrenadeTele == 1)
    {
        switch (m_Core.m_ActiveWeapon)
        {
            case WEAPON_GRENADE:
            {
                bool gores_dontfire = false;

                for(CProjectile *pProj = (CProjectile *)GameWorld()->FindFirst(CGameWorld::ENTTYPE_PROJECTILE); pProj; pProj = (CProjectile *)pProj->TypeNext())
                {
                    if(pProj->m_GoresTeleportGrenade && pProj->GetOwnerId() == GetCid() && pProj->GetType() == WEAPON_GRENADE)
                    {
                        gores_dontfire = true;

                        bool Found;
                        vec2 PossiblePos;

                        Found = GetNearestAirPos(pProj->GetPos((GameWorld()->GameTick() - pProj->GetStartTick() - 1) / (float)GameWorld()->GameTickSpeed()), pProj->GetPos((GameWorld()->GameTick() - pProj->GetStartTick()) / (float)GameWorld()->GameTickSpeed()), &PossiblePos);

                        if(Found)
                        {
                            m_Core.m_Pos = PossiblePos;
                            m_Core.m_Vel = vec2(0, 0);
                            m_DontMixPredictedPos = true;

                            pProj->Reset();
                        }

                        Fired = true;

                        break;
                    }
                }

                if(!gores_dontfire)
                {
                    int Lifetime = (int)(GameWorld()->GameTickSpeed() * GetTuning(m_TuneZone)->m_GrenadeLifetime);

                    CProjectile *p = new CProjectile(
                        GameWorld(),
                        WEAPON_GRENADE, // Type
                        GetCid(), // Owner
                        ProjStartPos, // Pos
                        Direction, // Dir
                        Lifetime, // Span
                        false, // Freeze
                        true, // Explosive
                        SOUND_GRENADE_EXPLODE // SoundImpact
                    ); // SoundImpact

                    if(!p)
                        return false;

                    p->m_GoresTeleportGrenade = true;

                    Fired = true;
                }
	        }
            break;
        }
    }

    if(Fired)
    {
        m_AttackTick = GameWorld()->GameTick(); // NOLINT(clang-analyzer-unix.Malloc)

        if(!m_ReloadTimer)
        {
            float FireDelay;
            GetTuning(GetOverriddenTuneZone())->Get(offsetof(CTuningParams, m_HammerFireDelay) / sizeof(CTuneParam) + m_Core.m_ActiveWeapon, &FireDelay);

            m_ReloadTimer = FireDelay * GameWorld()->GameTickSpeed() / 1000;
        }
    }

    return Fired;
}

void CCharacter::ResetPickups()
{
	for(int i = WEAPON_SHOTGUN; i < NUM_WEAPONS - 1; i++)
	{
		m_Core.m_aWeapons[i].m_Got = false;
		if(m_Core.m_ActiveWeapon == i)
			m_Core.m_ActiveWeapon = WEAPON_GUN;
	}
}