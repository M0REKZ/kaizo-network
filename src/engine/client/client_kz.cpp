// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include <engine/client/client.h>
#include <game/version.h>
#include "client.h"

void CClient::SendKaizoNetworkVersion(int Conn)
{
    CMsgPacker Msg(NETMSG_KZ_KAIZO_NETWORK_VERSION, true);
	Msg.AddInt(KAIZO_NETWORK_VERSION_LATEST);
	SendMsg(Conn, &Msg, MSGFLAG_VITAL);
}

void CClient::SendFastInputsInfo(int Conn)
{
	CMsgPacker Msg(NETMSG_FOXNET_FASTINPUTS, true);
	Msg.AddInt(g_Config.m_KaizoFastInput);
	SendMsg(Conn, &Msg, MSGFLAG_VITAL);
}
