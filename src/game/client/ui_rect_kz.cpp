// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// Just look for comments that indicate if code is from another mod...

#include "ui_rect.h"
#include <vector>
#include <engine/graphics.h>

//From EClient
void CUIRect::DrawOutlineEClient(ColorRGBA Color, int Corners, float Rounding) const
{
	const int NumSegments = 8;
	const float AngleStep = (pi / 2.0f) / NumSegments;

	std::vector<IGraphics::CLineItem> Lines;

	auto AddCornerArc = [&](float Cx, float Cy, float StartAngle, int CornerFlag) {
		if((Corners & CornerFlag) && Rounding > 0.0f)
		{
			for(int i = 0; i < NumSegments; ++i)
			{
				float a0 = StartAngle + i * AngleStep;
				float a1 = StartAngle + (i + 1) * AngleStep;
				float x0 = Cx + std::cos(a0) * Rounding;
				float y0 = Cy + std::sin(a0) * Rounding;
				float x1 = Cx + std::cos(a1) * Rounding;
				float y1 = Cy + std::sin(a1) * Rounding;
				Lines.emplace_back(x0, y0, x1, y1);
			}
		}
	};

	const bool Tl = (Corners & IGraphics::CORNER_TL);
	const bool Tr = (Corners & IGraphics::CORNER_TR);
	const bool Bl = (Corners & IGraphics::CORNER_BL);
	const bool Br = (Corners & IGraphics::CORNER_BR);

	float LengthEnd = 0.0f;
	float LengthStart = 0.0f;

	// Top
	LengthEnd = (Rounding > 0.0f && Tl) ? Rounding : 0.0f;
	LengthStart = (Rounding > 0.0f && Tr) ? Rounding : 0.0f;
	Lines.emplace_back(x + LengthEnd, y, x + w - LengthStart, y);

	// Right
	LengthEnd = (Rounding > 0.0f && Tr) ? Rounding : 0.0f;
	LengthStart = (Rounding > 0.0f && Br) ? Rounding : 0.0f;
	Lines.emplace_back(x + w, y + LengthEnd, x + w, y + h - LengthStart);

	// Bottom
	LengthEnd = (Rounding > 0.0f && Br) ? Rounding : 0.0f;
	LengthStart = (Rounding > 0.0f && Bl) ? Rounding : 0.0f;
	Lines.emplace_back(x + w - LengthEnd, y + h, x + LengthStart, y + h);

	// Left
	LengthEnd = (Rounding > 0.0f && Bl) ? Rounding : 0.0f;
	LengthStart = (Rounding > 0.0f && Tl) ? Rounding : 0.0f;
	Lines.emplace_back(x, y + h - LengthEnd, x, y + LengthStart);

	AddCornerArc(x + Rounding, y + Rounding, pi, IGraphics::CORNER_TL); // Top-left
	AddCornerArc(x + w - Rounding, y + Rounding, -pi / 2.0f, IGraphics::CORNER_TR); // Top-right
	AddCornerArc(x + w - Rounding, y + h - Rounding, 0.0f, IGraphics::CORNER_BR); // Bottom-right
	AddCornerArc(x + Rounding, y + h - Rounding, pi / 2.0f, IGraphics::CORNER_BL); // Bottom-left

	ms_pGraphics->TextureClear();
	ms_pGraphics->LinesBegin();
	ms_pGraphics->SetColor(Color);
	ms_pGraphics->LinesDraw(Lines.data(), Lines.size());
	ms_pGraphics->LinesEnd();
}
