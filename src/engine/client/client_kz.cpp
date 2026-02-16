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

void CClient::SendKaizoClientIAm(int Conn)
{
	CMsgPacker Msg(NETMSG_IAM_KAIZO_CLIENT, true);
	Msg.AddString("Kaizo Client by +KZ. Kaizo protocol version: " STRINGIFY(KAIZO_NETWORK_VERSION_LATEST) ", built on " __DATE__ ", " __TIME__ "");
	SendMsg(Conn, &Msg, MSGFLAG_VITAL);
}

void CClient::SendFastInputsInfo(int Conn)
{
	CMsgPacker Msg(NETMSG_FOXNET_FASTINPUTS, true);
	Msg.AddInt(g_Config.m_KaizoFastInput);
	Msg.AddInt(g_Config.m_KaizoFastInputAmount);
	SendMsg(Conn, &Msg, MSGFLAG_VITAL);
}
