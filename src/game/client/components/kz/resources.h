/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_RESOURCES_H
#define GAME_CLIENT_RESOURCES_H
#include <base/vmath.h>
#include <game/client/component.h>
#include <base/str.h>

#include <generated/protocol.h>
#include <engine/shared/http.h>

const int MAX_RESOURCE_ARRAY_SIZE = 64;
const int MAX_RESOURCES = 64;

class CResources : public CComponent
{
public:
	int Sizeof() const override { return sizeof(*this); }
	void OnUpdate() override;
	void OnStateChange(int NewState, int OldState) override;
	struct CResource
	{
		char m_aName[MAX_RESOURCE_ARRAY_SIZE];
		IGraphics::CTextureHandle m_Texture;

		bool operator<(const CResource &Other) { return str_comp_nocase(m_aName, Other.m_aName) < 0; }
	};

	void OnInit() override;
	const CResource *Get(int ResourceId);
	int Find(const char *pName);

	void OnResourceMessage(CNetMsg_Sv_ImageResourceTWPlus* msg);
	void OnResourceDownloadUrlMessage(CNetMsg_Sv_ResourceDownloadBaseUrlTWPlus* msg);
	
private:
	std::vector<CResource> m_aResources;
	char ResourceMapping[MAX_RESOURCES][MAX_RESOURCE_ARRAY_SIZE];
	static int FileScan(const char *pName, int IsDir, int DirType, void *pUser);

	struct CResourceTask
	{
		char m_aName[MAX_RESOURCE_ARRAY_SIZE];
		int m_ResourceId;
		std::shared_ptr<CHttpRequest> m_pTask;
	};
	CResourceTask m_pTasks [MAX_RESOURCES];
	
	char m_DownloadBaseUrl [256];
};

#endif
