// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// CheckNewInput() is from Fast Input commit

#include "gameclient.h"
#include <game/mapitems.h>
#include <game/client/components/countryflags.h>
#include <game/client/prediction/entities/character.h>

void CGameClient::OnKaizoConnected()
{
    m_Collision.m_pWorldCore = &m_GameWorld.m_Core;
    m_Collision.m_pTeamsCore = m_GameWorld.Teams();

    m_GameWorld.m_WorldConfig.m_IsPointerTWPlus = false; //initial value for this
    m_GameWorld.m_WorldConfig.m_IsPureVanilla = false;
    m_WaitingForPointerTWPlusInfo = false;   
    m_aClients[0].m_SentCustomClient = false; //to send custom client on connect
    m_SendingCustomClientTicks = 25; // too

    m_GameWorld.OnConnected();
}

void CGameClient::DoKaizoPredictionEffects(CCharacter *pCharacter)
{
    if(g_Config.m_KaizoPredictDeathTiles && !(pCharacter->Team() == TEAM_SUPER || pCharacter->Core()->m_Invincible)) 
    {
        if(pCharacter->m_IsInDeathTile && !m_DidDeathEffect)
        {
            m_Effects.PlayerDeath(pCharacter->m_Pos, pCharacter->GetCid(), 1.0f);
            m_DidDeathEffect = true;
        }
        else if(m_DidDeathEffect && !pCharacter->m_IsInDeathTile)
        {
            m_DidDeathEffect = false;
        }
    }
}

void CGameClient::UpdateKaizoPrediction()
{
    int Tick = (g_Config.m_ClPredict && m_PredictedTick >= MIN_TICK) ? m_PredictedTick : m_GameWorld.GameTick();
    m_GameWorld.m_Core.m_WorldTickKZ = Tick;
	m_Collision.SetTime(static_cast<double>(Tick - m_LastRoundStartTick) / m_GameWorld.GameTickSpeed());
	if(g_Config.m_SvGoresQuadsEnable)
		m_Collision.UpdateQuadCache();
}

void CGameClient::HandleKaizoMessage(int MsgId, CUnpacker *pUnpacker, int Conn, bool Dummy, void *pRawMsg)
{
    if(MsgId == NETMSGTYPE_SV_KAIZONETWORKCROWN)
	{
		CNetMsg_Sv_KaizoNetworkCrown *pMsg = (CNetMsg_Sv_KaizoNetworkCrown *)pRawMsg;
		int CrownId = pMsg->m_ClientId;
		if(CrownId >= 0 && CrownId < MAX_CLIENTS)
		{
			m_aClients[CrownId].m_CrownTick = m_GameWorld.GameTick();
		}
	}

    //for killing spree mode
    if(MsgId == NETMSGTYPE_SV_CHAT)
    {
        CNetMsg_Sv_Chat *pMsg = (CNetMsg_Sv_Chat *)pRawMsg;

        if(pMsg->m_ClientId < 0)
        {
            if(m_WaitingForPointerTWPlusInfo)
            {
                if(str_find(pMsg->m_pMessage, "by Pointer") && str_find(pMsg->m_pMessage, "TW+"))
                {
                    m_GameWorld.m_WorldConfig.m_IsPointerTWPlus = true;
                }
            }

            for(auto &Client : m_aClients)
            {
                char aTempName[MAX_NAME_LENGTH + 6] = {0};
                str_format(aTempName, sizeof(aTempName), "'%s'", Client.m_aName);
                if(str_startswith(pMsg->m_pMessage, Client.m_aName) || str_startswith(pMsg->m_pMessage, aTempName))
                {
                    if(str_find_nocase(pMsg->m_pMessage, "is on a killing spree") || str_find_nocase(pMsg->m_pMessage, "is on a rampage"))
                        Client.m_KillingSpreeMode = true;
                }
            }
        }
        else if(pMsg->m_ClientId < MAX_CLIENTS)
        {
            if(g_Config.m_KaizoReplyTabbedOut)
            {
                IEngineGraphics *pGraphics = ((IEngineGraphics *)Kernel()->RequestInterface<IEngineGraphics>());
		        if(pGraphics && !pGraphics->WindowActive() && Graphics())
                {
                    const char *pPname = m_aClients[m_aLocalIds[0]].m_aName;
                    const char *pDname = m_aClients[m_aLocalIds[1]].m_aName;
                    if(m_aClients[pMsg->m_ClientId].m_Active && ((pPname[0] ? str_find(pMsg->m_pMessage, pPname) : nullptr) || (pDname[0] ? str_find(pMsg->m_pMessage, pDname) : nullptr)))
                    {
                        char aBuf[256];
                        aBuf[0] = '\0';

                        str_format(aBuf, sizeof(aBuf), "/w %s %s", m_aClients[pMsg->m_ClientId].m_aName, g_Config.m_KaizoReplyTabbedOutMsg);
                        m_Chat.SendChat(0, aBuf);
                    }
                }
            }
        }
    }
}

void CGameClient::HandleKaizoSnapItem(const IClient::CSnapItem *pItem)
{
    if(!pItem)
        return;

    if(pItem->m_Type == NETOBJTYPE_KAIZONETWORKCHARACTER)
    {
        const CNetObj_KaizoNetworkCharacter *pKaizoChar = (const CNetObj_KaizoNetworkCharacter *)pItem->m_pData;
        if(!pKaizoChar)
            return;

        int ClientId = pItem->m_Id;
        if(ClientId < 0 || ClientId >= MAX_CLIENTS)
            return;

        m_aClients[ClientId].m_KaizoCharTick = pKaizoChar->m_Tick;
        m_aClients[ClientId].m_CharFlags = pKaizoChar->m_Flags;
        m_aClients[ClientId].m_KaizoCustomWeapon = pKaizoChar->m_RealCurrentWeapon;

        CCharacter *pChar = m_GameWorld.GetCharacterById(ClientId);

        if(pChar)
            pChar->m_KaizoNetworkChar = *pKaizoChar;
    }
    else if(pItem->m_Type == NETOBJTYPE_KAIZONETWORKPLAYERPING)
    {
        const CNetObj_KaizoNetworkPlayerPing *pKaizoPlayerPing = (const CNetObj_KaizoNetworkPlayerPing *)pItem->m_pData;
        if(!pKaizoPlayerPing)
            return;
        
        int ClientId = pItem->m_Id;
        if(ClientId < 0 || ClientId >= MAX_CLIENTS)
            return;

        m_aClients[ClientId].m_ReceivedPing = pKaizoPlayerPing->m_Ping;
    }
}

void CGameClient::PostSnapshotKaizo()
{
    for(auto &Client : m_aClients)
    {
        if(g_Config.m_KaizoSleepingInMenuPlayers && !Client.m_ReceivedDDNetPlayerInfoInLastSnapshot && !(m_Snap.m_aCharacters[Client.m_ClientId].m_Cur.m_PlayerFlags & PLAYERFLAG_IN_MENU)) //reset afk if not receiving ddnet player
        {
            Client.m_Afk = false;
        }
        
        if(!m_Snap.m_aCharacters[Client.m_ClientId].m_Active)
        {
            Client.m_KillingSpreeMode = false;
        }
    }
}

bool CGameClient::IsKaizoCharUpdated(int ClientId)
{
    if(m_aClients[ClientId].m_KaizoCharTick >= 0 && m_aClients[ClientId].m_KaizoCharTick > m_GameWorld.GameTick() - m_GameWorld.GameTickSpeed()/5)
        return true;
	return false;
}

void CGameClient::CClientData::KaizoReset()
{
    m_CrownTick = -1;
    m_KaizoCharTick = -1;
    m_CharFlags = 0;
    m_KaizoCustomWeapon = -1;
    m_ReceivedPing = -1;
    m_ReceivedDDNetPlayerInfoInLastSnapshot = false;

    m_CustomClient = 0;
    m_SentCustomClient = false;
}

bool CGameClient::CheckNewInput() 
{
	return m_Controls.CheckNewInput();
}

void CGameClient::GetKaizoInfo(CServerInfo *pServerInfo)
{
	Client()->GetServerInfo(pServerInfo);

    //Detect Specific mods

    //Pure Vanilla
    m_GameWorld.m_WorldConfig.m_IsPureVanilla = (str_comp(pServerInfo->m_aGameType, "CTF") == 0 ||
                                                str_comp(pServerInfo->m_aGameType, "DM") == 0 ||
                                                str_comp(pServerInfo->m_aGameType, "TDM") == 0 ||
                                                str_comp(pServerInfo->m_aGameType, "LMS") == 0 ||
                                                str_comp(pServerInfo->m_aGameType, "LTS") == 0);

    //InstaShield
    m_InstaShield = pServerInfo->m_aGameType[0] == 'i' && pServerInfo->m_aGameType[str_length(pServerInfo->m_aGameType) - 1] == ')';

    //Pointer's TW+
    if(g_Config.m_KaizoPredictPointerTWPlus && !m_WaitingForPointerTWPlusInfo)
    {
        //First verify flags that Pointer TW+ would and would not send
        if(m_GameInfo.m_GameInfoFlagsKZ & 
        (
        GAMEINFOFLAG_GAMETYPE_PLUS |
		GAMEINFOFLAG_ALLOW_EYE_WHEEL |
		GAMEINFOFLAG_ALLOW_HOOK_COLL |
		GAMEINFOFLAG_PREDICT_VANILLA |
		GAMEINFOFLAG_ENTITIES_DDNET |
		GAMEINFOFLAG_ENTITIES_DDRACE |
		GAMEINFOFLAG_ENTITIES_RACE
        ) && !(m_GameInfo.m_GameInfoFlagsKZ & 
        (
        GAMEINFOFLAG_GAMETYPE_FASTCAP |
        GAMEINFOFLAG_GAMETYPE_FNG |
        GAMEINFOFLAG_GAMETYPE_BLOCK_WORLDS |
        GAMEINFOFLAG_TIMESCORE |
        GAMEINFOFLAG_FLAG_STARTS_RACE |
        GAMEINFOFLAG_RACE |
        GAMEINFOFLAG_GAMETYPE_RACE |
        GAMEINFOFLAG_GAMETYPE_DDRACE |
        GAMEINFOFLAG_GAMETYPE_DDNET |
        GAMEINFOFLAG_GAMETYPE_BLOCK_WORLDS |
        GAMEINFOFLAG_GAMETYPE_VANILLA |
        GAMEINFOFLAG_DDRACE_RECORD_MESSAGE |
        GAMEINFOFLAG_RACE_RECORD_MESSAGE |
        GAMEINFOFLAG_PREDICT_FNG |
        GAMEINFOFLAG_BUG_VANILLA_BOUNCE |
        GAMEINFOFLAG_BUG_FNG_LASER_RANGE
        )))
        {
            //Very possibly this is Pointer's TW+, but to make extremely sure we will send a command that will say us if this is really the Pointer's mod
            m_Chat.SendChat(0, "/info");
            m_WaitingForPointerTWPlusInfo = true;
        }
    }

    //Set Gameinfo values

    //https://github.com/M0REKZ/kaizo-network/issues/17
    bool IsZoomAllowed = (
                        str_find_nocase(pServerInfo->m_aGameType, "race") ||
                        str_find_nocase(pServerInfo->m_aGameType, "monster")
                        );

    //Danger setting: allow zooming, but only do it if it is not 100000% prohibited
    if(IsZoomAllowed && m_GameInfo.m_GameInfoVersionKZ < 0 && g_Config.m_KaizoOldModsZooming)
        m_GameInfo.m_AllowZoom = true;
}

void CGameClient::KaizoReset()
{
    m_InstaShield = false;
    m_DidDeathEffect = false;
}

void CGameClient::KaizoPostUpdate()
{
    bool MustSendCustomClient = false;

    for(auto &Client : m_aClients)
    {
        if(Client.m_Active)
        {
            if(Client.m_ClientId == m_Snap.m_LocalClientId || Client.m_ClientId == m_aLocalIds[!g_Config.m_ClDummy])
            {
                Client.m_CustomClient = CUSTOM_CLIENT_ID_KAIZO_NETWORK; //force Kaizo Network client for us
            }

            if(!Client.m_SentCustomClient)
            {
                MustSendCustomClient = true;
                Client.m_SentCustomClient = true;
            }
        }
        else
        {
            Client.m_SentCustomClient = false;
        }
    }

    if(MustSendCustomClient)
    {
        m_SendingCustomClientTicks = 25;
    }

    switch (m_SendingCustomClientTicks)
    {
    case 25:
        SendInfo(false);
        if(m_aClients[m_aLocalIds[0]].m_Country >= MINIMUM_CUSTOM_CLIENT_ID)
            m_SendingCustomClientTicks = 24;
        break;
    case 24:
        SendDummyInfo(false);
        if(Client()->DummyConnected() ? m_aClients[m_aLocalIds[1]].m_Country >= MINIMUM_CUSTOM_CLIENT_ID : true)
            m_SendingCustomClientTicks = 23;
        break;
    case 1:
        SendInfo(false);
        if(m_aClients[m_aLocalIds[0]].m_Country < MINIMUM_CUSTOM_CLIENT_ID)
            m_SendingCustomClientTicks = 0;
    case 0:
        SendDummyInfo(false);
        if(Client()->DummyConnected() ? m_aClients[m_aLocalIds[1]].m_Country < MINIMUM_CUSTOM_CLIENT_ID : true)
            m_SendingCustomClientTicks = -1;
        break;
    default:
        if(m_SendingCustomClientTicks > 0)
            m_SendingCustomClientTicks--;
        break;
    }

    // check for 0.7 custom clients
    if(Client()->IsSixup())
    {
        bool Found = false;
        for(int CheckingId = 0; CheckingId < MAX_CLIENTS; CheckingId++)
        {
            if(m_aClients[CheckingId].m_CustomClient)
                continue;

            Found = false;
            for(int Client = 0; Client < 4; Client++)
            {
                const char* pClientString = "";
                int CustomClientId = 0;
                switch (Client)
                {
                case 0:
                    pClientString = "gamer!";
                    CustomClientId = CUSTOM_CLIENT_ID_GAMER_07;
                    break;
                case 1:
                    pClientString = "zilly!";
                    CustomClientId = CUSTOM_CLIENT_ID_ZILLYWOODS_07;
                    break;
                case 2:
                    pClientString = "fclient!";
                    CustomClientId = CUSTOM_CLIENT_ID_FCLIENT_07;
                    break;
                case 3:
                    pClientString = "kaizo!";
                    CustomClientId = CUSTOM_CLIENT_ID_KAIZO_NETWORK;
                    break;
                }

                for(int p = 0; p < protocol7::NUM_SKINPARTS; p++)
                {
                    if(str_startswith(m_aClients[CheckingId].m_aSixup[g_Config.m_ClDummy].m_aaSkinPartNames[p], pClientString)) ///seems that they put that info in the skin itself
                    {
                        m_aClients[CheckingId].m_CustomClient = CustomClientId;
                        Found = true;
                        break;
                    }
                }
                if(Found)
                    break;
            }
        }
    }
}

int CGameClient::ReplaceCountryFlagWithCustomClientId(int Country)
{
    if(!g_Config.m_KaizoSendClientType)
        return Country;
    
    if(m_SendingCustomClientTicks <= 1) //dont send custom flag
        return Country;

    //if some random day amount of flags conflicts with invalid flag, just send normal country
    if(m_CountryFlags.Num() >= CUSTOM_CLIENT_ID_KAIZO_NETWORK) 
    {
        return Country;
    }

	return CUSTOM_CLIENT_ID_KAIZO_NETWORK;
}

bool CGameClient::IsCustomClientId(int Country)
{
	return Country > m_CountryFlags.Num();
}

bool CGameClient::IsSkinPartDefault(int Dummy, int Part)
{
	const char *pDefault = "standard";
	if (Part == protocol7::SKINPART_MARKING || Part == protocol7::SKINPART_DECORATION)
		pDefault = "";
	return !str_comp(CSkins7::ms_apSkinVariables[Dummy][Part], pDefault);
}
