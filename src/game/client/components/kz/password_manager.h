// Copyright (C) Benjamín Gajardo (also known as +KZ)
#ifndef GAME_CLIENT_COMPONENTS_KZ_PASSWORD_MANAGER_H
#define GAME_CLIENT_COMPONENTS_KZ_PASSWORD_MANAGER_H

#include <game/client/component.h>

class CPasswordManagerKZ : public CComponent
{
    static void ConfigSaveCallback(IConfigManager *pConfigManager, void *pUserData);
    static void ConKaizoAddPassword(IConsole::IResult *pResult, void *pUserData);
    static void ConKaizoRemovePassword(IConsole::IResult *pResult, void *pUserData);
public:
    int Sizeof() const override { return sizeof(*this); }

    CPasswordManagerKZ();

    void OnConsoleInit() override;
    void OnNewSnapshot() override;
    void OnStateChange(int NewState, int OldState) override;
    void OnKaizoSetConnectPassword(const NETADDR *pAddresses, int NumAddresses, char *pOutPassword, int OutPasswordSize) override;
    size_t GetPasswordAmount();
    
    enum class EPasswordType
    {
        CHAT = 0,
        SERVER_CONNECT,
        RCON,
        MAX,
    };
    struct SPasswordKZ
    {
        std::string m_AccountName;
        std::string m_Password;
        std::string m_ServerIp;
        EPasswordType m_Type;
    };
    bool AddPassword(std::string Account, std::string Password, std::string ServerIp, EPasswordType Type);
    bool RemovePassword(std::string Account, std::string ServerIp, EPasswordType Type);
    CPasswordManagerKZ::SPasswordKZ * GetPassword(size_t Index) { return &m_aPasswords[Index]; };

private:
    std::vector<SPasswordKZ> m_aPasswords;

    bool m_NeedToSendSnapshotPass = true;
};

#endif
