// Copyright (C) Benjamín Gajardo (also known as +KZ)
//
// int64_t UiDoValueSelector() is a modified copypaste of editor_ui.cpp UiDoValueSelector()
// DoPropertiesWithStateLongLong() also is modified code from DDNet files

#include "editor.h"
#include "enums.h"

#include <engine/textrender.h>

#include <game/editor/mapitems/image.h>
#include <game/editor/mapitems/sound.h>
#include <engine/font_icons.h>

SEditResult<int64_t> CEditor::UiDoValueSelectorInt64(void *pId, CUIRect *pRect, const char *pLabel, int64_t Current, int64_t Min, int64_t Max, int Step, float Scale, const char *pToolTip, bool IsDegree, bool IsHex, int Corners, const ColorRGBA *pColor, bool ShowValue)
{
	// logic
	static bool s_DidScroll = false;
	static float s_ScrollValue = 0.0f;
	static CLineInputNumber s_NumberInput;
	static int s_ButtonUsed = -1;
	static void *s_pLastTextId = nullptr;

	const bool Inside = Ui()->MouseInside(pRect);
	const int Base = IsHex ? 16 : 10;

	if(Ui()->HotItem() == pId && s_ButtonUsed >= 0 && !Ui()->MouseButton(s_ButtonUsed))
	{
		Ui()->DisableMouseLock();
		if(Ui()->CheckActiveItem(pId))
		{
			Ui()->SetActiveItem(nullptr);
		}
		if(Inside && ((s_ButtonUsed == 0 && !s_DidScroll && Ui()->DoDoubleClickLogic(pId)) || s_ButtonUsed == 1))
		{
			s_pLastTextId = pId;
			s_NumberInput.SetInteger64(Current, Base);
			s_NumberInput.SelectAll();
		}
		s_ButtonUsed = -1;
	}

	if(s_pLastTextId == pId)
	{
		str_copy(m_aTooltip, "Type your number. Press enter to confirm.");
		Ui()->SetActiveItem(&s_NumberInput);
		DoEditBox(&s_NumberInput, pRect, 10.0f, Corners);

		if(Ui()->ConsumeHotkey(CUi::HOTKEY_ENTER) || ((Ui()->MouseButtonClicked(1) || Ui()->MouseButtonClicked(0)) && !Inside))
		{
			Current = std::clamp(s_NumberInput.GetInteger64(Base), Min, Max); //+KZ int64
			Ui()->DisableMouseLock();
			Ui()->SetActiveItem(nullptr);
			s_pLastTextId = nullptr;
		}

		if(Ui()->ConsumeHotkey(CUi::HOTKEY_ESCAPE))
		{
			Ui()->DisableMouseLock();
			Ui()->SetActiveItem(nullptr);
			s_pLastTextId = nullptr;
		}
	}
	else
	{
		if(Ui()->CheckActiveItem(pId))
		{
			if(s_ButtonUsed == 0 && Ui()->MouseButton(0))
			{
				s_ScrollValue += Ui()->MouseDeltaX() * (Input()->ShiftIsPressed() ? 0.05f : 1.0f);

				if(absolute(s_ScrollValue) >= Scale)
				{
					int Count = (int)(s_ScrollValue / Scale);
					s_ScrollValue = std::fmod(s_ScrollValue, Scale);
					Current += Step * Count;
					Current = std::clamp(Current, Min, Max);
					s_DidScroll = true;

					// Constrain to discrete steps
					if(Count > 0)
						Current = Current / Step * Step;
					else
						Current = std::ceil(Current / (float)Step) * Step;
				}
			}

			if(pToolTip && s_pLastTextId != pId)
				str_copy(m_aTooltip, pToolTip);
		}
		else if(Ui()->HotItem() == pId)
		{
			if(Ui()->MouseButton(0))
			{
				s_ButtonUsed = 0;
				s_DidScroll = false;
				s_ScrollValue = 0.0f;
				Ui()->SetActiveItem(pId);
				Ui()->EnableMouseLock(pId);
			}
			else if(Ui()->MouseButton(1))
			{
				s_ButtonUsed = 1;
				Ui()->SetActiveItem(pId);
			}

			if(pToolTip && s_pLastTextId != pId)
				str_copy(m_aTooltip, pToolTip);
		}

		// render
		char aBuf[128];
		if(pLabel[0] != '\0')
		{
			if(ShowValue)
				str_format(aBuf, sizeof(aBuf), "%s %d", pLabel, Current);
			else
				str_copy(aBuf, pLabel);
		}
		else if(IsDegree)
			str_format(aBuf, sizeof(aBuf), "%d°", Current);
		else if(IsHex)
			str_format(aBuf, sizeof(aBuf), "#%06X", Current);
		else
			str_format(aBuf, sizeof(aBuf), "%d", Current);
		pRect->Draw(pColor ? *pColor : GetButtonColor(pId, 0), Corners, 3.0f);
		Ui()->DoLabel(pRect, aBuf, 10, TEXTALIGN_MC);
	}

	if(Inside && !Ui()->MouseButton(0) && !Ui()->MouseButton(1))
		Ui()->SetHotItem(pId);

	static const void *s_pEditing = nullptr;
	EEditState State = EEditState::NONE;
	if(s_pEditing == pId)
	{
		State = EEditState::EDITING;
	}
	if(((Ui()->CheckActiveItem(pId) && Ui()->CheckMouseLock() && s_DidScroll) || s_pLastTextId == pId) && s_pEditing != pId)
	{
		State = EEditState::START;
		s_pEditing = pId;
	}
	if(!Ui()->CheckMouseLock() && s_pLastTextId != pId && s_pEditing == pId)
	{
		State = EEditState::END;
		s_pEditing = nullptr;
	}

	return SEditResult<int64_t>{State, Current};
}

SEditResult<int64_t> CEditor::DoPropertiesWithStateLongLong(CUIRect *pToolBox, CProperty *pProps, int *pIds, int64_t *pNewVal, const std::vector<ColorRGBA> &vColors)
{
	int Change = -1;
	EEditState State = EEditState::NONE;

	for(int i = 0; pProps[i].m_pName; i++)
	{
		const ColorRGBA *pColor = i >= (int)vColors.size() ? &ms_DefaultPropColor : &vColors[i];

		CUIRect Slot;
		pToolBox->HSplitTop(13.0f, &Slot, pToolBox);
		CUIRect Label, Shifter;
		Slot.VSplitMid(&Label, &Shifter);
		Shifter.HMargin(1.0f, &Shifter);
		Ui()->DoLabel(&Label, pProps[i].m_pName, 10.0f, TEXTALIGN_ML);

		if(pProps[i].m_Type == PROPTYPE_INT)
		{
			CUIRect Inc, Dec;
			char aBuf[64];

			Shifter.VSplitRight(10.0f, &Shifter, &Inc);
			Shifter.VSplitLeft(10.0f, &Dec, &Shifter);
			str_format(aBuf, sizeof(aBuf), "%d", pProps[i].m_Value);
			auto NewValueRes = UiDoValueSelectorInt64((char *)&pIds[i], &Shifter, "", pProps[i].m_Value, pProps[i].m_Min, pProps[i].m_Max, 1, 1.0f, "Use left mouse button to drag and change the value. Hold shift to be more precise. Right click to edit as text.", false, false, 0, pColor);
			int NewValue = NewValueRes.m_Value;
			if(NewValue != pProps[i].m_Value || (NewValueRes.m_State != EEditState::NONE && NewValueRes.m_State != EEditState::EDITING))
			{
				*pNewVal = NewValue;
				Change = i;
				State = NewValueRes.m_State;
			}
			if(DoButton_FontIcon((char *)&pIds[i] + 1, FontIcon::MINUS, 0, &Dec, BUTTONFLAG_LEFT, "Decrease value.", IGraphics::CORNER_L, 7.0f))
			{
				*pNewVal = std::clamp(pProps[i].m_Value - 1, pProps[i].m_Min, pProps[i].m_Max);
				Change = i;
				State = EEditState::ONE_GO;
			}
			if(DoButton_FontIcon(((char *)&pIds[i]) + 2, FontIcon::PLUS, 0, &Inc, BUTTONFLAG_LEFT, "Increase value.", IGraphics::CORNER_R, 7.0f))
			{
				*pNewVal = std::clamp(pProps[i].m_Value + 1, pProps[i].m_Min, pProps[i].m_Max);
				Change = i;
				State = EEditState::ONE_GO;
			}
		}
		else if(pProps[i].m_Type == PROPTYPE_BOOL)
		{
			CUIRect No, Yes;
			Shifter.VSplitMid(&No, &Yes);
			if(DoButton_Ex(&pIds[i], "No", !pProps[i].m_Value, &No, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_L))
			{
				*pNewVal = 0;
				Change = i;
				State = EEditState::ONE_GO;
			}
			if(DoButton_Ex(((char *)&pIds[i]) + 1, "Yes", pProps[i].m_Value, &Yes, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_R))
			{
				*pNewVal = 1;
				Change = i;
				State = EEditState::ONE_GO;
			}
		}
		else if(pProps[i].m_Type == PROPTYPE_ANGLE_SCROLL)
		{
			CUIRect Inc, Dec;
			Shifter.VSplitRight(10.0f, &Shifter, &Inc);
			Shifter.VSplitLeft(10.0f, &Dec, &Shifter);
			const bool Shift = Input()->ShiftIsPressed();
			int Step = Shift ? 1 : 45;
			int Value = pProps[i].m_Value;

			auto NewValueRes = UiDoValueSelectorInt64(&pIds[i], &Shifter, "", (int64_t)Value, pProps[i].m_Min, pProps[i].m_Max, Shift ? 1 : 45, Shift ? 1.0f : 10.0f, "Use left mouse button to drag and change the value. Hold shift to be more precise. Right click to edit as text.", false, false, 0);
			int NewValue = NewValueRes.m_Value;
			if(DoButton_FontIcon(&pIds[i] + 1, FontIcon::MINUS, 0, &Dec, BUTTONFLAG_LEFT, "Decrease value.", IGraphics::CORNER_L, 7.0f))
			{
				NewValue = (std::ceil((pProps[i].m_Value / (float)Step)) - 1) * Step;
				if(NewValue < 0)
					NewValue += 360;
				State = EEditState::ONE_GO;
			}
			if(DoButton_FontIcon(&pIds[i] + 2, FontIcon::PLUS, 0, &Inc, BUTTONFLAG_LEFT, "Increase value.", IGraphics::CORNER_R, 7.0f))
			{
				NewValue = (pProps[i].m_Value + Step) / Step * Step;
				State = EEditState::ONE_GO;
			}

			if(NewValue != pProps[i].m_Value || (NewValueRes.m_State != EEditState::NONE && NewValueRes.m_State != EEditState::EDITING))
			{
				*pNewVal = NewValue % 360;
				Change = i;
				State = NewValueRes.m_State;
			}
		}
		else if(pProps[i].m_Type == PROPTYPE_COLOR)
		{
			const auto &&SetColor = [&](ColorRGBA NewColor) {
				const int NewValue = NewColor.PackAlphaLast();
				if(NewValue != pProps[i].m_Value || m_ColorPickerPopupContext.m_State != EEditState::EDITING)
				{
					*pNewVal = NewValue;
					Change = i;
					State = m_ColorPickerPopupContext.m_State;
				}
			};
			DoColorPickerButton(&pIds[i], &Shifter, ColorRGBA::UnpackAlphaLast<ColorRGBA>(pProps[i].m_Value), SetColor);
		}
		else if(pProps[i].m_Type == PROPTYPE_IMAGE)
		{
			const char *pName;
			if(pProps[i].m_Value < 0)
				pName = "None";
			else
				pName = m_Map.m_vpImages[pProps[i].m_Value]->m_aName;

			if(DoButton_Ex(&pIds[i], pName, 0, &Shifter, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL))
				PopupSelectImageInvoke(pProps[i].m_Value, Ui()->MouseX(), Ui()->MouseY());

			int r = PopupSelectImageResult();
			if(r >= -1)
			{
				*pNewVal = r;
				Change = i;
				State = EEditState::ONE_GO;
			}
		}
		else if(pProps[i].m_Type == PROPTYPE_SHIFT)
		{
			CUIRect Left, Right, Up, Down;
			Shifter.VSplitMid(&Left, &Up, 2.0f);
			Left.VSplitLeft(10.0f, &Left, &Shifter);
			Shifter.VSplitRight(10.0f, &Shifter, &Right);
			Shifter.Draw(ColorRGBA(1, 1, 1, 0.5f), IGraphics::CORNER_NONE, 0.0f);
			Ui()->DoLabel(&Shifter, "X", 10.0f, TEXTALIGN_MC);
			Up.VSplitLeft(10.0f, &Up, &Shifter);
			Shifter.VSplitRight(10.0f, &Shifter, &Down);
			Shifter.Draw(ColorRGBA(1, 1, 1, 0.5f), IGraphics::CORNER_NONE, 0.0f);
			Ui()->DoLabel(&Shifter, "Y", 10.0f, TEXTALIGN_MC);
			if(DoButton_FontIcon(&pIds[i], FontIcon::MINUS, 0, &Left, BUTTONFLAG_LEFT, "Shift left.", IGraphics::CORNER_L, 7.0f))
			{
				*pNewVal = (int)EShiftDirection::LEFT;
				Change = i;
				State = EEditState::ONE_GO;
			}
			if(DoButton_FontIcon(((char *)&pIds[i]) + 3, FontIcon::PLUS, 0, &Right, BUTTONFLAG_LEFT, "Shift right.", IGraphics::CORNER_R, 7.0f))
			{
				*pNewVal = (int)EShiftDirection::RIGHT;
				Change = i;
				State = EEditState::ONE_GO;
			}
			if(DoButton_FontIcon(((char *)&pIds[i]) + 1, FontIcon::MINUS, 0, &Up, BUTTONFLAG_LEFT, "Shift up.", IGraphics::CORNER_L, 7.0f))
			{
				*pNewVal = (int)EShiftDirection::UP;
				Change = i;
				State = EEditState::ONE_GO;
			}
			if(DoButton_FontIcon(((char *)&pIds[i]) + 2, FontIcon::PLUS, 0, &Down, BUTTONFLAG_LEFT, "Shift down.", IGraphics::CORNER_R, 7.0f))
			{
				*pNewVal = (int)EShiftDirection::DOWN;
				Change = i;
				State = EEditState::ONE_GO;
			}
		}
		else if(pProps[i].m_Type == PROPTYPE_SOUND)
		{
			const char *pName;
			if(pProps[i].m_Value < 0)
				pName = "None";
			else
				pName = m_Map.m_vpSounds[pProps[i].m_Value]->m_aName;

			if(DoButton_Ex(&pIds[i], pName, 0, &Shifter, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL))
				PopupSelectSoundInvoke(pProps[i].m_Value, Ui()->MouseX(), Ui()->MouseY());

			int r = PopupSelectSoundResult();
			if(r >= -1)
			{
				*pNewVal = r;
				Change = i;
				State = EEditState::ONE_GO;
			}
		}
		else if(pProps[i].m_Type == PROPTYPE_AUTOMAPPER)
		{
			const char *pName;
			if(pProps[i].m_Value < 0 || pProps[i].m_Min < 0 || pProps[i].m_Min >= (int)m_Map.m_vpImages.size())
				pName = "None";
			else
				pName = m_Map.m_vpImages[pProps[i].m_Min]->m_Automapper.GetConfigName(pProps[i].m_Value);

			if(DoButton_Ex(&pIds[i], pName, 0, &Shifter, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL))
				PopupSelectAutomapperReferenceInvoke(pProps[i].m_Value, Ui()->MouseX(), Ui()->MouseY());

			int r = PopupSelectAutomapperReferenceResult();
			if(r >= -1)
			{
				*pNewVal = r;
				Change = i;
				State = EEditState::ONE_GO;
			}
		}
		else if(pProps[i].m_Type == PROPTYPE_AUTOMAPPER_REFERENCE)
		{
			const char *pName;
			if(pProps[i].m_Value < 0)
				pName = "None";
			else
				pName = AUTOMAP_REFERENCE_NAMES[pProps[i].m_Value];

			if(DoButton_Ex(&pIds[i], pName, 0, &Shifter, BUTTONFLAG_LEFT, nullptr, IGraphics::CORNER_ALL))
				PopupSelectAutomapperReferenceInvoke(pProps[i].m_Value, Ui()->MouseX(), Ui()->MouseY());

			const int Result = PopupSelectAutomapperReferenceResult();
			if(Result >= -1)
			{
				*pNewVal = Result;
				Change = i;
				State = EEditState::ONE_GO;
			}
		}
		else if(pProps[i].m_Type == PROPTYPE_ENVELOPE)
		{
			CUIRect Inc, Dec;
			char aBuf[8];
			int CurValue = pProps[i].m_Value;

			Shifter.VSplitRight(10.0f, &Shifter, &Inc);
			Shifter.VSplitLeft(10.0f, &Dec, &Shifter);

			if(CurValue <= 0)
				str_copy(aBuf, "None:");
			else if(m_Map.m_vpEnvelopes[CurValue - 1]->m_aName[0])
			{
				str_format(aBuf, sizeof(aBuf), "%s:", m_Map.m_vpEnvelopes[CurValue - 1]->m_aName);
				if(!str_endswith(aBuf, ":"))
				{
					aBuf[sizeof(aBuf) - 2] = ':';
					aBuf[sizeof(aBuf) - 1] = '\0';
				}
			}
			else
				aBuf[0] = '\0';

			auto NewValueRes = UiDoValueSelectorInt64((char *)&pIds[i], &Shifter, aBuf, CurValue, 0, m_Map.m_vpEnvelopes.size(), 1, 1.0f, "Select envelope.", false, false, IGraphics::CORNER_NONE);
			int NewVal = NewValueRes.m_Value;
			if(NewVal != CurValue || (NewValueRes.m_State != EEditState::NONE && NewValueRes.m_State != EEditState::EDITING))
			{
				*pNewVal = NewVal;
				Change = i;
				State = NewValueRes.m_State;
			}

			if(DoButton_FontIcon((char *)&pIds[i] + 1, FontIcon::MINUS, 0, &Dec, BUTTONFLAG_LEFT, "Select previous envelope.", IGraphics::CORNER_L, 7.0f))
			{
				*pNewVal = pProps[i].m_Value - 1;
				Change = i;
				State = EEditState::ONE_GO;
			}
			if(DoButton_FontIcon(((char *)&pIds[i]) + 2, FontIcon::PLUS, 0, &Inc, BUTTONFLAG_LEFT, "Select next envelope.", IGraphics::CORNER_R, 7.0f))
			{
				*pNewVal = pProps[i].m_Value + 1;
				Change = i;
				State = EEditState::ONE_GO;
			}
		}
	}

	return SEditResult<int64_t>{State, static_cast<int64_t>(Change)};
}