// Copyright (C) Benjamín Gajardo (also known as +KZ)

//This file is to fix building on Windows, since there is a conflict with IStorage class from objidl.h

#include "discord_kz.h"

//+KZ
extern int g_KaizoConfig_KaizoDiscordRpc;
extern int g_KaizoConfig_KaizoDiscordLaunchSteam;

CKaizoDiscordConfigAccess g_KaizoDiscordConfigAccess;

bool CKaizoDiscordConfigAccess::KaizoDiscordRpcEnabled()
{
	return g_KaizoConfig_KaizoDiscordRpc;
}

bool CKaizoDiscordConfigAccess::KaizoDiscordLaunchSteam()
{
	return g_KaizoConfig_KaizoDiscordLaunchSteam;
}
