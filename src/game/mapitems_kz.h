// (c) +KZ

#define KZ_GAME_LAYER_NAME (char[]){'+','K', 'Z', 'G', 'a', 'm', 'e', (char)27, '\0'}
#define KZ_FRONT_LAYER_NAME (char[]){'+','K', 'Z', 'F', 'r', 'o', 'n', 't', (char)27, '\0'}

enum
{
	KZ_TILE_SWITCHABLE = 1,
	KZ_TILE_SOLID_STOPPER,
};

class CKZTile
{
public:
	unsigned char m_Index;
	unsigned char m_Flags;
	unsigned char m_Number;
	long long int m_Value1;
    long long int m_Value2;
    long long int m_Value3;
};