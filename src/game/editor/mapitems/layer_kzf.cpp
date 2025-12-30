// Modified by +KZ

#include "layer_kz.h"

#include <game/editor/editor.h>

CLayerKZFront::CLayerKZFront(CEditorMap *pMap, int w, int h) :
	CLayerKZGame(pMap, w, h)
{
	str_copy(m_aName, KZ_FRONT_LAYER_NAME);
    m_HasKZGame = false;
    m_HasKZFront = true;

	m_pKZTile = new CKZTile[w * h];
	mem_zero(m_pKZTile, (size_t)w * h * sizeof(CKZTile));
}

CLayerKZFront::CLayerKZFront(const CLayerKZFront &Other) :
	CLayerKZGame(Other)
{
	str_copy(m_aName, "KZF copy");
    m_HasKZGame = false;
    m_HasKZFront = true;

	m_pKZTile = new CKZTile[m_Width * m_Height];
	mem_copy(m_pKZTile, Other.m_pKZTile, (size_t)m_Width * m_Height * sizeof(CKZTile));
}

void CLayerKZFront::BrushDraw(CLayer *pBrush, vec2 WorldPos)
{
	if(m_Readonly)
		return;

	CLayerKZGame *pSwitchLayer = static_cast<CLayerKZGame *>(pBrush);
	int sx = ConvertX(WorldPos.x);
	int sy = ConvertY(WorldPos.y);
	if(str_comp(pSwitchLayer->m_aFilename, pSwitchLayer->Map()->m_aFilename))
	{
		Editor()->m_KZFrontNumber = pSwitchLayer->m_Number;
		Editor()->m_KZFrontValue1 = pSwitchLayer->m_Value1;
		Editor()->m_KZFrontValue2 = pSwitchLayer->m_Value2;
		Editor()->m_KZFrontValue3 = pSwitchLayer->m_Value3;
	}

	bool Destructive = Editor()->m_BrushDrawDestructive || pSwitchLayer->IsEmpty();

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
				if(Editor()->m_KZFrontNumber != pSwitchLayer->m_Number || Editor()->m_KZFrontValue1 != pSwitchLayer->m_Value1 || Editor()->m_KZFrontValue2 != pSwitchLayer->m_Value2 || Editor()->m_KZFrontValue3 != pSwitchLayer->m_Value3)
				{
					m_pKZTile[TgtIndex].m_Number = Editor()->m_KZFrontNumber;
					m_pKZTile[TgtIndex].m_Value1 = Editor()->m_KZFrontValue1;
					m_pKZTile[TgtIndex].m_Value2 = Editor()->m_KZFrontValue2;
					m_pKZTile[TgtIndex].m_Value3 = Editor()->m_KZFrontValue3;
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
					m_pKZTile[TgtIndex].m_Number = Editor()->m_KZFrontNumber;
					m_pKZTile[TgtIndex].m_Value1 = Editor()->m_KZFrontValue1;
					m_pKZTile[TgtIndex].m_Value2 = Editor()->m_KZFrontValue2;
					m_pKZTile[TgtIndex].m_Value3 = Editor()->m_KZFrontValue3;
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

void CLayerKZFront::FillSelection(bool Empty, CLayer *pBrush, CUIRect Rect)
{
	if(m_Readonly || (!Empty && pBrush->m_Type != LAYERTYPE_TILES))
		return;

	Snap(&Rect); // corrects Rect; no need of <=

	Snap(&Rect);

	int sx = ConvertX(Rect.x);
	int sy = ConvertY(Rect.y);
	int w = ConvertX(Rect.w);
	int h = ConvertY(Rect.h);

	CLayerKZGame *pLt = static_cast<CLayerKZGame *>(pBrush);

	bool Destructive = Editor()->m_BrushDrawDestructive || Empty || pLt->IsEmpty();

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
				if(pLt->m_HasKZFront && m_pTiles[TgtIndex].m_Index > 0)
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