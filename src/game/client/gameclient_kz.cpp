// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// CheckNewInput() is from Fast Input commit

#include "gameclient.h"
#include <game/mapitems.h>
#include <game/client/components/countryflags.h>
#include <game/client/prediction/entities/character.h>
#include <engine/shared/linereader.h>
#include <base/io.h>

void CGameClient::OnKaizoConnected()
{
    m_Collision.m_pTeamsCore = m_GameWorld.Teams();
    m_Collision.m_IsKaizoServer = false;

    m_GameWorld.m_WorldConfig.m_IsPointerTWPlus = false; //initial value for this
    m_GameWorld.m_WorldConfig.m_IsPureVanilla = false;
    m_GameWorld.m_WorldConfig.m_HasLaserJump = false;
    m_GameWorld.m_WorldConfig.m_HasAutoPistol = false;
    m_GameWorld.m_WorldConfig.m_HasPointerTiles = false;
    m_WaitingForPointerTWPlusInfo = false;   

    m_GameWorld.m_Core.InitKZQuads(Layers());
    m_GameWorld.OnConnected();
    m_PredictedWorld.m_Core.CopyKZQuads(&m_GameWorld.m_Core.GetKZQuads());
}

void CGameClient::DoKaizoPredictionEffects(CCharacter *pCharacter)
{
    if(!(pCharacter->Team() == TEAM_SUPER || pCharacter->Core()->m_Invincible)) 
    {
        if(!m_aClients[pCharacter->GetCid()].m_DidDeathEffect)
        {
            if((g_Config.m_KaizoPredictDeathTiles && pCharacter->m_IsInDeathTile) || pCharacter->m_IsDead)
            {
                m_Effects.PlayerDeath(pCharacter->m_Pos, pCharacter->GetCid(), 1.0f);
                m_aClients[pCharacter->GetCid()].m_DidDeathEffect = true;
            }
        }
        else if(m_aClients[pCharacter->GetCid()].m_DidDeathEffect && !pCharacter->m_IsInDeathTile && !pCharacter->m_IsDead)
        {
            m_aClients[pCharacter->GetCid()].m_DidDeathEffect = false;
        }
    }
}

void CGameClient::UpdateKaizoPrediction()
{
    int Tick = (g_Config.m_ClPredict && m_PredictedTick >= MIN_TICK) ? m_PredictedTick : m_GameWorld.GameTick();
    m_GameWorld.m_Core.m_WorldTickKZ = Tick;
	m_GameWorld.m_Core.SetTime(static_cast<double>(Tick - m_LastRoundStartTick) / m_GameWorld.GameTickSpeed());
	if(g_Config.m_SvGoresQuadsEnable)
		m_Collision.UpdateQuadCache(&m_GameWorld.m_Core);
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

        m_Collision.m_IsKaizoServer = true;
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
    else if(MsgId == NETMSGTYPE_SV_PREINPUT)
    {
        CNetMsg_Sv_PreInput *pMsg = (CNetMsg_Sv_PreInput *)pRawMsg;
        m_aClients[pMsg->m_Owner].m_ReceivedPreInputs = true;
    }
    else if (MsgId == NETMSGTYPE_SV_IMAGERESOURCETWPLUS) //From Duck/Infclass Client
	{
		CNetMsg_Sv_ImageResourceTWPlus *pMsg = (CNetMsg_Sv_ImageResourceTWPlus *)pRawMsg;

		m_Resources.OnResourceMessage(pMsg);
	}
}

void CGameClient::HandleKaizoSnapItem(const IClient::CSnapItem *pItem)
{
    if(!pItem)
        return;

    if(pItem->m_Type == NETOBJTYPE_KAIZONETWORKCHARACTER)
    {
        m_Collision.m_IsKaizoServer = true;
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
        m_Collision.m_IsKaizoServer = true;
        const CNetObj_KaizoNetworkPlayerPing *pKaizoPlayerPing = (const CNetObj_KaizoNetworkPlayerPing *)pItem->m_pData;
        if(!pKaizoPlayerPing)
            return;
        
        int ClientId = pItem->m_Id;
        if(ClientId < 0 || ClientId >= MAX_CLIENTS)
            return;

        m_aClients[ClientId].m_ReceivedPing = pKaizoPlayerPing->m_Ping;
    }
    else if(pItem->m_Type == NETOBJTYPE_CLIENTINFO)
    {
        const CNetObj_ClientInfo *pInfo = (const CNetObj_ClientInfo *)pItem->m_pData;
		int ClientId = pItem->m_Id;
		if(ClientId < MAX_CLIENTS && ClientId >= 0)
		{
			CClientData *pClient = &m_aClients[ClientId];

            //identify kaizo

            if(MACRO_IS_SKIN_COLOR_CCID(
                pInfo->m_ColorBody, pInfo->m_ColorFeet,
                CCID_COLOR_BODY_KAIZO_CLIENT, CCID_COLOR_FEET_KAIZO_CLIENT
            ))
            {
                pClient->m_CustomClient = CUSTOM_CLIENT_ID_KAIZO_NETWORK;
            }
            else if(MACRO_IS_SKIN_COLOR_CCID(
                pInfo->m_ColorBody, pInfo->m_ColorFeet,
                CCID_COLOR_BODY_CHILLERBOTUX, CCID_COLOR_FEET_CHILLERBOTUX
            ))
            {
                pClient->m_CustomClient = CUSTOM_CLIENT_ID_CHILLERBOTUX;
            }
            else if(MACRO_IS_SKIN_COLOR_CCID(
                pInfo->m_ColorBody, pInfo->m_ColorFeet,
                CCID_COLOR_BODY_PDUCKCLIENT, CCID_COLOR_FEET_PDUCKCLIENT
            ))
            {
                pClient->m_CustomClient = CUSTOM_CLIENT_ID_PDUCKCLIENT;
            }
        }
    }
    else if(pItem->m_Type == NETOBJTYPE_GAMEINFOTWPLUS)
    {
        const CNetObj_GameInfoTWPlus *pInfo = (const CNetObj_GameInfoTWPlus *)pItem->m_pData;

        m_GameWorld.m_WorldConfig.m_HasPointerTiles = pInfo->m_Flags & GAMETWPLUSFLAG_PREDICT_PTWPLUS_TILES;
        m_GameWorld.m_WorldConfig.m_HasLaserJump = pInfo->m_Flags & GAMETWPLUSFLAG_LASERJUMPS;
        m_GameWorld.m_WorldConfig.m_HasAutoPistol = pInfo->m_Flags & GAMETWPLUSFLAG_GUN_FULLAUTO;
    }
    else if(pItem->m_Type == NETOBJTYPE_RESPAWNTIMER)
	{
		m_Snap.m_pRespawnTimer = (const CNetObj_RespawnTimer *)pItem->m_pData;
	}
    else if(pItem->m_Type == NETOBJTYPE_CHARACTER)
    {
        const CNetObj_Character *pCharObj = (const CNetObj_Character *)pItem->m_pData;

        //update local client input

        if(pItem->m_Id < MAX_CLIENTS && pItem->m_Id >= 0)
        {
            CNetObj_PlayerInput &Input = m_aClients[pItem->m_Id].m_GuessedInputKZ;

            Input.m_Direction = pCharObj->m_Direction;
            Input.m_Jump = pCharObj->m_Jumped & 1;
            Input.m_Fire = 0;
            //Input.m_Fire = (pCharObj->m_AttackTick == Client()->GameTick(g_Config.m_ClDummy)) ? 1 : 0;
            Input.m_Hook = pCharObj->m_HookState != HOOK_IDLE;
            Input.m_PlayerFlags = pCharObj->m_PlayerFlags;
            Input.m_WantedWeapon = pCharObj->m_Weapon + 1;
            Input.m_TargetX = std::cos(pCharObj->m_Angle / 256.0f) * 256.0f;
			Input.m_TargetY = std::sin(pCharObj->m_Angle / 256.0f) * 256.0f;
            
            if(CCharacter *pChar = m_GameWorld.GetCharacterById(pItem->m_Id))
            {
                Input.m_Fire = pChar->LatestInput()->m_Fire;
            }
        }
    }
    else if(pItem->m_Type == NETOBJTYPE_DDNETCHARACTER)
    {
        const CNetObj_DDNetCharacter *pCharObj = (const CNetObj_DDNetCharacter *)pItem->m_pData;

        //update local client input

        if(pItem->m_Id < MAX_CLIENTS && pItem->m_Id >= 0)
        {
            CNetObj_PlayerInput &Input = m_aClients[pItem->m_Id].m_GuessedInputKZ;

            Input.m_TargetX = pCharObj->m_TargetX;
            Input.m_TargetY = pCharObj->m_TargetY;
        }
    }
}

void CGameClient::PostSnapshotKaizo()
{
    for(auto &Client : m_aClients)
    {
        if(!Client.m_ReceivedDDNetPlayerInfoInLastSnapshot)
        {
            Client.m_CustomClient = 0; //reset custom client if not received
        }

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
    m_ReceivedPreInputs = false;
    m_DidDeathEffect = false;
    m_IsBOMBTick = -1;

    m_CustomClient = 0;
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

    //Pointer's TW+ (0.6 version)
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

    m_GameInfo.m_AllowXSkins |= (bool)g_Config.m_KaizoAlwaysAllowXSkins;
    m_GameInfo.m_AllowHookColl |= (bool)g_Config.m_KaizoAlwaysAllowShowHookColl;
    m_GameInfo.m_HudDDRace |= (bool)g_Config.m_KaizoForceDDNetHUD;
    m_GameInfo.m_BugDDRaceInput |= (bool)g_Config.m_KaizoForceBugDDRaceInput;

    //save gametype name
    m_PredControllerKZ.SetGameType(pServerInfo->m_aGameType);
}

void CGameClient::KaizoReset()
{
    m_InstaShield = false;
}

void CGameClient::KaizoPostUpdate()
{
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

void CGameClient::OnKaizoConsoleInit()
{
    
}

//This one is from T-Client
vec2 CGameClient::GetFastInputPos(int ClientId)
{
	float PredIntraTick = Client()->PredIntraGameTick(g_Config.m_ClDummy);
	int PredTick = Client()->PredGameTick(g_Config.m_ClDummy);

	vec2 Pos = mix(m_aClients[ClientId].m_PrevPredicted.m_Pos, m_aClients[ClientId].m_Predicted.m_Pos, PredIntraTick);

	float FastInputIntra = (g_Config.m_KaizoFastInputAmount % 20) / 20.0f;
	int FastInputTicks = g_Config.m_KaizoFastInputAmount / 20;

	float CombinedIntra = PredIntraTick + FastInputIntra;

	float IntraRemainder = 0.0f;
	float FinalIntra = std::modf(CombinedIntra, &IntraRemainder);
	int CarryOverTicks = static_cast<int>(IntraRemainder);

	FastInputTicks += CarryOverTicks;

	int FinalTick = PredTick + FastInputTicks;

	if (FinalTick > 0 &&
		m_aClients[ClientId].m_aPredTick[(FinalTick - 1) % 200] >= Client()->PrevGameTick(g_Config.m_ClDummy) &&
		m_aClients[ClientId].m_aPredTick[FinalTick % 200] <= Client()->PredGameTick(g_Config.m_ClDummy) + FastInputTicks)
	{
		Pos = mix(m_aClients[ClientId].m_aPredPos[(FinalTick - 1) % 200], m_aClients[ClientId].m_aPredPos[FinalTick % 200], FinalIntra);
	}

	return Pos;
}

//TCLIENT
bool CGameClient::GetDummyFastInput(CNetObj_PlayerInput& DummyFastInput, const CNetObj_PlayerInput* pDummyInputData, const CCharacter* pDummyChar, int LocalTee, int DummyTee) const
{
	if (!PredictDummy() || !pDummyChar)
		return false;

	if (g_Config.m_ClDummyHammer)
	{
		DummyFastInput = m_HammerInput;
		return true;
	}

	if (g_Config.m_ClDummyCopyMoves)
	{
		DummyFastInput = m_Controls.m_aFastInput[LocalTee];
		DummyFastInput.m_Fire = m_Controls.m_aFastInput[DummyTee].m_Fire;
		DummyFastInput.m_WantedWeapon = m_Controls.m_aFastInput[DummyTee].m_WantedWeapon;
		DummyFastInput.m_NextWeapon = m_Controls.m_aFastInput[DummyTee].m_NextWeapon;
		DummyFastInput.m_PrevWeapon = m_Controls.m_aFastInput[DummyTee].m_PrevWeapon;
		if (g_Config.m_ClDummyControl)
		{
			const CNetObj_PlayerInput BaseDummyInput = pDummyInputData ? *pDummyInputData : CNetObj_PlayerInput{};
			DummyFastInput.m_Jump = BaseDummyInput.m_Jump;
			DummyFastInput.m_Fire = BaseDummyInput.m_Fire;
			DummyFastInput.m_Hook = BaseDummyInput.m_Hook;
		}
		return true;
	}

	if (g_Config.m_ClDummyControl)
	{
		const CNetObj_PlayerInput BaseDummyInput = pDummyInputData ? *pDummyInputData : CNetObj_PlayerInput{};
		DummyFastInput = BaseDummyInput;
		DummyFastInput.m_Direction = m_Controls.m_aFastInput[DummyTee].m_Direction;
		DummyFastInput.m_PlayerFlags = m_Controls.m_aFastInput[DummyTee].m_PlayerFlags;
		DummyFastInput.m_TargetX = m_Controls.m_aFastInput[DummyTee].m_TargetX;
		DummyFastInput.m_TargetY = m_Controls.m_aFastInput[DummyTee].m_TargetY;
		DummyFastInput.m_WantedWeapon = m_Controls.m_aFastInput[DummyTee].m_WantedWeapon;
		DummyFastInput.m_NextWeapon = m_Controls.m_aFastInput[DummyTee].m_NextWeapon;
		DummyFastInput.m_PrevWeapon = m_Controls.m_aFastInput[DummyTee].m_PrevWeapon;
		return true;
	}

	return false;
}

// E-Client | TClient
void CGameClient::SetConnectInfo(const NETADDR *pAddress)
{
	m_ConnectServerInfo = std::nullopt;
	if(!pAddress)
		return;
	const auto *pEntry = ServerBrowser()->Find(*pAddress);
	if(!pEntry)
		return;
	m_ConnectServerInfo = pEntry->m_Info;
	const CNetObj_GameInfoEx GameInfoEx = {.m_Version = 0};
	m_GameInfo = GetGameInfo(&GameInfoEx, 0, &*m_ConnectServerInfo);
}

//E-Client/T-Client
vec2 CGameClient::GetFreezePos(int ClientId)
{
	vec2 Pos = mix(m_aClients[ClientId].m_PrevPredicted.m_Pos, m_aClients[ClientId].m_Predicted.m_Pos, Client()->PredIntraGameTick(g_Config.m_ClDummy));
	// int64_t Now = time_get();
	CCharacter *pChar = m_PredictedWorld.GetCharacterById(m_Snap.m_LocalClientId);
	CCharacter *pExtraChar = m_ExtraPredictedWorld.GetCharacterById(m_Snap.m_LocalClientId);

	// int64_t Len = clamp(m_aClients[ClientId].m_aSmoothLen[i], (int64_t)1, time_freq());
	// int64_t TimePassed = Now - m_aClients[ClientId].m_aSmoothStart[i];
	float MixAmount = 0.0f;
	int SmoothTick;
	float SmoothIntra;

	int AdjustTicks = 0;
	int DelayTicks = g_Config.m_KaizoUnfreezeLagDelayTicks;
	int FreezeTime = 0;
	if(pExtraChar && pChar)
	{
		AdjustTicks = pChar->m_FreezeAccumulation;
		if(pExtraChar->m_AliveAccumulation > 0)
			AdjustTicks -= pExtraChar->m_AliveAccumulation;

		AdjustTicks = std::max(AdjustTicks, 0);
		FreezeTime = pChar->m_FreezeTime;

		AdjustTicks = std::min(FreezeTime, AdjustTicks);
	}
	if(g_Config.m_KaizoRemoveAnti && pChar && AdjustTicks > 0 && FreezeTime > 0)
		MixAmount = mix(0.0f, 1.0f, 1.0f - AdjustTicks / (float)DelayTicks);
	// else if(AdjustTicks == 0 && ClientId != m_Snap.m_LocalClientId)
	//	MixAmount = 1.f - std::pow(1.f - TimePassed / (float)Len, 1.2f);
	else // our tee when not frozen
		MixAmount = 1.f;

	Client()->GetSmoothFreezeTick(&SmoothTick, &SmoothIntra, MixAmount);

	m_SmoothTick = SmoothTick;
	m_SmoothIntraTick = SmoothIntra;

	float FastInputIntra = (g_Config.m_KaizoFastInputAmount % 20) / 20.0f;
	int FastInputTicks = g_Config.m_KaizoFastInputAmount / 20;

	float CombinedIntra = SmoothIntra + FastInputIntra;

	float IntraRemainder = 0.0f;
	float FinalIntra = std::modf(CombinedIntra, &IntraRemainder);
	int CarryOverTicks = static_cast<int>(IntraRemainder);

	FastInputTicks += CarryOverTicks;

	const bool IsLocal = ClientId == m_Snap.m_LocalClientId || (PredictDummy() && ClientId == m_aLocalIds[!g_Config.m_ClDummy]);
	if(IsLocal && g_Config.m_KaizoFastInput)
	{
		SmoothTick += FastInputTicks;
		SmoothIntra = FinalIntra;
	}
	else if(!IsLocal && g_Config.m_KaizoFastInputOthers && g_Config.m_KaizoFastInput)
	{
		SmoothTick += FastInputTicks;
		SmoothIntra = FinalIntra;
	}

	if(SmoothTick > 0 &&
		m_aClients[ClientId].m_aPredTick[(SmoothTick - 1) % 200] >= Client()->PrevGameTick(g_Config.m_ClDummy) &&
		m_aClients[ClientId].m_aPredTick[SmoothTick % 200] <= Client()->PredGameTick(g_Config.m_ClDummy) + FastInputTicks)
	{
		Pos = mix(m_aClients[ClientId].m_aPredPos[(SmoothTick - 1) % 200], m_aClients[ClientId].m_aPredPos[SmoothTick % 200], SmoothIntra);
	}

	return Pos;
}

//E-Client
void CGameClient::ClientMessage(const char *pString)
{
	m_Chat.AddLine(-3, 0, pString);
}

void CGameClient::OnServerBrowserRefresh()
{
    m_Menus.UpdateOnlinePlayerCache(); //EClient
}
