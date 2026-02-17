#ifndef RCLIENT_RCLIENT_INDICATOR_H
#define RCLIENT_RCLIENT_INDICATOR_H

#include <game/client/component.h>

#include <engine/shared/console.h>
#include <engine/shared/http.h>

class CRClientIndicator : public CComponent
{
	std::shared_ptr<CHttpRequest> m_pAuthTokenTask = nullptr;
	void FetchAuthToken();
	void FinishAuthToken();
	void ResetAuthToken();
	char m_aAuthToken[128] = {0};

	static constexpr const char *RCLIENT_URL_USERS = "https://server.rushie-client.ru/users.json";
	static constexpr const char *RCLIENT_TOKEN_URL = "https://server.rushie-client.ru/token";
	// Server and Player Info Collection
	std::shared_ptr<CHttpRequest> m_pRClientUsersTaskSend = nullptr;
	void SendServerPlayerInfo();
	void SendPlayerData(const char *pServerAddress, int ClientId, int DummyClientId = -1);
	void FetchRClientUsers();
	void FinishRClientUsers();
	void ResetRClientUsers();
	// void FinishRClientUsersSend();
	void ResetRClientUsersSend();
	std::shared_ptr<CHttpRequest> m_pRClientUsersTask = nullptr;
	std::vector<std::pair<std::string, int>> m_vRClientUsers; // server address, player id
	void SyncRClientUsers();
	int64_t s_LastFetch = 0;
	bool s_InitialFetchDone = false;
	int s_RclientIndicatorCount = 0;
public:
	CRClientIndicator();
	int Sizeof() const override { return sizeof(*this); }
	void OnInit() override;

	void OnUpdate() override;

	bool IsPlayerRClient(int ClientId);
};

#endif // RCLIENT_RCLIENT_INDICATOR_H
