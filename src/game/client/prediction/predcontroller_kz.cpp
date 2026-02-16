// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include "predcontroller_kz.h"
#include "entities/character.h"

#include <game/client/gameclient.h>

CPredControllerKZ::~CPredControllerKZ()
{
    if(m_pController)
    {
        delete m_pController;
        m_pController = nullptr;
    }
}

void CPredControllerKZ::Init(CGameClient *pGameClient)
{
    m_pGameClient = pGameClient;
}

void CPredControllerKZ::SetGameType(const char *pGameTypeName)
{
    if(str_comp(m_aGameType, pGameTypeName) != 0)
    {
        if(m_pController)
        {
            delete m_pController;
            m_pController = nullptr;
        }

        //create new controller
        if(pGameTypeName[0] == 'i' || pGameTypeName[0] == 'g' || str_find(pGameTypeName, "zCatch"))
        {
            m_pController = new CInstagibController(this);
        }
        else if(str_find(pGameTypeName, "BOMB"))
        {
            m_pController = new CBOMBController(this);
        }
    }

    //save gametype name
    str_copy(m_aGameType, pGameTypeName, sizeof(m_aGameType));
}

CPredControllerKZ::CGameController::CGameController(CPredControllerKZ *pPredController)
{
    m_pPredController = pPredController;
}

CPredControllerKZ::CInstagibController::CInstagibController(CPredControllerKZ *pPredController) :
CGameController(pPredController)
{
}

void CPredControllerKZ::CInstagibController::OnCharacterTakeDamage(CGameWorld *pWorld, CCharacter *pChar, vec2 Force, int Dmg, int From, int Weapon)
{
    if(From == pChar->GetCid())
        return;

    if(From < 0 || From >= MAX_CLIENTS)
        return;

    if(PredController()->GameClient()->IsTeamPlay() && 
        PredController()->GameClient()->m_aClients[pChar->GetCid()].m_Team == PredController()->GameClient()->m_aClients[From].m_Team)
        return;

    bool Death = true;
    if(Weapon == WEAPON_GRENADE)
    {
        if(Dmg < 3)
        {
            Death = false;
        }
    }

    if(Death)
    {
        pChar->m_IsDead = true;
    }
}

CPredControllerKZ::CBOMBController::CBOMBController(CPredControllerKZ *pPredController) :
CGameController(pPredController)
{
}

void CPredControllerKZ::CBOMBController::Tick(CGameWorld &World)
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        if(CCharacter * pChar = World.GetCharacterById(i))
        {
            if(str_comp(PredController()->GameClient()->m_aClients[i].m_aSkinName, "bomb") == 0)
            {
                PredController()->GameClient()->m_aClients[pChar->GetCid()].m_IsBOMBTick = PredController()->GameClient()->Client()->PredGameTick(g_Config.m_ClDummy);
            }
        }
    }
}

void CPredControllerKZ::CBOMBController::OnCharacterTakeDamage(CGameWorld *pWorld, CCharacter *pChar, vec2 Force, int Dmg, int From, int Weapon)
{
    if(pWorld->GetCharacterById(From))
    {
        if(PredController()->GameClient()->m_aClients[From].m_IsBOMBTick >= PredController()->GameClient()->Client()->GameTick(g_Config.m_ClDummy))
        {
            PredController()->GameClient()->m_aClients[pChar->GetCid()].m_IsBOMBTick = PredController()->GameClient()->Client()->PredGameTick(g_Config.m_ClDummy);
            PredController()->GameClient()->m_aClients[From].m_IsBOMBTick = -1;
        }
    }
}
