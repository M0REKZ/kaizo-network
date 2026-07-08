// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// Just look for comments that indicate if code is from another mod...

#include "menus.h"
#include <engine/font_icons.h>
#include <engine/graphics.h>
#include <game/client/gameclient.h>
#include <game/client/animstate.h>

//+KZ
extern int g_KaizoConfig_KaizoClientIndicatorBrowser;

//From EClient, modified by +KZ
void CMenus::UpdateOnlinePlayerCache()
{
	m_vKaizoUsersCache.clear();
	//m_vWarlistCache.clear(); //+KZ: i dont have warlist

	for(int ServerIdx = 0; ServerIdx < ServerBrowser()->NumSortedServers(); ++ServerIdx)
	{
		const CServerInfo *pCurServer = ServerBrowser()->SortedGet(ServerIdx);
		for(int ClientIdx = 0; ClientIdx < std::min(pCurServer->m_NumClients, (int)SERVERINFO_MAX_CLIENTS); ++ClientIdx)
		{
			const CServerInfo::CClient *pClient = &pCurServer->m_aClients[ClientIdx];

			union
            {
                int m_Color = 0;
                unsigned char m_B[4];
            } Body, Feet;
			Body.m_Color = pClient->m_CustomSkinColorBody;
			Feet.m_Color = pClient->m_CustomSkinColorFeet;
			if(Body.m_B[3] == CCID_COLOR_BODY_KAIZO_CLIENT && Feet.m_B[3] == CCID_COLOR_FEET_KAIZO_CLIENT)
            {
				m_vKaizoUsersCache.emplace_back(pClient, pCurServer);
                continue;
            }

            //+KZ for 0.7 servers
            bool Found07 = false;
            for(int p = 0; p < protocol7::NUM_SKINPARTS; p++)
            {
                if(str_startswith(pClient->m_aaSkin7[p], "kaizo!"))
                {
                    m_vKaizoUsersCache.emplace_back(pClient, pCurServer);
                    Found07 = true;
                    break;
                }
            }
            if(Found07)
                continue;
		}
	}

	//+KZ: here was some warlist code from EClient but i dont have warlist
}

//From EClient
void CMenus::ServerBrowserUpdate()
{
	Client()->ServerBrowserUpdate();
	m_OnlinePlayersCacheDirty = true;
}

//From EClient
void CMenus::RenderKaizoClientUsers(CUIRect &View, CUIRect &List, CScrollRegion &ScrollRegion)
{
	if(!g_KaizoConfig_KaizoClientIndicatorBrowser)
		return;

	const float FontSize = 10.0f;
	const float SpacingH = 2.0f;

	static int s_Extended;

	char aBuf[256];

	// Header for warlist type
	CUIRect Header, Icon, Label;
	List.HSplitTop(ms_ListheaderHeight, &Header, &List);
	ScrollRegion.AddRect(Header);
	Header.Draw(ColorRGBA(1.0f, 1.0f, 1.0f, Ui()->HotItem() == &s_Extended ? 0.4f : 0.25f), IGraphics::CORNER_ALL, 5.0f);
	Header.VSplitLeft(Header.h, &Icon, &Label);
	Icon.Margin(2.0f, &Icon);

	// Plus/minus icon for extend/collapse
	TextRender()->SetFontPreset(EFontPreset::ICON_FONT);
	TextRender()->TextColor(Ui()->HotItem() == &s_Extended ? TextRender()->DefaultTextColor() : ColorRGBA(0.6f, 0.6f, 0.6f, 1.0f));
	Ui()->DoLabel(&Icon, s_Extended ? FontIcon::SQUARE_MINUS : FontIcon::SQUARE_PLUS, Icon.h * CUi::ms_FontmodHeight, TEXTALIGN_MC);
	TextRender()->TextColor(TextRender()->DefaultTextColor());
	TextRender()->SetFontPreset(EFontPreset::DEFAULT_FONT);

	str_format(aBuf, sizeof(aBuf), "Kaizo Client Users (%zu)", m_vKaizoUsersCache.size());
	Ui()->DoLabel(&Label, aBuf, FontSize, TEXTALIGN_ML);

	// Toggle on click
	if(Ui()->DoButtonLogic(&s_Extended, 0, &Header, BUTTONFLAG_LEFT))
		s_Extended = !s_Extended;

	// Only render entries if extended
	if(!s_Extended)
	{
		// Still add the space for the header
		CUIRect Space;
		List.HSplitTop(10.0f, &Space, &List);
		ScrollRegion.AddRect(Space);
		return;
	}

	for(const CGenericPlayerInfo &Entry : m_vKaizoUsersCache)
	{
		{
			CUIRect Space;
			List.HSplitTop(SpacingH, &Space, &List);
			ScrollRegion.AddRect(Space);
		}

		// Entry background and layout
		CUIRect Rect;
		List.HSplitTop(11.0f + 10.0f + 2 * 2.0f + 1.0f + 10.0f, &Rect, &List);
		ScrollRegion.AddRect(Rect);
		if(ScrollRegion.RectClipped(Rect))
			continue;

		Rect.y += 0.5f;
		Rect.w -= 1.0f;
		Rect.h -= 1.0f;
		Rect.x += 0.5f;

		const bool Inside = Ui()->HotItem() == Entry.ListItemId() || Ui()->HotItem() == Entry.CommunityTooltipId() || Ui()->HotItem() == Entry.SkinTooltipId();
		int ButtonResult = Ui()->DoButtonLogic(Entry.ListItemId(), 0, &Rect, BUTTONFLAG_LEFT);
		bool InSelectedServer = m_SelectedIndex >= 0 && Entry.m_ServerIndex == ServerBrowser()->SortedGet(m_SelectedIndex)->m_ServerIndex;

		ColorRGBA BgColor = PlayerBackgroundColor(false, false, Entry.m_IsAfk, InSelectedServer, Inside);
        //+KZ added PlayerBackgroundColor
		const float Rounding = 5.0f;
		Rect.Draw(BgColor, IGraphics::CORNER_ALL, Rounding);
		if(InSelectedServer)
			Rect.DrawOutlineEClient(ColorRGBA(1.0f, 1.0f, 1.0f, 0.5f), IGraphics::CORNER_ALL, Rounding);
		Rect.Margin(2.0f, &Rect);

		CUIRect RemoveButton, NameLabel, ClanLabel, InfoLabel;
		Rect.HSplitTop(16.0f, &RemoveButton, nullptr);
		RemoveButton.VSplitRight(13.0f, nullptr, &RemoveButton);
		RemoveButton.HMargin((RemoveButton.h - RemoveButton.w) / 2.0f, &RemoveButton);
		Rect.VSplitLeft(2.0f, nullptr, &Rect);

		Rect.HSplitBottom(10.0f, &Rect, &InfoLabel);
		Rect.HSplitTop(11.0f + 10.0f, &Rect, nullptr);

		// tee
		CUIRect Skin;
		Rect.VSplitLeft(Rect.h, &Skin, &Rect);
		Rect.VSplitLeft(2.0f, nullptr, &Rect);
		if(Entry.Skin()[0] != '\0')
		{
			const CTeeRenderInfo TeeInfo = GetTeeRenderInfo(vec2(Skin.w, Skin.h), Entry.Skin(), Entry.CustomSkinColors(), Entry.CustomSkinColorBody(), Entry.CustomSkinColorFeet());
			const CAnimState *pIdleState = Entry.m_IsAfk ? CAnimState::GetSpec() : CAnimState::GetIdle();
			vec2 OffsetToMid;
			CRenderTools::GetRenderTeeOffsetToRenderedTee(pIdleState, &TeeInfo, OffsetToMid);
			const vec2 TeeRenderPos = vec2(Skin.x + Skin.w / 2.0f, Skin.y + Skin.h * 0.55f + OffsetToMid.y);
			RenderTools()->RenderTee(pIdleState, &TeeInfo, Entry.IsAfk() ? EMOTE_BLINK : EMOTE_NORMAL, vec2(1.0f, 0.0f), TeeRenderPos);
			Ui()->DoButtonLogic(Entry.SkinTooltipId(), 0, &Skin, BUTTONFLAG_NONE);
			GameClient()->m_Tooltips.DoToolTip(Entry.SkinTooltipId(), &Skin, Entry.Skin());
		}
        //+KZ for 0.7
        else if(Entry.m_aaSkin7[protocol7::SKINPART_BODY][0] != '\0')
		{
			CTeeRenderInfo TeeInfo;
			TeeInfo.m_Size = std::min(Skin.w, Skin.h);
			for(int Part = 0; Part < protocol7::NUM_SKINPARTS; Part++)
			{
				GameClient()->m_Skins7.FindSkinPart(Part, Entry.m_aaSkin7[Part], true)->ApplyTo(TeeInfo.m_aSixup[g_Config.m_ClDummy]);
				GameClient()->m_Skins7.ApplyColorTo(TeeInfo.m_aSixup[g_Config.m_ClDummy], Entry.m_aUseCustomSkinColor7[Part], Entry.m_aCustomSkinColor7[Part], Part);
			}
			const CAnimState *pIdleState = Entry.m_IsAfk ? CAnimState::GetSpec() : CAnimState::GetIdle();
			vec2 OffsetToMid;
			CRenderTools::GetRenderTeeOffsetToRenderedTee(pIdleState, &TeeInfo, OffsetToMid);
			const vec2 TeeRenderPos = vec2(Skin.x + TeeInfo.m_Size / 2.0f, Skin.y + Skin.h / 2.0f + OffsetToMid.y);
			RenderTools()->RenderTee(pIdleState, &TeeInfo, Entry.m_IsAfk ? EMOTE_BLINK : EMOTE_NORMAL, vec2(1.0f, 0.0f), TeeRenderPos);
        }
		Rect.HSplitTop(11.0f, &NameLabel, &ClanLabel);

		// name
		char pNameBuf[MAX_NAME_LENGTH];
		str_copy(pNameBuf, Entry.Name());
		Ui()->DoLabel(&NameLabel, pNameBuf, FontSize - 1.0f, TEXTALIGN_ML);
		TextRender()->TextColor(TextRender()->DefaultTextColor());

		// clan
		Ui()->DoLabel(&ClanLabel, Entry.Clan(), FontSize - 2.0f, TEXTALIGN_ML);

		// community icon
		const CCommunity *pCommunity = ServerBrowser()->Community(Entry.m_aCommunityId);
		if(pCommunity != nullptr)
		{
			const CCommunityIcon *pIcon = m_CommunityIcons.Find(pCommunity->Id());
			if(pIcon != nullptr)
			{
				CUIRect CommunityIcon;
				InfoLabel.VSplitLeft(21.0f, &CommunityIcon, &InfoLabel);
				InfoLabel.VSplitLeft(2.0f, nullptr, &InfoLabel);
				m_CommunityIcons.Render(pIcon, CommunityIcon, true);
				Ui()->DoButtonLogic(Entry.CommunityTooltipId(), 0, &CommunityIcon, BUTTONFLAG_NONE);
				GameClient()->m_Tooltips.DoToolTip(Entry.CommunityTooltipId(), &CommunityIcon, pCommunity->Name());
			}
		}

		// server info text
		char aLatency[16];
		str_format(aLatency, sizeof(aLatency), "%d", Entry.Latency());
		if(aLatency[0] != '\0')
			str_format(aBuf, sizeof(aBuf), "%s | %s | %s", Entry.m_aMap, Entry.m_aGameType, aLatency);
		else
			str_format(aBuf, sizeof(aBuf), "%s | %s", Entry.m_aMap, Entry.m_aGameType);
		Ui()->DoLabel(&InfoLabel, aBuf, FontSize - 2.0f, TEXTALIGN_ML);

		// handle click and double click on item
		if(ButtonResult)
		{
			str_copy(g_Config.m_UiServerAddress, Entry.m_aAddress);
			m_ServerBrowserShouldRevealSelection = true;
			if(ButtonResult == 1 && Ui()->DoDoubleClickLogic(Entry.ListItemId()))
			{
				Connect(g_Config.m_UiServerAddress);
			}
		}
	}

	{
		CUIRect Space;
		List.HSplitTop(10.0f, &Space, &List);
		ScrollRegion.AddRect(Space);
	}
}
