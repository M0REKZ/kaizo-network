
#include <game/mapitems.h>
#include "render_layer.h"


// KZGame
CRenderLayerEntityKZGame::CRenderLayerEntityKZGame(int GroupId, int LayerId, int Flags, CMapItemLayerTilemap *pLayerTilemap) :
	CRenderLayerEntityBase(GroupId, LayerId, Flags, pLayerTilemap) {}

void CRenderLayerEntityKZGame::GetTileData(unsigned char *pIndex, unsigned char *pFlags, int *pAngleRotate, unsigned int x, unsigned int y, int CurOverlay) const
{
	*pIndex = m_pKZTiles[y * m_pLayerTilemap->m_Width + x].m_Index;
	*pFlags = 0;
}

int CRenderLayerEntityKZGame::GetDataIndex(unsigned int &TileSize) const
{
	TileSize = sizeof(CKZTile);
	return m_pLayerTilemap->m_KZGame;
}

void CRenderLayerEntityKZGame::InitTileData()
{
	m_pKZTiles = GetData<CKZTile>();
}

void CRenderLayerEntityKZGame::RenderTileLayerNoTileBuffer(const ColorRGBA &Color, const CRenderLayerParams &Params)
{
	Graphics()->BlendNone();
	RenderMap()->RenderKZGameMap(m_pKZTiles, m_pLayerTilemap->m_Width, m_pLayerTilemap->m_Height, 32.0f, Color, (Params.m_RenderTileBorder ? TILERENDERFLAG_EXTEND : 0) | LAYERRENDERFLAG_OPAQUE);
	Graphics()->BlendNormal();
	RenderMap()->RenderKZGameMap(m_pKZTiles, m_pLayerTilemap->m_Width, m_pLayerTilemap->m_Height, 32.0f, Color, (Params.m_RenderTileBorder ? TILERENDERFLAG_EXTEND : 0) | LAYERRENDERFLAG_TRANSPARENT);
}

// KZFront
CRenderLayerEntityKZFront::CRenderLayerEntityKZFront(int GroupId, int LayerId, int Flags, CMapItemLayerTilemap *pLayerTilemap) :
	CRenderLayerEntityBase(GroupId, LayerId, Flags, pLayerTilemap) {}

void CRenderLayerEntityKZFront::GetTileData(unsigned char *pIndex, unsigned char *pFlags, int *pAngleRotate, unsigned int x, unsigned int y, int CurOverlay) const
{
	*pIndex = m_pKZTiles[y * m_pLayerTilemap->m_Width + x].m_Index;
	*pFlags = 0;
}

int CRenderLayerEntityKZFront::GetDataIndex(unsigned int &TileSize) const
{
	TileSize = sizeof(CKZTile);
	return m_pLayerTilemap->m_KZFront;
}

void CRenderLayerEntityKZFront::InitTileData()
{
	m_pKZTiles = GetData<CKZTile>();
}

void CRenderLayerEntityKZFront::RenderTileLayerNoTileBuffer(const ColorRGBA &Color, const CRenderLayerParams &Params)
{
	Graphics()->BlendNone();
	RenderMap()->RenderKZFrontMap(m_pKZTiles, m_pLayerTilemap->m_Width, m_pLayerTilemap->m_Height, 32.0f, Color, (Params.m_RenderTileBorder ? TILERENDERFLAG_EXTEND : 0) | LAYERRENDERFLAG_OPAQUE);
	Graphics()->BlendNormal();
	RenderMap()->RenderKZFrontMap(m_pKZTiles, m_pLayerTilemap->m_Width, m_pLayerTilemap->m_Height, 32.0f, Color, (Params.m_RenderTileBorder ? TILERENDERFLAG_EXTEND : 0) | LAYERRENDERFLAG_TRANSPARENT);
}
