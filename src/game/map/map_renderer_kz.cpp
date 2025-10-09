// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include "render_layer.h"
#include "map_renderer.h"
#include <iterator>
#include <game/gamecore.h>

std::unique_ptr<CRenderLayer> CMapRenderer::GetKZLayerInLoad(CMapItemLayerTilemap *pTileLayer, int GroupId, int LayerId)
{
    if(!pTileLayer)
        return nullptr;

    char aBuf[30] = {0};

    IntsToStr(pTileLayer->m_aName, std::size(pTileLayer->m_aName), aBuf, std::size(aBuf));

    if(!str_comp_nocase(KZ_GAME_LAYER_NAME, aBuf))
    {
	    return std::make_unique<CRenderLayerEntityKZGame>(GroupId, LayerId, pTileLayer->m_Flags, pTileLayer);
    }
    else if(!str_comp_nocase(KZ_FRONT_LAYER_NAME, aBuf))
    {
	    return std::make_unique<CRenderLayerEntityKZFront>(GroupId, LayerId, pTileLayer->m_Flags, pTileLayer);
    }

    return nullptr;
}