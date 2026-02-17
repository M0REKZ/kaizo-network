// Copyright (C) Benjamín Gajardo (also known as +KZ)

#ifndef ENGINE_CLIENT_DISCORD_KZ_H
#define ENGINE_CLIENT_DISCORD_KZ_H

class CKaizoDiscordConfigAccess
{
    public:
    bool KaizoDiscordRpcEnabled();
    bool KaizoDiscordLaunchSteam();
};

extern CKaizoDiscordConfigAccess g_KaizoDiscordConfigAccess;

#endif
