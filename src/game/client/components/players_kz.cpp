// Copyright (C) Benjamín Gajardo (also known as +KZ)

// RenderKaizoWeapon() contains code from players.cpp

#include <generated/client_data.h>
#include <game/client/gameclient.h>
#include <game/client/animstate.h>
#include "players.h"
#include <game/mapitems.h>

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

    switch (GameClient()->m_aClients[ClientId].m_KaizoCustomWeapon)
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

void CPlayers::SetPlayerEmoteKZ(CNetObj_Character &Player, int ClientId)
{
    if(g_Config.m_KaizoEmotionalTees)
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
