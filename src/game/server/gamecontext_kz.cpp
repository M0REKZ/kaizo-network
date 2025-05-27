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
#include "gamemodes/DDRace.h"
#include "gamemodes/mod.h"
#include "gamemodes/kz/kz.h" // KZ
#include "player.h"
#include "score.h"

void CGameContext::RegisterKZCommands()
{
	Console()->Register("rejoin_shutdown", "", CFGFLAG_SERVER, ConRejoinShutdown, this, "Make players rejoin after shutdown");
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
			int PauseId = clamp(ParaI1, 0, Server()->MaxClients() - 1);
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