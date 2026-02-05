// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// RenderSettingsKaizo has code from menus_settings.cpp and menus_settings_assets.cpp

#include <game/localization.h>
#include "menus.h"
#include <game/client/ui_listbox.h>
#include <engine/shared/localization.h>
#include <game/client/gameclient.h>
#include <generated/client_data.h>

enum
{
	KAIZO_SETTINGS_TAB_KAIZO = 0,
	KAIZO_SETTINGS_TAB_INFO,
	NUM_KAIZO_SETTINGS_TABS,
};

static int s_CurCustomTab = KAIZO_SETTINGS_TAB_KAIZO;

/*class CKeyInfo
{
public:
	const char *m_pName;
	const char *m_pCommand;
	int m_KeyId;
	int m_ModifierCombination;
};

#define NUM_KAIZO_KEYBINDS 6
static CKeyInfo gs_aKeys[NUM_KAIZO_KEYBINDS] = {
	{Localizable("Rescue"), "say /rescue", 0, 0},
	{Localizable("Equip Portal Gun"), "say /portalgun", 0, 0},
	{Localizable("Equip Attractor Beam"), "say /attractorbeam", 0, 0},
	{Localizable("Orange Portal"), "say /orangeportal", 0, 0},
	{Localizable("Blue Portal"), "say /blueportal", 0, 0},
	{Localizable("Drop Flag"), "say /drop flag", 0, 0},
};*/

void CMenus::RenderSettingsKaizo(CUIRect MainView)
{
	CUIRect TabBar;

	CUIRect Button;

	CUIRect Label;

	CUIRect Left, Right;

	MainView.HSplitTop(20.0f, &TabBar, &MainView);
	const float TabWidth = TabBar.w / NUM_KAIZO_SETTINGS_TABS;
	static CButtonContainer s_aPageTabs[NUM_KAIZO_SETTINGS_TABS] = {};
	const char *apTabNames[NUM_KAIZO_SETTINGS_TABS] = {
		Localize("Kaizo"),
		Localize("Info"),
		};

	for(int Tab = KAIZO_SETTINGS_TAB_KAIZO; Tab < NUM_KAIZO_SETTINGS_TABS; ++Tab)
	{
		CUIRect PageButton;
		TabBar.VSplitLeft(TabWidth, &PageButton, &TabBar);
		const int Corners = Tab == KAIZO_SETTINGS_TAB_KAIZO ? IGraphics::CORNER_L : Tab == NUM_KAIZO_SETTINGS_TABS - 1 ? IGraphics::CORNER_R : IGraphics::CORNER_NONE;
		if(DoButton_MenuTab(&s_aPageTabs[Tab], apTabNames[Tab], s_CurCustomTab == Tab, &PageButton, Corners, nullptr, nullptr, nullptr, nullptr, 4.0f))
		{
			s_CurCustomTab = Tab;
		}
	}

	CUIRect SettingsBox, SettingsBoxContainer;
	MainView.HSplitTop(20.0f, nullptr, &SettingsBoxContainer);
	
	static CScrollRegion s_ScrollRegion;
	vec2 ScrollOffset(0, 0);
	CScrollRegionParams ScrollParams;
	ScrollParams.m_ScrollUnit = 120.0f;
	s_ScrollRegion.Begin(&MainView, &ScrollOffset, &ScrollParams);
	SettingsBoxContainer.y += ScrollOffset.y;

	float ScrollHeight = 0.f;

	// for now all is 0, but ready for future additions
	switch (s_CurCustomTab)
	{
	case KAIZO_SETTINGS_TAB_KAIZO:
		ScrollHeight = 512.f;
		break;
	default:
		ScrollHeight = 0.f;
		break;
	}

	SettingsBoxContainer.HSplitTop(ScrollHeight, &SettingsBox, &SettingsBoxContainer);

	if(!s_ScrollRegion.AddRect(SettingsBox))
	{
		s_ScrollRegion.End();
		return;
	}
	

	switch(s_CurCustomTab)
	{
		case KAIZO_SETTINGS_TAB_KAIZO:
		{
			SettingsBox.VSplitMid(&Left, &Right, 20.0f);

			Left.HSplitTop(20.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Kaizo Settings"), 20.0f, TEXTALIGN_ML);
			Left.HSplitTop(25.0f, &Label, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoFastInput, Localize("Fast input"), g_Config.m_KaizoFastInput, &Button))
			{
				g_Config.m_KaizoFastInput ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoFastInputOthers, Localize("Fast input others"), g_Config.m_KaizoFastInputOthers, &Button))
			{
				g_Config.m_KaizoFastInputOthers ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoShowCrowns, Localize("Show crowns on Kaizo servers"), g_Config.m_KaizoShowCrowns, &Button))
			{
				g_Config.m_KaizoShowCrowns ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoPingCircles, Localize("Show ping circles above players"), g_Config.m_KaizoPingCircles, &Button))
			{
				g_Config.m_KaizoPingCircles ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoSleepingInMenuPlayers, Localize("Show players navigating menu as sleeping tees"), g_Config.m_KaizoSleepingInMenuPlayers, &Button))
			{
				g_Config.m_KaizoSleepingInMenuPlayers ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoShowClientType, Localize("Try to identify custom clients of other players"), g_Config.m_KaizoShowClientType, &Button))
			{
				g_Config.m_KaizoShowClientType ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoSendClientType, Localize("Let know other clients we are using Kaizo Client"), g_Config.m_KaizoSendClientType, &Button))
			{
				g_Config.m_KaizoSendClientType ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoRotatingHammer, Localize("Rotating hammer (like other weapons)"), g_Config.m_KaizoRotatingHammer, &Button))
			{
				g_Config.m_KaizoRotatingHammer ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoHudRealPosition, Localize("If HUD player position is enabled, show real position"), g_Config.m_KaizoHudRealPosition, &Button))
			{
				g_Config.m_KaizoHudRealPosition ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoHudRealVelocity, Localize("If HUD player velocity is enabled, show real velocity"), g_Config.m_KaizoHudRealVelocity, &Button))
			{
				g_Config.m_KaizoHudRealVelocity ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoChatInputBackground, Localize("Show background while typing in chat"), g_Config.m_KaizoChatInputBackground, &Button))
			{
				g_Config.m_KaizoChatInputBackground ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoFastMapDownload, Localize("Fast map download (Experimental)"), g_Config.m_KaizoFastMapDownload, &Button))
			{
				g_Config.m_KaizoFastMapDownload ^= 1;
			}

			if(g_Config.m_KaizoFastMapDownload)
			{
				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(20.0f, &Button, &Left);
				Ui()->DoScrollbarOption(&g_Config.m_KaizoFastMapDownloadWindow, &g_Config.m_KaizoFastMapDownloadWindow, &Button, Localize("Map data to request"), 0, 500, &CUi::ms_LogarithmicScrollbarScale, 0u, " KiB/s");

				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(10.0f, &Label, &Left);
				Ui()->DoLabel(&Label, Localize("NOTE: high values may break map download"), 10.0f, TEXTALIGN_ML);
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoEmotionalTees, Localize("Emotional Tees in non-DDNet servers (Client-side)"), g_Config.m_KaizoEmotionalTees, &Button))
			{
				g_Config.m_KaizoEmotionalTees ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoInstaShieldShield, Localize("InstaShield Shield"), g_Config.m_KaizoInstaShieldShield, &Button))
			{
				g_Config.m_KaizoInstaShieldShield ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoKillingSpreeSparkles, Localize("Killing spree sparkles"), g_Config.m_KaizoKillingSpreeSparkles, &Button))
			{
				g_Config.m_KaizoKillingSpreeSparkles ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoReplyTabbedOut, Localize("Reply when tabbed out"), g_Config.m_KaizoReplyTabbedOut, &Button))
			{
				g_Config.m_KaizoReplyTabbedOut ^= 1;
			}

			if(g_Config.m_KaizoReplyTabbedOut)
			{
				static CLineInput s_TabbedOutInput;
				s_TabbedOutInput.SetBuffer(g_Config.m_KaizoReplyTabbedOutMsg, sizeof(g_Config.m_KaizoReplyTabbedOutMsg));

				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(10.0f, &Label, &Left);
				Ui()->DoLabel(&Label, Localize("Message to send:"), 10.0f, TEXTALIGN_ML);

				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(20.0f, &Button, &Left);
				Ui()->DoEditBox(&s_TabbedOutInput, &Button, 14.0f);
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			Ui()->DoScrollbarOption(&g_Config.m_KaizoScoreboardStyle, &g_Config.m_KaizoScoreboardStyle, &Button, Localize("Scoreboard style"), 0, 3, &CUi::ms_LogarithmicScrollbarScale, 0u, "");

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			Ui()->DoScrollbarOption(&g_Config.m_KaizoScoreboardShorten, &g_Config.m_KaizoScoreboardShorten, &Button, Localize("Shorten scoreboard"), 0, 2, &CUi::ms_LogarithmicScrollbarScale, 0u, "");

			// Prediction
			Right.HSplitTop(20.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Prediction"), 20.0f, TEXTALIGN_ML);
			Right.HSplitTop(25.0f, &Label, &Right);

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoAntiPingImproved, Localize("Better Antiping algorithm"), g_Config.m_KaizoAntiPingImproved, &Button))
			{
				g_Config.m_KaizoAntiPingImproved ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoAntiPingNegativeBuffer, Localize("Negative stability buffer (for Gores)"), g_Config.m_KaizoAntiPingNegativeBuffer, &Button))
			{
				g_Config.m_KaizoAntiPingNegativeBuffer ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoPredictGoresGrenadeTele, Localize("Predict Gores grenade teleport"), g_Config.m_KaizoPredictGoresGrenadeTele, &Button))
			{
				g_Config.m_KaizoPredictGoresGrenadeTele ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoPredictTeleToDeath, Localize("Predict death effect if teleport leads to Death"), g_Config.m_KaizoPredictTeleToDeath, &Button))
			{
				g_Config.m_KaizoPredictTeleToDeath ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoPredictDDNetTeleport, Localize("Predict DDNet teleports if there is only 1 exit"), g_Config.m_KaizoPredictDDNetTeleport, &Button))
			{
				g_Config.m_KaizoPredictDDNetTeleport ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoPredictDeathTiles, Localize("Predict Death tiles"), g_Config.m_KaizoPredictDeathTiles, &Button))
			{
				g_Config.m_KaizoPredictDeathTiles ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoPredictPointerTWPlus, Localize("Predict Pointer's TW+"), g_Config.m_KaizoPredictPointerTWPlus, &Button))
			{
				g_Config.m_KaizoPredictPointerTWPlus ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoPredictVanillaHammerFix, Localize("Fix Vanilla hammer hit prediction through walls"), g_Config.m_KaizoPredictVanillaHammerFix, &Button))
			{
				g_Config.m_KaizoPredictVanillaHammerFix ^= 1;
			}

			// dangerous settings, some communities may consider it a cheat, chill communities may not, use at your own risk!
			Right.HSplitTop(40.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Dangerous Settings!"), 20.0f, TEXTALIGN_ML);
			Right.HSplitTop(45.0f, &Label, &Right);
			Right.HSplitTop(20.0f, &Label, &SettingsBox);

			SLabelProperties DangerLabelProps;
			DangerLabelProps.SetColor(ColorRGBA(1.f,0.f,0.f));
			Ui()->DoLabel(&Label, Localize("Some communities may consider these features as cheats while others may not.\nUSE AT YOUR OWN RISK! (Some wont work in some servers and it is NOT a bug)"), 10.0f, TEXTALIGN_ML, DangerLabelProps);
			Right.HSplitTop(25.0f, &Label, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoOldModsZooming, Localize("Zooming in Old non-DDNet mods (only if zoom is not prohibited)"), g_Config.m_KaizoOldModsZooming, &Button))
			{
				g_Config.m_KaizoOldModsZooming ^= 1;
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoShowRechargeBar, Localize("Show weapon recharge bar"), g_Config.m_KaizoShowRechargeBar, &Button))
			{
				g_Config.m_KaizoShowRechargeBar ^= 1;
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoHideChatBubble, Localize("Dont send chat bubble"), g_Config.m_KaizoHideChatBubble, &Button))
			{
				g_Config.m_KaizoHideChatBubble ^= 1;
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoHideInMenuStatus, Localize("Dont send in-menu status"), g_Config.m_KaizoHideInMenuStatus, &Button))
			{
				g_Config.m_KaizoHideInMenuStatus ^= 1;
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoForceChatBubble, Localize("Always send chat bubble"), g_Config.m_KaizoForceChatBubble, &Button))
			{
				g_Config.m_KaizoForceChatBubble ^= 1;
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoForceInMenuStatus, Localize("Always send in-menu status"), g_Config.m_KaizoForceInMenuStatus, &Button))
			{
				g_Config.m_KaizoForceInMenuStatus ^= 1;
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoFastRespawn, Localize("Spam click to respawn"), g_Config.m_KaizoFastRespawn, &Button))
			{
				g_Config.m_KaizoFastRespawn ^= 1;
			}
			
			break;
		}
		case KAIZO_SETTINGS_TAB_INFO:
		{
			CUIRect ImageRect;
			SettingsBox.HSplitTop(30.0f, nullptr, &SettingsBox);
			SettingsBox.HSplitTop(103.0f*1.5f, &ImageRect, &SettingsBox);
			ImageRect.VSplitMid(&ImageRect, nullptr, 0.f);

			Graphics()->TextureSet(g_pData->m_aImages[IMAGE_BANNER].m_Id);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1, 1, 1, 1);
			IGraphics::CQuadItem QuadItem((ImageRect.w - 170*1.5f) + ImageRect.x, ImageRect.y, 360.f*1.5f, 103*1.5f);
			Graphics()->QuadsDrawTL(&QuadItem, 1);
			Graphics()->QuadsEnd();

			SettingsBox.HSplitTop(20.0f, nullptr, &SettingsBox);
			SettingsBox.HSplitTop(20.0f, &Label, &SettingsBox);

			Ui()->DoLabel(&Label, Localize("Kaizo Client By +KZ"), 30.0f, TEXTALIGN_MC);

			SettingsBox.HSplitTop(30.0f, nullptr, &SettingsBox);
			SettingsBox.HSplitTop(20.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Based on DDNet, with some code from other DDNet mods"), 15.0f, TEXTALIGN_MC);

			SettingsBox.HSplitTop(50.0f, nullptr, &SettingsBox);

			CUIRect ButtonsArea;

			CUIRect DiscordRect;
			CUIRect SourceCodeRect;

			CButtonContainer DiscordButton;
			CButtonContainer SourceCodeButton;

			SettingsBox.HSplitTop(35.f, &ButtonsArea, &SettingsBox);
			ButtonsArea.VSplitMid(&DiscordRect, &SourceCodeRect, 50.0f);
			DiscordRect.HSplitTop(30.0f, &DiscordRect, &ButtonsArea);
			SourceCodeRect.HSplitTop(30.0f, &SourceCodeRect, &ButtonsArea);

			CUIRect ButtonsArea2;

			CUIRect WebsiteRect;

			CButtonContainer WebsiteButton;

			SettingsBox.HSplitTop(60.f, &ButtonsArea2, &SettingsBox);
			ButtonsArea2.VMargin(ButtonsArea2.w/2 + 25.f - DiscordRect.w/2, &WebsiteRect);
			WebsiteRect.HSplitTop(30.0f, &WebsiteRect, &ButtonsArea2);

			if(DoButton_Menu(&DiscordButton, Localize("Discord"), false, &DiscordRect))
			{
				Client()->ViewLink(Localize("https://m0rekz.github.io/discord.html"));
			}

			if(DoButton_Menu(&SourceCodeButton, Localize("Source Code"), false, &SourceCodeRect))
			{
				Client()->ViewLink(Localize("https://www.github.com/m0rekz/kaizo-ddnet"));
			}

			if(DoButton_Menu(&WebsiteButton, Localize("+KZ Website"), false, &WebsiteRect))
			{
				Client()->ViewLink(Localize("https://m0rekz.github.io/"));
			}

			break;
		}
	}

	s_ScrollRegion.End();
}
