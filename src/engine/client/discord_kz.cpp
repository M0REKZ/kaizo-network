// Copyright (C) Benjamín Gajardo (also known as +KZ)

//This file is to fix building on Windows, since there is a conflict with IStorage class from objidl.h

#include "discord_kz.h"

//+KZ
#include <engine/shared/config.h>

CKaizoDiscordConfigAccess g_KaizoDiscordConfigAccess;

bool CKaizoDiscordConfigAccess::KaizoDiscordRpcEnabled()
{
	return g_Config.m_KaizoDiscordRpc;
}

bool CKaizoDiscordConfigAccess::KaizoDiscordLaunchSteam()
{
	return g_Config.m_KaizoDiscordLaunchSteam;
}
