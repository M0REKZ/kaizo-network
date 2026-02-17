// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// SaveKaizo() has code from config.cpp

#include <base/log.h>
#include <base/system.h>
#include "config.h"

bool CConfigManager::SaveKaizo()
{
	if(!m_pStorage || !g_Config.m_ClSaveSettings)
		return true;

	char aConfigFileTmp[IO_MAX_PATH_LENGTH];
	m_ConfigFile = m_pStorage->OpenFile(IStorage::FormatTmpPath(aConfigFileTmp, sizeof(aConfigFileTmp), KAIZO_CONFIG_FILE), IOFLAG_WRITE, IStorage::TYPE_SAVE);

	if(!m_ConfigFile)
	{
		log_error("config", "ERROR: opening %s failed", aConfigFileTmp);
		return false;
	}

	m_Failed = false;

	char aLineBuf[2048];
	for(const SConfigVariable *pVariable : m_vpAllVariables)
	{
        if(pVariable->m_pScriptName && str_find(pVariable->m_pScriptName, "kaizo")) // only save kaizo variables
        {
            if((pVariable->m_Flags & CFGFLAG_SAVE) != 0 && !pVariable->IsDefault())
            {
                pVariable->Serialize(aLineBuf, sizeof(aLineBuf));
                WriteLine(aLineBuf);
            }
        }
	}

	for(const auto &KaizoCallback : m_vKaizoCallbacks)
	{
		KaizoCallback.m_pfnFunc(this, KaizoCallback.m_pUserData);
	}

	for(const char *pCommand : m_vpUnknownCommands)
	{
        if(pCommand && str_find(pCommand, "kaizo")) // only save kaizo commands
        {
		    WriteLine(pCommand);
        }
	}

	if(m_Failed)
	{
		log_error("config", "ERROR: writing to %s failed", aConfigFileTmp);
	}

	if(io_sync(m_ConfigFile) != 0)
	{
		m_Failed = true;
		log_error("config", "ERROR: synchronizing %s failed", aConfigFileTmp);
	}

	if(io_close(m_ConfigFile) != 0)
	{
		m_Failed = true;
		log_error("config", "ERROR: closing %s failed", aConfigFileTmp);
	}

	m_ConfigFile = nullptr;

	if(m_Failed)
	{
		return false;
	}

	if(!m_pStorage->RenameFile(aConfigFileTmp, KAIZO_CONFIG_FILE, IStorage::TYPE_SAVE))
	{
		log_error("config", "ERROR: renaming %s to " KAIZO_CONFIG_FILE " failed", aConfigFileTmp);
		return false;
	}

	log_info("config", "kaizo saved to " KAIZO_CONFIG_FILE);
	return true;
}

void CConfigManager::RegisterKaizoCallback(SAVECALLBACKFUNC pfnFunc, void *pUserData)
{
	m_vKaizoCallbacks.emplace_back(pfnFunc, pUserData);
}
