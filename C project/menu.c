#include "IDE.h"
#include "lcd_drv.h"

extern const u8
	_Empty[],
	_InfoAuto[],
	_InfoSimple[],
	_ViewParams1[],
	_ViewParams2[],
	_PrePressing[],
	_Pressing[],
	_Heating[],
	_Forging[],
	_Modulation[],
	_Current[],
	_Editing[],
	_ChooseParam[],
	_ParametrHas[],
	_BeenSaved[],
	_ParametrHasNot[],
	_NotSaved[],
	_WeldingIs[],
	_Completed[],
	_PressingIs[],
	_HeatingIs[],
	_ForgingIs[],
	_Running[],
	_Auto[],
	_Simple[],
	_Pause_[];


	


//=================================================================================

extern u8 readByteEE(u16 addr);
extern MenuItem mPrograms;
extern void WrDec(u8 a_data, u8 a_x, u8 a_y);
extern void Wr3Dec(u8 a_data, u8 a_x, u8 a_y);
extern u8 get_key();
//extern void initParams(u8 a_nPrg);
extern void UpdateParams();
extern u8 GetValue(u8 a_nParamId);
extern void writeByteEE(u16 addr, u8 data);
extern void NoteTime();
extern BOOL is_time_expired(u32 time);

extern CURPRG curPrg;
extern CURMODE curMode;
static MenuItem* curMenu = &mPrograms;


void WriteMenu(MenuItem* a_m);
void SetMenu(const MenuItem* a_curMenu);

void WriteMenu(MenuItem* a_m)
{
#ifndef _DEBUG_
	lcd_gotoxy(0, 0);
	lcd_puts_p((const char *)a_m->str1);
	lcd_gotoxy(0, 1);
	lcd_puts_p((const char *)a_m->str2);
#endif
}

void WriteMenuString(const u8* a_s, u8 a_nStr)
{
#ifndef _DEBUG_
	lcd_gotoxy(0, a_nStr);
	lcd_puts_p((const char*)a_s);
#endif
}

//////////////////////////////////////////////////////////////////////////
// ќбновл€ет локальные данные на экране
//////////////////////////////////////////////////////////////////////////
void UpdateLcdParam(u8 a_ParamsId, u8 a_nVal)
{
	switch(a_ParamsId)
	{
		case paramPrePressing:
		case paramHeating:
		case paramForging:
		case paramPause:
			Wr3Dec(a_nVal, 12, lcdstr2);
			break;
		case paramPressing:
		case paramModulation:
		case paramCurrent:
			WrDec(a_nVal, 12, lcdstr2);
			break;
		case paramMode:
#ifndef _RUSSIAN_VERSION_
			lcd_gotoxy(6, lcdstr2);
#else
			lcd_gotoxy(7, lcdstr2);
#endif
			if (a_nVal == SIMPLE_MODE)
				lcd_puts_p((const char *)_Simple);
			else if (a_nVal == AUTO_MODE)
				lcd_puts_p((const char *)_Auto);
			break;
		case cmnprmStartPrg:
			WrDec(a_nVal, 14, lcdstr2);
			break;
	}
	NoteTime();
}

void SavedDlg(u8 a_bSaved)
{
	wdt_start(wdt_2s);
	if (a_bSaved)
	{
		WriteMenuString(_ParametrHas, lcdstr1);
		WriteMenuString(_BeenSaved, lcdstr2);
	}
	else
	{
		WriteMenuString(_ParametrHasNot, lcdstr1);
		WriteMenuString(_BeenSaved, lcdstr2);
	}
#ifndef _DEBUG_
	u8 del = 10;
	while(del--)
	{
		_delay_ms(100);
		if (get_key() != keyEmpty)
		{
			break;
		}
	}
#endif
	wdt_start(wdt_60ms);
}

void setScreen(u8 a_scr)
{ // выполн€етс€ 3мс
#ifndef _DEBUG_
	lcd_gotoxy(0, lcdstr1);
	switch(a_scr)
	{
		case scrViewParams:
			lcd_puts_p((const char *)_ViewParams1);
			break;
		case scrPrePressing:
			lcd_puts_p((const char *)_PrePressing);
			break;
		case scrPressing:
			lcd_puts_p((const char *)_PressingIs);
			break;
		case scrHeating:
			lcd_puts_p((const char *)_HeatingIs);
			break;
		case scrForging:
			lcd_puts_p((const char *)_ForgingIs);
			break;
		case scrWeldingCompleted:
			lcd_puts_p((const char *)_WeldingIs);
			break;
		case scrPause:
			lcd_puts_p((const char *)_Pause_);
			break;
		default:
			lcd_puts_p((const char *)_Empty);
			break;
	}
	lcd_gotoxy(0, lcdstr2);
	switch(a_scr)
	{
		case scrViewParams:
			lcd_puts_p((const char *)_ViewParams2);
			break;
		case scrWeldingCompleted:
			lcd_puts_p((const char *)_Completed);
			break;
		case scrPrePressing:
		case scrPressing:
		case scrHeating:
		case scrForging:
			lcd_puts_p((const char *)_Running);
			break;
		case scrPause:
		default:
			lcd_puts_p((const char *)_Empty);
			break;
	}
#endif
}

void SetMenuData(u8 a_id)
{
	u8 param = 0xff;
	switch(a_id)
	{
		case idInfo:
			lcd_gotoxy(0, lcdstr2);
			if (curMode.get() == AUTO_MODE)
			{
				lcd_puts_p((const char *)_InfoAuto);
				u8 tmp = readByteEE(curPrg.get() * paramNum + addrPause);
				Wr3Dec(tmp, 12, lcdstr2);
			}
			else
				lcd_puts_p((const char *)_InfoSimple);
			break;
		case idPrograms:
			UpdateParams(); // обновл€ю данные программы на экране
			break;
		case idChoosePrePressing:	case idEditPrePressing:
			param = paramPrePressing;
			break;
		case idChoosePressing:		case idEditPressing:
			param = paramPressing;
			break;
		case idChooseHeating:		case idEditHeating:
			param = paramHeating;
			break;
		case idChooseForging:		case idEditForging:
			param = paramForging;
			break;
		case idChooseModulation:	case idEditModulation:
			param = paramModulation;
			break;
		case idChooseCurrent:		case idEditCurrent:
			param = paramCurrent;
			break;
		case idChooseMode:			case idEditMode:
			param = paramMode;
			break;
		case idChoosePause:			case idEditPause:
			param = paramPause;
			break;
		case idChooseStartPrg:		case idEditStartPrg:
			param = cmnprmStartPrg;
			break;
	}
	switch(a_id)
	{
		case idChoosePrePressing:	case idEditPrePressing:
		case idChoosePressing:		case idEditPressing:
		case idChooseHeating:		case idEditHeating:
		case idChooseForging:		case idEditForging:
		case idChooseModulation:	case idEditModulation:
		case idChooseCurrent:		case idEditCurrent:
		case idChooseMode:			case idEditMode:
		case idChoosePause:			case idEditPause:
		case idChooseStartPrg:		case idEditStartPrg:
			if (param != 0xff)
				UpdateLcdParam(param, GetValue(param));
			break;
	}
}

u8 getCurMenuId()
{
	return curMenu->id;
}
void SetMenu(const MenuItem* a_curMenu) // установка текущего меню и вывод его на экран
{// выполн€етс€ 3-5 мс
	memcpy_P(curMenu, a_curMenu, sizeof(MenuItem));
	WriteMenu(curMenu);
	SetMenuData(curMenu->id);
	NoteTime();
}

void DoMenu()
{
	curMenu->pf();
	if (curMenu->id != idPrograms)
	{
		if (is_time_expired(TIME_FOR_MENU) == TRUE)
			SetMenu(&mPrograms);
	}
}

//====================================================================================
