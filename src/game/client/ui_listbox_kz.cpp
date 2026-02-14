// Copyright (C) Benjamín Gajardo (also known as +KZ)

#include "ui_listbox.h"
#include <engine/shared/config.h>

//this is from DDNet code
CListboxItem CListBox::DoNextItemHeader(const void *pId, bool Selected, float CornerRadius)
{
    //Get color Like DuckClient
    ColorRGBA color = color_cast<ColorRGBA>(ColorHSLA(g_Config.m_UiColor, false));
	color.a = 0.33;

	if(m_AutoSpacing > 0.0f && m_ListBoxItemIndex > 0)
		DoSpacing(m_AutoSpacing);

	const int ThisItemIndex = m_ListBoxItemIndex;
	if(Selected)
	{
		if(m_ListBoxSelectedIndex == m_ListBoxNewSelected)
			m_ListBoxNewSelected = ThisItemIndex;
		m_ListBoxSelectedIndex = ThisItemIndex;
	}

	CListboxItem Item = DoNextRow();
	const int ItemClicked = Item.m_Visible ? Ui()->DoButtonLogic(pId, 0, &Item.m_Rect, BUTTONFLAG_LEFT) : 0;
	if(ItemClicked)
	{
		m_ListBoxNewSelected = ThisItemIndex;
		m_ListBoxItemSelected = true;
		m_Active = true;
	}

	// process input, regard selected index
	if(m_ListBoxNewSelected == ThisItemIndex)
	{
		if(m_Active)
		{
			if(Ui()->ConsumeHotkey(CUi::HOTKEY_ENTER) || (ItemClicked == 1 && Ui()->DoDoubleClickLogic(pId)))
			{
				m_ListBoxItemActivated = true;
				Ui()->SetActiveItem(nullptr);
			}
		}

		Item.m_Rect.Draw(ColorRGBA(color.r, color.g, color.b, m_Active ? 0.5f : color.a), IGraphics::CORNER_ALL, CornerRadius);
	}
    else
	{
		Item.m_Rect.Draw(color, IGraphics::CORNER_ALL, CornerRadius);
	}

	return Item;
}
