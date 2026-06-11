// Copyright (C) Benjamín Gajardo (also known as +KZ)

// RenderKaizoWeapon() contains code from players.cpp and DuckDDNet

#include <generated/client_data.h>
#include <game/client/gameclient.h>
#include <game/client/animstate.h>
#include "players.h"
#include <game/mapitems.h>

#include <game/client/prediction/entities/character.h>
#include <game/client/prediction/entities/laser.h>
#include <game/client/prediction/entities/projectile.h>
#include <game/client/prediction/entity.h>
#include <game/client/prediction/gameworld.h>

//+KZ
extern int g_KaizoConfig_KaizoGrenadePath;
extern int g_KaizoConfig_KaizoLaserPath;
extern int g_KaizoConfig_KaizoEmotionalTees;

void CPlayers::OnKaizoInit()
{
    float ScaleX, ScaleY;
    Graphics()->GetSpriteScale(&g_pData->m_aSprites[SPRITE_KZ_PORTAL], ScaleX, ScaleY);
    Graphics()->QuadsSetSubset(0, 0, 1, 1);
	m_KaizoWeaponsOffsets[KZ_CUSTOM_WEAPON_PORTAL_GUN - KZ_CUSTOM_WEAPONS_START] = Graphics()->QuadContainerAddSprite(m_WeaponEmoteQuadContainerIndex, 92 * ScaleX, 92 * ScaleY);

    Graphics()->GetSpriteScale(&g_pData->m_aSprites[SPRITE_KZ_PORTAL_ORANGE], ScaleX, ScaleY);
    Graphics()->QuadsSetSubset(0, 0, 1, 1);
	m_KaizoWeaponsOrangePortalGunOffset = Graphics()->QuadContainerAddSprite(m_WeaponEmoteQuadContainerIndex, 92 * ScaleX, 92 * ScaleY);

    Graphics()->GetSpriteScale(&g_pData->m_aSprites[SPRITE_KZ_ATTRACTOR], ScaleX, ScaleY);
    Graphics()->QuadsSetSubset(0, 0, 1, 1);
	m_KaizoWeaponsOffsets[KZ_CUSTOM_WEAPON_ATTRACTOR_BEAM - KZ_CUSTOM_WEAPONS_START] = Graphics()->QuadContainerAddSprite(m_WeaponEmoteQuadContainerIndex, 92 * ScaleX, 92 * ScaleY);
}

void CPlayers::RenderKaizoWeapon(const CNetObj_Character *pPrevChar, const CNetObj_Character *pPlayerChar, const CTeeRenderInfo *pRenderInfo, int ClientId, float Intra, CAnimState &AnimState)
{
    if(GameClient()->m_aClients[ClientId].m_KaizoCustomWeapon >= KZ_CUSTOM_WEAPONS_END)
        return;

	float IntraTick = Intra;
	if(ClientId >= 0)
		IntraTick = GameClient()->m_aClients[ClientId].m_IsPredicted ? Client()->PredIntraGameTick(g_Config.m_ClDummy) : Client()->IntraGameTick(g_Config.m_ClDummy);
	float Angle = GetPlayerTargetAngle(pPrevChar, pPlayerChar, ClientId, IntraTick);
	vec2 Direction = direction(Angle);
	vec2 Position;
	if(in_range(ClientId, MAX_CLIENTS - 1))
		Position = GameClient()->m_aClients[ClientId].m_RenderPos;
	else
		Position = mix(vec2(pPrevChar->m_X, pPrevChar->m_Y), vec2(pPlayerChar->m_X, pPlayerChar->m_Y), IntraTick);
	vec2 Vel = mix(vec2(pPrevChar->m_VelX / 256.0f, pPrevChar->m_VelY / 256.0f), vec2(pPlayerChar->m_VelX / 256.0f, pPlayerChar->m_VelY / 256.0f), IntraTick);
	bool Stationary = pPlayerChar->m_VelX <= 1 && pPlayerChar->m_VelX >= -1;
	bool InAir = !Collision()->CheckPoint(pPlayerChar->m_X, pPlayerChar->m_Y + 16);
	bool Running = pPlayerChar->m_VelX >= 5000 || pPlayerChar->m_VelX <= -5000;
	bool WantOtherDir = (pPlayerChar->m_Direction == -1 && Vel.x > 0) || (pPlayerChar->m_Direction == 1 && Vel.x < 0);
	bool Inactive = ClientId >= 0 && (GameClient()->m_aClients[ClientId].m_Afk || GameClient()->m_aClients[ClientId].m_Paused);
	bool IsSit = Inactive && !InAir && Stationary;

	bool Local = GameClient()->m_Snap.m_LocalClientId == ClientId;

	switch(GameClient()->m_aClients[ClientId].m_KaizoCustomWeapon)
	{
		case KZ_CUSTOM_WEAPON_PORTAL_GUN - KZ_CUSTOM_WEAPONS_START:
		{
			vec2 WeaponPosition = Position + Direction * 24.f;
			//WeaponPosition.y += -2.f;
			if(IsSit)
				WeaponPosition.y += 3.0f;
			Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
			Graphics()->QuadsSetRotation(AnimState.GetAttach()->m_Angle * pi * 2 + (Direction.x < 0 ? -Angle + pi : Angle));
			if(GameClient()->m_aClients[ClientId].m_CharFlags & KAIZOCHARACTERFLAG_BLUEPORTAL)
			{
				Graphics()->TextureSet(g_pData->m_aImages[IMAGE_KZ_PORTAL].m_Id);
				Graphics()->RenderQuadContainerAsSprite(m_WeaponEmoteQuadContainerIndex, m_KaizoWeaponsOffsets[KZ_CUSTOM_WEAPON_PORTAL_GUN - KZ_CUSTOM_WEAPONS_START], WeaponPosition.x, WeaponPosition.y, Direction.x < 0 ? -1.0f : 1.0f);
			}
			else
			{
				Graphics()->TextureSet(g_pData->m_aImages[IMAGE_KZ_PORTAL_ORANGE].m_Id);
				Graphics()->RenderQuadContainerAsSprite(m_WeaponEmoteQuadContainerIndex, m_KaizoWeaponsOrangePortalGunOffset, WeaponPosition.x, WeaponPosition.y, Direction.x < 0 ? -1.0f : 1.0f);
			}
		}
		break;
		case KZ_CUSTOM_WEAPON_ATTRACTOR_BEAM - KZ_CUSTOM_WEAPONS_START:
		{
			vec2 WeaponPosition = Position + Direction * 24.f;
			WeaponPosition.y += -2.f;
			if(IsSit)
				WeaponPosition.y += 3.0f;
			Graphics()->TextureSet(g_pData->m_aImages[IMAGE_KZ_ATTRACTOR].m_Id);
			Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	    	Graphics()->QuadsSetRotation(AnimState.GetAttach()->m_Angle * pi * 2 + (Direction.x < 0 ? -Angle + pi : Angle));
	    	Graphics()->RenderQuadContainerAsSprite(m_WeaponEmoteQuadContainerIndex, m_KaizoWeaponsOffsets[KZ_CUSTOM_WEAPON_ATTRACTOR_BEAM - KZ_CUSTOM_WEAPONS_START], WeaponPosition.x, WeaponPosition.y, Direction.x < 0 ? -1.0f : 1.0f);
	    }
	    break;
	}
}

//DuckDDNet
void CPlayers::RenderDuckDDNetCollisions(const CNetObj_Character *pPrevChar, const CNetObj_Character *pPlayerChar, int ClientId, float Intra)
{
    float IntraTick = Intra;
	if(ClientId >= 0)
		IntraTick = GameClient()->m_aClients[ClientId].m_IsPredicted ? Client()->PredIntraGameTick(g_Config.m_ClDummy) : Client()->IntraGameTick(g_Config.m_ClDummy);
	float Angle = GetPlayerTargetAngle(pPrevChar, pPlayerChar, ClientId, IntraTick);
	vec2 Direction = direction(Angle);
	vec2 Position;
	if(in_range(ClientId, MAX_CLIENTS - 1))
		Position = GameClient()->m_aClients[ClientId].m_RenderPos;
	else
		Position = mix(vec2(pPrevChar->m_X, pPrevChar->m_Y), vec2(pPlayerChar->m_X, pPlayerChar->m_Y), IntraTick);
	vec2 Vel = mix(vec2(pPrevChar->m_VelX / 256.0f, pPrevChar->m_VelY / 256.0f), vec2(pPlayerChar->m_VelX / 256.0f, pPlayerChar->m_VelY / 256.0f), IntraTick);
	bool Stationary = pPlayerChar->m_VelX <= 1 && pPlayerChar->m_VelX >= -1;
	bool InAir = !Collision()->CheckPoint(pPlayerChar->m_X, pPlayerChar->m_Y + 16);
	bool Running = pPlayerChar->m_VelX >= 5000 || pPlayerChar->m_VelX <= -5000;
	bool WantOtherDir = (pPlayerChar->m_Direction == -1 && Vel.x > 0) || (pPlayerChar->m_Direction == 1 && Vel.x < 0);
	bool Inactive = ClientId >= 0 && (GameClient()->m_aClients[ClientId].m_Afk || GameClient()->m_aClients[ClientId].m_Paused);
	bool IsSit = Inactive && !InAir && Stationary;

	bool Local = GameClient()->m_Snap.m_LocalClientId == ClientId;

    bool Aim = (pPlayerChar->m_PlayerFlags & PLAYERFLAG_AIM);
	if(!Client()->ServerCapAnyPlayerFlag())
	{
		for(int i = 0; i < NUM_DUMMIES; i++)
		{
			if(ClientId == GameClient()->m_aLocalIds[i])
			{
				Aim = GameClient()->m_Controls.m_aShowHookColl[i];
				break;
			}
		}
	}

    if(GameClient()->PredictDummy() && g_Config.m_ClDummyCopyMoves && GameClient()->m_aLocalIds[!g_Config.m_ClDummy] == ClientId)
		Aim = false; // don't use unpredicted with copy moves

    bool AlwaysRenderHookColl = (Local ? g_Config.m_ClShowHookCollOwn : g_Config.m_ClShowHookCollOther) == 2;
	bool RenderHookCollPlayer = Aim && (Local ? g_Config.m_ClShowHookCollOwn : g_Config.m_ClShowHookCollOther) > 0;
	if(Local && Client()->State() != IClient::STATE_DEMOPLAYBACK)
		RenderHookCollPlayer = GameClient()->m_Controls.m_aShowHookColl[g_Config.m_ClDummy] && g_Config.m_ClShowHookCollOwn > 0;

	if(GameClient()->PredictDummy() && g_Config.m_ClDummyCopyMoves &&
		GameClient()->m_aLocalIds[!g_Config.m_ClDummy] == ClientId && GameClient()->m_Controls.m_aShowHookColl[g_Config.m_ClDummy] &&
		Client()->State() != IClient::STATE_DEMOPLAYBACK)
	{
		RenderHookCollPlayer = g_Config.m_ClShowHookCollOther > 0;
	}

	
        // DClient Projectile prediction
        if(g_KaizoConfig_KaizoGrenadePath && (pPlayerChar->m_Weapon == WEAPON_GRENADE || pPlayerChar->m_Weapon == WEAPON_GUN) && g_Config.m_ClShowOthersAlpha > 0)
        {
            bool PredEvents = GameClient()->m_PredictedWorld.m_WorldConfig.m_PredictEvents;
            GameClient()->m_PredictedWorld.m_WorldConfig.m_PredictEvents = false; //dont predict events for this temp projectile

            float Alpha = GameClient()->IsOtherTeam(ClientId) ? g_Config.m_ClShowOthersAlpha / 100.0f : 1.0f;
            Alpha *= (float)g_Config.m_ClHookCollAlpha / 100;

            int projlifetime = (int)(GameClient()->m_PredictedWorld.GameTickSpeed() * (pPlayerChar->m_Weapon == WEAPON_GRENADE ? GameClient()->m_PredictedWorld.GlobalTuning()->m_GrenadeLifetime : GameClient()->m_PredictedWorld.GlobalTuning()->m_GunLifetime));
            vec2 OldGPos = vec2(Position.x, Position.y) + Direction * CCharacterCore::PhysicalSize() * 0.75f;
            CProjectile *proj = new CProjectile(&GameClient()->m_PredictedWorld, pPlayerChar->m_Weapon, ClientId, OldGPos, Direction, projlifetime, false, false, 0, LAYER_GAME, -1);
            Graphics()->TextureClear();
            Graphics()->LinesBegin();
            Graphics()->SetColor(color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClHookCollColorNoColl)).WithAlpha(Alpha));
            int InitialTick = proj->GameWorld()->m_GameTick;
            vec2 FinalGPos = Position;
            for(int i = 1; !proj->IsMarkedForDestroy(); i++)
            {
                proj->GameWorld()->m_GameTick = InitialTick + i;
                proj->Tick();
                if(!proj->IsMarkedForDestroy())
                {
                    vec2 NewGPos;
                    NewGPos = proj->GetPos((float)i / (float)GameClient()->m_GameWorld.GameTickSpeed());
                    IGraphics::CLineItem LineItem(OldGPos.x, OldGPos.y, NewGPos.x, NewGPos.y);
                    Graphics()->LinesDraw(&LineItem, 1);
                    OldGPos = NewGPos;
                    FinalGPos = NewGPos;
                }
                else
                {
                    float Pt = (proj->GameWorld()->GameTick() - proj->GetStartTick() - 1) / (float)proj->GameWorld()->GameTickSpeed();
                    float Ct = (proj->GameWorld()->GameTick() - proj->GetStartTick()) / (float)proj->GameWorld()->GameTickSpeed();
                    vec2 projPrevPos = proj->GetPos(Pt);
                    vec2 projCurPos = proj->GetPos(Ct);
                    vec2 projColPos;
                    vec2 projNewPos;
                    int Collide = proj->Collision()->IntersectLine(projPrevPos, projCurPos, &projColPos, &projNewPos);
                    if(Collide)
                    {
                        FinalGPos = projColPos;
                    }
                    else
                    {
                        FinalGPos = projNewPos;
                    }
                    IGraphics::CLineItem LineItem(OldGPos.x, OldGPos.y, FinalGPos.x, FinalGPos.y);
                    Graphics()->LinesDraw(&LineItem, 1);
                    break;
                }
            }
            proj->GameWorld()->m_GameTick = InitialTick;
            GameClient()->m_PredictedWorld.RemoveEntity(proj);
            delete proj;
            Graphics()->LinesEnd();
            Graphics()->QuadsBegin();
            Graphics()->SetColor(color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClHookCollColorNoColl)).WithAlpha(Alpha));
            const float MarkerSize = 16.0f;
            IGraphics::CQuadItem QuadItem(FinalGPos.x, FinalGPos.y, MarkerSize, MarkerSize);
            Graphics()->QuadsDraw(&QuadItem, 1);
            Graphics()->QuadsEnd();

            GameClient()->m_PredictedWorld.m_WorldConfig.m_PredictEvents = PredEvents; //restore predict events state
        }
        // DClient FNG laser prediction
        if(Local && g_KaizoConfig_KaizoLaserPath && pPlayerChar->m_Weapon == WEAPON_LASER && (GameClient()->m_GameWorld.m_WorldConfig.m_IsFNG || AlwaysRenderHookColl || RenderHookCollPlayer) && g_Config.m_ClShowOthersAlpha > 0)
        {
            bool PredEvents = GameClient()->m_PredictedWorld.m_WorldConfig.m_PredictEvents;
            GameClient()->m_PredictedWorld.m_WorldConfig.m_PredictEvents = false; //dont predict events for this temp laser

            float Alpha = GameClient()->IsOtherTeam(ClientId) ? g_Config.m_ClShowOthersAlpha / 100.0f : 1.0f;
            Alpha *= (float)g_Config.m_ClHookCollAlpha / 100;

            float LaserReach = GameClient()->m_PredictedWorld.GlobalTuning()->m_LaserReach;
            CLaser *Proj = new CLaser(&GameClient()->m_PredictedWorld, Position, Direction, LaserReach, ClientId, WEAPON_LASER);
            int InitialTick = Proj->GameWorld()->m_GameTick;
            vec2 PrevPos = Position;
            std::vector<vec2> Lines;
            Lines.push_back(Position);

            for(int i = 1; !Proj->IsMarkedForDestroy(); i++)
            {
                Proj->GameWorld()->m_GameTick = InitialTick + i;
                Proj->Tick();
                vec2 CurPos = Proj->GetPos();

                if(CurPos != PrevPos)
                {
                    PrevPos = CurPos;
                    Lines.push_back(CurPos);
                }
            }
            bool Hit = (Proj->m_pLaserLastHit) && ((!GameClient()->m_GameWorld.m_WorldConfig.m_IsFNG) || (GameClient()->m_aClients[Proj->m_pLaserLastHit->GetCid()].m_Team != GameClient()->m_aClients[ClientId].m_Team));
            if(Hit || AlwaysRenderHookColl || RenderHookCollPlayer)
            {
                Graphics()->TextureClear();
                Graphics()->LinesBegin();
                if(Hit)
                {
                    Graphics()->SetColor(color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClHookCollColorHookableColl)).WithAlpha(Alpha));
                }
                else
                {
                    Graphics()->SetColor(color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClHookCollColorNoColl)).WithAlpha(Alpha));
                }
                for(size_t i = 1; i < Lines.size(); i++)
                {
                    IGraphics::CLineItem LineItem(Lines[i - 1].x, Lines[i - 1].y, Lines[i].x, Lines[i].y);
                    Graphics()->LinesDraw(&LineItem, 1);
                }
                Graphics()->LinesEnd();
                Graphics()->QuadsBegin();
                IGraphics::CQuadItem QuadItem;
                if(Hit)
                {
                    Graphics()->SetColor(color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClHookCollColorHookableColl)).WithAlpha(Alpha));
                    QuadItem = IGraphics::CQuadItem(PrevPos.x, PrevPos.y, 24.0, 24.0);
                }
                else
                {
                    Graphics()->SetColor(color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClHookCollColorNoColl)).WithAlpha(Alpha));
                    QuadItem = IGraphics::CQuadItem(PrevPos.x, PrevPos.y, 8.0, 8.0);
                }
                Graphics()->QuadsDraw(&QuadItem, 1);
                Graphics()->QuadsEnd();
            }

            Proj->GameWorld()->m_GameTick = InitialTick;
            GameClient()->m_PredictedWorld.RemoveEntity(Proj);
            delete Proj;

            GameClient()->m_PredictedWorld.m_WorldConfig.m_PredictEvents = PredEvents; //restore predict events state
        }
}

void CPlayers::SetPlayerEmoteKZ(CNetObj_Character &Player, int ClientId)
{
    if(g_KaizoConfig_KaizoEmotionalTees)
    {
        if(GameClient()->m_aClients[ClientId].m_EmoticonStartTick != -1)
        {
            float SinceStart = (Client()->GameTick(g_Config.m_ClDummy) - GameClient()->m_aClients[ClientId].m_EmoticonStartTick) + (Client()->IntraGameTickSincePrev(g_Config.m_ClDummy) - GameClient()->m_aClients[ClientId].m_EmoticonStartFraction);
            float FromEnd = (2 * Client()->GameTickSpeed()) - SinceStart;

            if(0 <= SinceStart && FromEnd > 0)
            {
                switch (GameClient()->m_aClients[ClientId].m_Emoticon)
                {
                case EMOTICON_DEVILTEE:
                case EMOTICON_SPLATTEE:
                case EMOTICON_ZOMG:
                    Player.m_Emote = EMOTE_ANGRY;
                    break;
                
                case EMOTICON_DOTDOT:
                case EMOTICON_DROP:
                case EMOTICON_ZZZ:
                    Player.m_Emote = EMOTE_BLINK;
                    break;
                
                case EMOTICON_EXCLAMATION:
                case EMOTICON_GHOST:
                case EMOTICON_QUESTION:
                case EMOTICON_WTF:
                    Player.m_Emote = EMOTE_SURPRISE;
                    break;

                case EMOTICON_EYES:
                case EMOTICON_HEARTS:
                case EMOTICON_MUSIC:
                    Player.m_Emote = EMOTE_HAPPY;
                    break;

                case EMOTICON_SUSHI:
                case EMOTICON_OOP:
                case EMOTICON_SORRY:
                    Player.m_Emote = EMOTE_PAIN;
                    break;
                }
            }
        }
    }
}

//using code from ddnet:
void CPlayers::CreateBOMBTeeRenderInfo()
{
    CTeeRenderInfo BOMBTeeRenderInfo;
	BOMBTeeRenderInfo.m_Size = 64.0f;
	CSkinDescriptor BOMBSkinDescriptor;
	BOMBSkinDescriptor.m_Flags |= CSkinDescriptor::FLAG_SIX;
	str_copy(BOMBSkinDescriptor.m_aSkinName, "bomb");
	m_pBOMBTeeRenderInfo = GameClient()->CreateManagedTeeRenderInfo(BOMBTeeRenderInfo, BOMBSkinDescriptor);
}
