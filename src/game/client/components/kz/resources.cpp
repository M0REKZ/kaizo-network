/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include <engine/client.h>
#include <engine/shared/config.h>

#include "resources.h"
#include <base/str.h>
#include <base/dbg.h>

int CResources::FileScan(const char *pName, int IsDir, int DirType, void *pUser)
{
	if(IsDir || !str_endswith(pName, ".png"))
		return 0;

	CResources *pSelf = (CResources *)pUser;
	
	size_t ResourceNameSize;
	size_t ResourceNameCount;
	str_utf8_stats(pName, str_length(pName) - str_length(".png") + 1, IO_MAX_PATH_LENGTH, &ResourceNameSize, &ResourceNameCount);
	if(ResourceNameSize >= MAX_RESOURCE_ARRAY_SIZE)
	{
		char aBuf[IO_MAX_PATH_LENGTH + 64];
		str_format(aBuf, sizeof(aBuf), "failed to load resource '%s': name too long", pName);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "resources", aBuf);
		return 0;
	}

	CResource Res;
	str_copy(Res.m_aName, pName, minimum<int>(ResourceNameSize + 1, sizeof(Res.m_aName)));

	char aBuf[IO_MAX_PATH_LENGTH];
	str_format(aBuf, sizeof(aBuf), "resources/%s", pName);
	CImageInfo Info;
	const char *paBuf = aBuf;
	if(!pSelf->Graphics()->LoadPng(Info, paBuf, DirType))
	{
		str_format(aBuf, sizeof(aBuf), "failed to load resource '%s'", pName);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "resources", aBuf);
		return 0;
	}
	if(Info.m_Format != CImageInfo::FORMAT_RGBA)
	{
		str_format(aBuf, sizeof(aBuf), "failed to load resource '%s': must be RGBA format", pName);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "resources", aBuf);
		return 0;
	}

	Res.m_Texture = pSelf->Graphics()->LoadTextureRaw(Info, 0);
	
	{
		str_format(aBuf, sizeof(aBuf), "load resource %s", Res.m_aName);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "resources", aBuf);
	}

	pSelf->m_aResources.emplace_back(Res);

	return 0;
}

void CResources::OnInit()
{
	for (int index = 0; index < MAX_RESOURCES; index++)
	{
		m_pTasks[index].m_pTask = nullptr;
	}

	for (int index = 0; index < MAX_RESOURCES; index++)
	{
		str_copy(ResourceMapping[index], "\0", 64);
	}

	str_copy(m_DownloadBaseUrl, "https://raw.githubusercontent.com/Pointer31/teeworlds-custom-resources/refs/heads/main/");

	m_aResources.clear();
	Storage()->ListDirectory(IStorage::TYPE_ALL, "resources", FileScan, this);
	
	dbg_assert(Find("unknown") >= 0, "data/resources/unknown.png has not been loaded");
}

void CResources::OnUpdate()
{
	for (int index = 0; index < MAX_RESOURCES; index++)
	{
		if (m_pTasks[index].m_pTask != nullptr && m_pTasks[index].m_pTask->Done())
		{
			// reload list of resources
			m_aResources.clear();
			Storage()->ListDirectory(IStorage::TYPE_ALL, "resources", FileScan, this);
			dbg_assert(Find("unknown") >= 0, "data/resources/unknown.png has not been loaded");

			// send I have resource message
			CNetMsg_Cl_IHaveResource MsgIHaveResource;
			MsgIHaveResource.m_Id = m_pTasks[index].m_ResourceId;
			Client()->SendPackMsgActive(&MsgIHaveResource, MSGFLAG_VITAL);

			m_pTasks[index].m_pTask = nullptr;
		}
	}
}

const CResources::CResource *CResources::Get(int ResourceId)
{
	if (ResourceId < 0 || ResourceId >= MAX_RESOURCES)
		return &m_aResources[Find("unknown")];
	else if (ResourceMapping[ResourceId] && Find(ResourceMapping[ResourceId]) >= 0)
		return &m_aResources[Find(ResourceMapping[ResourceId])];
	else
		return &m_aResources[Find("unknown")];
}

int CResources::Find(const char *pName)
{
	for(size_t i = 0; i < m_aResources.size(); i++)
	{
		if(str_comp(m_aResources[i].m_aName, pName) == 0)
			return i;
	}
	return -1;
}

void CResources::OnResourceMessage(CNetMsg_Sv_ImageResourceTWPlus* msg)
{
	char aBuf[IO_MAX_PATH_LENGTH];

	int Id = msg->m_Id;
	const char* pName = msg->m_pName;

	if (!pName[0])
	{
		str_format(aBuf, sizeof(aBuf), "got invalid resource id %i, name='%s'", Id, pName);
		Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "resources", aBuf);
		return;
	}
	if (Id < 0 || Id >= MAX_RESOURCES)
	{
		str_format(aBuf, sizeof(aBuf), "got out of bounds resource id %i, name='%s'", Id, pName);
		Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "resources", aBuf);
		return;
	}
	str_format(aBuf, sizeof(aBuf), "got resource id %i, name='%s'", Id, pName);
	Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "resources", aBuf);

	if (Find(pName) >= 0) 
	{
		CNetMsg_Cl_IHaveResourceTWPlus MsgIHaveResource;
		MsgIHaveResource.m_Id = Id;
		Client()->SendPackMsgActive(&MsgIHaveResource, MSGFLAG_VITAL);
	}
	else
	{
		char downloadUrl [256];
		char saveUrl [256];
		str_format(downloadUrl, sizeof(downloadUrl), "%s%s.png", m_DownloadBaseUrl, pName);
		str_format(saveUrl, sizeof(saveUrl), "resources/%s.png", pName);

		int newTaskIndex = -1;
		for (int index = 0; index < MAX_RESOURCES; index++)
			if (m_pTasks[index].m_pTask == nullptr)
				newTaskIndex = index;

		if (newTaskIndex >= 0)
		{
			m_pTasks[newTaskIndex].m_pTask = HttpGetFile(downloadUrl, Storage(), saveUrl, IStorage::TYPE_SAVE);
			m_pTasks[newTaskIndex].m_pTask->Timeout(CTimeout{10000, 0, 500, 10});
			// Task->SkipByFileTime(false); // Always re-download.
			m_pTasks[newTaskIndex].m_pTask->IpResolve(IPRESOLVE::V4);
			Http()->Run(m_pTasks[newTaskIndex].m_pTask);
			m_pTasks[newTaskIndex].m_ResourceId = Id;
		}
	}

	str_copy(ResourceMapping[Id], pName, sizeof(ResourceMapping[Id]));
	return;
}