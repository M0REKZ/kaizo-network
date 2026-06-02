// Copyright (C) Benjamín Gajardo (also known as +KZ)

const char * str_format_time_kz(double Time);
void get_str_double_kz(char * buffer, int buffer_size, double value);
int not_builtin_popcount(int v);

//From FoxNet, note that it may return a pointer to a static char array or a pointer to the string passed
const char *GetParsedArgument(const char *pStr, int Index, bool Rest);
