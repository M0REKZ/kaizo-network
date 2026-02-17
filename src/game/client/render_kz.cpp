// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include "render.h"
#include <generated/client_data.h>

//From TClient
void CRenderTools::RenderCursorKaizo(vec2 Center, float Size, float Alpha) const
{
	Graphics()->WrapClamp();
	Graphics()->TextureSet(g_pData->m_aImages[IMAGE_CURSOR].m_Id);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(1.0f, 1.0f, 1.0f, Alpha);
	IGraphics::CQuadItem QuadItem(Center.x, Center.y, Size, Size);
	Graphics()->QuadsDrawTL(&QuadItem, 1);
	Graphics()->QuadsEnd();
	Graphics()->WrapNormal();
}
