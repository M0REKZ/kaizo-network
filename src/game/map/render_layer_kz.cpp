// Copyright (C) Benjamín Gajardo (also known as +KZ)

// both CRenderLayerEntityKZGame and CRenderLayerEntityKZFront have code from render_layer.cpp

#include <engine/storage.h>
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

IGraphics::CTextureHandle CRenderLayerEntityKZGame::GetTexture() const
{
	return m_Texture;
}

int CRenderLayerEntityKZGame::GetDataIndex(unsigned int &TileSize) const
{
	TileSize = sizeof(CKZTile);
	return m_pLayerTilemap->m_KZGame;
}

void CRenderLayerEntityKZGame::Init()
{
	m_Texture = Graphics()->LoadTexture("editor/KZ_gamelayer.png", IStorage::TYPE_ALL, Graphics()->Uses2DTextureArrays() ? IGraphics::TEXLOAD_TO_2D_ARRAY_TEXTURE : IGraphics::TEXLOAD_TO_3D_TEXTURE);
}

void CRenderLayerEntityKZGame::InitTileData()
{
	m_pKZTiles = GetData<CKZTile>();
}

void CRenderLayerEntityKZGame::Unload()
{
	CRenderLayerEntityBase::Unload();
	m_Texture.Invalidate();
}

void CRenderLayerEntityKZGame::RenderTileLayerWithTileBuffer(const ColorRGBA &Color, const CRenderLayerParams &Params)
{
	RenderTileLayerNoTileBuffer(Color,Params); //+KZ: i wont overcomplicate with tilebuffering, feel free to contribute this if you want
}

void CRenderLayerEntityKZGame::RenderTileLayerNoTileBuffer(const ColorRGBA &Color, const CRenderLayerParams &Params)
{
	if(!m_Texture.IsValid())
		return;

	UseTexture(GetTexture());
	Graphics()->BlendNone();
	RenderMap()->RenderKZGameMap(m_pKZTiles, m_pLayerTilemap->m_Width, m_pLayerTilemap->m_Height, 32.0f, Color, (Params.m_RenderTileBorder ? TILERENDERFLAG_EXTEND : 0) | LAYERRENDERFLAG_OPAQUE);
	Graphics()->BlendNormal();
	RenderMap()->RenderKZGameMap(m_pKZTiles, m_pLayerTilemap->m_Width, m_pLayerTilemap->m_Height, 32.0f, Color, (Params.m_RenderTileBorder ? TILERENDERFLAG_EXTEND : 0) | LAYERRENDERFLAG_TRANSPARENT);
	int OverlayRenderFlags = (Params.m_RenderText ? OVERLAYRENDERFLAG_TEXT : 0) | (Params.m_RenderInvalidTiles ? OVERLAYRENDERFLAG_EDITOR : 0);
	RenderMap()->RenderKZGameOverlay(m_pKZTiles, m_pLayerTilemap->m_Width, m_pLayerTilemap->m_Height, 32.0f, OverlayRenderFlags, Color.a);
}

// KZFront
CRenderLayerEntityKZFront::CRenderLayerEntityKZFront(int GroupId, int LayerId, int Flags, CMapItemLayerTilemap *pLayerTilemap) :
	CRenderLayerEntityBase(GroupId, LayerId, Flags, pLayerTilemap) {}

void CRenderLayerEntityKZFront::GetTileData(unsigned char *pIndex, unsigned char *pFlags, int *pAngleRotate, unsigned int x, unsigned int y, int CurOverlay) const
{
	*pIndex = m_pKZTiles[y * m_pLayerTilemap->m_Width + x].m_Index;
	*pFlags = 0;
}

IGraphics::CTextureHandle CRenderLayerEntityKZFront::GetTexture() const
{
	return m_Texture;
}

int CRenderLayerEntityKZFront::GetDataIndex(unsigned int &TileSize) const
{
	TileSize = sizeof(CKZTile);
	return m_pLayerTilemap->m_KZFront;
}

void CRenderLayerEntityKZFront::Init()
{
	m_Texture = Graphics()->LoadTexture("editor/KZ_frontlayer.png", IStorage::TYPE_ALL, Graphics()->Uses2DTextureArrays() ? IGraphics::TEXLOAD_TO_2D_ARRAY_TEXTURE : IGraphics::TEXLOAD_TO_3D_TEXTURE);
}

void CRenderLayerEntityKZFront::InitTileData()
{
	m_pKZTiles = GetData<CKZTile>();
}

void CRenderLayerEntityKZFront::Unload()
{
	CRenderLayerEntityBase::Unload();
	m_Texture.Invalidate();
}

void CRenderLayerEntityKZFront::RenderTileLayerWithTileBuffer(const ColorRGBA &Color, const CRenderLayerParams &Params)
{
	RenderTileLayerNoTileBuffer(Color,Params); //+KZ: i wont overcomplicate with tilebuffering, feel free to contribute this if you want
}

void CRenderLayerEntityKZFront::RenderTileLayerNoTileBuffer(const ColorRGBA &Color, const CRenderLayerParams &Params)
{
	if(!m_Texture.IsValid())
		return;

	Graphics()->BlendNone();
	RenderMap()->RenderKZFrontMap(m_pKZTiles, m_pLayerTilemap->m_Width, m_pLayerTilemap->m_Height, 32.0f, Color, (Params.m_RenderTileBorder ? TILERENDERFLAG_EXTEND : 0) | LAYERRENDERFLAG_OPAQUE);
	Graphics()->BlendNormal();
	RenderMap()->RenderKZFrontMap(m_pKZTiles, m_pLayerTilemap->m_Width, m_pLayerTilemap->m_Height, 32.0f, Color, (Params.m_RenderTileBorder ? TILERENDERFLAG_EXTEND : 0) | LAYERRENDERFLAG_TRANSPARENT);
	int OverlayRenderFlags = (Params.m_RenderText ? OVERLAYRENDERFLAG_TEXT : 0) | (Params.m_RenderInvalidTiles ? OVERLAYRENDERFLAG_EDITOR : 0);
	RenderMap()->RenderKZGameOverlay(m_pKZTiles, m_pLayerTilemap->m_Width, m_pLayerTilemap->m_Height, 32.0f, OverlayRenderFlags, Color.a);
}
