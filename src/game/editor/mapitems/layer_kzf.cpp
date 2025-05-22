
#include "layer_kz.h"

#include <game/editor/editor.h>

CLayerKZFront::CLayerKZFront(CEditor *pEditor, int w, int h) :
	CLayerKZGame(pEditor, w, h)
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

void CLayerKZFront::BrushDraw(std::shared_ptr<CLayer> pBrush, vec2 WorldPos)
{
	if(m_Readonly)
		return;

	std::shared_ptr<CLayerKZGame> pSwitchLayer = std::static_pointer_cast<CLayerKZGame>(pBrush);
	int sx = ConvertX(WorldPos.x);
	int sy = ConvertY(WorldPos.y);
	if(str_comp(pSwitchLayer->m_aFileName, m_pEditor->m_aFileName))
	{
		m_pEditor->m_KZFrontNumber = pSwitchLayer->m_Number;
		m_pEditor->m_KZFrontValue1 = pSwitchLayer->m_Value1;
		m_pEditor->m_KZFrontValue2 = pSwitchLayer->m_Value2;
		m_pEditor->m_KZFrontValue3 = pSwitchLayer->m_Value3;
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
				if(m_pEditor->m_KZFrontNumber != pSwitchLayer->m_Number || m_pEditor->m_KZFrontValue1 != pSwitchLayer->m_Value1 || m_pEditor->m_KZFrontValue2 != pSwitchLayer->m_Value2 || m_pEditor->m_KZFrontValue3 != pSwitchLayer->m_Value3)
				{
					m_pKZTile[TgtIndex].m_Number = m_pEditor->m_KZFrontNumber;
					m_pKZTile[TgtIndex].m_Value1 = m_pEditor->m_KZFrontValue1;
					m_pKZTile[TgtIndex].m_Value2 = m_pEditor->m_KZFrontValue2;
					m_pKZTile[TgtIndex].m_Value3 = m_pEditor->m_KZFrontValue3;
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
					m_pKZTile[TgtIndex].m_Number = m_pEditor->m_KZFrontNumber;
					m_pKZTile[TgtIndex].m_Value1 = m_pEditor->m_KZFrontValue1;
					m_pKZTile[TgtIndex].m_Value2 = m_pEditor->m_KZFrontValue2;
					m_pKZTile[TgtIndex].m_Value3 = m_pEditor->m_KZFrontValue3;
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