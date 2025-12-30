// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// MakeKZGameLayer and MakeKZFrontLayer have code from map.cpp

#include "map.h"
#include "layer_kz.h"

void CEditorMap::MakeKZGameLayer(const std::shared_ptr<CLayer> &pLayer) //+KZ
{
	m_pKZGameLayer = std::static_pointer_cast<CLayerKZGame>(pLayer);
}

void CEditorMap::MakeKZFrontLayer(const std::shared_ptr<CLayer> &pLayer) //+KZ
{
	m_pKZFrontLayer = std::static_pointer_cast<CLayerKZFront>(pLayer);
}
