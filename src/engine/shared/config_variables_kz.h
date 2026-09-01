//Copyright (C) Benjamín Gajardo (also known as +KZ)
//
//Config variables for:
//  Kaizo Network Server and Client by +KZ
//  Kaizo Client by +KZ
//Starting all configs with sv_kaizo_ or kaizo_ to avoid conflicts with DDNet
//The ones that does not start with sv_kaizo_ are kept for compatibility with old Kaizo Network versions
//sv_kog_ variables are for compatibility with Gores maps

#ifndef MACRO_CONFIG_INT
#error "The config macros must be defined"
// This helps IDEs properly syntax highlight the uses of the macro below.
#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Save, Desc)
#define MACRO_CONFIG_COL(Name, ScriptName, Def, Save, Desc)
#define MACRO_CONFIG_STR(Name, ScriptName, Len, Def, Save, Desc)
#endif

//*========================================================*
//KAIZO NETWORK SERVER MAP CONFIGS
//*========================================================*

//Old configs

MACRO_CONFIG_INT(SvPortalMode, sv_portal_mode, 1, 0, 2, CFGFLAG_SERVER | CFGFLAG_GAME, "Portal spawning behavior (0 = on every tile, 1 = only on allow portal tile, 2 = PPRace compatibility)")
MACRO_CONFIG_INT(SvPortalLaserReach, sv_portal_laser_reach, 9999, 0, 9999, CFGFLAG_SERVER | CFGFLAG_GAME, "Portal gun laser reach")
MACRO_CONFIG_INT(SvMaxHealth, sv_max_health, 10, 1, 10000, CFGFLAG_SERVER | CFGFLAG_GAME, "Max amount of life")
MACRO_CONFIG_INT(SvDamageLaserCooldown, sv_damage_laser_cooldown, 4, 0, 10000, CFGFLAG_SERVER | CFGFLAG_GAME, "Cooldown for damage laser")
MACRO_CONFIG_INT(SvDamageLaserDmg, sv_damage_laser_dmg, 2, 0, 10000, CFGFLAG_SERVER | CFGFLAG_GAME, "Damage laser damage")
MACRO_CONFIG_INT(SvDamageTurretDmg, sv_damage_turret_dmg, 1, 0, 10000, CFGFLAG_SERVER | CFGFLAG_GAME, "Plasma turret damage")
MACRO_CONFIG_INT(SvDamageTurretExplosiveDmg, sv_damage_turret_explosive_dmg, 3, 0, 10000, CFGFLAG_SERVER | CFGFLAG_GAME, "Explosive plasma turret damage")
MACRO_CONFIG_INT(SvDamageMineDmg, sv_damage_mine_dmg, 3, 0, 10000, CFGFLAG_SERVER | CFGFLAG_GAME, "Mines damage")

//New configs

MACRO_CONFIG_INT(SvKaizoLaserRecoverJump, sv_kaizo_laser_recover_jump, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Player recovers jump when hit by laser")
MACRO_CONFIG_INT(SvKaizoMaxVel, sv_kaizo_max_vel, 300, 0, 9999, CFGFLAG_SERVER | CFGFLAG_GAME, "Max player velocity to prevent high CPU usage (due to Kaizo tiles collision)")
MACRO_CONFIG_INT(SvKaizoVanillaMode, sv_kaizo_vanilla_mode, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Get damaged by your own weapons like in vanilla and be able to collect health and armor")

//Compat configs

//SvPortalMode has a PPRace compatibility option
MACRO_CONFIG_INT(SvPortalProjectile, sv_portal_projectile, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Kaizo-Insta compatibility: Use Portal Projectile instead of Laser")
MACRO_CONFIG_INT(SvGoresQuadsEnable, sv_kog_qquads_enable, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Gores compatibility: Moving tiles")
MACRO_CONFIG_INT(SvGoresGrenadeTele, sv_kog_grenade_tele, 0, 0, 2, CFGFLAG_SERVER | CFGFLAG_GAME, "Gores compatibility: Teleport grenade") //Pointer's ddrace mod supports mode 2

//*========================================================*
//KAIZO CLIENT CONFIGS
//*========================================================*

//*=========================================*
//VISUAL
//*=========================================*
MACRO_CONFIG_INT(KaizoShowCrowns, kaizo_show_crowns, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show crowns on top of the players in Kaizo servers")
MACRO_CONFIG_INT(KaizoInstaShieldShield, kaizo_instashield_shield, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable InstaShield Shield")
MACRO_CONFIG_INT(KaizoKillingSpreeSparkles, kaizo_killing_spree_sparkles, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Killing spree sparkles")
MACRO_CONFIG_INT(KaizoRotatingHammer, kaizo_rotating_hammer, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Hammer rotates like other weapons")
MACRO_CONFIG_INT(KaizoPredictTeleToDeath, kaizo_predict_tele_to_death, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Predict death effect for teleports that only lead to death tiles")
MACRO_CONFIG_INT(KaizoStartMenu, kaizo_start_menu, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Use Kaizo Client start menu")
MACRO_CONFIG_INT(KaizoEmotionalTees, kaizo_emotional_tees, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Make tees have emotions in non-DDNet servers")

//From Duck/InfClass Client by Pointer
MACRO_CONFIG_INT(KaizoChatInputBackground, kaizo_chat_input_background, 1, 0, 1, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Whether to add a background for the input when chatting")
MACRO_CONFIG_INT(KaizoScoreboardStyle, kaizo_scoreboard_style, 0, 0, 10, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Style of the scoreboard")
MACRO_CONFIG_INT(KaizoScoreboardShorten, kaizo_scoreboard_shorten, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Shorten the scoreboard if there are not so many players")

//From TaterClient
MACRO_CONFIG_INT(KaizoAnimateWheelTime, kaizo_animate_wheel_time, 80, 0, 1000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Duration of emote and bind wheel animations, in milliseconds (0 == no animation, 1000 = 1 second)")

//*=========================================*
//PREDICTION
//*=========================================*
MACRO_CONFIG_INT(KaizoPredictDDNetTeleport, kaizo_predict_ddnet_teleport, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Predict DDNet teleports if there is only 1 exit")
MACRO_CONFIG_INT(KaizoPredictDeathTiles, kaizo_predict_death_tiles, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Predict death effect in death tiles")
MACRO_CONFIG_INT(KaizoPredictPointerTWPlus, kaizo_predict_pointer_twplus, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Try to detect and predict Pointer's TW+")
MACRO_CONFIG_INT(KaizoPredictVanillaHammerFix, kaizo_predict_vanilla_hammer_fix, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Fix hammer prediction in vanilla when hitting other players through walls")
MACRO_CONFIG_INT(KaizoPredictGoresGrenadeTele, kaizo_predict_grenade_gores_tele, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Predict Gores grenade teleport")
//MACRO_CONFIG_INT(KaizoApplyGuessedInput, kaizo_apply_guessed_input, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply guessed input to predicted characters (kinda useless honestly i should remove this)")
MACRO_CONFIG_INT(KaizoPredictGameTypes, kaizo_predict_gametypes, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Predict game type specific logic")
MACRO_CONFIG_INT(KaizoPredictOthersEffects, kaizo_predict_others_effects, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Also predict effects for others (like death)")
MACRO_CONFIG_INT(KaizoAlwaysRepredict, kaizo_always_repredict, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Always repredict (no idea if it is even useful)")

//From TaterClient
MACRO_CONFIG_INT(KaizoFastInput, kaizo_fast_input, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable fast input")
MACRO_CONFIG_INT(KaizoFastInputOthers, kaizo_fast_input_others, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable fast input for other players")
MACRO_CONFIG_INT(KaizoFastInputAmount, kaizo_fast_input_amount, 20, 1, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "How many milliseconds fast input will apply")
MACRO_CONFIG_INT(KaizoUnpredOthersInFreeze, kaizo_unpred_others_in_freeze, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Dont predict other players if you are frozen")
MACRO_CONFIG_INT(KaizoPredMarginInFreeze, kaizo_pred_margin_in_freeze, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable changing prediction margin while frozen")
MACRO_CONFIG_INT(KaizoPredMarginInFreezeAmount, kaizo_pred_margin_in_freeze_amount, 15, 0, 2000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Set what your prediction margin while frozen should be")
MACRO_CONFIG_INT(KaizoRemoveAnti, kaizo_remove_anti, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Removes some amount of antiping & player prediction in freeze")
MACRO_CONFIG_INT(KaizoUnfreezeLagTicks, kaizo_remove_anti_ticks, 5, 0, 20, CFGFLAG_CLIENT | CFGFLAG_SAVE, "The biggest amount of prediction ticks that are removed")
MACRO_CONFIG_INT(KaizoUnfreezeLagDelayTicks, kaizo_remove_anti_delay_ticks, 25, 5, 150, CFGFLAG_CLIENT | CFGFLAG_SAVE, "How many ticks it takes to remove the maximum prediction after being frozen")

//*=========================================*
//UTILITY
//*=========================================*
MACRO_CONFIG_INT(KaizoPingCircles, kaizo_ping_circles, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ping circles above players")
MACRO_CONFIG_INT(KaizoSleepingInMenuPlayers, kaizo_sleeping_in_menu_players, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Players navigating menu appear as sleeping")
MACRO_CONFIG_INT(KaizoFastMapDownload, kaizo_fast_map_download, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Fast map download (Experimental)")
MACRO_CONFIG_INT(KaizoFastMapDownloadWindow, kaizo_fast_map_download_window, 100, 0, 500, CFGFLAG_CLIENT | CFGFLAG_SAVE, "How much map data is requested each time data is received")
MACRO_CONFIG_INT(KaizoSendLanguage, kaizo_send_language, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Send client language (only works in compatible servers)")
MACRO_CONFIG_INT(KaizoAlwaysAllowXSkins, kaizo_always_allow_x_skins, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Always be able to see players using \"x_\" prefixed skins")
MACRO_CONFIG_INT(KaizoShowXSkinsInSettings, kaizo_show_x_skins_in_settings, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Include \"x_\" prefixed skins in Skin settings")
MACRO_CONFIG_INT(KaizoPrefer07Protocol, kaizo_prefer_07_protocol, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Always connect by using the 0.7 protocol")
MACRO_CONFIG_INT(KaizoHideBroadcastOnChat, kaizo_hide_broadcast_on_chat, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Hide broadcasts when looking the chat")
MACRO_CONFIG_INT(KaizoKeepMenuAfterVoteInMods, kaizo_keep_menu_after_vote_in_mods, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Dont close the menu while changing settings in F-DDrace, FoxNet, MMOTEE and other similar mods")
MACRO_CONFIG_INT(KaizoEmoticonToEmoji, kaizo_emoticon_to_emoji, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "When sending chat messages, convert :emoticons: to emojis")
MACRO_CONFIG_INT(KaizoShowPreciseAuthedState, kaizo_show_precise_authed_state, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show different color names for moderators and helpers")
MACRO_CONFIG_INT(KaizoShowAuthedStateInGame, kaizo_show_authed_state_in_game, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show authed state color in-game")
MACRO_CONFIG_COL(KaizoModeratorAuthedColor, kaizo_moderator_authed_color, 0x59FFE3, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Moderator authed state color")
MACRO_CONFIG_COL(KaizoHelperAuthedColor, kaizo_helper_authed_color, 0x59FFE3, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Helper authed state color")

//Idenfity other custom clients
MACRO_CONFIG_INT(KaizoSendClientType, kaizo_send_client_type, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Let know other clients that we are Kaizo Client")
MACRO_CONFIG_INT(KaizoShowClientType, kaizo_show_client_type, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Try to identify and show other players clients as icons")

//HUD
MACRO_CONFIG_INT(KaizoHudRealPosition, kaizo_hud_real_position, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "If HUD player position is enabled, show the real position instead of the calculated one")
MACRO_CONFIG_INT(KaizoHudRealVelocity, kaizo_hud_real_velocity, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "If HUD player velocity is enabled, show the real velocity instead of the calculated one")

//Autoreply
MACRO_CONFIG_INT(KaizoReplyTabbedOut, kaizo_reply_tabbed_out, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Reply when someone ping you and you are tabbed out")
MACRO_CONFIG_STR(KaizoReplyTabbedOutMsg, kaizo_reply_tabbed_out_msg, 200, "I am tabbed out (AFK)", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Message to sent when tabbed out")

//Discord
MACRO_CONFIG_INT(KaizoDiscordRpc, kaizo_discord_rpc, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable Discord Rich Presence")
MACRO_CONFIG_INT(KaizoDiscordLaunchSteam, kaizo_discord_launch_steam, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Tell Discord to launch DDNet from Steam when joining with a invite (requires restarting both Client and Discord)")

//From TaterClient
MACRO_CONFIG_STR(KaizoCustomCommunitiesUrl, kaizo_custom_communities_url, 256, "https://raw.githubusercontent.com/TaterClient/ddnet-custom-communities/refs/heads/main/custom-communities-ddnet-info.json", CFGFLAG_CLIENT | CFGFLAG_SAVE, "URL to fetch custom communities from (must be https), empty to disable")
MACRO_CONFIG_INT(KaizoAntiPingImproved, kaizo_antiping_improved, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Different antiping smoothing algorithm, overrides cl_antiping_smooth")
MACRO_CONFIG_INT(KaizoAntiPingNegativeBuffer, kaizo_antiping_negative_buffer, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Helps in Gores. Allows internal certainty value to be negative which causes more conservative prediction")
MACRO_CONFIG_INT(KaizoAntiPingStableDirection, kaizo_antiping_stable_direction, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Predicts optimistically along the tees stable axis to reduce delay in gaining overall stability")
MACRO_CONFIG_INT(KaizoAntiPingUncertaintyScale, kaizo_antiping_uncertainty_scale, 150, 25, 400, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Determines uncertainty duration as a factor of ping, 100 = 1.0")
MACRO_CONFIG_INT(KaizoRevertHookLine, kaizo_revert_hook_line, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Revert that weird hook line update")
MACRO_CONFIG_INT(KaizoResetBindWheelMouse, kaizo_reset_bindwheel_mouse, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Reset position of mouse when opening bindwheel")

//From Entity Client
MACRO_CONFIG_STR(KaizoExecuteOnConnect, kaizo_execute_on_connect, 100, "", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Run a console command before connect")
MACRO_CONFIG_STR(KaizoRunOnJoinConsole, kaizo_run_on_join_console, 128, "", CFGFLAG_CLIENT | CFGFLAG_SAVE, "What it should run inside of the Console")
MACRO_CONFIG_INT(KaizoRunOnJoinConsoleDelay, kaizo_run_on_join_delay, 2, 7, 50000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Tick Delay before using run on join")
MACRO_CONFIG_INT(KaizoClientIndicatorBrowser, kaizo_client_indicator_browser, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to show a list of people using Kaizo Client in the server browser")
MACRO_CONFIG_INT(KaizoStatisticsShowFps, kaizo_statistics_show_fps, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to show FPS in the statistics")
MACRO_CONFIG_INT(KaizoStatisticsShowPing, kaizo_statistics_show_ping, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to show Ping in the statistics")
MACRO_CONFIG_INT(KaizoStatisticsShowSnapRate, kaizo_statistics_show_snap_rate, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to show Snap Rate in the statistics")

//From CMClient
//Splits
MACRO_CONFIG_INT(KaizoSplits, kaizo_splits, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to track splits (requires a map reload)")
MACRO_CONFIG_INT(KaizoSplitsTrackDummy, kaizo_splits_trackdummy, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to track splits for dummies")
MACRO_CONFIG_INT(KaizoSplitsForceAny, kaizo_splits_forceany, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to force all runs to be in the any rank category")
MACRO_CONFIG_INT(KaizoSplitsResetAfterDeath, kaizo_splits_resetafterdeath, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to reset the splits after a death")
MACRO_CONFIG_INT(KaizoSplitsResetAfterFinishDeath, kaizo_splits_resetafterfinishdeath, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to reset the splits after a death after crossing the finish line")
MACRO_CONFIG_INT(KaizoSplitsComparisonSource, kaizo_splits_comparisonsource, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "The source to use for the comparison time (0: best run, 1: best segment total, 2: best segment segmented)")
MACRO_CONFIG_INT(KaizoSplitsComparisonForceCategory, kaizo_splits_comparisonforcecategory, 0, 0, 4, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to always use ranks from a certain category for the comparison time (1: any, 2: team, 3: dummy, 4: solo)")
MACRO_CONFIG_INT(KaizoShowhudSplits, kaizo_showhud_splits, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ingame HUD (Splits) (2 to show on maps with no splits)")
MACRO_CONFIG_INT(KaizoSplitsDisplaySegmentTime, kaizo_splits_displaysegmenttime, 0, 0, 3, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to show split times as run time or segment time (0: both run, 1: check segment and total run, 2: check run and total segment, 3: both segment)")
MACRO_CONFIG_INT(KaizoSplitsNameFormat, kaizo_splits_nameformat, 0, 0, 4, CFGFLAG_CLIENT | CFGFLAG_SAVE, "The format of split names (0: from → to, 1: from, 2: to, 3/4: same as 1/2 if segments are in order else same as 0)")
MACRO_CONFIG_INT(KaizoSplitsMaxRowsInRun, kaizo_splits_maxrowsinrun, 5, 0, 26, CFGFLAG_CLIENT | CFGFLAG_SAVE, "The number of splits to show while in a run")
MACRO_CONFIG_INT(KaizoSplitsMaxRowsBeforeRun, kaizo_splits_maxrowsbeforerun, 26, 0, 26, CFGFLAG_CLIENT | CFGFLAG_SAVE, "The number of splits to show before starting a run")
MACRO_CONFIG_INT(KaizoSplitsMaxRowsAfterRun, kaizo_splits_maxrowsafterrun, 26, 0, 26, CFGFLAG_CLIENT | CFGFLAG_SAVE, "The number of splits to show after finishing a run")
MACRO_CONFIG_INT(KaizoSplitsAlwaysShowFinalSplit, kaizo_splits_alwaysshowfinalsplit, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to always show the final split")
MACRO_CONFIG_INT(KaizoSplitsMaximumCurrentCheck, kaizo_splits_maximumcurrentcheck, 10, -1, 86400, CFGFLAG_CLIENT | CFGFLAG_SAVE, "The maximum current split check time to show in seconds (-1 to always show)")
MACRO_CONFIG_INT(KaizoSplitsShowMap, kaizo_splits_showmap, 1, 0, 9, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show the current map on the splits HUD")
MACRO_CONFIG_INT(KaizoSplitsShowSplits, kaizo_splits_showsplits, 1, 0, 9, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show the splits on the splits HUD")
MACRO_CONFIG_INT(KaizoSplitsShowCategory, kaizo_splits_showcategory, 0, 0, 9, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show the current rank category on the splits HUD")
MACRO_CONFIG_INT(KaizoSplitsShowTimer, kaizo_splits_showtimer, 0, 0, 9, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show the run timer on the splits HUD")
MACRO_CONFIG_INT(KaizoSplitsShowLastSegment, kaizo_splits_showlastsegment, 0, 0, 9, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show the previous segment time on the splits HUD")
MACRO_CONFIG_INT(KaizoSplitsShowFurthest, kaizo_splits_showfurthest, 0, 0, 9, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show the furthest segment on the splits HUD")
MACRO_CONFIG_INT(KaizoSplitsShowSumOfBest, kaizo_splits_showsumofbest, 0, 0, 9, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show the sum of best segments on the splits HUD")
MACRO_CONFIG_INT(KaizoSplitsShowCurrentPace, kaizo_splits_showcurrentpace, 0, 0, 9, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show the current pace on the splits HUD")
MACRO_CONFIG_INT(KaizoSplitsShowBestPossible, kaizo_splits_showbestpossible, 0, 0, 9, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show the best possible finish time on the splits HUD")
MACRO_CONFIG_INT(KaizoSplitsLabels, kaizo_splits_labels, 1, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to show labels for items on the splits HUD (0: no, 1: small, 2: full size)")
MACRO_CONFIG_INT(KaizoSplitsX, kaizo_splits_x, 22, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Horizontal position of splits as percentage of screen width")

//From Duck/Infclass Client
// Custom Resource Protocol
MACRO_CONFIG_INT(KaizoResourcesEnable, kaizo_resources_enable, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to enable the custom resource system/protocol")
MACRO_CONFIG_INT(KaizoResourcesDownload, kaizo_resources_download, 2, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to download custom resources (2=allow server to set the url)")
MACRO_CONFIG_STR(KaizoResourcesDownloadUrl, kaizo_resources_download_url, 128, "", CFGFLAG_CLIENT | CFGFLAG_SAVE, "The URL to be used for downloading custom resources if kaizo_resources_download is enabled")

//*=========================================*
//DANGEROUS
//*=========================================*

//User must enable this by himself
MACRO_CONFIG_INT(KaizoShowDangerousSettings, kaizo_show_dangerous_settings, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show 'Dangerous' tab in settings")

MACRO_CONFIG_INT(KaizoOldModsZooming, kaizo_old_mods_zooming, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Allow to zoom in some old non-DDNet mods (not all) (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoShowRechargeBar, kaizo_show_recharge_bar, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show weapon recharge bar (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoHideChatBubble, kaizo_hide_chat_bubble, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Dont send chat bubble")
MACRO_CONFIG_INT(KaizoHideInMenuStatus, kaizo_hide_in_menu_status, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Dont send in-menu status")
MACRO_CONFIG_INT(KaizoFastRespawn, kaizo_fast_respawn, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Spam click to respawn faster after death (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoForceChatBubble, kaizo_force_chat_bubble, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Always send chat bubble (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoForceInMenuStatus, kaizo_force_in_menu_status, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Always send in-menu status (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoAlwaysAllowDummy, kaizo_always_allow_dummy, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Always be able to try connecting dummy (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoAlwaysAllowShowHookColl, kaizo_always_allow_show_hook_coll, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Always be able to use +showhookcoll (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoForceDDNetHUD, kaizo_force_ddnet_hud, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Always show DDNet HUD even if server does not support it (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoForceBugDDRaceInput, kaizo_force_bug_ddrace_input, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Act as if Bug DDRace input is enabled (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoHideAiming, kaizo_hide_aiming, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Only send aiming when fire is pressed (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoFakeMaxZoom, kaizo_fake_max_zoom, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Make server think you have max zoom, may improve prediction (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoGlitchyInput, kaizo_glitchy_input, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Make your gameplay worse and inaccurate (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoShowTrackedAmmo, kaizo_show_tracked_ammo, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show tracked ammo for all weapons (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoFasterHardcoreMouse, kaizo_faster_hardcore_mouse, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable fast hardcore mouse (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoFasterHardcoreMouseDistance, kaizo_faster_hardcore_mouse_distance, 0, 0, 1000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Fast hardcore mouse work required distance (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoFasterHardcoreMouseScale, kaizo_faster_hardcore_mouse_scale, 4, 0, 50, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Fast hardcore mouse work scale multiplier (WARNING: can be considered as a cheat, use at your own risk!)")

//From DuckDDNet
MACRO_CONFIG_INT(KaizoShowCharFlags, kaizo_show_char_flags, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show Character flags (Deep/Jetpack/etc) (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoShowCharFlagsSize, kaizo_show_char_flags_size, 30, 25, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of Character flags indicators")
MACRO_CONFIG_INT(KaizoLaserPath, kaizo_laser_path, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show laser path (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoGrenadePath, kaizo_grenade_path, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show grenade path (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoShowCharJumps, kaizo_show_char_jumps, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show remaining double jumps of a tee (WARNING: can be considered as a cheat, use at your own risk!)")
MACRO_CONFIG_INT(KaizoShowCharJumpsSize, kaizo_show_char_jumps_size, 30, 25, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of double jump indicators")
MACRO_CONFIG_INT(KaizoFreeMouse, kaizo_free_mouse, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Free mouse (WARNING: can be considered as a cheat, use at your own risk!)")

//From RushieClient
MACRO_CONFIG_INT(KaizoShowHitbox, kaizo_show_hitbox, 0, 0, 3, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show hitbox (1-other's 2-everyone 3-self)")
MACRO_CONFIG_INT(KaizoShowHitboxSize, kaizo_show_hitbox_size, 50, 1, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Hitbox size")
MACRO_CONFIG_INT(KaizoShowHitboxQuality, kaizo_show_hitbox_quality, 8, 1, 32, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Hitbox quality (segments)")
MACRO_CONFIG_COL(KaizoShowHitboxColor, kaizo_show_hitbox_color, 0xFF9700FF, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Hitbox color")
