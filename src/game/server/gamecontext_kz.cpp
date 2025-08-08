#include "gamecontext.h"

#include <vector>

#include "teeinfo.h"
#include <antibot/antibot_data.h>
#include <base/logger.h>
#include <base/math.h>
#include <base/system.h>
#include <engine/console.h>
#include <engine/engine.h>
#include <engine/map.h>
#include <engine/server/server.h>
#include <engine/shared/config.h>
#include <engine/shared/datafile.h>
#include <engine/shared/json.h>
#include <engine/shared/linereader.h>
#include <engine/shared/memheap.h>
#include <engine/shared/protocolglue.h>
#include <engine/storage.h>

#include <game/collision.h>
#include <game/gamecore.h>
#include <game/mapitems.h>
#include <game/version.h>

#include <game/generated/protocol7.h>
#include <game/generated/protocolglue.h>

#include "entities/character.h"
#include "entities/kz/portal.h"
#include "gamemodes/DDRace.h"
#include "gamemodes/mod.h"
#include "gamemodes/kz/kz.h" // KZ
#include "player.h"
#include "score.h"

void CGameContext::RegisterKZCommands()
{
	Console()->Register("rejoin_shutdown", "", CFGFLAG_SERVER, ConRejoinShutdown, this, "Make players rejoin after shutdown");
	Console()->Register("portalgun", "", CFGFLAG_CHAT |  CFGFLAG_SERVER, ConPortalGun, this, "Set Portal Gun as active weapon (if have it)");
	Console()->Register("unportalgun", "?i[id]", CFGFLAG_SERVER, ConUnPortalGun, this, "Remove Portal Gun");
	Console()->Register("getportalgun", "?i[id]", CFGFLAG_SERVER, ConGetPortalGun, this, "Get Portal Gun");
	Console()->Register("orangeportal", "", CFGFLAG_CHAT |  CFGFLAG_SERVER, ConOrangePortal, this, "Use Orange Portal");
	Console()->Register("blueportal", "", CFGFLAG_CHAT |  CFGFLAG_SERVER, ConBluePortal, this, "Use Blue Portal");
	Console()->Register("resetportals", "", CFGFLAG_CHAT |  CFGFLAG_SERVER, ConResetPortals, this, "Reset both Portals");
	Console()->Register("showcrowns", "", CFGFLAG_CHAT |  CFGFLAG_SERVER, ConShowCrowns, this, "Toggle crowns");
}

void CGameContext::SendGameMsg(int GameMsgId, int ClientId) const
{
	dbg_assert(
		GameMsgId == protocol7::GAMEMSG_TEAM_SWAP ||
			GameMsgId == protocol7::GAMEMSG_SPEC_INVALIDID ||
			GameMsgId == protocol7::GAMEMSG_TEAM_SHUFFLE ||
			GameMsgId == protocol7::GAMEMSG_TEAM_BALANCE ||
			GameMsgId == protocol7::GAMEMSG_CTF_DROP ||
			GameMsgId == protocol7::GAMEMSG_CTF_RETURN,
		"the passed game message id does not take 0 arguments");

	CMsgPacker Msg(protocol7::NETMSGTYPE_SV_GAMEMSG, false, true);
	Msg.AddInt(GameMsgId);
	if(ClientId != -1 && Server()->IsSixup(ClientId))
	{
		Server()->SendMsg(&Msg, MSGFLAG_VITAL, ClientId);
		return;
	}
	for(int i = 0; i < Server()->MaxClients(); i++)
	{
		if(Server()->IsSixup(i))
		{
			Server()->SendMsg(&Msg, MSGFLAG_VITAL, i);
			continue;
		}
		// TODO: 0.6
	}
}

void CGameContext::SendGameMsg(int GameMsgId, int ParaI1, int ClientId) const
{
	dbg_assert(
		GameMsgId == protocol7::GAMEMSG_TEAM_ALL ||
			GameMsgId == protocol7::GAMEMSG_TEAM_BALANCE_VICTIM ||
			GameMsgId == protocol7::GAMEMSG_CTF_GRAB ||
			GameMsgId == protocol7::GAMEMSG_GAME_PAUSED,
		"the passed game message id does not take 1 argument");

	CMsgPacker Msg(protocol7::NETMSGTYPE_SV_GAMEMSG, false, true);
	Msg.AddInt(GameMsgId);
	Msg.AddInt(ParaI1);
	if(ClientId != -1 && Server()->IsSixup(ClientId))
	{
		Server()->SendMsg(&Msg, MSGFLAG_VITAL, ClientId);
		return;
	}
	for(int i = 0; i < Server()->MaxClients(); i++)
	{
		if(!m_apPlayers[i])
			continue;

		if(Server()->IsSixup(i))
		{
			Server()->SendMsg(&Msg, MSGFLAG_VITAL, i);
			continue;
		}
		if(GameMsgId == protocol7::GAMEMSG_GAME_PAUSED)
		{
			char aBuf[512];
			int PauseId = std::clamp(ParaI1, 0, Server()->MaxClients() - 1);
			str_format(aBuf, sizeof(aBuf), "'%s' initiated a pause. If you are ready do /ready", Server()->ClientName(PauseId));
			SendChatTarget(i, aBuf);
		}
	}
}

void CGameContext::SendGameMsg(int GameMsgId, int ParaI1, int ParaI2, int ParaI3, int ClientId) const
{
	dbg_assert(GameMsgId == protocol7::GAMEMSG_CTF_CAPTURE, "the passed game message id does not take 3 arguments");

	CMsgPacker Msg(protocol7::NETMSGTYPE_SV_GAMEMSG, false, true);
	Msg.AddInt(GameMsgId);
	Msg.AddInt(ParaI1);
	Msg.AddInt(ParaI2);
	Msg.AddInt(ParaI3);
	if(ClientId != -1)
		Server()->SendMsg(&Msg, MSGFLAG_VITAL, ClientId);
	for(int i = 0; i < Server()->MaxClients(); i++)
	{
		if(Server()->IsSixup(i))
		{
			Server()->SendMsg(&Msg, MSGFLAG_VITAL, i);
			continue;
		}
		// TODO: 0.6
	}
}

void CGameContext::CreateMapSoundEvent(vec2 Pos, int Id, CClientMask Mask)
{
	CNetEvent_MapSoundWorld *pEvent = m_Events.Create<CNetEvent_MapSoundWorld>(Mask);
	if(pEvent)
	{
		pEvent->m_X = (int)Pos.x;
		pEvent->m_Y = (int)Pos.y;
		pEvent->m_SoundId = Id;
	}
}

void CGameContext::CreateMapSoundEventForClient(vec2 Pos, int Id, int ClientId, CClientMask Mask)
{
	CNetEvent_MapSoundWorld *pEvent = m_Events.CreateForClient<CNetEvent_MapSoundWorld>(ClientId, Mask);
	if(pEvent)
	{
		pEvent->m_X = (int)Pos.x;
		pEvent->m_Y = (int)Pos.y;
		pEvent->m_SoundId = Id;
	}
}

void CGameContext::ConRejoinShutdown(IConsole::IResult *pResult, void *pUserData)
{
    CGameContext *pSelf = (CGameContext *)pUserData;
	
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(!pSelf->m_apPlayers[i])
			continue;

		if(pSelf->m_apPlayers[i]->GetClientVersion() < VERSION_DDNET_REDIRECT)
			continue;
		pSelf->Server()->RedirectClient(i,pSelf->Server()->Port());
	}

	pSelf->Console()->ExecuteLine("shutdown Reserved. Please wait or reconnect to the server.");
}

void CGameContext::ConPortalGun(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	int ClientID = pResult->m_ClientId;

	if(ClientID < 0 || ClientID >= MAX_CLIENTS)
		return;

	if(!pSelf->m_apPlayers[ClientID])
		return;

	if(!pSelf->m_apPlayers[ClientID]->GetCharacter())
		return;

	bool got = pSelf->m_apPlayers[ClientID]->GetCharacter()->GetWeaponGot(KZ_CUSTOM_WEAPON_PORTAL_GUN);

	if(got)
		pSelf->m_apPlayers[ClientID]->GetCharacter()->SetWeapon(KZ_CUSTOM_WEAPON_PORTAL_GUN);
}

void CGameContext::ConUnPortalGun(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	int ClientID;

	if(pResult->NumArguments())
	{
		ClientID = pResult->GetInteger(0);
	}
	else
	{
		ClientID = pResult->m_ClientId;
	}

	if(ClientID < 0 || ClientID >= MAX_CLIENTS)
		return;

	if(!pSelf->m_apPlayers[ClientID])
		return;

	if(!pSelf->m_apPlayers[ClientID]->GetCharacter())
		return;

	bool got = pSelf->m_apPlayers[ClientID]->GetCharacter()->GetWeaponGot(KZ_CUSTOM_WEAPON_PORTAL_GUN);

	if(got)
	{
		pSelf->m_apPlayers[ClientID]->GetCharacter()->SetWeaponGot(KZ_CUSTOM_WEAPON_PORTAL_GUN, false);
		pSelf->m_apPlayers[ClientID]->GetCharacter()->SetWeaponAmmo(KZ_CUSTOM_WEAPON_PORTAL_GUN, 0);
	}

	pSelf->m_apPlayers[ClientID]->GetCharacter()->SetWeapon(WEAPON_GUN);
}

void CGameContext::ConGetPortalGun(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	int ClientID;

	if(pResult->NumArguments())
	{
		ClientID = pResult->GetInteger(0);
	}
	else
	{
		ClientID = pResult->m_ClientId;
	}

	if(ClientID < 0 || ClientID >= MAX_CLIENTS)
		return;

	if(!pSelf->m_apPlayers[ClientID])
		return;

	if(!pSelf->m_apPlayers[ClientID]->GetCharacter())
		return;

	bool got = pSelf->m_apPlayers[ClientID]->GetCharacter()->GetWeaponGot(KZ_CUSTOM_WEAPON_PORTAL_GUN);

	if(!got)
	{
		pSelf->m_apPlayers[ClientID]->GetCharacter()->SetWeaponGot(KZ_CUSTOM_WEAPON_PORTAL_GUN, true);
		pSelf->m_apPlayers[ClientID]->GetCharacter()->SetWeaponAmmo(KZ_CUSTOM_WEAPON_PORTAL_GUN, 10);
	}

	pSelf->m_apPlayers[ClientID]->GetCharacter()->SetWeapon(KZ_CUSTOM_WEAPON_PORTAL_GUN);
}

void CGameContext::ConOrangePortal(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	int ClientID;

	ClientID = pResult->m_ClientId;

	if(ClientID < 0 || ClientID >= MAX_CLIENTS)
		return;

	if(!pSelf->m_apPlayers[ClientID])
		return;

	if(!pSelf->m_apPlayers[ClientID]->GetCharacter())
		return;

	pSelf->m_apPlayers[ClientID]->GetCharacter()->m_BluePortal = false;
}

void CGameContext::ConBluePortal(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	int ClientID;

	ClientID = pResult->m_ClientId;
	
	if(ClientID < 0 || ClientID >= MAX_CLIENTS)
		return;

	if(!pSelf->m_apPlayers[ClientID])
		return;

	if(!pSelf->m_apPlayers[ClientID]->GetCharacter())
		return;

	pSelf->m_apPlayers[ClientID]->GetCharacter()->m_BluePortal = true;
}

void CGameContext::ConResetPortals(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	int ClientID;

	ClientID = pResult->m_ClientId;

	if(ClientID < 0 || ClientID >= MAX_CLIENTS)
		return;

	if(!pSelf->m_apPlayers[ClientID])
		return;


	for(CPortalKZ* p = (CPortalKZ*)pSelf->m_World.FindFirst(CGameWorld::CUSTOM_ENTTYPE_PORTAL);p;p = (CPortalKZ*)p->TypeNext())
	{
		if(p->m_Owner == ClientID)
		{
			p->Reset();
			CPortalKZ* p2 = p->GetOtherPortal();
			if(p2)
			{
				p2->Reset();
			}
			return;
		}
	}
}

void CGameContext::ConShowCrowns(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	int ClientID;

	ClientID = pResult->m_ClientId;
	
	if(ClientID < 0 || ClientID >= SERVER_MAX_CLIENTS)
		return;

	if(!pSelf->m_apPlayers[ClientID])
		return;

	pSelf->m_apPlayers[ClientID]->m_SendCrowns = !pSelf->m_apPlayers[ClientID]->m_SendCrowns;
}

void CGameContext::SendDiscordChatMessage(int ClientID, const char* msg)
{
	char aPayload[4048];
	char aStatsStr[4000];
	char aStr[275];
	aStr[0] = '\0';
	if(CheckClientId(ClientID))
		str_format(aStr, sizeof(aStr),"%s: %s",Server()->ClientName(ClientID),msg);
	else
		str_format(aStr, sizeof(aStr),"%s: %s","Server",msg);

	str_format(
		aPayload,
		sizeof(aPayload),
		"{\"allowed_mentions\": {\"parse\": []}, \"content\": \"%s\"}",
		EscapeJson(aStatsStr, sizeof(aStatsStr), aStr));
	const int PayloadSize = str_length(aPayload);
	// TODO: use HttpPostJson()
	std::shared_ptr<CHttpRequest> pDiscord = HttpPost(g_Config.m_SvChatDiscordWebhook, (const unsigned char *)aPayload, PayloadSize);
	pDiscord->LogProgress(HTTPLOG::FAILURE);
	pDiscord->IpResolve(IPRESOLVE::V4);
	pDiscord->Timeout(CTimeout{4000, 15000, 500, 5});
	pDiscord->HeaderString("Content-Type", "application/json");
	m_pHttp->Run(pDiscord);
}

void CGameContext::SendDiscordRecordMessage(int ClientID, float Time, float PrevTime)
{
	char aPayload[4048];
	char aStatsStr[4000];
	char aStr[500];
	aStr[0] = '\0';

	str_format(aStr, sizeof(aStr),"New record on map %s by %s: %d minute(s) %5.2f second(s)!!!", Server()->GetMapName(), Server()->ClientName(ClientID), (int)Time / 60, Time - ((int)Time / 60 * 60));

	str_format(
		aPayload,
		sizeof(aPayload),
		"{\"allowed_mentions\": {\"parse\": []}, \"content\": \"%s\"}",
		EscapeJson(aStatsStr, sizeof(aStatsStr), aStr));
	const int PayloadSize = str_length(aPayload);
	// TODO: use HttpPostJson()
	std::shared_ptr<CHttpRequest> pDiscord = HttpPost(g_Config.m_SvRecordsDiscordWebhook, (const unsigned char *)aPayload, PayloadSize);
	pDiscord->LogProgress(HTTPLOG::FAILURE);
	pDiscord->IpResolve(IPRESOLVE::V4);
	pDiscord->Timeout(CTimeout{4000, 15000, 500, 5});
	pDiscord->HeaderString("Content-Type", "application/json");
	m_pHttp->Run(pDiscord);
}

void CGameContext::IdentifyClientName(int ClientId, char *pName, int StrSize)
{
	if(!m_apPlayers[ClientId])
		return;

	char aName[StrSize];

	aName[0] = '\0';

	if(Server()->IsSixup(ClientId))
	{
		for(int Client = 0; Client < 3; Client++)
		{
			const char* pClientString = "";
			const char* pClientName = "";
			int Sprite = -1;
			switch (Client)
			{
			case 0:
				pClientString = "gamer!";
				pClientName = "Gamer (0.7)";
				break;
			case 1:
				pClientString = "zilly!";
				pClientName = "ZillyWoods (0.7)";
				break;
			case 2:
				pClientString = "fclient!";
				pClientName = "F-Client (0.7)";
				break;
			}

			for(int p = 0; p < protocol7::NUM_SKINPARTS; p++)
			{
				if(str_startswith(m_apPlayers[ClientId]->m_TeeInfos.m_apSkinPartNames[p], pClientString)) ///seems that they put that info in the skin itself
				{
					str_copy(aName, pClientName, StrSize);
					break;
				}
			}
			if(aName[0])
				break;
		}
		if(!aName[0])
		{
			str_copy(aName, "Teeworlds (0.7)", StrSize);
		}
	}
	else
	{
		int InfClass = Server()->GetClientInfclassVersion(ClientId);
		bool Tater = Server()->IsTaterClient(ClientId);
		bool Qxd = Server()->IsQxdClient(ClientId);
		bool Chillerbot = Server()->IsChillerbotClient(ClientId);
		bool StA = Server()->IsStAClient(ClientId);
		bool AllTheHaxx = Server()->IsAllTheHaxxClient(ClientId);
		bool Pulse = Server()->IsPulseClient(ClientId);

		if(InfClass)
		{
			str_copy(aName, "InfClass Client (0.6)", StrSize);
		}
		else if(Tater)
		{
			str_copy(aName, "T-Client (0.6)", StrSize);
		}
		else if(Qxd)
		{
			str_copy(aName, "E-Client (0.6)", StrSize);
		}
		else if(Chillerbot)
		{
			str_copy(aName, "Chillerbot-ux (0.6)", StrSize);
		}
		else if(StA)
		{
			str_copy(aName, "StA Client (0.6)", StrSize);
		}
		else if(AllTheHaxx)
		{
			str_copy(aName, "AllTheHaxx Client (0.6)", StrSize);
		}
		else if(Pulse)
		{
			str_copy(aName, "Pulse Client (0.6)", StrSize);
		}
		else
		{

			int Version = Server()->GetClientVersion(ClientId);

			if(Version >= VERSION_DDNET_OLD)
			{
				str_copy(aName, "DDNet (0.6)", StrSize);
			}
			else if(Version >= VERSION_DDRACE)
			{
				str_copy(aName, "DDRace (0.6)", StrSize);
			}
			else
			{
				str_copy(aName, "Teeworlds (0.6)", StrSize);
			}
		}
	}
	str_copy(pName, aName, StrSize);
}