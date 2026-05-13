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
	KAIZO_SETTINGS_TAB_TCLIENT_BINDWHEEL,
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
		Localize("Bind Wheel"),
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
	CScrollRegionParams ScrollParams;
	ScrollParams.m_ScrollUnit = 120.0f;
	s_ScrollRegion.Begin(&SettingsBoxContainer, &ScrollParams);

	float ScrollHeight = 0.f;

	// for now all is 0, but ready for future additions
	switch (s_CurCustomTab)
	{
	case KAIZO_SETTINGS_TAB_KAIZO:
		ScrollHeight = 800.f;
		break;
	case KAIZO_SETTINGS_TAB_TCLIENT_BINDWHEEL:
		ScrollHeight = SettingsBoxContainer.h;
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

	SettingsBox.VSplitRight(20.f, &SettingsBox, nullptr);

	switch(s_CurCustomTab)
	{
		case KAIZO_SETTINGS_TAB_KAIZO:
		{
			SettingsBox.VSplitMid(&Left, &Right, 20.0f);

			Left.HSplitTop(20.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Kaizo Settings"), 20.0f, TEXTALIGN_ML);
			Left.HSplitTop(25.0f, &Label, &Left);

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

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoRevertHookLine, Localize("Old Hook line"), g_Config.m_KaizoRevertHookLine, &Button))
			{
				if(g_Config.m_KaizoRevertHookLine)
				{
					g_Config.m_KaizoRevertHookLine = 0;
				}
				else
				{
					g_Config.m_KaizoRevertHookLine = 1;
				}
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoSendLanguage, Localize("Send Client language to server"), g_Config.m_KaizoSendLanguage, &Button))
			{
				g_Config.m_KaizoSendLanguage ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoPrefer07Protocol, Localize("Always prefer 0.7 connection"), g_Config.m_KaizoPrefer07Protocol, &Button))
			{
				g_Config.m_KaizoPrefer07Protocol ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoAlwaysAllowXSkins, Localize("Be able to see players using \"x_\" prefixed skins"), g_Config.m_KaizoAlwaysAllowXSkins, &Button))
			{
				g_Config.m_KaizoAlwaysAllowXSkins ^= 1;
			}

			Left.HSplitTop(40.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Discord Settings"), 20.0f, TEXTALIGN_ML);
			Left.HSplitTop(40.0f, &Label, &Left);

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoDiscordRpc, Localize("Enable Discord Rich Presence (Requires restart)"), g_Config.m_KaizoDiscordRpc, &Button))
			{
				g_Config.m_KaizoDiscordRpc ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_KaizoDiscordLaunchSteam, Localize("Open Steam when joining through Discord (requires restarting Discord and Client)"), g_Config.m_KaizoDiscordLaunchSteam, &Button))
			{
				g_Config.m_KaizoDiscordLaunchSteam ^= 1;
			}

			// Prediction
			Right.HSplitTop(20.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Prediction"), 20.0f, TEXTALIGN_ML);
			Right.HSplitTop(25.0f, &Label, &Right);

			Right.HSplitTop(2.0f, &Label, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoFastInput, Localize("Fast input"), g_Config.m_KaizoFastInput, &Button))
			{
				g_Config.m_KaizoFastInput ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoFastInputOthers, Localize("Fast input others"), g_Config.m_KaizoFastInputOthers, &Button))
			{
				g_Config.m_KaizoFastInputOthers ^= 1;
			}

			if(g_Config.m_KaizoFastInput || g_Config.m_KaizoFastInputOthers)
			{
				Right.HSplitTop(2.0f, nullptr, &Right);
				Right.HSplitTop(20.0f, &Button, &Right);
				Ui()->DoScrollbarOption(&g_Config.m_KaizoFastInputAmount, &g_Config.m_KaizoFastInputAmount, &Button, Localize("Fast input amount"), 1, 100, &CUi::ms_LogarithmicScrollbarScale, 0u, " ms");

				Right.HSplitTop(2.0f, nullptr, &Right);
				Right.HSplitTop(10.0f, &Label, &Right);
				Ui()->DoLabel(&Label, Localize("20 or 30 ms is recommended"), 10.0f, TEXTALIGN_ML);
			}

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
			if(DoButton_CheckBox(&g_Config.m_KaizoApplyGuessedInput, Localize("Use received info as player input for prediction"), g_Config.m_KaizoApplyGuessedInput, &Button))
			{
				g_Config.m_KaizoApplyGuessedInput ^= 1;
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
			if(DoButton_CheckBox(&g_Config.m_KaizoPredictOthersEffects, Localize("Predict other players effects"), g_Config.m_KaizoPredictOthersEffects, &Button))
			{
				g_Config.m_KaizoPredictOthersEffects ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoPredictGameTypes, Localize("Predict GameTypes"), g_Config.m_KaizoPredictGameTypes, &Button))
			{
				g_Config.m_KaizoPredictGameTypes ^= 1;
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

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoAlwaysAllowDummy, Localize("Always be able to connect dummy"), g_Config.m_KaizoAlwaysAllowDummy, &Button))
			{
				g_Config.m_KaizoAlwaysAllowDummy ^= 1;
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoAlwaysAllowShowHookColl, Localize("Always be able to use \"Show Hook Collisions\""), g_Config.m_KaizoAlwaysAllowShowHookColl, &Button))
			{
				g_Config.m_KaizoAlwaysAllowShowHookColl ^= 1;
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoShowXSkinsInSettings, Localize("Show \"x_\" prefixed skins in Skin settings"), g_Config.m_KaizoShowXSkinsInSettings, &Button))
			{
				g_Config.m_KaizoShowXSkinsInSettings ^= 1;
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoForceDDNetHUD, Localize("Always try to show DDNet HUD"), g_Config.m_KaizoForceDDNetHUD, &Button))
			{
				g_Config.m_KaizoForceDDNetHUD ^= 1;
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoForceBugDDRaceInput, Localize("Act as if \'Bug DDRace Input\' is enabled"), g_Config.m_KaizoForceBugDDRaceInput, &Button))
			{
				g_Config.m_KaizoForceBugDDRaceInput ^= 1;
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoShowCharFlags, Localize("Show Character Flags in nameplate"), g_Config.m_KaizoShowCharFlags, &Button))
			{
				g_Config.m_KaizoShowCharFlags ^= 1;
			}

			if(g_Config.m_KaizoShowCharFlags)
			{
				Right.HSplitTop(2.0f, nullptr, &Right);
				Right.HSplitTop(20.0f, &Button, &Right);
				Ui()->DoScrollbarOption(&g_Config.m_KaizoShowCharFlagsSize, &g_Config.m_KaizoShowCharFlagsSize, &Button, Localize("Size of Characer Flags indicators"), 25, 100, &CUi::ms_LogarithmicScrollbarScale, 0u);
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoShowCharJumps, Localize("Show Character Jumps in nameplate"), g_Config.m_KaizoShowCharJumps, &Button))
			{
				g_Config.m_KaizoShowCharJumps ^= 1;
			}

			if(g_Config.m_KaizoShowCharJumps)
			{
				Right.HSplitTop(2.0f, nullptr, &Right);
				Right.HSplitTop(20.0f, &Button, &Right);
				Ui()->DoScrollbarOption(&g_Config.m_KaizoShowCharJumpsSize, &g_Config.m_KaizoShowCharJumpsSize, &Button, Localize("Size of Characer Jumps indicator"), 25, 100, &CUi::ms_LogarithmicScrollbarScale, 0u);
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoGrenadePath, Localize("Show Grenade and Gun collisions"), g_Config.m_KaizoGrenadePath, &Button))
			{
				g_Config.m_KaizoGrenadePath ^= 1;
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoLaserPath, Localize("Show Laser collisions"), g_Config.m_KaizoLaserPath, &Button))
			{
				g_Config.m_KaizoLaserPath ^= 1;
			}

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_KaizoFreeMouse, Localize("Free mouse"), g_Config.m_KaizoFreeMouse, &Button))
			{
				g_Config.m_KaizoFreeMouse ^= 1;
			}
			
			break;
		}
		case KAIZO_SETTINGS_TAB_TCLIENT_BINDWHEEL:
		{
			RenderSettingsTClientBindWheel(SettingsBox);
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
			Ui()->DoLabel(&Label, Localize("Based on Kaizo Network and DDNet, with some code from other DDNet mods"), 15.0f, TEXTALIGN_MC);

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

//copypasted TClient consts
const float FontSize = 14.0f;
const float EditBoxFontSize = 12.0f;
const float LineSize = 20.0f;
const float ColorPickerLineSize = 25.0f;
const float HeadlineFontSize = 20.0f;
const float StandardFontSize = 14.0f;

const float HeadlineHeight = HeadlineFontSize + 0.0f;
const float Margin = 10.0f;
const float MarginSmall = 5.0f;
const float MarginExtraSmall = 2.5f;
const float MarginBetweenSections = 30.0f;
const float MarginBetweenViews = 30.0f;

const float ColorPickerLabelSize = 13.0f;
const float ColorPickerLineSpacing = 5.0f;

//this function is from TClient:
bool CMenus::DoLine_KeyReader(CUIRect &View, CButtonContainer &ReaderButton, CButtonContainer &ClearButton, const char *pName, const char *pCommand)
{
	CBindSlot Bind(0, 0);
	for(int Mod = 0; Mod < KeyModifier::COMBINATION_COUNT; Mod++)
	{
		for(int KeyId = 0; KeyId < KEY_LAST; KeyId++)
		{
			const char *pBind = GameClient()->m_Binds.Get(KeyId, Mod);
			if(!pBind[0])
				continue;

			if(str_comp(pBind, pCommand) == 0)
			{
				Bind.m_Key = KeyId;
				Bind.m_ModifierMask = Mod;
				break;
			}
		}
	}

	CUIRect KeyButton, KeyLabel;
	View.HSplitTop(LineSize, &KeyButton, &View);
	KeyButton.VSplitMid(&KeyLabel, &KeyButton);

	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "%s:", pName);
	Ui()->DoLabel(&KeyLabel, aBuf, FontSize, TEXTALIGN_ML);

	View.HSplitTop(MarginExtraSmall, nullptr, &View);

	const auto Result = GameClient()->m_KeyBinder.DoKeyReader(&ReaderButton, &ClearButton, &KeyButton, Bind, false);
	if(Result.m_Bind != Bind)
	{
		if(Bind.m_Key != KEY_UNKNOWN)
			GameClient()->m_Binds.Bind(Bind.m_Key, "", false, Bind.m_ModifierMask);
		if(Result.m_Bind.m_Key != KEY_UNKNOWN)
			GameClient()->m_Binds.Bind(Result.m_Bind.m_Key, pCommand, false, Result.m_Bind.m_ModifierMask);
		return true;
	}
	return false;
}

//this function is from TClient:
void CMenus::RenderSettingsTClientBindWheel(CUIRect MainView)
{
	CUIRect LeftView, RightView, Label, Button;
	MainView.VSplitLeft(MainView.w / 2.1f, &LeftView, &RightView);

	const float Radius = minimum(RightView.w, RightView.h) / 2.0f;
	vec2 Center = RightView.Center();
	// Draw Circle
	Graphics()->TextureClear();
	Graphics()->QuadsBegin();
	Graphics()->SetColor(0.0f, 0.0f, 0.0f, 0.3f);
	Graphics()->DrawCircle(Center.x, Center.y, Radius, 64);
	Graphics()->QuadsEnd();

	static char s_aBindName[BINDWHEEL_MAX_NAME];
	static char s_aBindCommand[BINDWHEEL_MAX_CMD];

	static int s_SelectedBindIndex = -1;
	int HoveringIndex = -1;

	float MouseDist = distance(Center, Ui()->MousePos());
	const int SegmentCount = GameClient()->m_BindWheel.m_vBinds.size();
	if(MouseDist < Radius && MouseDist > Radius * 0.25f && SegmentCount > 0)
	{
		float SegmentAngle = 2.0f * pi / SegmentCount;

		float HoveringAngle = angle(Ui()->MousePos() - Center) + SegmentAngle / 2.0f;
		if(HoveringAngle < 0.0f)
			HoveringAngle += 2.0f * pi;

		HoveringIndex = (int)(HoveringAngle / (2.0f * pi) * SegmentCount);
		HoveringIndex = std::clamp(HoveringIndex, 0, SegmentCount - 1);
		if(Ui()->MouseButtonClicked(0))
		{
			s_SelectedBindIndex = HoveringIndex;
			str_copy(s_aBindName, GameClient()->m_BindWheel.m_vBinds[HoveringIndex].m_aName);
			str_copy(s_aBindCommand, GameClient()->m_BindWheel.m_vBinds[HoveringIndex].m_aCommand);
		}
		else if(Ui()->MouseButtonClicked(1) && s_SelectedBindIndex >= 0 && HoveringIndex >= 0 && HoveringIndex != s_SelectedBindIndex)
		{
			CBindWheel::CBind BindA = GameClient()->m_BindWheel.m_vBinds[s_SelectedBindIndex];
			CBindWheel::CBind BindB = GameClient()->m_BindWheel.m_vBinds[HoveringIndex];
			str_copy(GameClient()->m_BindWheel.m_vBinds[s_SelectedBindIndex].m_aName, BindB.m_aName);
			str_copy(GameClient()->m_BindWheel.m_vBinds[s_SelectedBindIndex].m_aCommand, BindB.m_aCommand);
			str_copy(GameClient()->m_BindWheel.m_vBinds[HoveringIndex].m_aName, BindA.m_aName);
			str_copy(GameClient()->m_BindWheel.m_vBinds[HoveringIndex].m_aCommand, BindA.m_aCommand);
		}
		else if(Ui()->MouseButtonClicked(2))
		{
			s_SelectedBindIndex = HoveringIndex;
		}
	}
	else if(MouseDist < Radius && Ui()->MouseButtonClicked(0))
	{
		s_SelectedBindIndex = -1;
		str_copy(s_aBindName, "");
		str_copy(s_aBindCommand, "");
	}

	const float Theta = pi * 2.0f / std::max<float>(1.0f, GameClient()->m_BindWheel.m_vBinds.size());
	for(int i = 0; i < static_cast<int>(GameClient()->m_BindWheel.m_vBinds.size()); i++)
	{
		TextRender()->TextColor(ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));

		float SegmentFontSize = FontSize * 1.1f;
		if(i == s_SelectedBindIndex)
		{
			SegmentFontSize = FontSize * 1.7f;
			TextRender()->TextColor(ColorRGBA(0.5f, 1.0f, 0.75f, 1.0f));
		}
		else if(i == HoveringIndex)
		{
			SegmentFontSize = FontSize * 1.35f;
		}

		const CBindWheel::CBind Bind = GameClient()->m_BindWheel.m_vBinds[i];
		const float Angle = Theta * i;

		const vec2 Pos = direction(Angle) * (Radius * 0.75f) + Center;
		const CUIRect Rect = CUIRect{Pos.x - 50.0f, Pos.y - 50.0f, 100.0f, 100.0f};
		Ui()->DoLabel(&Rect, Bind.m_aName, SegmentFontSize, TEXTALIGN_MC);
	}

	TextRender()->TextColor(ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	LeftView.HSplitTop(LineSize, &Button, &LeftView);
	Button.VSplitLeft(100.0f, &Label, &Button);
	Ui()->DoLabel(&Label, Localize("Name:"), FontSize, TEXTALIGN_ML);
	static CLineInput s_NameInput;
	s_NameInput.SetBuffer(s_aBindName, sizeof(s_aBindName));
	s_NameInput.SetEmptyText(Localize("Name"));
	Ui()->DoEditBox(&s_NameInput, &Button, EditBoxFontSize);

	LeftView.HSplitTop(MarginSmall, nullptr, &LeftView);
	LeftView.HSplitTop(LineSize, &Button, &LeftView);
	Button.VSplitLeft(100.0f, &Label, &Button);
	Ui()->DoLabel(&Label, Localize("Command:"), FontSize, TEXTALIGN_ML);
	static CLineInput s_BindInput;
	s_BindInput.SetBuffer(s_aBindCommand, sizeof(s_aBindCommand));
	s_BindInput.SetEmptyText(Localize("Command"));
	Ui()->DoEditBox(&s_BindInput, &Button, EditBoxFontSize);

	static CButtonContainer s_AddButton, s_RemoveButton, s_OverrideButton;

	LeftView.HSplitTop(MarginSmall, nullptr, &LeftView);
	LeftView.HSplitTop(LineSize, &Button, &LeftView);
	if(DoButton_Menu(&s_OverrideButton, Localize("Override Selected"), 0, &Button) && s_SelectedBindIndex >= 0 && s_SelectedBindIndex < static_cast<int>(GameClient()->m_BindWheel.m_vBinds.size()))
	{
		CBindWheel::CBind TempBind;
		if(str_length(s_aBindName) == 0)
			str_copy(TempBind.m_aName, "*");
		else
			str_copy(TempBind.m_aName, s_aBindName);

		str_copy(GameClient()->m_BindWheel.m_vBinds[s_SelectedBindIndex].m_aName, TempBind.m_aName);
		str_copy(GameClient()->m_BindWheel.m_vBinds[s_SelectedBindIndex].m_aCommand, s_aBindCommand);
	}
	LeftView.HSplitTop(MarginSmall, nullptr, &LeftView);
	LeftView.HSplitTop(LineSize, &Button, &LeftView);
	CUIRect ButtonAdd, ButtonRemove;
	Button.VSplitMid(&ButtonRemove, &ButtonAdd, MarginSmall);
	if(DoButton_Menu(&s_AddButton, Localize("Add Bind"), 0, &ButtonAdd))
	{
		CBindWheel::CBind TempBind;
		if(str_length(s_aBindName) == 0)
			str_copy(TempBind.m_aName, "*");
		else
			str_copy(TempBind.m_aName, s_aBindName);

		GameClient()->m_BindWheel.AddBind(TempBind.m_aName, s_aBindCommand);
		s_SelectedBindIndex = static_cast<int>(GameClient()->m_BindWheel.m_vBinds.size()) - 1;
	}
	if(DoButton_Menu(&s_RemoveButton, Localize("Remove Bind"), 0, &ButtonRemove) && s_SelectedBindIndex >= 0)
	{
		GameClient()->m_BindWheel.RemoveBind(s_SelectedBindIndex);
		s_SelectedBindIndex = -1;
	}

	LeftView.HSplitTop(MarginSmall, nullptr, &LeftView);
	LeftView.HSplitTop(LineSize, &Label, &LeftView);
	Ui()->DoLabel(&Label, Localize("The command is ran in console not chat"), FontSize, TEXTALIGN_ML);
	LeftView.HSplitTop(LineSize * 0.8f, &Label, &LeftView);
	Ui()->DoLabel(&Label, Localize("Use left mouse to select"), FontSize * 0.8f, TEXTALIGN_ML);
	LeftView.HSplitTop(LineSize * 0.8f, &Label, &LeftView);
	Ui()->DoLabel(&Label, Localize("Use right mouse to swap with selected"), FontSize * 0.8f, TEXTALIGN_ML);
	LeftView.HSplitTop(LineSize * 0.8f, &Label, &LeftView);
	Ui()->DoLabel(&Label, Localize("Use middle mouse select without copy"), FontSize * 0.8f, TEXTALIGN_ML);

	LeftView.HSplitBottom(LineSize, &LeftView, &Label);
	static CButtonContainer s_ReaderButtonWheel, s_ClearButtonWheel;
	DoLine_KeyReader(Label, s_ReaderButtonWheel, s_ClearButtonWheel, Localize("Bind Wheel Key"), "+bindwheel");

	LeftView.HSplitBottom(LineSize, &LeftView, &Label);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_KaizoResetBindWheelMouse, Localize("Reset position of mouse when opening bindwheel"), &g_Config.m_KaizoResetBindWheelMouse, &Label, LineSize);
}
