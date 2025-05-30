#include "layer_kz.h"

#include <game/editor/editor.h>

CLayerKZGame::CLayerKZGame(CEditor *pEditor, int w, int h) :
	CLayerTiles(pEditor, w, h)
{
	str_copy(m_aName, KZ_GAME_LAYER_NAME);
	m_HasKZGame = true;
	m_HasKZFront = false;

	m_pKZTile = new CKZTile[w * h];
	mem_zero(m_pKZTile, (size_t)w * h * sizeof(CKZTile));
}

CLayerKZGame::CLayerKZGame(const CLayerKZGame &Other) :
	CLayerTiles(Other)
{
	str_copy(m_aName, "KZ copy");
	m_HasKZGame = true;
	m_HasKZFront = false;

	m_pKZTile = new CKZTile[m_Width * m_Height];
	mem_copy(m_pKZTile, Other.m_pKZTile, (size_t)m_Width * m_Height * sizeof(CKZTile));
}

CLayerKZGame::~CLayerKZGame()
{
	delete[] m_pKZTile;
}

void CLayerKZGame::Resize(int NewW, int NewH)
{
	// resize switch data
	CKZTile *pNewSwitchData = new CKZTile[NewW * NewH];
	mem_zero(pNewSwitchData, (size_t)NewW * NewH * sizeof(CKZTile));

	// copy old data
	for(int y = 0; y < minimum(NewH, m_Height); y++)
		mem_copy(&pNewSwitchData[y * NewW], &m_pKZTile[y * m_Width], minimum(m_Width, NewW) * sizeof(CKZTile));

	// replace old
	delete[] m_pKZTile;
	m_pKZTile = pNewSwitchData;

	// resize tile data
	CLayerTiles::Resize(NewW, NewH);

	// resize gamelayer too
	if(m_pEditor->m_Map.m_pGameLayer->m_Width != NewW || m_pEditor->m_Map.m_pGameLayer->m_Height != NewH)
		m_pEditor->m_Map.m_pGameLayer->Resize(NewW, NewH);
}

void CLayerKZGame::Shift(int Direction)
{
	CLayerTiles::Shift(Direction);
	ShiftImpl(m_pKZTile, Direction, m_pEditor->m_ShiftBy);
}

bool CLayerKZGame::IsEmpty(const std::shared_ptr<CLayerTiles> &pLayer)
{
	for(int y = 0; y < pLayer->m_Height; y++)
		for(int x = 0; x < pLayer->m_Width; x++)
			if(m_pEditor->IsAllowPlaceUnusedTiles() || IsValidSwitchTile(pLayer->GetTile(x, y).m_Index))
				return false;

	return true;
}

void CLayerKZGame::BrushDraw(std::shared_ptr<CLayer> pBrush, vec2 WorldPos)
{
	if(m_Readonly)
		return;

	std::shared_ptr<CLayerKZGame> pSwitchLayer = std::static_pointer_cast<CLayerKZGame>(pBrush);
	int sx = ConvertX(WorldPos.x);
	int sy = ConvertY(WorldPos.y);
	if(str_comp(pSwitchLayer->m_aFileName, m_pEditor->m_aFileName))
	{
		m_pEditor->m_KZGameNumber = pSwitchLayer->m_Number;
		m_pEditor->m_KZGameValue1 = pSwitchLayer->m_Value1;
		m_pEditor->m_KZGameValue2 = pSwitchLayer->m_Value2;
		m_pEditor->m_KZGameValue3 = pSwitchLayer->m_Value3;
	}

	bool Destructive = m_pEditor->m_BrushDrawDestructive || IsEmpty(pSwitchLayer);

	for(int y = 0; y < pSwitchLayer->m_Height; y++)
		for(int x = 0; x < pSwitchLayer->m_Width; x++)
		{
			int fx = x + sx;
			int fy = y + sy;

			if(fx < 0 || fx >= m_Width || fy < 0 || fy >= m_Height)
				continue;

			if(!Destructive && GetTile(fx, fy).m_Index)
				continue;

			const int SrcIndex = y * pSwitchLayer->m_Width + x;
			const int TgtIndex = fy * m_Width + fx;

			SKZTileStateChange::SData Previous{
				m_pKZTile[TgtIndex].m_Index,
				m_pKZTile[TgtIndex].m_Flags,
				m_pKZTile[TgtIndex].m_Number,
				m_pKZTile[TgtIndex].m_Value1,
				m_pKZTile[TgtIndex].m_Value2,
				m_pKZTile[TgtIndex].m_Value3,
				m_pTiles[TgtIndex].m_Index};

			if(pSwitchLayer->m_pTiles[SrcIndex].m_Index != TILE_AIR)
			{
				if(m_pEditor->m_KZGameNumber != pSwitchLayer->m_Number || m_pEditor->m_KZGameValue1 != pSwitchLayer->m_Value1 || m_pEditor->m_KZGameValue2 != pSwitchLayer->m_Value2 || m_pEditor->m_KZGameValue3 != pSwitchLayer->m_Value3)
				{
					m_pKZTile[TgtIndex].m_Number = m_pEditor->m_KZGameNumber;
					m_pKZTile[TgtIndex].m_Value1 = m_pEditor->m_KZGameValue1;
					m_pKZTile[TgtIndex].m_Value2 = m_pEditor->m_KZGameValue2;
					m_pKZTile[TgtIndex].m_Value3 = m_pEditor->m_KZGameValue3;
				}
				else if(pSwitchLayer->m_pKZTile[SrcIndex].m_Number)
				{
					m_pKZTile[TgtIndex].m_Number = pSwitchLayer->m_pKZTile[SrcIndex].m_Number;
					m_pKZTile[TgtIndex].m_Value1 = pSwitchLayer->m_pKZTile[SrcIndex].m_Value1;
					m_pKZTile[TgtIndex].m_Value2 = pSwitchLayer->m_pKZTile[SrcIndex].m_Value2;
					m_pKZTile[TgtIndex].m_Value3 = pSwitchLayer->m_pKZTile[SrcIndex].m_Value3;
				}
				else
				{
					m_pKZTile[TgtIndex].m_Number = m_pEditor->m_KZGameNumber;
					m_pKZTile[TgtIndex].m_Value1 = m_pEditor->m_KZGameValue1;
					m_pKZTile[TgtIndex].m_Value2 = m_pEditor->m_KZGameValue2;
					m_pKZTile[TgtIndex].m_Value3 = m_pEditor->m_KZGameValue3;
				}

				m_pKZTile[TgtIndex].m_Index = pSwitchLayer->m_pTiles[SrcIndex].m_Index;
				m_pKZTile[TgtIndex].m_Flags = pSwitchLayer->m_pTiles[SrcIndex].m_Flags;
				m_pTiles[TgtIndex].m_Index = pSwitchLayer->m_pTiles[SrcIndex].m_Index;
				m_pTiles[TgtIndex].m_Flags = pSwitchLayer->m_pTiles[SrcIndex].m_Flags;

				if(!IsSwitchTileFlagsUsed(pSwitchLayer->m_pTiles[SrcIndex].m_Index))
				{
					m_pKZTile[TgtIndex].m_Flags = 0;
				}
			}
			else
			{
				m_pKZTile[TgtIndex].m_Index = 0;
				m_pKZTile[TgtIndex].m_Number = 0;
				m_pKZTile[TgtIndex].m_Value1 = 0;
				m_pKZTile[TgtIndex].m_Value2 = 0;
				m_pKZTile[TgtIndex].m_Value3 = 0;
				m_pTiles[TgtIndex].m_Index = 0;
			}

			SKZTileStateChange::SData Current{
				m_pKZTile[TgtIndex].m_Index,
				m_pKZTile[TgtIndex].m_Flags,
				m_pKZTile[TgtIndex].m_Number,
				m_pKZTile[TgtIndex].m_Value1,
				m_pKZTile[TgtIndex].m_Value2,
				m_pKZTile[TgtIndex].m_Value3,
				m_pTiles[TgtIndex].m_Index};

			RecordStateChange(fx, fy, Previous, Current);
		}
	FlagModified(sx, sy, pSwitchLayer->m_Width, pSwitchLayer->m_Height);
}

void CLayerKZGame::RecordStateChange(int x, int y, SKZTileStateChange::SData Previous, SKZTileStateChange::SData Current)
{
	if(!m_History[y][x].m_Changed)
		m_History[y][x] = SKZTileStateChange{true, Previous, Current};
	else
		m_History[y][x].m_Current = Current;
}

void CLayerKZGame::BrushFlipX()
{
	CLayerTiles::BrushFlipX();
	BrushFlipXImpl(m_pKZTile);
}

void CLayerKZGame::BrushFlipY()
{
	CLayerTiles::BrushFlipY();
	BrushFlipYImpl(m_pKZTile);
}

void CLayerKZGame::BrushRotate(float Amount)
{
	int Rotation = (round_to_int(360.0f * Amount / (pi * 2)) / 90) % 4; // 0=0°, 1=90°, 2=180°, 3=270°
	if(Rotation < 0)
		Rotation += 4;

	if(Rotation == 1 || Rotation == 3)
	{
		// 90° rotation
		CKZTile *pTempData1 = new CKZTile[m_Width * m_Height];
		CTile *pTempData2 = new CTile[m_Width * m_Height];
		mem_copy(pTempData1, m_pKZTile, (size_t)m_Width * m_Height * sizeof(CKZTile));
		mem_copy(pTempData2, m_pTiles, (size_t)m_Width * m_Height * sizeof(CTile));
		CKZTile *pDst1 = m_pKZTile;
		CTile *pDst2 = m_pTiles;
		for(int x = 0; x < m_Width; ++x)
			for(int y = m_Height - 1; y >= 0; --y, ++pDst1, ++pDst2)
			{
				*pDst1 = pTempData1[y * m_Width + x];
				*pDst2 = pTempData2[y * m_Width + x];
				
				if(pDst2->m_Flags & TILEFLAG_ROTATE)
					pDst2->m_Flags ^= (TILEFLAG_YFLIP | TILEFLAG_XFLIP);
				pDst2->m_Flags ^= TILEFLAG_ROTATE;
			}

		std::swap(m_Width, m_Height);
		delete[] pTempData1;
		delete[] pTempData2;
	}

	if(Rotation == 2 || Rotation == 3)
	{
		BrushFlipX();
		BrushFlipY();
	}
}

void CLayerKZGame::FillSelection(bool Empty, std::shared_ptr<CLayer> pBrush, CUIRect Rect)
{
	if(m_Readonly || (!Empty && pBrush->m_Type != LAYERTYPE_TILES))
		return;

	Snap(&Rect); // corrects Rect; no need of <=

	Snap(&Rect);

	int sx = ConvertX(Rect.x);
	int sy = ConvertY(Rect.y);
	int w = ConvertX(Rect.w);
	int h = ConvertY(Rect.h);

	std::shared_ptr<CLayerKZGame> pLt = std::static_pointer_cast<CLayerKZGame>(pBrush);

	bool Destructive = m_pEditor->m_BrushDrawDestructive || Empty || IsEmpty(pLt);

	for(int y = 0; y < h; y++)
	{
		for(int x = 0; x < w; x++)
		{
			int fx = x + sx;
			int fy = y + sy;

			if(fx < 0 || fx >= m_Width || fy < 0 || fy >= m_Height)
				continue;

			if(!Destructive && GetTile(fx, fy).m_Index)
				continue;

			const int SrcIndex = Empty ? 0 : (y * pLt->m_Width + x % pLt->m_Width) % (pLt->m_Width * pLt->m_Height);
			const int TgtIndex = fy * m_Width + fx;

			SKZTileStateChange::SData Previous{
				m_pKZTile[TgtIndex].m_Index,
				m_pKZTile[TgtIndex].m_Flags,
				m_pKZTile[TgtIndex].m_Number,
				m_pKZTile[TgtIndex].m_Value1,
				m_pKZTile[TgtIndex].m_Value2,
				m_pKZTile[TgtIndex].m_Value3,
				m_pTiles[TgtIndex].m_Index};

			if(Empty)
			{
				m_pKZTile[TgtIndex].m_Index = 0;
				m_pKZTile[TgtIndex].m_Number = 0;
				m_pKZTile[TgtIndex].m_Value1 = 0;
				m_pKZTile[TgtIndex].m_Value2 = 0;
				m_pKZTile[TgtIndex].m_Value3 = 0;
				m_pTiles[TgtIndex].m_Index = 0;
			}
			else
			{
				m_pTiles[TgtIndex] = pLt->m_pTiles[SrcIndex];
				m_pKZTile[TgtIndex].m_Index = m_pTiles[TgtIndex].m_Index;
				if(pLt->m_HasKZGame && m_pTiles[TgtIndex].m_Index > 0)
				{
					
						m_pKZTile[TgtIndex].m_Number = pLt->m_pKZTile[SrcIndex].m_Number;
						m_pKZTile[TgtIndex].m_Index = pLt->m_pKZTile[SrcIndex].m_Index;
						m_pKZTile[TgtIndex].m_Flags = pLt->m_pKZTile[SrcIndex].m_Flags;
						m_pKZTile[TgtIndex].m_Value1 = pLt->m_pKZTile[SrcIndex].m_Value1;
						m_pKZTile[TgtIndex].m_Value2 = pLt->m_pKZTile[SrcIndex].m_Value2;
						m_pKZTile[TgtIndex].m_Value3 = pLt->m_pKZTile[SrcIndex].m_Value3;
				}
				else
				{
					m_pKZTile[TgtIndex].m_Index = 0;
					m_pKZTile[TgtIndex].m_Number = 0;
					m_pKZTile[TgtIndex].m_Value1 = 0;
					m_pKZTile[TgtIndex].m_Value2 = 0;
					m_pKZTile[TgtIndex].m_Value3 = 0;
					m_pTiles[TgtIndex].m_Index = 0;
				}
			}

			SKZTileStateChange::SData Current{
				m_pKZTile[TgtIndex].m_Index,
				m_pKZTile[TgtIndex].m_Flags,
				m_pKZTile[TgtIndex].m_Number,
				m_pKZTile[TgtIndex].m_Value1,
				m_pKZTile[TgtIndex].m_Value2,
				m_pKZTile[TgtIndex].m_Value3,
				m_pTiles[TgtIndex].m_Index};

			RecordStateChange(fx, fy, Previous, Current);
		}
	}
	FlagModified(sx, sy, w, h);
}

std::shared_ptr<CLayer> CLayerKZGame::Duplicate() const
{
	return std::make_shared<CLayerKZGame>(*this);
}

const char *CLayerKZGame::TypeName() const
{
	return "switch";
}
