// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include <engine/client/client.h>
#include <game/version.h>
#include <game/localization.h>
#include "client.h"
#include <base/time.h>

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

void CClient::SendSupportsCosmeticSnapInfo(int Conn)
{
	CMsgPacker Msg(NETMSG_FOXNET_COSMETIC_SNAPS, true);
	SendMsg(Conn, &Msg, MSGFLAG_VITAL);
}

void CClient::SendClientLanguage(int Conn)
{
	if(g_Config.m_KaizoSendLanguage && g_Config.m_ClLanguagefile[0])
	{
		//first check if the client supports that language
		for(const CLanguage &Lang : g_Localization.Languages())
		{
			if(!str_comp_nocase(Lang.m_Filename.c_str(), g_Config.m_ClLanguagefile) && Lang.m_vLanguageCodes.size() > 0)
			{
				for(const std::string &LangCode : Lang.m_vLanguageCodes)
				{
					CNetMsg_Cl_Language MsgLang;
					MsgLang.m_pLanguage = LangCode.c_str();
					CMsgPacker Packer(&MsgLang);
					MsgLang.Pack(&Packer);
					SendMsgActive(&Packer, MSGFLAG_VITAL);
				}

				return;
			}
		}
	}
}

//T-Client
void CClient::GetSmoothFreezeTick(int *pSmoothTick, float *pSmoothIntraTick, float MixAmount)
{
	int64_t GameTime = m_aGameTime[g_Config.m_ClDummy].Get(time_get());
	int64_t PredTime = m_PredictedTime.Get(time_get());
	GameTime = std::min(GameTime, PredTime);

	int64_t UpperPredTime = std::clamp(PredTime - (time_freq() / 50) * g_Config.m_KaizoUnfreezeLagTicks, GameTime, PredTime);
	int64_t LowestPredTime = std::clamp(PredTime, GameTime, UpperPredTime);
	int64_t SmoothTime = std::clamp(LowestPredTime + (int64_t)(MixAmount * (PredTime - LowestPredTime)), LowestPredTime, PredTime);

	*pSmoothTick = (int)(SmoothTime * 50 / time_freq()) + 1;
	*pSmoothIntraTick = (SmoothTime - (*pSmoothTick - 1) * time_freq() / 50) / (float)(time_freq() / 50);
}
