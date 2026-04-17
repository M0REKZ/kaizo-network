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

void CClient::SendSupportsCosmeticSnapInfo(int Conn)
{
	CMsgPacker Msg(NETMSG_FOXNET_COSMETIC_SNAPS, true);
	SendMsg(Conn, &Msg, MSGFLAG_VITAL);
}

void CClient::SendClientLanguage(int Conn)
{
	static std::unordered_map<std::string, const char*> s_aLanguageMap;
	static bool s_LanguageMapInitialized = false;

	if(!s_LanguageMapInitialized)
	{
		//listed in the same order, with the same tags, as shown in /data/languages/index.txt
		//since multiple tags are not in ddnet-community protocol standard we are only sending one of them
		s_aLanguageMap["languages/arabic.txt"] = "ar";
		s_aLanguageMap["languages/belarusian.txt"] = "be";
		s_aLanguageMap["languages/bosnian.txt"] = "bs-Latn";
		s_aLanguageMap["languages/brazilian_portuguese.txt"] = "pt-BR";
		s_aLanguageMap["languages/bulgarian.txt"] = "bg";
		s_aLanguageMap["languages/catalan.txt"] = "ca";
		s_aLanguageMap["languages/chuvash.txt"] = "cv";
		s_aLanguageMap["languages/czech.txt"] = "cs";
		s_aLanguageMap["languages/danish.txt"] = "da";
		s_aLanguageMap["languages/dutch.txt"] = "nl";
		s_aLanguageMap["languages/esperanto.txt"] = "eo";
		s_aLanguageMap["languages/estonian.txt"] = "ee";
		s_aLanguageMap["languages/finnish.txt"] = "fi";
		s_aLanguageMap["languages/french.txt"] = "fr";
		s_aLanguageMap["languages/galician.txt"] = "gl";
		s_aLanguageMap["languages/german.txt"] = "de";
		s_aLanguageMap["languages/greek.txt"] = "el";
		s_aLanguageMap["languages/hungarian.txt"] = "hu";
		s_aLanguageMap["languages/italian.txt"] = "it";
		s_aLanguageMap["languages/japanese.txt"] = "ja";
		s_aLanguageMap["languages/korean.txt"] = "ko";
		s_aLanguageMap["languages/kyrgyz.txt"] = "ky";
		s_aLanguageMap["languages/norwegian.txt"] = "no"; // multiple tags: no;nb
		s_aLanguageMap["languages/persian.txt"] = "fa";
		s_aLanguageMap["languages/polish.txt"] = "pl";
		s_aLanguageMap["languages/portuguese.txt"] = "pt";
		s_aLanguageMap["languages/romanian.txt"] = "ro";
		s_aLanguageMap["languages/russian.txt"] = "ru";
		s_aLanguageMap["languages/serbian.txt"] = "sr-Latn";
		s_aLanguageMap["languages/serbian_cyrillic.txt"] = "sr-Cyrl";
		s_aLanguageMap["languages/simplified_chinese.txt"] = "zh-CN"; // multiple tags: zh-Hans;zh-CN;zh-SG
		s_aLanguageMap["languages/slovak.txt"] = "sk";
		s_aLanguageMap["languages/spanish.txt"] = "es";
		s_aLanguageMap["languages/swedish.txt"] = "sv";
		s_aLanguageMap["languages/traditional_chinese.txt"] = "zh-HK"; // multiple tags: zh-Hant;zh-HK;zh-MO;zh-TW
		s_aLanguageMap["languages/turkish.txt"] = "tr";
		s_aLanguageMap["languages/azerbaijani.txt"] = "az";
		s_aLanguageMap["languages/ukrainian.txt"] = "uk";

		s_LanguageMapInitialized = true;
	}

	if(g_Config.m_KaizoSendLanguage && g_Config.m_ClLanguagefile[0])
	{
		//first check if we know the string
		if(s_aLanguageMap.find(g_Config.m_ClLanguagefile) == s_aLanguageMap.end())
			return;

		CNetMsg_Cl_Language MsgLang;
		MsgLang.m_pLanguage = s_aLanguageMap[g_Config.m_ClLanguagefile];
		CMsgPacker Packer(&MsgLang);
		MsgLang.Pack(&Packer);
		SendMsgActive(&Packer, MSGFLAG_VITAL);
	}
}
