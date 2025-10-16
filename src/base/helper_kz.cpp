// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include "system.h"

const char * str_format_time_kz(float Time)
{
    static char stringvar[256];
	float tempvar;
	if(Time >= 60.f) //+KZ minutes
	{
		if(Time / 60.f >= 60) //+KZ hours
		{
			str_format(stringvar, sizeof(stringvar), "%d:%d:%f", (int)(Time / 60) / 60, (int)Time / 60, ((int)Time) % 60 + std::modf(Time, &tempvar));
		}
		str_format(stringvar, sizeof(stringvar), "%d:%f", (int)Time / 60, ((int)Time) % 60 + std::modf(Time, &tempvar));
	}
	else
	{
		str_format(stringvar, sizeof(stringvar), "%f", ((int)Time) % 60 + std::modf(Time, &tempvar));
	}

    return stringvar;
}
