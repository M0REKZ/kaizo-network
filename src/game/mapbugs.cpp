#include "mapbugs.h"

#include <base/dbg.h>
#include <base/str.h>

struct CMapDescription
{
	const char *m_pName;
	int m_Size;
	SHA256_DIGEST m_Sha256;

	bool operator==(const CMapDescription &Other) const
	{
		return str_comp(m_pName, Other.m_pName) == 0 &&
		       m_Size == Other.m_Size &&
		       m_Sha256 == Other.m_Sha256;
	}
};

struct CMapBugsInternal
{
	CMapDescription m_Map;
	unsigned int m_BugFlags;
};

static unsigned int BugToFlag(int Bug)
{
	unsigned int Result;
	dbg_assert((unsigned)Bug < 8 * sizeof(Result), "invalid shift");
	Result = (1 << Bug);
	return Result;
}

static unsigned int IsBugFlagSet(int Bug, unsigned int BugFlags)
{
	return (BugFlags & BugToFlag(Bug)) != 0;
}

static SHA256_DIGEST s(const char *pSha256)
{
	SHA256_DIGEST Result;
	dbg_assert(sha256_from_str(&Result, pSha256) == 0, "invalid sha256 in mapbugs");
	return Result;
}

static CMapBugsInternal MAP_BUGS[] =
	{
		{{"Binary", 2022597, s("65b410e197fd2298ec270e89a84b762f6739d1d18089529f8ef6cf2104d3d600")}, BugToFlag(BUG_GRENADE_DOUBLEEXPLOSION)},
	
		//+KZ Kaizo Network bug: duplicated entities
		{{"Campain 2", 40799, s("ee2bdee00619da3a66322b84ca5455e699ad0e246897aeefc417abb1cb46a0e4")}, BugToFlag(BUG_KAIZO_DUPLICATEDENTITIES)},
		{{"GoResTee3", 406264, s("7ac0136f423c1489b1df27b681b25223cce1441d4f2b06d849ae6de8a5693a25")}, BugToFlag(BUG_KAIZO_DUPLICATEDENTITIES)},
		{{"Kaizo Paradise", 123532, s("6a86e72e26c457f88cfa2a7c9e34e2ac06ced44667c635037614c8084a5d3e46")}, BugToFlag(BUG_KAIZO_DUPLICATEDENTITIES)},
		{{"Campain 4", 85476, s("13f43277cb1f79c08be482eed4d3b7c804fccea4acc2e685ffe7513a08d2f068")}, BugToFlag(BUG_KAIZO_DUPLICATEDENTITIES)},
		{{"ddr_storm", 526227, s("283a4004da20783d4c5d1ce35d8285117999651a55c7dd7e837b805f4d0b26e8")}, BugToFlag(BUG_KAIZO_DUPLICATEDENTITIES)},
		{{"ddrace_telegrenade", 449442, s("5d420753ff9b1599bc29a75368488d0de39c41e999429e509227e67e0d0c7a6f")}, BugToFlag(BUG_KAIZO_DUPLICATEDENTITIES)},
		{{"titan_tower", 521025, s("6e01c52be5cd9ec86054755ac147fa98a09dedd5185591397fe3667888601ea1")}, BugToFlag(BUG_KAIZO_DUPLICATEDENTITIES)},
		{{"Campain 1", 619091, s("c59d2caba5f8b16b3919ec0f3def75278b869c8dd661d06b46312df0ad23c86e")}, BugToFlag(BUG_KAIZO_DUPLICATEDENTITIES)},
		{{"Glitched Tower", 947656, s("7258e91dae3244265484d539efcd802ef1a758037ff8a7d95cfc7f0cda6aa806")}, BugToFlag(BUG_KAIZO_DUPLICATEDENTITIES)},
		{{"dragger_hike", 441429, s("a3a1580dd5d352aa7cd5ad76cc63392ca3e3c3a209bc8139a0cacff847df466d")}, BugToFlag(BUG_KAIZO_DUPLICATEDENTITIES)},
	};

CMapBugs CMapBugs::Create(const char *pName, int Size, SHA256_DIGEST Sha256)
{
	CMapDescription Map = {pName, Size, Sha256};
	CMapBugs Result;
	Result.m_Extra = 0;
	for(auto &MapBug : MAP_BUGS)
	{
		if(Map == MapBug.m_Map)
		{
			Result.m_pData = &MapBug;
			return Result;
		}
	}
	Result.m_pData = nullptr;
	return Result;
}

bool CMapBugs::Contains(int Bug) const
{
	CMapBugsInternal *pInternal = (CMapBugsInternal *)m_pData;
	if(!pInternal)
	{
		return IsBugFlagSet(Bug, m_Extra);
	}
	return IsBugFlagSet(Bug, pInternal->m_BugFlags);
}

EMapBugUpdate CMapBugs::Update(const char *pBug)
{
	CMapBugsInternal *pInternal = (CMapBugsInternal *)m_pData;
	int Bug = -1;
	if(false) {}
#define MAPBUG(constname, string) \
	else if(str_comp(pBug, string) == 0) \
	{ \
		Bug = (constname); \
	}
#include "mapbugs_list.h"
#undef MAPBUG
	if(Bug == -1)
	{
		return EMapBugUpdate::NOTFOUND;
	}
	if(pInternal)
	{
		return EMapBugUpdate::OVERRIDDEN;
	}
	m_Extra |= BugToFlag(Bug);
	return EMapBugUpdate::OK;
}

void CMapBugs::Dump() const
{
	CMapBugsInternal *pInternal = (CMapBugsInternal *)m_pData;
	unsigned int Flags;
	if(pInternal)
	{
		Flags = pInternal->m_BugFlags;
	}
	else if(m_Extra)
	{
		Flags = m_Extra;
	}
	else
	{
		return;
	}
	char aBugs[NUM_BUGS + 1] = {0};
	for(int i = 0; i < NUM_BUGS; i++)
	{
		aBugs[i] = IsBugFlagSet(i, Flags) ? 'X' : 'O';
	}

	dbg_msg("mapbugs", "enabling map compatibility mode %s", aBugs);
	if(pInternal)
	{
		char aSha256[SHA256_MAXSTRSIZE];
		sha256_str(pInternal->m_Map.m_Sha256, aSha256, sizeof(aSha256));
		dbg_msg("mapbugs", "map='%s' map_size=%d map_sha256=%s",
			pInternal->m_Map.m_pName,
			pInternal->m_Map.m_Size,
			aSha256);
	}
}
