// Copyright (C) Benjamín Gajardo (also known as +KZ)

// CNamePlatePartKaizoPlayerPing has code from nameplates.cpp

#include <game/client/gameclient.h>
#include <game/mapitems.h>

class CNamePlatePartKaizoPlayerPing : public CNamePlatePart
{
protected:
	ColorRGBA m_Color = ColorRGBA(1.0f, 1.0f, 1.0f, 0.0f);
	void Update(CGameClient &This, const CNamePlateData &Data) override
	{
		if(!g_Config.m_KaizoPingCircles || Data.m_ClientId == This.m_Snap.m_LocalClientId)
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
		m_Color = ColorRGBA(minimum(This.m_aClients[Data.m_ClientId].m_ReceivedPing, 350) / 350.f, 1.0f - minimum(This.m_aClients[Data.m_ClientId].m_ReceivedPing, 350) / 350.f, 0.0f, Data.m_Color.a);
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
		if(!g_Config.m_KaizoShowClientType || Data.m_ClientId == This.m_Snap.m_LocalClientId)
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