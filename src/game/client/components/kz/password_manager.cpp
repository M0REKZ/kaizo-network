// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include <engine/shared/config.h>

#include "password_manager.h"
#include <base/log.h>
#include <base/str.h>
#include <game/client/gameclient.h>

void CPasswordManagerKZ::ConfigSaveCallback(IConfigManager *pConfigManager, void *pUserData)
{
    CPasswordManagerKZ * pThis = (CPasswordManagerKZ *)pUserData;
    
    for(SPasswordKZ &Password : pThis->m_aPasswords)
    {
        std::string TempBuf;

        TempBuf.append("kaizo_password_add ");
        if(Password.m_AccountName.c_str()[0])
            TempBuf.append(Password.m_AccountName);
        else
            TempBuf.append("\"\"");
        TempBuf.append(" ");
        if(Password.m_Password.c_str()[0])
            TempBuf.append(Password.m_Password);
        else
            TempBuf.append("\"\"");
        TempBuf.append(" ");
        if(Password.m_ServerIp.c_str()[0])
            TempBuf.append(Password.m_ServerIp);
        else
            TempBuf.append("\"\"");
        TempBuf.append(" ");
        TempBuf.append(
            Password.m_Type == CPasswordManagerKZ::EPasswordType::CHAT ? "chat" : (
                    Password.m_Type == CPasswordManagerKZ::EPasswordType::RCON ? "rcon" : (
                        "server_connect"
                    )
                )
        );

        pConfigManager->WriteLine(TempBuf.c_str());
    }
}

void CPasswordManagerKZ::ConKaizoAddPassword(IConsole::IResult *pResult, void *pUserData)
{
    CPasswordManagerKZ * pThis = (CPasswordManagerKZ *)pUserData;

    const char * pType = pResult->GetString(3);
    EPasswordType OutType;

    if(!str_comp(pType, "chat"))
    {
        OutType = EPasswordType::CHAT;
    }
    else if(!str_comp(pType, "rcon"))
    {
        OutType = EPasswordType::RCON;
    }
    else if(!str_comp(pType, "server_connect"))
    {
        OutType = EPasswordType::SERVER_CONNECT;
    }
    else
    {
        log_error("kaizo_password_add", "invalid type");
        return;
    }

    pThis->AddPassword(pResult->GetString(0), pResult->GetString(1), pResult->GetString(2), OutType);
}

void CPasswordManagerKZ::ConKaizoRemovePassword(IConsole::IResult *pResult, void *pUserData)
{
    CPasswordManagerKZ * pThis = (CPasswordManagerKZ *)pUserData;
    
    const char * pType = pResult->GetString(2);
    EPasswordType OutType;

    if(!str_comp(pType, "chat"))
    {
        OutType = EPasswordType::CHAT;
    }
    else if(!str_comp(pType, "rcon"))
    {
        OutType = EPasswordType::RCON;
    }
    else if(!str_comp(pType, "server_connect"))
    {
        OutType = EPasswordType::SERVER_CONNECT;
    }
    else
    {
        log_error("kaizo_password_remove", "invalid type");
        return;
    }

    pThis->RemovePassword(pResult->GetString(0), pResult->GetString(1), OutType);
}

CPasswordManagerKZ::CPasswordManagerKZ()
{
    m_aPasswords.clear();
}

void CPasswordManagerKZ::OnConsoleInit()
{
    IConfigManager *pConfigManager = Kernel()->RequestInterface<IConfigManager>();
	if(pConfigManager)
		pConfigManager->RegisterKaizoCallback(ConfigSaveCallback, this);

    Console()->Register("kaizo_password_add", "s[account name] s[account password] s[server ip with port] s[chat|rcon|server_connect]", CFGFLAG_CLIENT, ConKaizoAddPassword, this, "Register a password in Kaizo Client password manager");
    Console()->Register("kaizo_password_remove", "s[account name] s[server ip with port] s[chat|rcon|server_connect]", CFGFLAG_CLIENT, ConKaizoRemovePassword, this, "Remove a password from Kaizo Client password manager");
}

void CPasswordManagerKZ::OnNewSnapshot()
{
    //only ingame
    if(Client()->State() != IClient::STATE_ONLINE)
        return;

    CServerInfo CurrentServerInfo;
	Client()->GetServerInfo(&CurrentServerInfo);

    //on first snapshot there is still no address here????
    if(CurrentServerInfo.m_aAddress[0] == '\0')
        return;

    //things does not seem to work correctly if we are TOO fast... check OnStateChange()
    if(m_PassSnapshotDelay == 0)
    {
        m_PassSnapshotDelay = -1;
    }
    else if(m_PassSnapshotDelay > 0)
    {
        m_PassSnapshotDelay--;
        return;
    }
    else
    {
        return;
    }

    for(auto &SavedPassword : m_aPasswords)
    {
        if(!SavedPassword.m_ServerIp.compare(CurrentServerInfo.m_aAddress))
        {
            if(SavedPassword.m_Type == EPasswordType::CHAT)
            {
                std::string Message = "/login ";
                Message.append(SavedPassword.m_AccountName);
                Message.append(" ");
                Message.append(SavedPassword.m_Password);
                GameClient()->m_Chat.SendChat(0, Message.c_str());
                return;
            }
            else if(SavedPassword.m_Type == EPasswordType::RCON && !Client()->RconAuthed())
            {
                Client()->RconAuth(SavedPassword.m_AccountName.c_str(), SavedPassword.m_Password.c_str(), CClient::CONN_MAIN);
                return;
            }
        }
    }
}

void CPasswordManagerKZ::OnStateChange(int NewState, int OldState)
{
    if(NewState != CClient::STATE_ONLINE)
    {
        m_PassSnapshotDelay = (int)(SERVER_TICK_SPEED * 1.5f);
    }
}

void CPasswordManagerKZ::OnKaizoSetConnectPassword(const NETADDR *pAddresses, int NumAddresses, char *pOutPassword, int OutPasswordSize)
{
    for(int i = 0; i < NumAddresses; i++)
    {
        for(auto &SavedPassword : m_aPasswords)
        {
            NETADDR savedaddr;
            if(net_addr_from_str(&savedaddr, SavedPassword.m_ServerIp.c_str()))
                return; //fail
            if(savedaddr == pAddresses[i])
            {
                if(SavedPassword.m_Type == EPasswordType::SERVER_CONNECT)
                {
                    str_copy(pOutPassword, SavedPassword.m_Password.c_str(), OutPasswordSize);
                    return;
                }
            }
        }
    }
}

size_t CPasswordManagerKZ::GetPasswordAmount()
{
	return m_aPasswords.size();
}

bool CPasswordManagerKZ::AddPassword(std::string Account, std::string Password, std::string ServerIp, EPasswordType Type)
{
    for(size_t i = 0; i < m_aPasswords.size(); i++)
    {
        if(
            !Account.compare(m_aPasswords[i].m_AccountName) &&
            !Password.compare(m_aPasswords[i].m_Password) &&
            !ServerIp.compare(m_aPasswords[i].m_ServerIp) &&
            Type == m_aPasswords[i].m_Type
        )
        {
            //password already exists
            log_error("kaizo password manager", "password already exists");
            return false;
        }
    }

    m_aPasswords.push_back({Account, Password, ServerIp, Type});
    log_info("kaizo password manager", "password saved");
    return true;
}

bool CPasswordManagerKZ::RemovePassword(std::string Account, std::string ServerIp, EPasswordType Type)
{
    for(size_t i = 0; i < m_aPasswords.size(); i++)
    {
        if(
            !Account.compare(m_aPasswords[i].m_AccountName) &&
            !ServerIp.compare(m_aPasswords[i].m_ServerIp) &&
            Type == m_aPasswords[i].m_Type
        )
        {
            m_aPasswords.erase(m_aPasswords.begin() + i);
            log_info("kaizo password manager", "password deleted");
            return true;
        }
    }

    log_error("kaizo password manager", "password does not exist");
    return false;
}
