// Copyright (C) Benjamín Gajardo (also known as +KZ)

#ifndef GAME_SERVER_CLASSES_KZ
#define GAME_SERVER_CLASSES_KZ

#include <game/teamscore.h>

class CCharacter;

class CKZSubTickKeep
{
public:
	int m_StartSubTick;
	int m_StartedTickKZ;
	int m_StartDivisor;

	int m_FinishSubTick;
	int m_FinishedTickKZ;
	int m_FinishDivisor;

	void Reset();
	void Keep(CCharacter *pChr);
	void Restore(CCharacter *pChr);
    
    void KeepStart(CCharacter *pChr);
    void RestoreStart(CCharacter *pChr);

    void KeepFinish(CCharacter *pChr);
    void RestoreFinish(CCharacter *pChr);
	CKZSubTickKeep();

};

#endif
