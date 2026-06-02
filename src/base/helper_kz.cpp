// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include "str.h"
#include <cmath>
#include "helper_kz.h"
#include <cstdio>
#include "dbg.h"
#include <cctype>

#if defined(_MSC_VER)
    #include <intrin.h> // Required for MSVC intrinsics
#endif

const char * str_format_time_kz(double Time)
{
    static char stringvar[256];
	char tempseconds[16];
	double tempvar, floattime;
	floattime = ((int)Time) % 60 + std::modf(Time, &tempvar);
	if(Time >= 60.f) //+KZ minutes
	{
		if(Time / 60.f >= 60) //+KZ hours
		{
			if(floattime < 10)
			{
				get_str_double_kz(tempseconds, sizeof(tempseconds), floattime);
				str_format(stringvar, sizeof(stringvar), "%0d:%0d:0%s", (int)(Time / 60) / 60, (int)Time / 60, tempseconds);
			}
			else
			{
				get_str_double_kz(tempseconds, sizeof(tempseconds), floattime);
				str_format(stringvar, sizeof(stringvar), "%0d:%0d:%s", (int)(Time / 60) / 60, (int)Time / 60, tempseconds);
			}
		}
		else
		{
			if(floattime < 10)
			{
				get_str_double_kz(tempseconds, sizeof(tempseconds), floattime);
				str_format(stringvar, sizeof(stringvar), "%0d:0%s", (int)Time / 60, tempseconds);
			}
			else
			{
				get_str_double_kz(tempseconds, sizeof(tempseconds), floattime);
				str_format(stringvar, sizeof(stringvar), "%0d:%s", (int)Time / 60, tempseconds);
			}
		}
	}
	else
	{
		get_str_double_kz(tempseconds, sizeof(tempseconds), floattime);
		str_format(stringvar, sizeof(stringvar), "%s", tempseconds);
	}

    return stringvar;
}

void get_str_double_kz(char *buffer, int buffer_size, double value)
{
	if(snprintf(buffer, buffer_size, "%.6f", value) < 0)
		dbg_break();

	int len = strlen(buffer);

	//go back in the array until we found no 0 or .
	do
	{
		len--;
	} while((buffer[len] == '0' || buffer[len] == '.') && len > 0);

	if(len <= 0) //value is 0.000000 or similar
	{
		if(buffer_size > 1)
		{
			buffer[1] = '\0'; //only keep 1 zero, remove the dot
		}
		return;
	}

	if(len < buffer_size) //keep the decimal digit but first check for string size
		len++;
	buffer[len] = '\0';
}

int not_builtin_popcount(int v)
{
#if defined(_MSC_VER)
    return __popcnt(v);
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_popcount(v);
#else
    #error "need to add another popcount in helper_kz.cpp"
#endif
}

//From FoxNet
const char *GetParsedArgument(const char *pStr, int Index, bool Rest)
{
	static char aOutBuf[2048]; // persistent buffer for non-Rest results
	const char *pCur = pStr;
	int TokenIndex = 0;

	// Scan and locate the start of the requested token
	while(*pCur)
	{
		// skip leading spaces
		while(*pCur && std::isspace(static_cast<unsigned char>(*pCur)))
			pCur++;
		if(!*pCur)
			break;

		const char *pTokenStart = pCur;

		// If this is the token we want and Rest is requested, return the rest from here.
		if(TokenIndex == Index && Rest)
		{
			return pTokenStart;
		}

		// Parse and advance pCur to the end of this token
		if(*pCur == '"')
		{
			pCur++; // skip opening quote
			while(*pCur)
			{
				if(*pCur == '\\' && (pCur[1] == '\\' || pCur[1] == '"'))
				{
					pCur += 2; // skip escaped char
					continue;
				}
				if(*pCur == '"')
				{
					pCur++; // consume closing quote
					break;
				}
				pCur++;
			}
		}
		else
		{
			while(*pCur && !std::isspace(static_cast<unsigned char>(*pCur)))
				pCur++;
		}

		// If this is the token we want and Rest is false, extract the single token into aOutBuf.
		if(TokenIndex == Index && !Rest)
		{
			// Re-parse the token content to produce the unescaped token into aOutBuf.
			char *pDst = aOutBuf;
			size_t Remaining = sizeof(aOutBuf) - 1;

			const char *pRead = pTokenStart;
			if(*pRead == '"')
			{
				pRead++; // skip opening quote
				while(*pRead && Remaining)
				{
					if(*pRead == '\\' && (pRead[1] == '\\' || pRead[1] == '"'))
					{
						*pDst++ = pRead[1];
						Remaining--;
						pRead += 2;
						continue;
					}
					if(*pRead == '"')
					{
						// end of quoted token
						pRead++;
						break;
					}
					*pDst++ = *pRead++;
					Remaining--;
				}
			}
			else
			{
				while(*pRead && !std::isspace(static_cast<unsigned char>(*pRead)) && Remaining)
				{
					*pDst++ = *pRead++;
					Remaining--;
				}
			}

			*pDst = '\0';
			return aOutBuf[0] ? aOutBuf : nullptr;
		}

		TokenIndex++;
	}

	// Index out of range
	return nullptr;
}
