//From RushieClient (modified by +KZ)
#include "rclient_indicator.h"

#include "engine/client.h"
#include "engine/external/json-parser/json.h"
#include "engine/serverbrowser.h"
#include "game/client/gameclient.h"


CRClientIndicator::CRClientIndicator()
{
}

void CRClientIndicator::OnInit()
{
	//if(g_Config.m_RiShowRclientIndicator)
		FetchAuthToken();
}

void CRClientIndicator::OnUpdate()
{
	auto HandleTaskDone = [&](std::shared_ptr<CHttpRequest> &pTask, auto &&Finish, auto &&Reset) {
		if(!pTask || pTask->State() != EHttpState::DONE)
			return;
		Finish();
		Reset();
	};
	HandleTaskDone(m_pAuthTokenTask, [this]() { FinishAuthToken(); }, [this]() { ResetAuthToken(); });
	HandleTaskDone(m_pRClientUsersTask, [this]() { FinishRClientUsers(); }, [this]() { ResetRClientUsers(); });
	HandleTaskDone(m_pRClientUsersTaskSend, []() {}, [this]() { ResetRClientUsersSend(); });

	//if(!g_Config.m_RiShowRclientIndicator)
	//	return;

	// Do initial fetch when first connected
	if(Client()->State() == IClient::STATE_ONLINE && !s_InitialFetchDone)
	{
		s_InitialFetchDone = true;
		GameClient()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "RClient", "Send/Get RClient Players for indicator on init");
		SyncRClientUsers();
		return;
	}

	if(Client()->State() != IClient::STATE_ONLINE)
	{
		s_InitialFetchDone = false;
		return;
	}

	if(m_pRClientUsersTask && !m_pRClientUsersTask->Done())
		return;

	const int64_t Now = time_get();
	static constexpr int FETCH_INTERVAL_SECONDS = 30;
	if(Now - s_LastFetch > time_freq() * FETCH_INTERVAL_SECONDS)
	{
		if(s_RclientIndicatorCount == 10)
		{
			GameClient()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "RClient", "Send/Get RClient Players for indicator x10");
			s_RclientIndicatorCount = 0;
		}
		else
			s_RclientIndicatorCount++;

		SyncRClientUsers();
	}
}
// Server and Player Info Collection
void CRClientIndicator::SendServerPlayerInfo()
{
	//+KZ NO
	return;

	if(Client()->State() != IClient::STATE_ONLINE)
		return;

	// Get server address
	CServerInfo CurrentServerInfo;
	Client()->GetServerInfo(&CurrentServerInfo);

	// Send data for main player
	int LocalClientId = GameClient()->m_aLocalIds[0];
	int DummyClientId = -1;
	if(Client()->DummyConnected())
	{
		DummyClientId = GameClient()->m_aLocalIds[1];
	}
	if(LocalClientId >= 0)
	{
		SendPlayerData(CurrentServerInfo.m_aAddress, LocalClientId, DummyClientId);
	}
	// Store current server info for comparison
}

void CRClientIndicator::SendPlayerData(const char *pServerAddress, int ClientId, int DummyClientId)
{
	//+KZ NO
	return;

	if(m_aAuthToken[0] == '\0')
	{
		// Token not yet fetched, try again later.
		if(!m_pAuthTokenTask)
			FetchAuthToken();
		return;
	}
	// Create JSON data for this specific player
	char aJsonData[512];

	if(DummyClientId >= 0)
		str_format(aJsonData, sizeof(aJsonData),
			"{"
			"\"server_address\":\"%s\","
			"\"player_id\":%d,"
			"\"dummy_id\":%d,"
			"\"auth_token\":\"%s\","
			"\"timestamp\":%lld"
			"}",
			pServerAddress,
			ClientId,
			DummyClientId,
			m_aAuthToken,
			(long long)time_get());
	else
		str_format(aJsonData, sizeof(aJsonData),
			"{"
			"\"server_address\":\"%s\","
			"\"player_id\":%d,"
			"\"auth_token\":\"%s\","
			"\"timestamp\":%lld"
			"}",
			pServerAddress,
			ClientId,
			m_aAuthToken,
			(long long)time_get());

	// Create and send HTTP request
	m_pRClientUsersTaskSend = std::make_shared<CHttpRequest>(CRClientIndicator::RCLIENT_URL_USERS);
	m_pRClientUsersTaskSend->PostJson(aJsonData);
	m_pRClientUsersTaskSend->Timeout(CTimeout{10000, 0, 500, 5});
	m_pRClientUsersTaskSend->IpResolve(IPRESOLVE::V4);
	Http()->Run(m_pRClientUsersTaskSend);
}

void CRClientIndicator::FetchRClientUsers()
{
	if(m_pRClientUsersTask && !m_pRClientUsersTask->Done())
		return;

	m_pRClientUsersTask = HttpGet(CRClientIndicator::RCLIENT_URL_USERS);
	m_pRClientUsersTask->Timeout(CTimeout{10000, 0, 500, 5});
	m_pRClientUsersTask->IpResolve(IPRESOLVE::V4);
	Http()->Run(m_pRClientUsersTask);
}

void CRClientIndicator::SyncRClientUsers()
{
	SendServerPlayerInfo();
	FetchRClientUsers();
	s_LastFetch = time_get();
}

void CRClientIndicator::FetchAuthToken()
{
	if(m_pAuthTokenTask && !m_pAuthTokenTask->Done())
		return;

	m_pAuthTokenTask = HttpGet(CRClientIndicator::RCLIENT_TOKEN_URL);
	m_pAuthTokenTask->Timeout(CTimeout{10000, 0, 500, 5});
	m_pAuthTokenTask->IpResolve(IPRESOLVE::V4);
	Http()->Run(m_pAuthTokenTask);
}

void CRClientIndicator::FinishAuthToken()
{
	if(m_pAuthTokenTask->State() != EHttpState::DONE)
		return;

	json_value *pJson = m_pAuthTokenTask->ResultJson();
	if(!pJson)
	{
		GameClient()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "RClient", "Failed to fetch auth token: no JSON");
		return;
	}

	const json_value &Json = *pJson;
	const json_value &Token = Json["token"];

	if(Token.type == json_string)
	{
		str_copy(m_aAuthToken, Token.u.string.ptr, sizeof(m_aAuthToken));
		// The token might have a newline at the end
		str_utf8_trim_right(m_aAuthToken);
		GameClient()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "RClient", "Fetched auth token");
	}
	else
	{
		GameClient()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "RClient", "Failed to fetch auth token: token not found in JSON");
	}
	json_value_free(pJson);
}

void CRClientIndicator::ResetAuthToken()
{
	if(m_pAuthTokenTask)
	{
		m_pAuthTokenTask->Abort();
		m_pAuthTokenTask = nullptr;
	}
}

void CRClientIndicator::ResetRClientUsersSend()
{
	if(m_pRClientUsersTaskSend)
	{
		m_pRClientUsersTaskSend->Abort();
		m_pRClientUsersTaskSend = nullptr;
	}
}

void CRClientIndicator::FinishRClientUsers()
{
	json_value *pJson = m_pRClientUsersTask->ResultJson();
	if(!pJson)
		return;

	const json_value &Json = *pJson;
	m_vRClientUsers.clear();

	// Parse the JSON response to get list of RClient users
	if(Json.type == json_object)
	{
		// The response format is: {"server_address": {"player_id": {...}}}
		for(unsigned int i = 0; i < Json.u.object.length; i++)
		{
			const char *pServerAddr = Json.u.object.values[i].name;
			const json_value &PlayersObj = *Json.u.object.values[i].value;

			if(PlayersObj.type == json_object)
			{
				for(unsigned int j = 0; j < PlayersObj.u.object.length; j++)
				{
					const char *pPlayerIdStr = PlayersObj.u.object.values[j].name;
					int PlayerId = atoi(pPlayerIdStr);
					const json_value &PlayerData = *PlayersObj.u.object.values[j].value;

					// Add main player ID
					m_vRClientUsers.emplace_back(std::string(pServerAddr), PlayerId);

					// Check if this player has a dummy and add dummy ID too
					if(PlayerData.type == json_object)
					{
						for(unsigned int k = 0; k < PlayerData.u.object.length; k++)
						{
							if(str_comp(PlayerData.u.object.values[k].name, "dummy_id") == 0 &&
								PlayerData.u.object.values[k].value->type == json_integer)
							{
								int DummyId = PlayerData.u.object.values[k].value->u.integer;
								m_vRClientUsers.emplace_back(std::string(pServerAddr), DummyId);
								break;
							}
						}
					}
				}
			}
		}
	}

	json_value_free(pJson);
}

void CRClientIndicator::ResetRClientUsers()
{
	if(m_pRClientUsersTask)
	{
		m_pRClientUsersTask->Abort();
		m_pRClientUsersTask = nullptr;
	}
}

bool CRClientIndicator::IsPlayerRClient(int ClientId)
{
	if(Client()->State() != IClient::STATE_ONLINE)
		return false;

	// // Always show RClient indicator for local players (main and dummy)
	// if(GameClient()->m_Snap.m_apPlayerInfos[ClientId] && GameClient()->m_Snap.m_apPlayerInfos[ClientId]->m_Local)
	// 	return true;
	//
	// // Also check if this is our dummy using m_aLocalIds
	// if(ClientId == GameClient()->m_aLocalIds[0] || ClientId == GameClient()->m_aLocalIds[1])
	// 	return true;

	CServerInfo CurrentServerInfo;
	Client()->GetServerInfo(&CurrentServerInfo);

	// Check if this player is in our RClient users list
	for(const auto &User : m_vRClientUsers)
	{
		if(str_comp(User.first.c_str(), CurrentServerInfo.m_aAddress) == 0 && User.second == ClientId)
			return true;
	}

	return false;
}
