// Copyright (C) Benjamín Gajardo (also known as +KZ)

// This is intended to predict specific gamemodes logic, like gamecontroller.cpp but client-side

#ifndef GAME_CLIENT_PREDICTION_PREDCONTROLLER_KZ
#define GAME_CLIENT_PREDICTION_PREDCONTROLLER_KZ

#include <base/vmath.h>

class CGameClient;
class CGameWorld;
class CCharacter;

class CPredControllerKZ
{
    CGameClient * m_pGameClient = nullptr;

    char m_aGameType[32] = {'\0'};

    public:

    ~CPredControllerKZ();

    void Init(CGameClient * pGameClient);
    void SetGameType(const char * pGameTypeName);

    CGameClient * GameClient() { return m_pGameClient; }

    //gametypes
    class CGameController
    {
        CPredControllerKZ * m_pPredController = nullptr;
        public:
        CGameController(CPredControllerKZ * pPredController);
        virtual ~CGameController() {}

        CPredControllerKZ * PredController() { return m_pPredController; }

        virtual void Tick(CGameWorld & World) {};
        virtual void OnCharacterTakeDamage(CGameWorld * pWorld, CCharacter * pChar, vec2 Force, int Dmg, int From, int Weapon) {};
    };

    CGameController * m_pController = nullptr;

    class CInstagibController : public CGameController
    {
        public:
        CInstagibController(CPredControllerKZ * pPredController);
        virtual void OnCharacterTakeDamage(CGameWorld * pWorld, CCharacter * pChar, vec2 Force, int Dmg, int From, int Weapon) override;
    };

    class CBOMBController : public CGameController
    {
        public:
        CBOMBController(CPredControllerKZ * pPredController);
        virtual void Tick(CGameWorld & World) override;
        virtual void OnCharacterTakeDamage(CGameWorld * pWorld, CCharacter * pChar, vec2 Force, int Dmg, int From, int Weapon) override;
    };
};

#endif
