
#include "menus.h"
#include <game/localization.h>

//from RushieClient
ColorHSLA CMenus::DoButton_ColorPickerAutoVMargin(CButtonContainer *pResetId, const char *pText, unsigned int *pColorValue, const ColorRGBA DefaultColor, CUIRect *pRect, const float VMargin, const bool Alpha, int *pCheckBoxValue)
{
	CUIRect Section, ColorPickerButton, ResetButton, Label;

	pRect->HSplitTop(VMargin, &Section, pRect);

	Section.VSplitRight(60.0f, &Section, &ResetButton);
	Section.VSplitRight(8.0f, &Section, nullptr);
	Section.VSplitRight(Section.h, &Section, &ColorPickerButton);
	Section.VSplitRight(8.0f, &Label, nullptr);

	if(pCheckBoxValue != nullptr)
	{
		if(DoButton_CheckBox(pCheckBoxValue, pText, *pCheckBoxValue, &Label))
			*pCheckBoxValue ^= 1;
	}
	else
	{
		Ui()->DoLabel(&Label, pText, Label.h * CUi::ms_FontmodHeight, TEXTALIGN_ML);
	}

	const ColorHSLA PickedColor = DoButton_ColorPicker(&ColorPickerButton, pColorValue, Alpha);

	ResetButton.HMargin(2.0f, &ResetButton);
	if(DoButton_Menu(pResetId, Localize("Reset"), 0, &ResetButton, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL, 4.0f, 0.1f, ColorRGBA(1.0f, 1.0f, 1.0f, 0.25f)))
	{
		*pColorValue = color_cast<ColorHSLA>(DefaultColor).Pack(Alpha);
	}

	return PickedColor;
}
