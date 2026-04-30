// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include "linereader.h"
#include <base/dbg.h>

const char *CLineReader::GetInverted()
{
    dbg_assert(m_pBuffer != nullptr, "Line reader not initialized");

    dbg_assert(m_GetStateKZ == GETKZ_NONE || m_GetStateKZ == GETKZ_INVERTED, "Cant alternate between Get() and GetInverted()");

	if(m_ReadLastLine)
	{
		return nullptr;
	}

    if(m_GetStateKZ != GETKZ_INVERTED)
    {
        //get to the end
        while(m_pBuffer[m_BufferPos])
        {
            m_BufferPos++;
        }
        //set buffer on the end of the next line (the one above this one)
        while(!m_ReadLastLine && (m_pBuffer[m_BufferPos] == '\n' || m_pBuffer[m_BufferPos] == '\0' || m_pBuffer[m_BufferPos] == '\r'))
        {
            m_pBuffer[m_BufferPos] = '\0';
            m_BufferPos--;
            if(m_BufferPos == 0)
            {
                m_ReadLastLine = true;
                break;
            }
        }

        m_GetStateKZ = GETKZ_INVERTED;
    }

    const char *pLine = nullptr;

    //get to the start of the line
    while(m_pBuffer[m_BufferPos] != '\n' && m_pBuffer[m_BufferPos] != '\0' && m_pBuffer[m_BufferPos] != '\r')
    {
        pLine = &m_pBuffer[m_BufferPos];
        m_BufferPos--;
        if(m_BufferPos == 0)
        {
            pLine = &m_pBuffer[0];
            m_ReadLastLine = true;
            break;
        }
    }

    //set buffer on the end of the next line (the one above this one)
    while(!m_ReadLastLine && (m_pBuffer[m_BufferPos] == '\n' || m_pBuffer[m_BufferPos] == '\0' || m_pBuffer[m_BufferPos] == '\r'))
    {
        m_pBuffer[m_BufferPos] = '\0';
        m_BufferPos--;
        if(m_BufferPos == 0)
        {
            m_ReadLastLine = true;
            break;
        }
    }

	return pLine;
}
