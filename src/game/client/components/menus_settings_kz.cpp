// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// RenderSettingsKaizo has code from menus_settings.cpp and menus_settings_assets.cpp

#include <game/localization.h>
#include "menus.h"
#include <game/client/ui_listbox.h>
#include <engine/shared/localization.h>
#include <game/client/gameclient.h>
#include <generated/client_data.h>
#include <base/helper_kz.h>
#include <game/version.h>

//+KZ im not writing all of the configs one by one here :P
//+KZ start
#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Save, Desc)	\
extern int g_KaizoConfig_##Name;
#define MACRO_CONFIG_COL(Name, ScriptName, Def, Save, Desc)	\
extern unsigned g_KaizoConfig_##Name;
#define MACRO_CONFIG_STR(Name, ScriptName, Len, Def, Save, Desc)	\
extern CKZConfigString g_KaizoConfig_##Name;

#include <engine/shared/config_variables_kz.h>

#undef MACRO_CONFIG_INT
#undef MACRO_CONFIG_COL
#undef MACRO_CONFIG_STR
//+KZ end

enum
{
	KAIZO_SETTINGS_TAB_KAIZO = 0,
	KAIZO_SETTINGS_TAB_TCLIENT_BINDWHEEL,
	KAIZO_SETTINGS_TAB_SPLITS,
	KAIZO_SETTINGS_TAB_PASSWORDS,
	KAIZO_SETTINGS_TAB_DANGEROUS, //toggled by kaizo_show_dangerous_settings
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
//end of copypasted TClient consts

void CMenus::RenderSettingsKaizo(CUIRect MainView)
{
	CUIRect TabBar;

	CUIRect Button;

	CUIRect Label;

	CUIRect Left, Right;

	int NumTabsToShow = g_KaizoConfig_KaizoShowDangerousSettings ? NUM_KAIZO_SETTINGS_TABS : (NUM_KAIZO_SETTINGS_TABS - 1);

	MainView.HSplitTop(20.0f, &TabBar, &MainView);
	const float TabWidth = TabBar.w / NumTabsToShow;
	static CButtonContainer s_aPageTabs[NUM_KAIZO_SETTINGS_TABS] = {};
	const char *apTabNames[NUM_KAIZO_SETTINGS_TABS] = {
		Localize("Kaizo"),
		Localize("Bind Wheel"),
		Localize("Splits", "Kaizo Client Settings"),
		Localize("Passwords"),
		Localize("Dangerous", "Kaizo Client Settings"),
		Localize("Info"),
		};

	for(int Tab = KAIZO_SETTINGS_TAB_KAIZO; Tab < NUM_KAIZO_SETTINGS_TABS; ++Tab)
	{
		if(!g_KaizoConfig_KaizoShowDangerousSettings && Tab == KAIZO_SETTINGS_TAB_DANGEROUS)
			continue;

		CUIRect PageButton;
		TabBar.VSplitLeft(TabWidth, &PageButton, &TabBar);
		const int Corners = Tab == KAIZO_SETTINGS_TAB_KAIZO ? IGraphics::CORNER_L : Tab == NumTabsToShow - 1 ? IGraphics::CORNER_R : IGraphics::CORNER_NONE;
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

	// settings tab height so we can scroll
	switch (s_CurCustomTab)
	{
	case KAIZO_SETTINGS_TAB_KAIZO:
		ScrollHeight = 920.f;
		break;
	case KAIZO_SETTINGS_TAB_TCLIENT_BINDWHEEL:
		ScrollHeight = SettingsBoxContainer.h;
		break;
	case KAIZO_SETTINGS_TAB_PASSWORDS:
		{
			float Calc = 24.f;

			Calc += 24.f * GameClient()->m_PasswordManager.GetPasswordAmount();

			ScrollHeight = Calc;
		}
		break;
	case KAIZO_SETTINGS_TAB_DANGEROUS:
		ScrollHeight = 750.f;
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
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoPingCircles, Localize("Show ping circles above players"), g_KaizoConfig_KaizoPingCircles, &Button))
			{
				g_KaizoConfig_KaizoPingCircles ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoSleepingInMenuPlayers, Localize("Show players navigating menu as sleeping tees"), g_KaizoConfig_KaizoSleepingInMenuPlayers, &Button))
			{
				g_KaizoConfig_KaizoSleepingInMenuPlayers ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoHudRealPosition, Localize("If HUD player position is enabled, show real position"), g_KaizoConfig_KaizoHudRealPosition, &Button))
			{
				g_KaizoConfig_KaizoHudRealPosition ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoHudRealVelocity, Localize("If HUD player velocity is enabled, show real velocity"), g_KaizoConfig_KaizoHudRealVelocity, &Button))
			{
				g_KaizoConfig_KaizoHudRealVelocity ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoFastMapDownload, Localize("Fast map download (Experimental)"), g_KaizoConfig_KaizoFastMapDownload, &Button))
			{
				g_KaizoConfig_KaizoFastMapDownload ^= 1;
			}

			if(g_KaizoConfig_KaizoFastMapDownload)
			{
				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(20.0f, &Button, &Left);
				Ui()->DoScrollbarOption(&g_KaizoConfig_KaizoFastMapDownloadWindow, &g_KaizoConfig_KaizoFastMapDownloadWindow, &Button, Localize("Map data to request"), 0, 500, &CUi::ms_LogarithmicScrollbarScale, 0u, " KiB/s");

				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(10.0f, &Label, &Left);
				Ui()->DoLabel(&Label, Localize("NOTE: high values may break map download"), 10.0f, TEXTALIGN_ML);
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoReplyTabbedOut, Localize("Reply when tabbed out"), g_KaizoConfig_KaizoReplyTabbedOut, &Button))
			{
				g_KaizoConfig_KaizoReplyTabbedOut ^= 1;
			}

			if(g_KaizoConfig_KaizoReplyTabbedOut)
			{
				static CLineInput s_TabbedOutInput;
				s_TabbedOutInput.SetBuffer(g_KaizoConfig_KaizoReplyTabbedOutMsg.m_pStr, g_KaizoConfig_KaizoReplyTabbedOutMsg.m_Length);

				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(10.0f, &Label, &Left);
				Ui()->DoLabel(&Label, Localize("Message to send:"), 10.0f, TEXTALIGN_ML);

				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(20.0f, &Button, &Left);
				Ui()->DoEditBox(&s_TabbedOutInput, &Button, 14.0f);
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoRevertHookLine, Localize("Old Hook line"), g_KaizoConfig_KaizoRevertHookLine, &Button))
			{
				if(g_KaizoConfig_KaizoRevertHookLine)
				{
					g_KaizoConfig_KaizoRevertHookLine = 0;
				}
				else
				{
					g_KaizoConfig_KaizoRevertHookLine = 1;
				}
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoAlwaysAllowXSkins, Localize("Be able to see players using \"x_\" prefixed skins"), g_KaizoConfig_KaizoAlwaysAllowXSkins, &Button))
			{
				g_KaizoConfig_KaizoAlwaysAllowXSkins ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoStatisticsShowFps, Localize("Show FPS"), g_KaizoConfig_KaizoStatisticsShowFps, &Button))
			{
				g_KaizoConfig_KaizoStatisticsShowFps ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoStatisticsShowPing, Localize("Show ping"), g_KaizoConfig_KaizoStatisticsShowPing, &Button))
			{
				g_KaizoConfig_KaizoStatisticsShowPing ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoStatisticsShowSnapRate, Localize("Show snapshot rate"), g_KaizoConfig_KaizoStatisticsShowSnapRate, &Button))
			{
				g_KaizoConfig_KaizoStatisticsShowSnapRate ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			{ //to make a char buffer only accesible here

				static char s_aBuf[128] = {'\0'};
				static int s_PrevKaizoEmoticonToEmoji = -1;

				if(g_KaizoConfig_KaizoEmoticonToEmoji != s_PrevKaizoEmoticonToEmoji)
				{
					const char *pHamburger = g_KaizoConfig_KaizoEmoticonToEmoji ? "🍔" : ":hamburger:";
					str_format(s_aBuf, sizeof(s_aBuf), "%s %s",Localize("Send :emoticons: as emojis"), pHamburger);
					s_PrevKaizoEmoticonToEmoji = g_KaizoConfig_KaizoEmoticonToEmoji;
				}

				Left.HSplitTop(20.0f, &Button, &Left);
				if(DoButton_CheckBox(&g_KaizoConfig_KaizoEmoticonToEmoji, s_aBuf, g_KaizoConfig_KaizoEmoticonToEmoji, &Button))
				{
					g_KaizoConfig_KaizoEmoticonToEmoji ^= 1;
				}
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoShowAuthedStateInGame, Localize("Show authed state in-game"), g_KaizoConfig_KaizoShowAuthedStateInGame, &Button))
			{
				g_KaizoConfig_KaizoShowAuthedStateInGame ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoShowPreciseAuthedState, Localize("Show more precise authed colors"), g_KaizoConfig_KaizoShowPreciseAuthedState, &Button))
			{
				g_KaizoConfig_KaizoShowPreciseAuthedState ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			{
				static CButtonContainer s_Color;
				DoButton_ColorPickerAutoVMargin(&s_Color, Localize("Admin color"), &g_Config.m_ClAuthedPlayerColor, color_cast<ColorRGBA>(ColorHSLA(5898211 /*default value*/)), &Left, LineSize, true);
			}

			if(g_KaizoConfig_KaizoShowPreciseAuthedState)
			{
				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(20.0f, &Button, &Left);
				{
					static CButtonContainer s_Color;
					DoButton_ColorPickerAutoVMargin(&s_Color, Localize("Moderator color"), &g_KaizoConfig_KaizoModeratorAuthedColor, color_cast<ColorRGBA>(ColorHSLA(5898211 /*default value*/)), &Left, LineSize, true);
				}
				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(20.0f, &Button, &Left);
				{
					static CButtonContainer s_Color;
					DoButton_ColorPickerAutoVMargin(&s_Color, Localize("Helper color"), &g_KaizoConfig_KaizoHelperAuthedColor, color_cast<ColorRGBA>(ColorHSLA(5898211 /*default value*/)), &Left, LineSize, true);
				}
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			//Mod support
			Left.HSplitTop(40.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Mod support and improvements"), 20.0f, TEXTALIGN_ML);
			Left.HSplitTop(40.0f, &Label, &Left);

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoKeepMenuAfterVoteInMods, Localize("Fix vote menu in F-DDrace, FoxNet, MMOTEE, etc..."), g_KaizoConfig_KaizoKeepMenuAfterVoteInMods, &Button))
			{
				g_KaizoConfig_KaizoKeepMenuAfterVoteInMods ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoEmotionalTees, Localize("Emotional Tees in non-DDNet servers (Client-side)"), g_KaizoConfig_KaizoEmotionalTees, &Button))
			{
				g_KaizoConfig_KaizoEmotionalTees ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoSendLanguage, Localize("Send Client language to server"), g_KaizoConfig_KaizoSendLanguage, &Button))
			{
				g_KaizoConfig_KaizoSendLanguage ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoShowCrowns, Localize("Show crowns in Kaizo Network"), g_KaizoConfig_KaizoShowCrowns, &Button))
			{
				g_KaizoConfig_KaizoShowCrowns ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoInstaShieldShield, Localize("InstaShield Shield (for \"iCTF)\")"), g_KaizoConfig_KaizoInstaShieldShield, &Button))
			{
				g_KaizoConfig_KaizoInstaShieldShield ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoKillingSpreeSparkles, Localize("Killing spree sparkles"), g_KaizoConfig_KaizoKillingSpreeSparkles, &Button))
			{
				g_KaizoConfig_KaizoKillingSpreeSparkles ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoPrefer07Protocol, Localize("Always prefer 0.7 connection"), g_KaizoConfig_KaizoPrefer07Protocol, &Button))
			{
				g_KaizoConfig_KaizoPrefer07Protocol ^= 1;
			}

			//Client identify
			Left.HSplitTop(40.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Client identification"), 20.0f, TEXTALIGN_ML);
			Left.HSplitTop(40.0f, &Label, &Left);

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoShowClientType, Localize("Try to identify custom clients of other players"), g_KaizoConfig_KaizoShowClientType, &Button))
			{
				g_KaizoConfig_KaizoShowClientType ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoSendClientType, Localize("Let know other clients we are using Kaizo Client"), g_KaizoConfig_KaizoSendClientType, &Button))
			{
				g_KaizoConfig_KaizoSendClientType ^= 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoClientIndicatorBrowser, Localize("Show detected Kaizo Client users in friends tab"), g_KaizoConfig_KaizoClientIndicatorBrowser, &Button))
			{
				g_KaizoConfig_KaizoClientIndicatorBrowser ^= 1;
			}

			// Prediction
			Right.HSplitTop(20.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Prediction"), 20.0f, TEXTALIGN_ML);
			Right.HSplitTop(25.0f, &Label, &Right);

			Right.HSplitTop(2.0f, &Label, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoFastInput, Localize("Fast input"), g_KaizoConfig_KaizoFastInput, &Button))
			{
				g_KaizoConfig_KaizoFastInput ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoFastInputOthers, Localize("Fast input others"), g_KaizoConfig_KaizoFastInputOthers, &Button))
			{
				g_KaizoConfig_KaizoFastInputOthers ^= 1;
			}

			if(g_KaizoConfig_KaizoFastInput || g_KaizoConfig_KaizoFastInputOthers)
			{
				Right.HSplitTop(2.0f, nullptr, &Right);
				Right.HSplitTop(20.0f, &Button, &Right);
				Ui()->DoScrollbarOption(&g_KaizoConfig_KaizoFastInputAmount, &g_KaizoConfig_KaizoFastInputAmount, &Button, Localize("Fast input amount"), 1, 100, &CUi::ms_LogarithmicScrollbarScale, 0u, " ms");

				Right.HSplitTop(2.0f, nullptr, &Right);
				Right.HSplitTop(10.0f, &Label, &Right);
				Ui()->DoLabel(&Label, Localize("20 or 30 ms is recommended"), 10.0f, TEXTALIGN_ML);
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoAntiPingImproved, Localize("Better Antiping algorithm"), g_KaizoConfig_KaizoAntiPingImproved, &Button))
			{
				g_KaizoConfig_KaizoAntiPingImproved ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoAntiPingNegativeBuffer, Localize("Negative stability buffer (for Gores)"), g_KaizoConfig_KaizoAntiPingNegativeBuffer, &Button))
			{
				g_KaizoConfig_KaizoAntiPingNegativeBuffer ^= 1;
			}

			/*
			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoApplyGuessedInput, Localize("Use received info as player input for prediction"), g_KaizoConfig_KaizoApplyGuessedInput, &Button))
			{
				g_KaizoConfig_KaizoApplyGuessedInput ^= 1;
			}
			*/

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoPredictGoresGrenadeTele, Localize("Predict Gores grenade teleport"), g_KaizoConfig_KaizoPredictGoresGrenadeTele, &Button))
			{
				g_KaizoConfig_KaizoPredictGoresGrenadeTele ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoPredictTeleToDeath, Localize("Predict death effect if teleport leads to Death"), g_KaizoConfig_KaizoPredictTeleToDeath, &Button))
			{
				g_KaizoConfig_KaizoPredictTeleToDeath ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoPredictDDNetTeleport, Localize("Predict DDNet teleports if there is only 1 exit"), g_KaizoConfig_KaizoPredictDDNetTeleport, &Button))
			{
				g_KaizoConfig_KaizoPredictDDNetTeleport ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoPredictDeathTiles, Localize("Predict Death tiles"), g_KaizoConfig_KaizoPredictDeathTiles, &Button))
			{
				g_KaizoConfig_KaizoPredictDeathTiles ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoPredictOthersEffects, Localize("Predict other players effects"), g_KaizoConfig_KaizoPredictOthersEffects, &Button))
			{
				g_KaizoConfig_KaizoPredictOthersEffects ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoPredictGameTypes, Localize("Predict GameTypes"), g_KaizoConfig_KaizoPredictGameTypes, &Button))
			{
				g_KaizoConfig_KaizoPredictGameTypes ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoPredictPointerTWPlus, Localize("Predict Pointer's TW+"), g_KaizoConfig_KaizoPredictPointerTWPlus, &Button))
			{
				g_KaizoConfig_KaizoPredictPointerTWPlus ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoPredictVanillaHammerFix, Localize("Fix Vanilla hammer hit prediction through walls"), g_KaizoConfig_KaizoPredictVanillaHammerFix, &Button))
			{
				g_KaizoConfig_KaizoPredictVanillaHammerFix ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoUnpredOthersInFreeze, Localize("Dont predict others if we are in freeze"), g_KaizoConfig_KaizoUnpredOthersInFreeze, &Button))
			{
				g_KaizoConfig_KaizoUnpredOthersInFreeze ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoPredMarginInFreeze, Localize("Change prediction margin while frozen"), g_KaizoConfig_KaizoPredMarginInFreeze, &Button))
			{
				g_KaizoConfig_KaizoPredMarginInFreeze ^= 1;
			}

			if(g_KaizoConfig_KaizoPredMarginInFreeze)
			{
				Right.HSplitTop(2.0f, nullptr, &Right);
				Right.HSplitTop(20.0f, &Button, &Right);
				Ui()->DoScrollbarOption(&g_KaizoConfig_KaizoPredMarginInFreezeAmount, &g_KaizoConfig_KaizoPredMarginInFreezeAmount, &Button, Localize("Margin while frozen"), 1, 100, &CUi::ms_LogarithmicScrollbarScale, 0u, "");
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoAlwaysRepredict, Localize("Always repredict (no idea if it is even useful)"), g_KaizoConfig_KaizoAlwaysRepredict, &Button))
			{
				g_KaizoConfig_KaizoAlwaysRepredict ^= 1;
			}

			//Discord
			Right.HSplitTop(40.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Discord Settings"), 20.0f, TEXTALIGN_ML);
			Right.HSplitTop(40.0f, &Label, &Right);

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoDiscordRpc, Localize("Enable Discord Rich Presence (Requires restart)"), g_KaizoConfig_KaizoDiscordRpc, &Button))
			{
				g_KaizoConfig_KaizoDiscordRpc ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoDiscordLaunchSteam, Localize("Open Steam when joining through Discord (requires restarting Discord and Client)"), g_KaizoConfig_KaizoDiscordLaunchSteam, &Button))
			{
				g_KaizoConfig_KaizoDiscordLaunchSteam ^= 1;
			}

			//Visual-only
			Right.HSplitTop(40.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Visual-only Settings"), 20.0f, TEXTALIGN_ML);
			Right.HSplitTop(40.0f, &Label, &Right);

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoChatInputBackground, Localize("Show background while typing in chat"), g_KaizoConfig_KaizoChatInputBackground, &Button))
			{
				g_KaizoConfig_KaizoChatInputBackground ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoRotatingHammer, Localize("Rotating hammer (like other weapons)"), g_KaizoConfig_KaizoRotatingHammer, &Button))
			{
				g_KaizoConfig_KaizoRotatingHammer ^= 1;
			}

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			Ui()->DoScrollbarOption(&g_KaizoConfig_KaizoScoreboardStyle, &g_KaizoConfig_KaizoScoreboardStyle, &Button, Localize("Scoreboard style"), 0, 3, &CUi::ms_LogarithmicScrollbarScale, 0u, "");

			Right.HSplitTop(2.0f, nullptr, &Right);

			Right.HSplitTop(20.0f, &Button, &Right);
			Ui()->DoScrollbarOption(&g_KaizoConfig_KaizoScoreboardShorten, &g_KaizoConfig_KaizoScoreboardShorten, &Button, Localize("Shorten scoreboard"), 0, 2, &CUi::ms_LogarithmicScrollbarScale, 0u, "");

			break;
		}
		case KAIZO_SETTINGS_TAB_TCLIENT_BINDWHEEL:
		{
			RenderSettingsTClientBindWheel(SettingsBox);
			break;
		}
		case KAIZO_SETTINGS_TAB_SPLITS:
		{
			//SettingsBox.VSplitMid(&Left, &Right, 20.0f);
			Left = SettingsBox;

			Left.HSplitTop(20.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Splits Settings"), 20.0f, TEXTALIGN_ML);
			Left.HSplitTop(40.0f, &Label, &Left);

			//Allowing the user to disable it will make fake reports of it being bugged
			/*Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoSplits, Localize("Track Splits"), g_KaizoConfig_KaizoSplits, &Button))
			{
				g_KaizoConfig_KaizoSplits ^= 1;
			}*/

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			Ui()->DoLabel(&Button, Localize("Show Splits HUD"), Button.h * CUi::ms_FontmodHeight * 0.8f, TEXTALIGN_ML);

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox("splits hud disabled", Localize("Disabled"), !g_KaizoConfig_KaizoShowhudSplits, &Button))
			{
				g_KaizoConfig_KaizoShowhudSplits = 0;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox("splits hud enabled on finished maps", Localize("Enabled on finished maps"), g_KaizoConfig_KaizoShowhudSplits == 1, &Button))
			{
				g_KaizoConfig_KaizoShowhudSplits = 1;
			}

			Left.HSplitTop(2.0f, nullptr, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox("splits hud always enabled", Localize("Always enabled"), g_KaizoConfig_KaizoShowhudSplits == 2, &Button))
			{
				g_KaizoConfig_KaizoShowhudSplits = 2;
			}

			if(g_KaizoConfig_KaizoShowhudSplits)
			{
				Left.HSplitTop(5.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				Ui()->DoLabel(&Button, Localize("HUD settings"), Button.h * CUi::ms_FontmodHeight * 0.8f, TEXTALIGN_ML);

				Left.HSplitTop(2.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				if(DoButton_CheckBox(&g_KaizoConfig_KaizoSplitsShowMap, Localize("Show map in HUD"), g_KaizoConfig_KaizoSplitsShowMap, &Button))
				{
					g_KaizoConfig_KaizoSplitsShowMap = !g_KaizoConfig_KaizoSplitsShowMap;
				}

				Left.HSplitTop(2.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				if(DoButton_CheckBox(&g_KaizoConfig_KaizoSplitsShowSplits, Localize("Show splits in HUD (xD)"), g_KaizoConfig_KaizoSplitsShowSplits, &Button))
				{
					g_KaizoConfig_KaizoSplitsShowSplits = !g_KaizoConfig_KaizoSplitsShowSplits;
				}

				Left.HSplitTop(2.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				if(DoButton_CheckBox(&g_KaizoConfig_KaizoSplitsShowCategory, Localize("Show rank category in HUD"), g_KaizoConfig_KaizoSplitsShowCategory, &Button))
				{
					g_KaizoConfig_KaizoSplitsShowCategory = !g_KaizoConfig_KaizoSplitsShowCategory;
				}

				Left.HSplitTop(2.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				if(DoButton_CheckBox(&g_KaizoConfig_KaizoSplitsShowTimer, Localize("Show timer in HUD"), g_KaizoConfig_KaizoSplitsShowTimer, &Button))
				{
					g_KaizoConfig_KaizoSplitsShowTimer = !g_KaizoConfig_KaizoSplitsShowTimer;
				}

				Left.HSplitTop(2.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				if(DoButton_CheckBox(&g_KaizoConfig_KaizoSplitsShowLastSegment, Localize("Show previous segment time in HUD"), g_KaizoConfig_KaizoSplitsShowLastSegment, &Button))
				{
					g_KaizoConfig_KaizoSplitsShowLastSegment = !g_KaizoConfig_KaizoSplitsShowLastSegment;
				}

				Left.HSplitTop(2.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				if(DoButton_CheckBox(&g_KaizoConfig_KaizoSplitsShowFurthest, Localize("Show furthest segment in HUD"), g_KaizoConfig_KaizoSplitsShowFurthest, &Button))
				{
					g_KaizoConfig_KaizoSplitsShowFurthest = !g_KaizoConfig_KaizoSplitsShowFurthest;
				}

				Left.HSplitTop(2.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				if(DoButton_CheckBox(&g_KaizoConfig_KaizoSplitsShowSumOfBest, Localize("Show sum of best segments in HUD"), g_KaizoConfig_KaizoSplitsShowSumOfBest, &Button))
				{
					g_KaizoConfig_KaizoSplitsShowSumOfBest = !g_KaizoConfig_KaizoSplitsShowSumOfBest;
				}

				Left.HSplitTop(2.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				Ui()->DoScrollbarOption(&g_KaizoConfig_KaizoSplitsX, &g_KaizoConfig_KaizoSplitsX, &Button, Localize("HUD X Position"), 0, 100, &CUi::ms_LogarithmicScrollbarScale, 0u, "%");

				//Splits Labels Start

				Left.HSplitTop(5.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				Ui()->DoLabel(&Button, Localize("Splits labels"), Button.h * CUi::ms_FontmodHeight * 0.8f, TEXTALIGN_ML);

				Left.HSplitTop(2.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				if(DoButton_CheckBox("splits label none", Localize("None"), !g_KaizoConfig_KaizoSplitsLabels, &Button))
				{
					g_KaizoConfig_KaizoSplitsLabels = 0;
				}

				Left.HSplitTop(2.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				if(DoButton_CheckBox("splits label small", Localize("Small"), g_KaizoConfig_KaizoSplitsLabels == 1, &Button))
				{
					g_KaizoConfig_KaizoSplitsLabels = 1;
				}

				Left.HSplitTop(2.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				if(DoButton_CheckBox("splits label full size", Localize("Full size"), g_KaizoConfig_KaizoSplitsLabels == 2, &Button))
				{
					g_KaizoConfig_KaizoSplitsLabels = 2;
				}

				//Splits Labels End

				Left.HSplitTop(5.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				Ui()->DoLabel(&Button, Localize("Advanced settings can be changed in F1 console"), Button.h * CUi::ms_FontmodHeight * 0.8f, TEXTALIGN_ML);

				//Preview HUD
				//does not work for now, need more changes
				/*static bool s_DoPreview = false;

				Left.HSplitTop(2.0f, nullptr, &Left);

				Left.HSplitTop(20.0f, &Button, &Left);
				if(DoButton_CheckBox(&s_DoPreview, Localize("Preview"), s_DoPreview, &Button))
				{
					s_DoPreview = !s_DoPreview;
				}

				if(s_DoPreview)
				{
					GameClient()->m_Hud.RenderSplits();
				}*/
			}

			break;
		}
		case KAIZO_SETTINGS_TAB_PASSWORDS:
		{
			//text row
			CUIRect Row;
			Row.x = SettingsBox.x;
			Row.w = SettingsBox.w;
			Row.y = SettingsBox.y;
			Row.h = 22.f;

			//text to write
			CUIRect Account;
			CUIRect Password;
			CUIRect ServerIp;

			//type button
			CUIRect TypeButton;
			
			//save button
			CUIRect SaveButton;

			//warning
			SLabelProperties DangerLabelProps;
			DangerLabelProps.SetColor(ColorRGBA(1.f,0.f,0.f));
			Ui()->DoLabel(&Row, Localize("-> Warning: Passwords are saved in your config file! <-"), Row.h * CUi::ms_FontmodHeight * 0.8f, TEXTALIGN_MC, DangerLabelProps);
			Row.y += Row.h;

			//split text boxes and button
			Row.VSplitRight(22.f, &Row, &SaveButton);
			Row.w -= 2.f;
			Row.VSplitRight(50.f, &Row, &TypeButton);
			Row.w -= 2.f;

			//Add password save thing
			{
				static CLineInput s_AccountInput;
				static char s_aAccount[128] = {'\0'};
				static CLineInput s_PasswordInput;
				static char s_aPassword[128] = {'\0'};
				static CLineInput s_ServerIpInput;
				static char s_aServerIp[256] = {'\0'};
				s_AccountInput.SetBuffer(s_aAccount, sizeof(s_aAccount));
				s_PasswordInput.SetBuffer(s_aPassword, sizeof(s_aPassword));
				s_ServerIpInput.SetBuffer(s_aServerIp, sizeof(s_aServerIp));
				s_AccountInput.SetEmptyText("Account");
				s_PasswordInput.SetEmptyText("Password");
				s_ServerIpInput.SetEmptyText("Server Ip");

				/*Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(10.0f, &Label, &Left);
				Ui()->DoLabel(&Label, Localize("Message to send:"), 10.0f, TEXTALIGN_ML);*/

				Account = Row;
				Account.w /= 3;
				Password = Account;
				Password.x += Password.w;
				ServerIp = Password;
				ServerIp.x += ServerIp.w;

				Account.w -= 1.f;
				Password.x += 1.f;
				Password.w -= 2.f;
				ServerIp.x += 1.f;
				ServerIp.w -= 1.f;

				Ui()->DoEditBox(&s_AccountInput, &Account, 14.0f);
				Ui()->DoEditBox(&s_PasswordInput, &Password, 14.0f);
				Ui()->DoEditBox(&s_ServerIpInput, &ServerIp, 14.0f);

				CButtonContainer TypeButtonContainer;
				static CPasswordManagerKZ::EPasswordType CurrentType = CPasswordManagerKZ::EPasswordType::CHAT;

				if(DoButton_Menu(&TypeButtonContainer,
					CurrentType == CPasswordManagerKZ::EPasswordType::CHAT ? "Chat" : (
							CurrentType == CPasswordManagerKZ::EPasswordType::RCON ? "Rcon" : (
								"Server"
							)
						)
					, false, &TypeButton))
				{
					CurrentType =
						CurrentType == CPasswordManagerKZ::EPasswordType::CHAT ? CPasswordManagerKZ::EPasswordType::RCON : (
							CurrentType == CPasswordManagerKZ::EPasswordType::RCON ? CPasswordManagerKZ::EPasswordType::SERVER_CONNECT : (
								CPasswordManagerKZ::EPasswordType::CHAT
							)
						)
					;
				}

				CButtonContainer SaveButtonContainer;

				if(DoButton_Menu(&SaveButtonContainer, "💾", false, &SaveButton))
				{
					GameClient()->m_PasswordManager.AddPassword(s_aAccount, s_aPassword, s_aServerIp, CurrentType);
				}
			}

			Row.h = 24.f;

			//Add saved passwords
			{
				size_t Amount = GameClient()->m_PasswordManager.GetPasswordAmount();
				for(size_t i = 0; i < Amount; i++)
				{
					CPasswordManagerKZ::SPasswordKZ * SavedPassword = GameClient()->m_PasswordManager.GetPassword(i);
					Row.y += Row.h;
					Account.y += Row.h;
					Password.y += Row.h;
					ServerIp.y += Row.h;
					TypeButton.y += Row.h;
					SaveButton.y += Row.h;

					char aBuf[256] = {'\0'};

					Ui()->DoLabel(&Account, SavedPassword->m_AccountName.c_str(), Account.h * CUi::ms_FontmodHeight * 0.8f, TEXTALIGN_ML);
					Ui()->DoLabel(&Password, get_censored_str(SavedPassword->m_Password.c_str(), aBuf, sizeof(aBuf)), Password.h * CUi::ms_FontmodHeight * 0.8f, TEXTALIGN_ML);
					Ui()->DoLabel(&ServerIp, SavedPassword->m_ServerIp.c_str(), ServerIp.h * CUi::ms_FontmodHeight * 0.8f, TEXTALIGN_ML);

					Ui()->DoLabel(&TypeButton, 
						SavedPassword->m_Type == CPasswordManagerKZ::EPasswordType::CHAT ? "Chat" : (
							SavedPassword->m_Type == CPasswordManagerKZ::EPasswordType::RCON ? "Rcon" : (
								"Server"
							)
						)
						, TypeButton.h * CUi::ms_FontmodHeight * 0.8f, TEXTALIGN_ML);

					{
						static_assert(sizeof(CButtonContainer) == 1);
						if(DoButton_Menu((CButtonContainer *)SavedPassword, "🗑️", false, &SaveButton))
						{
							GameClient()->m_PasswordManager.RemovePassword(SavedPassword->m_AccountName.c_str(), SavedPassword->m_ServerIp.c_str(), SavedPassword->m_Type);
						}
					}
				}
			}

			break;
		}
		case KAIZO_SETTINGS_TAB_DANGEROUS:
		{
			//SettingsBox.VSplitMid(&Left, &Right, 20.0f);

			Left = SettingsBox;

			// dangerous settings, some communities may consider it a cheat, chill communities may not, use at your own risk!
			Left.HSplitTop(40.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Dangerous Settings!"), 20.0f, TEXTALIGN_ML);
			Left.HSplitTop(45.0f, &Label, &Left);
			Left.HSplitTop(20.0f, &Label, &SettingsBox);

			SLabelProperties DangerLabelProps;
			DangerLabelProps.SetColor(ColorRGBA(1.f,0.f,0.f));
			Ui()->DoLabel(&Label, Localize("Some communities may consider these features as cheats while others may not.\nUSE AT YOUR OWN RISK! (Some wont work in some servers and it is NOT a bug)\nAlso note that they may cause bugs in the game, try disabling them if something is wrong"), 10.0f, TEXTALIGN_ML, DangerLabelProps);
			Left.HSplitTop(25.0f, &Label, &Left);

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoOldModsZooming, Localize("Zooming in Old non-DDNet mods (only if zoom is not prohibited)"), g_KaizoConfig_KaizoOldModsZooming, &Button))
			{
				g_KaizoConfig_KaizoOldModsZooming ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoShowRechargeBar, Localize("Show weapon recharge bar"), g_KaizoConfig_KaizoShowRechargeBar, &Button))
			{
				g_KaizoConfig_KaizoShowRechargeBar ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoHideChatBubble, Localize("Dont send chat bubble"), g_KaizoConfig_KaizoHideChatBubble, &Button))
			{
				g_KaizoConfig_KaizoHideChatBubble ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoHideInMenuStatus, Localize("Dont send in-menu status"), g_KaizoConfig_KaizoHideInMenuStatus, &Button))
			{
				g_KaizoConfig_KaizoHideInMenuStatus ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoForceChatBubble, Localize("Always send chat bubble"), g_KaizoConfig_KaizoForceChatBubble, &Button))
			{
				g_KaizoConfig_KaizoForceChatBubble ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoForceInMenuStatus, Localize("Always send in-menu status"), g_KaizoConfig_KaizoForceInMenuStatus, &Button))
			{
				g_KaizoConfig_KaizoForceInMenuStatus ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoFastRespawn, Localize("Fast respawn (Broken?)"), g_KaizoConfig_KaizoFastRespawn, &Button))
			{
				g_KaizoConfig_KaizoFastRespawn ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoAlwaysAllowDummy, Localize("Always be able to connect dummy"), g_KaizoConfig_KaizoAlwaysAllowDummy, &Button))
			{
				g_KaizoConfig_KaizoAlwaysAllowDummy ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoAlwaysAllowShowHookColl, Localize("Always be able to use \"Show Hook Collisions\""), g_KaizoConfig_KaizoAlwaysAllowShowHookColl, &Button))
			{
				g_KaizoConfig_KaizoAlwaysAllowShowHookColl ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoShowXSkinsInSettings, Localize("Show \"x_\" prefixed skins in Skin settings"), g_KaizoConfig_KaizoShowXSkinsInSettings, &Button))
			{
				g_KaizoConfig_KaizoShowXSkinsInSettings ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoForceDDNetHUD, Localize("Always try to show DDNet HUD"), g_KaizoConfig_KaizoForceDDNetHUD, &Button))
			{
				g_KaizoConfig_KaizoForceDDNetHUD ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoForceBugDDRaceInput, Localize("Act as if \'Bug DDRace Input\' is enabled"), g_KaizoConfig_KaizoForceBugDDRaceInput, &Button))
			{
				g_KaizoConfig_KaizoForceBugDDRaceInput ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoShowCharFlags, Localize("Show Character Flags in nameplate"), g_KaizoConfig_KaizoShowCharFlags, &Button))
			{
				g_KaizoConfig_KaizoShowCharFlags ^= 1;
			}

			if(g_KaizoConfig_KaizoShowCharFlags)
			{
				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(20.0f, &Button, &Left);
				Ui()->DoScrollbarOption(&g_KaizoConfig_KaizoShowCharFlagsSize, &g_KaizoConfig_KaizoShowCharFlagsSize, &Button, Localize("Size of Characer Flags indicators"), 25, 100, &CUi::ms_LogarithmicScrollbarScale, 0u);
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoShowCharJumps, Localize("Show Character Jumps in nameplate"), g_KaizoConfig_KaizoShowCharJumps, &Button))
			{
				g_KaizoConfig_KaizoShowCharJumps ^= 1;
			}

			if(g_KaizoConfig_KaizoShowCharJumps)
			{
				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(20.0f, &Button, &Left);
				Ui()->DoScrollbarOption(&g_KaizoConfig_KaizoShowCharJumpsSize, &g_KaizoConfig_KaizoShowCharJumpsSize, &Button, Localize("Size of Characer Jumps indicator"), 25, 100, &CUi::ms_LogarithmicScrollbarScale, 0u);
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoGrenadePath, Localize("Show Grenade and Gun collisions"), g_KaizoConfig_KaizoGrenadePath, &Button))
			{
				g_KaizoConfig_KaizoGrenadePath ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoLaserPath, Localize("Show Laser collisions"), g_KaizoConfig_KaizoLaserPath, &Button))
			{
				g_KaizoConfig_KaizoLaserPath ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoFreeMouse, Localize("Free mouse"), g_KaizoConfig_KaizoFreeMouse, &Button))
			{
				g_KaizoConfig_KaizoFreeMouse ^= 1;
			}
			
			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoHideAiming, Localize("Hide aiming (may break /pause and /spec)"), g_KaizoConfig_KaizoHideAiming, &Button))
			{
				g_KaizoConfig_KaizoHideAiming ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoFakeMaxZoom, Localize("Send fake max zoom (may improve prediction)"), g_KaizoConfig_KaizoFakeMaxZoom, &Button))
			{
				g_KaizoConfig_KaizoFakeMaxZoom ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoGlitchyInput, Localize("Make your own gameplay less accurate xD"), g_KaizoConfig_KaizoGlitchyInput, &Button))
			{
				g_KaizoConfig_KaizoGlitchyInput ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoShowTrackedAmmo, Localize("Track ammo and show it"), g_KaizoConfig_KaizoShowTrackedAmmo, &Button))
			{
				g_KaizoConfig_KaizoShowTrackedAmmo ^= 1;
			}

			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_KaizoConfig_KaizoFasterHardcoreMouse, Localize("Enable fast hardcore mouse"), g_KaizoConfig_KaizoFasterHardcoreMouse, &Button))
			{
				g_KaizoConfig_KaizoFasterHardcoreMouse ^= 1;
			}

			if(g_KaizoConfig_KaizoFasterHardcoreMouse)
			{
				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(20.0f, &Button, &Left);
				Ui()->DoScrollbarOption(&g_KaizoConfig_KaizoFasterHardcoreMouseDistance, &g_KaizoConfig_KaizoFasterHardcoreMouseDistance, &Button, Localize("Hardcore mouse required distance (recommended: 400)"), 0, 700, &CUi::ms_LogarithmicScrollbarScale, 0u);

				Left.HSplitTop(2.0f, nullptr, &Left);
				Left.HSplitTop(20.0f, &Button, &Left);
				Ui()->DoScrollbarOption(&g_KaizoConfig_KaizoFasterHardcoreMouseScale, &g_KaizoConfig_KaizoFasterHardcoreMouseScale, &Button, Localize("Hardcore mouse scale multiplier (recommended: 4)"), 0, 50, &CUi::ms_LogarithmicScrollbarScale, 0u);
			}

			//Freeze hitbox
			Left.HSplitTop(40.0f, &Label, &SettingsBox);
			Ui()->DoLabel(&Label, Localize("Freeze hitbox"), 20.0f, TEXTALIGN_ML);
			Left.HSplitTop(40.0f, &Label, &Left);

			Left.HSplitTop(2.0f, nullptr, &Left);
			{
				static std::vector<CButtonContainer> s_vButtonContainersPlayersHitbox = {{}, {}, {}, {}};
				DoLine_RadioMenu(Left, Localize("Show player hitbox"),
					s_vButtonContainersPlayersHitbox,
					{Localize("Off"), Localize("Others"), Localize("Everyone"), Localize("Own")},
					{0, 1, 2, 3},
					g_KaizoConfig_KaizoShowHitbox);
				Left.HSplitTop(LineSize, &Button, &Left);
				Ui()->DoScrollbarOption(&g_KaizoConfig_KaizoShowHitboxSize, &g_KaizoConfig_KaizoShowHitboxSize, &Button, Localize("Size of hitbox"), 1, 100, &CUi::ms_LinearScrollbarScale, 0);
				Left.HSplitTop(LineSize, &Button, &Left);
				Ui()->DoScrollbarOption(&g_KaizoConfig_KaizoShowHitboxQuality, &g_KaizoConfig_KaizoShowHitboxQuality, &Button, Localize("Quality of hitbox"), 1, 32, &CUi::ms_LinearScrollbarScale, 0);
				static CButtonContainer s_HitboxColor;
				DoButton_ColorPickerAutoVMargin(&s_HitboxColor, Localize("Hitbox color"), &g_KaizoConfig_KaizoShowHitboxColor, color_cast<ColorRGBA>(ColorHSLA(4288086271 /*default value*/)), &Left, LineSize, true);
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

			//added version
			Label.h = 20.f;
			Label.y = SettingsBox.y - Label.h;

			Ui()->DoLabel(&Label, Localize("V" KAIZO_CLIENT_VERSION_LATEST_STR), 20.0f, TEXTALIGN_MC);

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
				Client()->ViewLink(Localize("https://m0rekz.github.io/kaizo-client-discord.html"));
			}

			if(DoButton_Menu(&SourceCodeButton, Localize("Source Code"), false, &SourceCodeRect))
			{
				Client()->ViewLink(Localize("https://www.github.com/m0rekz/kaizo-client"));
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

	const float Radius = std::min(RightView.w, RightView.h) / 2.0f;
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
	DoButton_CheckBoxAutoVMarginAndSet(&g_KaizoConfig_KaizoResetBindWheelMouse, Localize("Reset position of mouse when opening bindwheel"), &g_KaizoConfig_KaizoResetBindWheelMouse, &Label, LineSize);
}
