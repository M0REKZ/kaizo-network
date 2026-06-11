// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// RenderKaizoPickup() has some code from items.cpp

#include <generated/client_data.h>
#include <generated/protocol.h>
#include <game/client/gameclient.h>
#include <game/client/render.h>
#include "items.h"

//+KZ
extern int g_KaizoConfig_KaizoShowCrowns;

void CItems::OnInitKZ()
{
	Graphics()->QuadsSetSubset(0, 0, 1, 1);
	m_CrownOffset = Graphics()->QuadContainerAddSprite(m_ItemsQuadContainerIndex, -21.f, -21.f, 42.f, 42.f);

	Graphics()->QuadsSetSubset(0, 0, 1, 1);
	m_TurretOffset_1 = Graphics()->QuadContainerAddSprite(m_ItemsQuadContainerIndex, -16.f, -16.f, 32.f, 32.f);

	Graphics()->QuadsSetSubset(0, 0, 1, 1);
	m_TurretOffset_2 = Graphics()->QuadContainerAddSprite(m_ItemsQuadContainerIndex, -16.f, -16.f, 32.f, 32.f);

	Graphics()->QuadsSetSubset(0, 0, 1, 1);
	m_MineOffset = Graphics()->QuadContainerAddSprite(m_ItemsQuadContainerIndex, -16.f, -16.f, 32.f, 32.f);

	//TODO: maybe find a way to deduplicate the code below with the one in players_kz.cpp

	float ScaleX, ScaleY;
    Graphics()->GetSpriteScale(&g_pData->m_aSprites[SPRITE_KZ_PORTAL], ScaleX, ScaleY);
    Graphics()->QuadsSetSubset(0, 0, 1, 1);
	m_KaizoWeaponsOffsets[KZ_CUSTOM_WEAPON_PORTAL_GUN - KZ_CUSTOM_WEAPONS_START] = Graphics()->QuadContainerAddSprite(m_ItemsQuadContainerIndex, 92 * ScaleX, 92 * ScaleY);
    Graphics()->GetSpriteScale(&g_pData->m_aSprites[SPRITE_KZ_ATTRACTOR], ScaleX, ScaleY);
    Graphics()->QuadsSetSubset(0, 0, 1, 1);
	m_KaizoWeaponsOffsets[KZ_CUSTOM_WEAPON_ATTRACTOR_BEAM - KZ_CUSTOM_WEAPONS_START] = Graphics()->QuadContainerAddSprite(m_ItemsQuadContainerIndex, 92 * ScaleX, 92 * ScaleY);
}

void CItems::RenderCrown()
{
	if(!g_KaizoConfig_KaizoShowCrowns)
		return;

	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		if(!GameClient()->m_Snap.m_aCharacters[i].m_Active || GameClient()->m_aClients[i].m_Team == TEAM_SPECTATORS)
			continue;

		if(GameClient()->m_aClients[i].m_CrownTick != -1 && GameClient()->m_aClients[i].m_CrownTick + Client()->GameTickSpeed() > Client()->GameTick(g_Config.m_ClDummy))
		{
			Graphics()->TextureSet(g_pData->m_aImages[IMAGE_KZ_CROWN].m_Id);
			Graphics()->QuadsSetRotation(0);
			Graphics()->SetColor(1.f, 1.f, 1.f, 1.f);
			Graphics()->RenderQuadContainerAsSprite(m_ItemsQuadContainerIndex, m_CrownOffset, GameClient()->m_aClients[i].m_RenderPos.x, GameClient()->m_aClients[i].m_RenderPos.y - 64.0f, 1.0f, 1.0f);
		}
	}
}

void CItems::HandleKaizoSnapItem(const IClient::CSnapItem &Item, bool Front)
{
	if(Front)
	{
		//TODO
	}
	else
	{
		if(Item.m_Type == NETOBJTYPE_KAIZONETWORKTURRET)
		{
			RenderTurret((CNetObj_KaizoNetworkTurret *)Item.m_pData);
		}
		else if(Item.m_Type == NETOBJTYPE_KAIZONETWORKMINE)
		{
			RenderMine((CNetObj_KaizoNetworkMine *)Item.m_pData);
		}
		else if(Item.m_Type == NETOBJTYPE_KAIZONETWORKPICKUP)
		{
			RenderKaizoPickup((CNetObj_KaizoNetworkPickup *)Item.m_pData);
		}
	}
}

void CItems::RenderTurret(CNetObj_KaizoNetworkTurret *pTurret)
{
    if(!pTurret)
		return;

	int Type = pTurret->m_Type;
	int Offset = 0;

	if(Type == 0)
	{
		Graphics()->TextureSet(g_pData->m_aImages[IMAGE_KZ_TURRET_1].m_Id);
		Offset = m_TurretOffset_1;
	}
	else if(Type == 1)
	{
		Graphics()->TextureSet(g_pData->m_aImages[IMAGE_KZ_TURRET_2].m_Id);
		Offset = m_TurretOffset_2;
	}
	else
	{	//Fallback to 1
		Graphics()->TextureSet(g_pData->m_aImages[IMAGE_KZ_TURRET_1].m_Id);
		Offset = m_TurretOffset_1;
	}

	Graphics()->QuadsSetRotation(Client()->GameTick(g_Config.m_ClDummy) * pi * 2 / 3);
	Graphics()->SetColor(1.f, 1.f, 1.f, 1.f);
	Graphics()->RenderQuadContainerAsSprite(m_ItemsQuadContainerIndex, Offset, pTurret->m_X, pTurret->m_Y, 1.0f, 1.0f);
}

void CItems::RenderMine(CNetObj_KaizoNetworkMine *pMine)
{
	if(!pMine)
		return;

	int Tick = Client()->GameTick(g_Config.m_ClDummy);

	Graphics()->TextureSet(g_pData->m_aImages[IMAGE_KZ_MINE].m_Id);
	Graphics()->QuadsSetRotation(Tick%360 * pi/180);
	Graphics()->SetColor(1.f, 1.f, 1.f, 1.f);
	Graphics()->RenderQuadContainerAsSprite(m_ItemsQuadContainerIndex, m_MineOffset, pMine->m_X, pMine->m_Y + 8 * sin((float)Tick / 25.0f), 1.0f, 1.0f);
}

void CItems::RenderKaizoPickup(CNetObj_KaizoNetworkPickup *pPickup)
{
	if(!pPickup)
		return;

	if(pPickup->m_Type < KZ_CUSTOM_WEAPONS_START || pPickup->m_Type >= KZ_CUSTOM_WEAPONS_END)
		return;

	int SwitcherTeam = GameClient()->SwitchStateTeam();
	auto &aSwitchers = GameClient()->Switchers();
	int Tick = Client()->GameTick(g_Config.m_ClDummy);
	
	switch(pPickup->m_Type)
	{
		case KZ_CUSTOM_WEAPON_PORTAL_GUN:
			Graphics()->TextureSet(g_pData->m_aImages[IMAGE_KZ_PORTAL].m_Id);
			break;
		case KZ_CUSTOM_WEAPON_ATTRACTOR_BEAM:
			Graphics()->TextureSet(g_pData->m_aImages[IMAGE_KZ_ATTRACTOR].m_Id);
			break;
	}
	Graphics()->QuadsSetRotation(0);

	if(pPickup->m_Switch > 0 && pPickup->m_Switch < (int)aSwitchers.size() && !aSwitchers[pPickup->m_Switch].m_aStatus[SwitcherTeam])
		Graphics()->SetColor(1.f, 1.f, 1.f, 0.3f);
	else
		Graphics()->SetColor(1.f, 1.f, 1.f, 1.f);
	Graphics()->RenderQuadContainerAsSprite(m_ItemsQuadContainerIndex, m_KaizoWeaponsOffsets[pPickup->m_Type - KZ_CUSTOM_WEAPONS_START], pPickup->m_X, pPickup->m_Y + 8 * sin((float)Tick / 25.0f), 1.0f, 1.0f);
}

//FoxNet
void CItems::RenderCosmeticLaser(const CNetObj_CosmeticLaser *pPrev, const CNetObj_CosmeticLaser *pCurrent)
{
	const int Flags = pCurrent->m_Flags;
	const bool Anchored = Flags & COSMETIC_FLAG_ANCHORED;
	const int Owner = pCurrent->m_Owner;

	const float IntraTick = (Owner >= 0 && GameClient()->m_aClients[Owner].m_IsPredicted) ? Client()->PredIntraGameTick(g_Config.m_ClDummy) : Client()->IntraGameTick(g_Config.m_ClDummy);

	vec2 PrevFrom = vec2(pPrev->m_FromX, pPrev->m_FromY);
	vec2 PrevTo = vec2(pPrev->m_ToX, pPrev->m_ToY);
	vec2 CurFrom = vec2(pCurrent->m_FromX, pCurrent->m_FromY);
	vec2 CurTo = vec2(pCurrent->m_ToX, pCurrent->m_ToY);

	vec2 From = mix(PrevFrom, CurFrom, IntraTick);
	vec2 To = mix(PrevTo, CurTo, IntraTick);

	if(Anchored && Owner >= 0 && GameClient()->m_aClients[Owner].m_Team != TEAM_SPECTATORS)
	{
		const vec2 AnchorPos = GameClient()->m_aClients[Owner].m_RenderPos;
		From += AnchorPos;
		To += AnchorPos;
	}

	int Type = std::clamp(pCurrent->m_Type, -1, NUM_LASERTYPES - 1);
	int ColorIn;
	int ColorOut;
	switch(Type)
	{
	case LASERTYPE_RIFLE:
		ColorOut = g_Config.m_ClLaserRifleOutlineColor;
		ColorIn = g_Config.m_ClLaserRifleInnerColor;
		break;
	case LASERTYPE_SHOTGUN:
		ColorOut = g_Config.m_ClLaserShotgunOutlineColor;
		ColorIn = g_Config.m_ClLaserShotgunInnerColor;
		break;
	case LASERTYPE_DOOR:
		ColorOut = g_Config.m_ClLaserDoorOutlineColor;
		ColorIn = g_Config.m_ClLaserDoorInnerColor;
		break;
	case LASERTYPE_FREEZE:
		ColorOut = g_Config.m_ClLaserFreezeOutlineColor;
		ColorIn = g_Config.m_ClLaserFreezeInnerColor;
		break;
	case LASERTYPE_DRAGGER:
		ColorOut = g_Config.m_ClLaserDraggerOutlineColor;
		ColorIn = g_Config.m_ClLaserDraggerInnerColor;
		break;
	case LASERTYPE_GUN:
	case LASERTYPE_PLASMA:
	default:
		ColorOut = g_Config.m_ClLaserRifleOutlineColor;
		ColorIn = g_Config.m_ClLaserRifleInnerColor;
		break;
	}

	bool IsOtherTeam = Owner >= 0 && GameClient()->IsOtherTeam(Owner);

	float Alpha = pCurrent->m_Alpha * 0.01f;
	if(pCurrent->m_Alpha == -1)
		Alpha = IsOtherTeam ? g_Config.m_ClShowOthersAlpha / 100.0f : 1.0f;

	if(Alpha <= 0)
		return; // Invisible

	const ColorRGBA OuterColor = color_cast<ColorRGBA>(ColorHSLA(ColorOut).WithAlpha(Alpha));
	const ColorRGBA InnerColor = color_cast<ColorRGBA>(ColorHSLA(ColorIn).WithAlpha(Alpha));

	float TicksHead = Client()->GameTick(g_Config.m_ClDummy) + IntraTick;
	float Len = distance(To, From);

	if(Len > 0)
	{
		vec2 Dir = normalize_pre_length(To - From, Len);
		float Thickness = (8 - pCurrent->m_TickOffset) * 0.125f;
		Graphics()->TextureClear();
		Graphics()->QuadsBegin();

		Graphics()->SetColor(OuterColor);
		vec2 Out = vec2(Dir.y, -Dir.x) * (7.0f * Thickness);

		IGraphics::CFreeformItem Freeform(
			From - Out, From + Out,
			To - Out, To + Out);
		Graphics()->QuadsDrawFreeform(&Freeform, 1);

		Out = vec2(Dir.y, -Dir.x) * (5.0f * Thickness);
		vec2 ExtraOutlinePos = Dir;
		vec2 ExtraOutlineFrom = Type == LASERTYPE_DOOR ? vec2(0, 0) : Dir;
		Graphics()->SetColor(InnerColor);

		Freeform = IGraphics::CFreeformItem(
			From - Out + ExtraOutlineFrom, From + Out + ExtraOutlineFrom,
			To - Out - ExtraOutlinePos, To + Out - ExtraOutlinePos);
		Graphics()->QuadsDrawFreeform(&Freeform, 1);

		Graphics()->QuadsEnd();
	}

	auto RenderHead = [&](vec2 Pos) {
		if(Type == LASERTYPE_DOOR)
		{
			Graphics()->TextureClear();
			Graphics()->QuadsSetRotation(0);
			Graphics()->SetColor(OuterColor);
			Graphics()->RenderQuadContainerEx(m_ItemsQuadContainerIndex, m_DoorHeadOffset, 1, Pos.x - 8.0f, Pos.y - 8.0f);
			Graphics()->SetColor(InnerColor);
			Graphics()->RenderQuadContainerEx(m_ItemsQuadContainerIndex, m_DoorHeadOffset, 1, Pos.x - 6.0f, Pos.y - 6.0f, 6.f / 8.f, 6.f / 8.f);
		}
		else if(Type == LASERTYPE_DRAGGER)
		{
			Graphics()->TextureSet(GameClient()->m_ExtrasSkin.m_SpritePulley);
			for(int Inner = 0; Inner < 2; ++Inner)
			{
				Graphics()->SetColor(Inner ? InnerColor : OuterColor);

				float Size = Inner ? 4.f / 5.f : 1.f;

				if(Len > 0)
				{
					Graphics()->QuadsSetRotation(0);
					Graphics()->RenderQuadContainerAsSprite(m_ItemsQuadContainerIndex, m_PulleyHeadOffset, Pos.x, Pos.y, Size, Size);
				}

				Size = Inner ? 0.75f - 1.f / 5.f : 0.75f;
				for(int Orb = 0; Orb < 3; ++Orb)
				{
					vec2 Offset(10.f, 0);
					Offset = rotate(Offset, Orb * 120 + TicksHead);
					Graphics()->QuadsSetRotation(TicksHead + Orb * pi * 2.f / 3.f);
					Graphics()->RenderQuadContainerAsSprite(m_ItemsQuadContainerIndex, m_PulleyHeadOffset, Pos.x + Offset.x, Pos.y + Offset.y, Size, Size);
				}
			}
		}
		else if(Type == LASERTYPE_FREEZE)
		{
			float Pulsation = 6.f / 5.f + 1.f / 10.f * std::sin(TicksHead / 2.f);
			float Angle = angle(Pos - From);
			Graphics()->TextureSet(GameClient()->m_ExtrasSkin.m_SpriteHectagon);
			Graphics()->QuadsSetRotation(Angle);
			Graphics()->SetColor(OuterColor);
			Graphics()->RenderQuadContainerAsSprite(m_ItemsQuadContainerIndex, m_FreezeHeadOffset, Pos.x, Pos.y, 6.f / 5.f * Pulsation, 6.f / 5.f * Pulsation);
			Graphics()->TextureSet(GameClient()->m_ExtrasSkin.m_SpriteParticleSnowflake);
			Graphics()->SetColor(ColorRGBA(1.f, 1.f, 1.f));
			Graphics()->RenderQuadContainerAsSprite(m_ItemsQuadContainerIndex, m_FreezeHeadOffset, Pos.x, Pos.y, Pulsation, Pulsation);
		}
		else
		{
			int CurParticle = (int)TicksHead % 3;
			Graphics()->TextureSet(GameClient()->m_ParticlesSkin.m_aSpriteParticleSplat[CurParticle]);
			Graphics()->QuadsSetRotation((int)TicksHead);
			Graphics()->SetColor(OuterColor);
			Graphics()->RenderQuadContainerAsSprite(m_ItemsQuadContainerIndex, m_aParticleSplatOffset[CurParticle], Pos.x, Pos.y);
			Graphics()->SetColor(InnerColor);
			Graphics()->RenderQuadContainerAsSprite(m_ItemsQuadContainerIndex, m_aParticleSplatOffset[CurParticle], Pos.x, Pos.y, 20.f / 24.f, 20.f / 24.f);
		}
	};

	const bool FromHead = Flags & COSMETIC_LASER_FLAG_FROM_HEAD;
	const bool ToHead = Flags & COSMETIC_LASER_FLAG_TO_HEAD;

	if(From == To && (FromHead || ToHead))
	{
		RenderHead(From);
	}
	else
	{
		if(FromHead)
			RenderHead(From);
		if(ToHead)
			RenderHead(To);
	}
}

//FoxNet
void CItems::RenderCosmeticPickup(const CNetObj_CosmeticPickup *pPrev, const CNetObj_CosmeticPickup *pCurrent)
{
	const int Flags = pCurrent->m_Flags;
	const bool Anchored = Flags & COSMETIC_FLAG_ANCHORED;
	const int Owner = pCurrent->m_Owner;

	int CurWeapon = std::clamp(pCurrent->m_Subtype, 0, NUM_WEAPONS - 1);
	int QuadOffset = 2;
	float IntraTick = (Owner >= 0 && GameClient()->m_aClients[Owner].m_IsPredicted) ? Client()->PredIntraGameTick(g_Config.m_ClDummy) : Client()->IntraGameTick(g_Config.m_ClDummy);

	vec2 PrevPos = vec2(pPrev->m_X, pPrev->m_Y);
	vec2 CurPos = vec2(pCurrent->m_X, pCurrent->m_Y);
	vec2 Pos = mix(PrevPos, CurPos, IntraTick);

	if(Anchored && Owner >= 0 && GameClient()->m_aClients[Owner].m_Team != TEAM_SPECTATORS)
		Pos += GameClient()->m_aClients[Owner].m_RenderPos;

	if(pCurrent->m_Type == POWERUP_HEALTH)
	{
		QuadOffset = m_PickupHealthOffset;
		Graphics()->TextureSet(GameClient()->m_GameSkin.m_SpritePickupHealth);
	}
	else if(pCurrent->m_Type == POWERUP_ARMOR)
	{
		QuadOffset = m_PickupArmorOffset;
		Graphics()->TextureSet(GameClient()->m_GameSkin.m_SpritePickupArmor);
	}
	else if(pCurrent->m_Type == POWERUP_WEAPON)
	{
		QuadOffset = m_aPickupWeaponOffset[CurWeapon];
		Graphics()->TextureSet(GameClient()->m_GameSkin.m_aSpritePickupWeapons[CurWeapon]);
	}
	else if(pCurrent->m_Type == POWERUP_NINJA)
	{
		QuadOffset = m_PickupNinjaOffset;
		//if(Flags & PICKUPFLAG_ROTATE)
		//	GameClient()->m_Effects.PowerupShine(Pos, vec2(18, 96), 1.0f);
		//else
		//	GameClient()->m_Effects.PowerupShine(Pos, vec2(96, 18), 1.0f);

		Graphics()->TextureSet(GameClient()->m_GameSkin.m_SpritePickupNinja);
	}
	else if(pCurrent->m_Type >= POWERUP_ARMOR_SHOTGUN && pCurrent->m_Type <= POWERUP_ARMOR_LASER)
	{
		QuadOffset = m_aPickupWeaponArmorOffset[pCurrent->m_Type - POWERUP_ARMOR_SHOTGUN];
		Graphics()->TextureSet(GameClient()->m_GameSkin.m_aSpritePickupWeaponArmor[pCurrent->m_Type - POWERUP_ARMOR_SHOTGUN]);
	}
	Graphics()->QuadsSetRotation(0);
	bool IsOtherTeam = (pCurrent->m_Owner >= 0 && GameClient()->IsOtherTeam(pCurrent->m_Owner));

	float Alpha = pCurrent->m_Alpha * 0.01f;
	if(pCurrent->m_Alpha == -1)
		Alpha = IsOtherTeam ? g_Config.m_ClShowOthersAlpha / 100.0f : 1.0f;
	if(Alpha <= 0)
		return; // Invisible

	Graphics()->SetColor(1.0f, 1.0f, 1.0f, Alpha);

	vec2 Scale = vec2(1, 1);
	if(Flags & COSMETIC_FLAG_XFLIP)
		Scale.x = -Scale.x;

	int Rotation = pCurrent->m_Rotation;

	Graphics()->QuadsSetRotation(Rotation * (pi / 180));

	static float s_Time = 0.0f;
	static float s_LastLocalTime = LocalTime();
	float Offset = Pos.y / 32.0f + Pos.x / 32.0f;
	if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
	{
		const IDemoPlayer::CInfo *pInfo = DemoPlayer()->BaseInfo();
		if(!pInfo->m_Paused)
			s_Time += (LocalTime() - s_LastLocalTime) * pInfo->m_Speed;
	}
	else
	{
		if(GameClient()->m_Snap.m_pGameInfoObj && !(GameClient()->m_Snap.m_pGameInfoObj->m_GameStateFlags & GAMESTATEFLAG_PAUSED))
			s_Time += LocalTime() - s_LastLocalTime;
	}
	Pos += direction(s_Time * 2.0f + Offset) * 2.5f;
	s_LastLocalTime = LocalTime();

	Graphics()->RenderQuadContainerAsSprite(m_ItemsQuadContainerIndex, QuadOffset, Pos.x, Pos.y, Scale.x, Scale.y);
	Graphics()->QuadsSetRotation(0);
}

//FoxNet
void CItems::RenderCosmeticProjectile(const CNetObj_CosmeticProjectile *pPrev, const CNetObj_CosmeticProjectile *pCurrent)
{
	const int Flags = pCurrent->m_Flags;
	const bool Anchored = Flags & COSMETIC_FLAG_ANCHORED;
	const int Owner = pCurrent->m_Owner;

	int CurWeapon = std::clamp(pCurrent->m_Type, 0, NUM_WEAPONS - 1);
	float IntraTick = (Owner >= 0 && GameClient()->m_aClients[Owner].m_IsPredicted) ? Client()->PredIntraGameTick(g_Config.m_ClDummy) : Client()->IntraGameTick(g_Config.m_ClDummy);

	vec2 PrevPos = vec2(pPrev->m_X, pPrev->m_Y);
	vec2 CurPos = vec2(pCurrent->m_X, pCurrent->m_Y);

	if(Anchored && Owner >= 0 && GameClient()->m_aClients[Owner].m_Team != TEAM_SPECTATORS)
	{
		const vec2 AnchorPos = GameClient()->m_aClients[Owner].m_RenderPos;
		PrevPos += AnchorPos;
		CurPos += AnchorPos;
	}

	vec2 Pos = mix(PrevPos, CurPos, IntraTick);
	vec2 Vel = CurPos - PrevPos;

	bool IsOtherTeam = Owner >= 0 && GameClient()->IsOtherTeam(Owner);

	float Alpha = pCurrent->m_Alpha * 0.01f;
	if(pCurrent->m_Alpha == -1)
		Alpha = IsOtherTeam ? g_Config.m_ClShowOthersAlpha / 100.0f : 1.0f;

	if(Alpha <= 0.0f)
		return;

	if(CurWeapon == WEAPON_GRENADE)
		GameClient()->m_Effects.SmokeTrail(Pos, Vel * -1, Alpha, 0.0f);
	else
		GameClient()->m_Effects.BulletTrail(Pos, Alpha, 0.0f);

	if(!GameClient()->m_GameSkin.m_aSpriteWeaponProjectiles[CurWeapon].IsValid())
		return;

	vec2 Scale = vec2(1, 1);
	if(Flags & COSMETIC_FLAG_XFLIP)
		Scale.x = -Scale.x;

	Graphics()->TextureSet(GameClient()->m_GameSkin.m_aSpriteWeaponProjectiles[CurWeapon]);
	Graphics()->QuadsSetRotation(pCurrent->m_Rotation * (pi / 180));
	Graphics()->SetColor(1.0f, 1.0f, 1.0f, Alpha);
	Graphics()->RenderQuadContainerAsSprite(m_ItemsQuadContainerIndex, m_aProjectileOffset[CurWeapon], Pos.x, Pos.y, Scale.x, Scale.y);
	Graphics()->QuadsSetRotation(0);
}

//From Pointer's Duck/Infclass client
void CItems::RenderPickupCustom(const CNetObj_PickupCustomTWPlus *pPrev, const CNetObj_PickupCustomTWPlus *pCurrent, bool IsPredicted, int Flags)
{
	float IntraTick = IsPredicted ? Client()->PredIntraGameTick(g_Config.m_ClDummy) : Client()->IntraGameTick(g_Config.m_ClDummy);
	vec2 Pos = mix(vec2(pPrev->m_X, pPrev->m_Y), vec2(pCurrent->m_X, pCurrent->m_Y), IntraTick);

	Graphics()->QuadsSetRotation(0);
	Graphics()->SetColor(1.f, 1.f, 1.f, 1.f);

	vec2 Scale = vec2(1, 1);
	if(Flags & PICKUPFLAG_XFLIP)
		Scale.x = -Scale.x;

	if(Flags & PICKUPFLAG_YFLIP)
		Scale.y = -Scale.y;

	if(Flags & PICKUPFLAG_ROTATE)
	{
		Graphics()->QuadsSetRotation(90.f * (pi / 180));
		std::swap(Scale.x, Scale.y);
	}

	static float s_Time = 0.0f;
	static float s_LastLocalTime = LocalTime();
	float Offset = Pos.y / 32.0f + Pos.x / 32.0f;
	if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
	{
		const IDemoPlayer::CInfo *pInfo = DemoPlayer()->BaseInfo();
		if(!pInfo->m_Paused)
			s_Time += (LocalTime() - s_LastLocalTime) * pInfo->m_Speed;
	}
	else
	{
		if(GameClient()->m_Snap.m_pGameInfoObj && !(GameClient()->m_Snap.m_pGameInfoObj->m_GameStateFlags & GAMESTATEFLAG_PAUSED))
			s_Time += LocalTime() - s_LastLocalTime;
	}
	Pos += direction(s_Time * 2.0f + Offset) * 2.5f;
	s_LastLocalTime = LocalTime();

	Graphics()->QuadsSetRotation(0);

	const CResources::CResource* res = GameClient()->m_Resources.Get(pCurrent->m_ResourceId);	
	IGraphics::CQuadItem QuadItem(Pos.x, Pos.y, 128, 128);
	Graphics()->TextureSet(res->m_Texture);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	Graphics()->QuadsDraw(&QuadItem, 1);
	Graphics()->QuadsEnd();
}
