// Copyright (C) Benjamín Gajardo (also known as +KZ)

// CNamePlatePartKaizoPlayerPing has code from nameplates.cpp
// CNamePlatePartFlags and CNamePlatePartJumps are from DuckDDNet

#include <game/client/gameclient.h>
#include <game/mapitems.h>
#include <base/helper_kz.h>

class CNamePlatePartKaizoPlayerPing : public CNamePlatePart
{
protected:
	ColorRGBA m_Color = ColorRGBA(1.0f, 1.0f, 1.0f, 0.0f);
	void Update(CGameClient &This, const CNamePlateData &Data) override
	{
		if(!g_KaizoConfig_KaizoPingCircles || Data.m_ClientId == This.m_Snap.m_LocalClientId)
		{
			m_Visible = false;
			return;
		}

		if(This.m_aClients[Data.m_ClientId].m_ReceivedPing < 0)
		{
			m_Visible = false;
			return;
		}
		
		m_Visible = true;

		m_Size = vec2(20 + DEFAULT_PADDING, 20 + DEFAULT_PADDING);
		m_Color = ColorRGBA(std::min(This.m_aClients[Data.m_ClientId].m_ReceivedPing, 350) / 350.f, 1.0f - std::min(This.m_aClients[Data.m_ClientId].m_ReceivedPing, 350) / 350.f, 0.0f, Data.m_Color.a);
	}

public:
	void Render(CGameClient &This, vec2 Pos) const override
	{
		IGraphics::CQuadItem QuadItem(Pos.x - Size().x / 2.0f, Pos.y - Size().y / 2.0f, Size().x, Size().y);
		This.Graphics()->TextureClear();
		This.Graphics()->QuadsBegin();
		This.Graphics()->SetColor(m_Color);
		This.Graphics()->DrawCircle(Pos.x,Pos.y, Size().x/2, 32);
		This.Graphics()->QuadsEnd();
	}

	CNamePlatePartKaizoPlayerPing(CGameClient &This) :
		CNamePlatePart(This) {}
};

class CNamePlatePartKaizoCustomClient : public CNamePlatePartSprite
{
public:
	void Update(CGameClient &This, const CNamePlateData &Data) override
	{
		if(!g_KaizoConfig_KaizoShowClientType || Data.m_ClientId == This.m_Snap.m_LocalClientId)
		{
			m_Visible = false;
			return;
		}

		if(This.m_aClients[Data.m_ClientId].m_CustomClient == CUSTOM_CLIENT_ID_KAIZO_NETWORK)
		{
			m_Texture = g_pData->m_aImages[IMAGE_KZ_KAIZOICON].m_Id;
			m_Sprite = SPRITE_KZ_KAIZOICON;
			m_Visible = true;
		}
		else if(This.m_aClients[Data.m_ClientId].m_CustomClient == CUSTOM_CLIENT_ID_CHILLERBOTUX)
		{
			m_Texture = g_pData->m_aImages[IMAGE_KZ_CHILLERBOTUXICON].m_Id;
			m_Sprite = SPRITE_KZ_CHILLERBOTUXICON;
			m_Visible = true;
		}
		else if(This.m_aClients[Data.m_ClientId].m_CustomClient == CUSTOM_CLIENT_ID_GAMER_07)
		{
			m_Texture = g_pData->m_aImages[IMAGE_KZ_GAMERICON].m_Id;
			m_Sprite = SPRITE_KZ_GAMERICON;
			m_Visible = true;
		}
		else if(This.m_aClients[Data.m_ClientId].m_CustomClient == CUSTOM_CLIENT_ID_ZILLYWOODS_07)
		{
			m_Texture = g_pData->m_aImages[IMAGE_KZ_ZILLYWOODSICON].m_Id;
			m_Sprite = SPRITE_KZ_ZILLYWOODSICON;
			m_Visible = true;
		}
		else if(This.m_aClients[Data.m_ClientId].m_CustomClient == CUSTOM_CLIENT_ID_FCLIENT_07)
		{
			m_Texture = g_pData->m_aImages[IMAGE_KZ_FCLIENTICON].m_Id;
			m_Sprite = SPRITE_KZ_FCLIENTICON;
			m_Visible = true;
		}
		else if(This.m_aClients[Data.m_ClientId].m_CustomClient == CUSTOM_CLIENT_ID_PDUCKCLIENT)
		{
			m_Texture = g_pData->m_aImages[IMAGE_KZ_DUCKICON].m_Id;
			m_Sprite = SPRITE_KZ_DUCKICON;
			m_Visible = true;
		}
		else
		{
			m_Visible = false;
		}
		m_Color.a = Data.m_Color.a;
	}

	CNamePlatePartKaizoCustomClient(CGameClient &This) :
		CNamePlatePartSprite(This)
		{
			m_Texture = g_pData->m_aImages[IMAGE_KZ_KAIZOICON].m_Id;
			m_Padding = vec2(0.0f, 0.0f);
			m_Sprite = SPRITE_KZ_KAIZOICON;
			m_Size = vec2(20 + DEFAULT_PADDING, 20 + DEFAULT_PADDING);
		}
};

//DuckDDNet Client
class CNamePlatePartFlags : public CNamePlatePart
{
private:
	int m_Flags = 0;
	ColorRGBA m_Color = ColorRGBA(1.0, 1.0, 1.0);

protected:
	void Update(CGameClient &This, const CNamePlateData &Data) override
	{
		int Count = not_builtin_popcount(Data.m_TrackedFlags);
		if(!g_KaizoConfig_KaizoShowCharFlags || !Data.m_TrackedFlags || Data.m_ClientId == This.m_Snap.m_LocalClientId)
		{
			m_Size = vec2();
			m_Visible = false;
			return;
		}
		m_Size = vec2((Data.m_FontSizeFlags + m_Padding.x) * Count - m_Padding.x, Data.m_FontSizeFlags);
		m_Flags = Data.m_TrackedFlags;
		m_Color.a = Data.m_Color.a;
	}
	void Render(CGameClient &This, vec2 Pos) const override
	{
		Pos.x -= m_Size.x/2;
		Pos.x += m_Size.y/2;
		const std::pair<int, IGraphics::CTextureHandle> FlagData[] = {
			std::make_pair(CHARACTERFLAG_MOVEMENTS_DISABLED, This.m_HudSkin.m_SpriteHudLiveFrozen),
			std::make_pair(CHARACTERFLAG_IN_FREEZE, This.m_HudSkin.m_SpriteHudDeepFrozen),
			std::make_pair(CHARACTERFLAG_ENDLESS_HOOK, This.m_HudSkin.m_SpriteHudEndlessHook),
			std::make_pair(CHARACTERFLAG_ENDLESS_JUMP, This.m_HudSkin.m_SpriteHudEndlessJump),
			std::make_pair(CHARACTERFLAG_JETPACK, This.m_HudSkin.m_SpriteHudJetpack),
			std::make_pair(CHARACTERFLAG_HOOK_HIT_DISABLED, This.m_HudSkin.m_SpriteHudHookHitDisabled),
			std::make_pair(CHARACTERFLAG_HAMMER_HIT_DISABLED, This.m_HudSkin.m_SpriteHudHammerHitDisabled),
			std::make_pair(CHARACTERFLAG_SHOTGUN_HIT_DISABLED, This.m_HudSkin.m_SpriteHudShotgunHitDisabled), //+KZ
			std::make_pair(CHARACTERFLAG_GRENADE_HIT_DISABLED, This.m_HudSkin.m_SpriteHudGrenadeHitDisabled), //+KZ
			std::make_pair(CHARACTERFLAG_LASER_HIT_DISABLED, This.m_HudSkin.m_SpriteHudLaserHitDisabled), //+KZ
			std::make_pair(CHARACTERFLAG_COLLISION_DISABLED, This.m_HudSkin.m_SpriteHudCollisionDisabled),
			std::make_pair(CHARACTERFLAG_TELEGUN_GUN, This.m_HudSkin.m_SpriteHudTeleportGun),
			std::make_pair(CHARACTERFLAG_TELEGUN_LASER, This.m_HudSkin.m_SpriteHudTeleportLaser),
			std::make_pair(CHARACTERFLAG_TELEGUN_GRENADE, This.m_HudSkin.m_SpriteHudTeleportGrenade),
			std::make_pair(CHARACTERFLAG_WEAPON_HAMMER, This.m_GameSkin.m_SpriteWeaponHammer), //+KZ
			std::make_pair(CHARACTERFLAG_WEAPON_GUN, This.m_GameSkin.m_SpriteWeaponGunProjectile), //+KZ
			std::make_pair(CHARACTERFLAG_WEAPON_SHOTGUN, This.m_GameSkin.m_SpriteWeaponShotgunProjectile), //+KZ
			std::make_pair(CHARACTERFLAG_WEAPON_GRENADE, This.m_GameSkin.m_SpriteWeaponGrenadeProjectile), //+KZ
			std::make_pair(CHARACTERFLAG_WEAPON_LASER, This.m_GameSkin.m_SpriteWeaponLaserProjectile), //+KZ
			std::make_pair(CHARACTERFLAG_WEAPON_NINJA, This.m_GameSkin.m_SpriteWeaponNinja), //+KZ
		};
		for(const auto &Flag : FlagData)
		{
			if((m_Flags & Flag.first) == 0)
				continue;

			This.Graphics()->TextureSet(Flag.second);
			This.Graphics()->QuadsBegin();
			This.Graphics()->SetColor(m_Color);

			This.Graphics()->DrawSprite(Pos.x, Pos.y, m_Size.y, m_Size.y);
			This.Graphics()->QuadsEnd();
			Pos.x += m_Size.y;
		}
	}

public:
	CNamePlatePartFlags(CGameClient &This) :
		CNamePlatePart(This)
	{
		m_Padding = vec2();
	}
};

//DuckDDNet Client
class CNamePlatePartJumps : public CNamePlatePart
{
private:
	int m_JumpsLeft;
	int m_JumpsUsed;
	ColorRGBA m_Color = ColorRGBA(1.0, 1.0, 1.0);

protected:
	void Update(CGameClient &This, const CNamePlateData &Data) override
	{
		int Count = Data.m_JumpsLeft + Data.m_JumpsUsed;
		if(!g_KaizoConfig_KaizoShowCharJumps || Data.m_ClientId == This.m_Snap.m_LocalClientId)
		{
			m_Size = vec2();
			m_Visible = false;
			return;
		}
		m_Size = vec2((Data.m_FontSizeJumps + m_Padding.x) * Count - m_Padding.x, Data.m_FontSizeJumps);
		m_JumpsLeft = Data.m_JumpsLeft;
		m_JumpsUsed = Data.m_JumpsUsed;
		m_Color.a = Data.m_Color.a;
	}
	void Render(CGameClient &This, vec2 Pos) const override
	{
		Pos.x -= (m_Size.x - m_Size.y) * 0.5f;
		if(m_JumpsLeft > 0)
		{
			This.Graphics()->TextureSet(This.m_HudSkin.m_SpriteHudAirjump);
			This.Graphics()->QuadsBegin();
			This.Graphics()->SetColor(m_Color);
			for(int i = 0; i < m_JumpsLeft; i++)
			{
				This.Graphics()->DrawSprite(Pos.x, Pos.y, m_Size.y, m_Size.y);
				Pos.x += m_Size.y + m_Padding.x;
			}
			This.Graphics()->QuadsEnd();
		}
		if(m_JumpsUsed > 0)
		{
			This.Graphics()->TextureSet(This.m_HudSkin.m_SpriteHudAirjumpEmpty);
			This.Graphics()->QuadsBegin();
			This.Graphics()->SetColor(m_Color);
			for(int i = 0; i < m_JumpsUsed; i++)
			{
				This.Graphics()->DrawSprite(Pos.x, Pos.y, m_Size.y, m_Size.y);
				Pos.x += m_Size.y + m_Padding.x;
			}
			This.Graphics()->QuadsEnd();
		}
	}

public:
	CNamePlatePartJumps(CGameClient &This) :
		CNamePlatePart(This)
	{
		m_Padding = vec2();
	}
};
