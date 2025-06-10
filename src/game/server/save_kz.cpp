#include "save.h"

#include <cstdio> // sscanf

#include "entities/character.h"
#include "gamemodes/DDRace.h"
#include "player.h"
#include "teams.h"
#include <engine/shared/config.h>
#include <engine/shared/protocol.h>

void CSaveTee::SaveKZ(CCharacter *pchr, bool AddPenalty)
{
    m_Health = pchr->m_Health;
    m_CustomWeapon = pchr->m_CustomWeapon;
	m_BluePortal = pchr->m_BluePortal;
    m_TuneZoneOverrideKZ = pchr->m_TuneZoneOverrideKZ;

    for(int i = 0; i < KZ_NUM_CUSTOM_WEAPONS - KZ_CUSTOM_WEAPONS_START; i++)
    {
	    m_aCustomWeapons[i].m_Ammo = pchr->m_aCustomWeapons[i].m_Ammo;
        m_aCustomWeapons[i].m_Got = pchr->m_aCustomWeapons[i].m_Got;
    }
}

bool CSaveTee::LoadKZ(CCharacter *pchr, int Team, bool IsSwap)
{
    pchr->m_Health = m_Health;
    pchr->m_CustomWeapon = m_CustomWeapon;
	pchr->m_BluePortal = m_BluePortal;
    pchr->m_TuneZoneOverrideKZ = m_TuneZoneOverrideKZ;

    for(int i = 0; i < KZ_NUM_CUSTOM_WEAPONS - KZ_CUSTOM_WEAPONS_START; i++)
    {
	    pchr->m_aCustomWeapons[i].m_Ammo = m_aCustomWeapons[i].m_Ammo;
        pchr->m_aCustomWeapons[i].m_Got = m_aCustomWeapons[i].m_Got;
    }

	return true;
}
