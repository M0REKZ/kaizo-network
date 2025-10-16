// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include "classes_kz.h"
#include "entities/character.h"

void CKZSubTickKeep::Reset()
{
	m_StartSubTick = -1;
	m_StartedTickKZ = -1;
	m_StartDivisor = 1;

	m_FinishSubTick = -1;
	m_FinishedTickKZ = -1;
	m_FinishDivisor = 1;
}

void CKZSubTickKeep::Keep(CCharacter *pChr)
{
	if(!pChr)
		return;

	m_StartSubTick = pChr->m_StartSubTick;
	m_StartedTickKZ = pChr->m_StartedTickKZ;
	m_StartDivisor = pChr->m_StartDivisor;

	m_FinishSubTick = pChr->m_FinishSubTick;
	m_FinishedTickKZ = pChr->m_FinishedTickKZ;
	m_FinishDivisor = pChr->m_FinishDivisor;
}

void CKZSubTickKeep::Restore(CCharacter *pChr)
{
	if(!pChr)
		return;

	pChr->m_StartSubTick = m_StartSubTick;
	pChr->m_StartedTickKZ = m_StartedTickKZ;
	pChr->m_StartDivisor = m_StartDivisor;

	pChr->m_FinishSubTick = m_FinishSubTick;
	pChr->m_FinishedTickKZ = m_FinishedTickKZ;
	pChr->m_FinishDivisor = m_FinishDivisor;
}

void CKZSubTickKeep::KeepStart(CCharacter *pChr)
{
	if(!pChr)
		return;

	m_StartSubTick = pChr->m_StartSubTick;
	m_StartedTickKZ = pChr->m_StartedTickKZ;
	m_StartDivisor = pChr->m_StartDivisor;
}

void CKZSubTickKeep::RestoreStart(CCharacter *pChr)
{
	if(!pChr)
		return;

	pChr->m_StartSubTick = m_StartSubTick;
	pChr->m_StartedTickKZ = m_StartedTickKZ;
	pChr->m_StartDivisor = m_StartDivisor;
}

void CKZSubTickKeep::KeepFinish(CCharacter *pChr)
{
	if(!pChr)
		return;

	m_FinishSubTick = pChr->m_FinishSubTick;
	m_FinishedTickKZ = pChr->m_FinishedTickKZ;
	m_FinishDivisor = pChr->m_FinishDivisor;
}

void CKZSubTickKeep::RestoreFinish(CCharacter *pChr)
{
	if(!pChr)
		return;

	pChr->m_FinishSubTick = m_FinishSubTick;
	pChr->m_FinishedTickKZ = m_FinishedTickKZ;
	pChr->m_FinishDivisor = m_FinishDivisor;
}

CKZSubTickKeep::CKZSubTickKeep()
{
	Reset();
}
