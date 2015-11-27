#include "IDE.h"

void fVoid(){}
void fInfo();
void fPrograms();
void fChoosePrgStngs();
void fChooseCmnStngs();
void fParamMode();
void fParamPause();
void fParamPrePressing();
void fParamPressing();
void fParamHeating();
void fParamForging();
void fParamModulation();
void fParamCurrent();
void fCmnPrmStartPrg();
void fEditPrePressing();
void fEditPressing();
void fEditHeating();
void fEditForging();
void fEditModulation();
void fEditCurrent();
void fEditStartPrg();
void fEditMode();
void fEditPause();
BOOL CheckUpEditTime(u32 time);

//============================= eeprom ==============================
__attribute__((section(".eeprom")))u8 unused_var = 0;
__attribute__((section(".eeprom")))
u8 eeMass[programNumber][paramNum] =
{
// {PrePressing,	Pressing,	Heating,	Forging,	Modulation,	Current,	Mode,			Pause}
	{10,			3,			30,			30,			1,			1,			SIMPLE_MODE,	DEF_PAUSE},
	{10,			3,			30,			30,			2,			2,			SIMPLE_MODE,	DEF_PAUSE},
	{123,			75,			10,			234,			3,			7,			SIMPLE_MODE,	DEF_PAUSE},
	{10,			3,			30,			30,			4,			4,			SIMPLE_MODE,	DEF_PAUSE},
	{10,			3,			30,			30,			5,			5,			SIMPLE_MODE,	DEF_PAUSE},
	{10,			3,			30,			30,			6,			6,			SIMPLE_MODE,	DEF_PAUSE},
	{10,			3,			30,			30,			7,			7,			SIMPLE_MODE,	DEF_PAUSE},
	{10,			3,			30,			30,			8,			8,			SIMPLE_MODE,	DEF_PAUSE},
	{10,			3,			30,			30,			9,			9,			SIMPLE_MODE,	DEF_PAUSE},
	{10,			3,			30,			30,			0,			0,			SIMPLE_MODE,	DEF_PAUSE}
};
__attribute__((section(".eeprom")))u8 ee_startprg = 2;
//__attribute__((section(".eeprom")))u8 ee_mode = SIMPLE_MODE;
//__attribute__((section(".eeprom")))u8 ee_pause = MAX_PAUSE;
//=========================== строки меню ===========================
const u8 PROGMEM
	_Empty[]			= "                ",
#ifdef _ENGLISH_VERSION_
	_ViewInfo1[]		= "AutoWelding v1.0",
	_InfoAuto[]			= "Auto (Pause    )",
	_InfoSimple[]		= " Mode is Simple ",
	_ViewParams1[]		= "7 H: 30 M:5 I:3 ",
	_ViewParams2[]		= "Pr:450*30 Fg:110",
	_PrePressing[]		= "PrePressing   0 ",
	_Pressing[]			= "Pressing        ",
	_Heating[]			= "Heating         ",
	_Modulation[]		= "Modulation      ",
	_Forging[]			= "Forging         ",
	_Current[]			= "Current         ",
	_Editing[]			= "    Editing:    ",
	_ChooseMenu[]		= "Choose menu:    ",
	_ProgramStngs[]		= "Program settings",
	_CommonStngs[]		= "Common settings ",
	_ChooseParam[]		= "Choose parameter",
	_Mode[]				= "Mode            ",
	_Pause[]			= "Pause           ",
	_ParametrHas[]		= "  Parameter has ",
	_BeenSaved[]		= "   been saved!  ",
	_ParametrHasNot[]	= "Parameter hasn't",
	_WeldingIs[]		= " Welding is     ",
	_Completed[]		= "     completed! ",
	_StartPrg[]			= "Start Program 01",
	_PressingIs[]		= " Pressing is    ",
	_HeatingIs[]		= " Heating is     ",
	_ForgingIs[]		= " Forging is     ",
	_Auto[]				= "  is Auto",
	_Simple[]			= "is Simple",
	_Running[]			= "       running! ",
	_Ready[]			= "Ready! Push     ",
	_ForWelding[]		= "  pedal for weld",
	_Pause_[]			= "   Pause        ",
	_Splash1[]			= " Welcome to the ",
	_Splash2[]			= "   auto welding!",
	_Demo1[]			= " Demo version   ",
	_Demo2[]			= "   Demo version ";
#else
	_ViewInfo1[]		= "АвтоСварка  v1.0",
	_InfoAuto[]			= "Авто (Пауза    )",
	_InfoSimple[]		= "Режим Одиночный ",
	_ViewParams1[]		= "7 Н: 30 М:5 Т:3 ",
	_ViewParams2[]		= "Сж:450*30 Пр:110",
	_PrePressing[]		= "Предвар.Сж.   0 ",
	_Pressing[]			= "Сжатие          ",
	_Heating[]			= "Нагрев          ",
	_Modulation[]		= "Модуляция       ",
	_Forging[]			= "Проковка        ",
	_Current[]			= "Ток             ",
	_Editing[]			= "Редактирование: ",
	_ChooseMenu[]		= "Выбор меню:     ",
	_ProgramStngs[]		= "Настр. программ ",
	_CommonStngs[]		= "Общие настройки ",
	_ChooseParam[]		= "Выбор параметра ",
	_Mode[]				= "Режим           ",
	_Pause[]			= "Пауза           ",
	_ParametrHas[]		= "  Параметр      ",
	_BeenSaved[]		= "     сохранён!  ",
	_ParametrHasNot[]	= "Параметр не был ",
	_WeldingIs[]		= " Цикл сварки    ",
	_Completed[]		= "     завершён!  ",
	_StartPrg[]			= "Стартовая прг 01",
	_PressingIs[]		= " Сжатие         ",
	_HeatingIs[]		= " Нагрев         ",
	_ForgingIs[]		= " Проковка       ",
	_Auto[]				= "Автоматический",
	_Simple[]			= "     Одиночный",
	_Running[]			= "    в процессе! ",
	_Ready[]			= "Готово! Нажмите ",
	_ForWelding[]		= "педальдля сварки",
	_Pause_[]			= "   Пауза        ",
	_Splash1[]			= "Да прибудет с   ",
	_Splash2[]			= "вами сила, друг!",
	_Demo1[]			= " Демо версия    ",
	_Demo2[]			= "   Демо версия  ";
#endif


//=========================== элементы меню =========================
const MenuItem
	// Menu item					// menu id			// 1st string	// 2nd string	// menu's function
	mInfo				PROGMEM = { idInfo,				_ViewInfo1,		_InfoSimple,	fInfo },
	mPrograms			PROGMEM = { idPrograms,			_ViewParams1,	_ViewParams2,	fPrograms },
	mChoosePrgStngs		PROGMEM = { idChoosePrgStngs,	_ChooseMenu,	_ProgramStngs,	fChoosePrgStngs },
	mChooseCmnStngs		PROGMEM = { idChooseCmnStngs,	_ChooseMenu,	_CommonStngs,	fChooseCmnStngs },
	mParamMode			PROGMEM = { idChooseMode,		_ChooseParam,	_Mode,			fParamMode },
	mParamPause			PROGMEM = { idChoosePause,		_ChooseParam,	_Pause,			fParamPause },
	mCmnPrmStartPrg		PROGMEM = { idChooseStartPrg,	_CommonStngs,	_StartPrg,		fCmnPrmStartPrg },
	mParamPrePressing	PROGMEM = { idChoosePrePressing,_ChooseParam,	_PrePressing,	fParamPrePressing },
	mParamPressing		PROGMEM = { idChoosePressing,	_ChooseParam,	_Pressing,		fParamPressing },
	mParamHeating		PROGMEM = { idChooseHeating,	_ChooseParam,	_Heating,		fParamHeating },
	mParamForging		PROGMEM = { idChooseForging,	_ChooseParam,	_Forging,		fParamForging },
	mParamModulation	PROGMEM = { idChooseModulation,	_ChooseParam,	_Modulation,	fParamModulation },
	mParamCurrent		PROGMEM = { idChooseCurrent,	_ChooseParam,	_Current,		fParamCurrent },
	mEditPrePressing	PROGMEM = { idEditPrePressing,	_Editing,		_PrePressing,	fEditPrePressing },
	mEditPressing		PROGMEM = { idEditPressing,		_Editing,		_Pressing,		fEditPressing },
	mEditHeating		PROGMEM = { idEditHeating,		_Editing,		_Heating,		fEditHeating },
	mEditForging		PROGMEM = { idEditForging,		_Editing,		_Forging,		fEditForging },
	mEditModulation		PROGMEM = { idEditModulation,	_Editing,		_Modulation,	fEditModulation },
	mEditCurrent		PROGMEM = { idEditCurrent,		_Editing, 		_Current,		fEditCurrent },
	mEditStartPrg		PROGMEM = { idEditStartPrg,		_Editing, 		_StartPrg,		fEditStartPrg },
	mEditMode			PROGMEM = { idEditMode,			_Editing,		_Mode,			fEditMode },
	mEditPause			PROGMEM = { idEditPause,		_Editing,		_Pause,			fEditPause };

//====================== глобальные переменные ======================
extern volatile u32 nTimeMs;
extern volatile u32 nStartTime;
//=========================== функции меню ==========================
extern u8 get_key();
extern void WriteMenu(MenuItem* a_m);
extern void UpdateParams();
extern volatile tagFlags flags;
extern CURPRG curPrg;
extern CURMODE curMode;
extern void UpdateLcdParam(u8 a_ParamsId, u8 a_nVal);
extern u8 GetValue(u8 a_nParamId);
extern void SetValue(u8 a_nParamId, u8 a_nVal);
extern void SavedDlg(u8 a_bSaved);
extern void SetMenu(const MenuItem* a_curMenu); // установка текущего меню из других файлов
extern void DoMenu();
extern u8 readByteEE(u16 addr);
extern BOOL is_time_expired(u32 time);
extern void NoteTime();
extern u8 getCurMenuId();

void fInfo()
{
	wdt_start(wdt_2s);
	NoteTime();
	BOOL b = FALSE;
	for(u8 i = 0; i < 3; i++)
	{
		while(is_time_expired(1000) == FALSE)
		{
			if (get_key() != keyEmpty)
			{
				b = TRUE;
				break;
			}
		}
		if (b == TRUE)
			break;
		NoteTime();
		wdt_feed();
	}
	SetMenu(&mPrograms);
	wdt_start(wdt_60ms);
}
void fPrograms()
{
	if (flags.scanKey)
	{
		switch(get_key())
		{
			case keyLeft:
				if (curPrg.get() <= firstPrg)
					curPrg.set(lastPrg);
				else
					curPrg.set(curPrg.get()-1);
				UpdateParams();
			break;
			case keyRight:
				if (curPrg.get() >= lastPrg)
					curPrg.set(firstPrg);
				else
					curPrg.set(curPrg.get()+1);
				UpdateParams();
			break;
			case keyUp:
				SetMenu(&mInfo);
			break;
			case keyDown:
				SetMenu(&mChoosePrgStngs);
			break;
		}
	}
}
void fChoosePrgStngs()
{
	if (flags.scanKey)
	{
		switch(get_key())
		{
			case keyLeft:
			case keyRight:
				SetMenu(&mChooseCmnStngs);
			break;
			case keyUp:
				SetMenu(&mPrograms);
			break;
			case keyDown:
				SetMenu(&mParamPrePressing);
			break;
		}
	}
}
void fChooseCmnStngs()
{
	if (flags.scanKey)
	{
		switch(get_key())
		{
			case keyLeft:
			case keyRight:
				SetMenu(&mChoosePrgStngs);
			break;
			case keyUp:
				SetMenu(&mPrograms);
			break;
			case keyDown:
				SetMenu(&mCmnPrmStartPrg);
			break;
		}
	}
}
void fParamMode()
{
	if (flags.scanKey)
	{
		switch(get_key())
		{
			case keyLeft:
				SetMenu(&mParamCurrent);
			break;
			case keyRight:
				if (curMode.get() == AUTO_MODE)
					SetMenu(&mParamPause);
				else
					SetMenu(&mParamPrePressing);
			break;
			case keyUp:
				SetMenu(&mChoosePrgStngs);
			break;
			case keyDown:
				SetMenu(&mEditMode);
			break;
		}
	}
}
void fParamPause()
{
	if (flags.scanKey)
	{
		switch(get_key())
		{
			case keyLeft:
				SetMenu(&mParamMode);
			break;
			case keyRight:
				SetMenu(&mParamPrePressing);
			break;
			case keyUp:
				SetMenu(&mChoosePrgStngs);
			break;
			case keyDown:
				SetMenu(&mEditPause);
			break;
		}
	}
}
void fCmnPrmStartPrg()
{
	if (flags.scanKey)
		switch(get_key())
		{
			/*case keyLeft:
				if (curMode.get() == AUTO_MODE)
					SetMenu(&mParamPause);
				else
					SetMenu(&mParamMode);
			break;
			case keyRight:
				SetMenu(&mParamPrePressing);
			break;*/
			case keyUp:
				SetMenu(&mChooseCmnStngs);
			break;
			case keyDown:
				SetMenu(&mEditStartPrg);
			break;
		}
}
void fParamPrePressing()
{
	if (flags.scanKey)
		switch(get_key())
		{
			case keyLeft:
				if (curMode.get() == AUTO_MODE)
					SetMenu(&mParamPause);
				else
					SetMenu(&mParamMode);
			break;
			case keyRight:
				SetMenu(&mParamPressing);
			break;
			case keyUp:
				SetMenu(&mChoosePrgStngs);
			break;
			case keyDown:
				SetMenu(&mEditPrePressing);
			break;
		}
}
void fParamPressing()
{
	if (flags.scanKey)
		switch(get_key())
		{
			case keyLeft:
				SetMenu(&mParamPrePressing);
			break;
			case keyRight:
				SetMenu(&mParamHeating);
			break;
			case keyUp:
				SetMenu(&mChoosePrgStngs);
			break;
			case keyDown:
				SetMenu(&mEditPressing);
			break;
		}
}
void fParamHeating()
{
	if (flags.scanKey)
		switch(get_key())
		{
			case keyLeft:
				SetMenu(&mParamPressing);
			break;
			case keyRight:
				SetMenu(&mParamForging);
			break;
			case keyUp:
				SetMenu(&mChoosePrgStngs);
			break;
			case keyDown:
				SetMenu(&mEditHeating);
			break;
		}
}
void fParamForging()
{
	if (flags.scanKey)
		switch(get_key())
		{
			case keyLeft:
				SetMenu(&mParamHeating);
			break;
			case keyRight:
				SetMenu(&mParamModulation);
			break;
			case keyUp:
				SetMenu(&mChoosePrgStngs);
			break;
			case keyDown:
				SetMenu(&mEditForging);
			break;
		}
}
void fParamModulation()
{
	if (flags.scanKey)
		switch(get_key())
		{
			case keyLeft:
				SetMenu(&mParamForging);
			break;
			case keyRight:
				SetMenu(&mParamCurrent);
			break;
			case keyUp:
				SetMenu(&mChoosePrgStngs);
			break;
			case keyDown:
				SetMenu(&mEditModulation);
			break;
		}
}
void fParamCurrent()
{
	if (flags.scanKey)
		switch(get_key())
		{
			case keyLeft:
				SetMenu(&mParamModulation);
			break;
			case keyRight:
				SetMenu(&mParamMode);
			break;
			case keyUp:
				SetMenu(&mChoosePrgStngs);
			break;
			case keyDown:
				SetMenu(&mEditCurrent);
			break;
		}
}
void fEditPrePressing()
{
	wdt_start(wdt_60ms);
	u8 val = GetValue(paramPrePressing);
	u8 oldVal = val;
	if (val > maxPrePressing)
	{
		val = minPrePressing;
		UpdateLcdParam(paramPrePressing, val);
	}
	NoteTime();
	while(1)
	{
		if (flags.scanKey)
		{
			wdt_feed();
			switch(get_key())
			{
				case keyLeft:
					if (val <= minPrePressing)
						val = maxPrePressing;
					else
						val--;
					UpdateLcdParam(paramPrePressing, val);
				break;
				case keyRight:
					if (val >= maxPrePressing)
						val = minPrePressing;
					else
						val++;
					UpdateLcdParam(paramPrePressing, val);
				break;
				case keyUp:
					SavedDlg(0);
					SetMenu(&mParamPrePressing);
				return;
				case keyDown:
					if (val != oldVal)
						SetValue(paramPrePressing, val);
					SavedDlg(1);
					SetMenu(&mParamPrePressing);
				return;
			}
		}			
		if (CheckUpEditTime(TIME_FOR_SAVE) == TRUE)
			return;
	}
}
void fEditPressing()
{
	wdt_start(wdt_60ms);
	u8 val = GetValue(paramPressing);
	u8 oldVal = val;
	if (val > maxPressing)
	{
		val = minPressing;
		UpdateLcdParam(paramPressing, val);
	}
	NoteTime();
	while(1)
	{
		if (flags.scanKey)
		{
			wdt_feed();
			switch(get_key())
			{
				case keyLeft:
					if (val <= minPressing)
						val = maxPressing;
					else
						val--;
					UpdateLcdParam(paramPressing, val);
				break;
				case keyRight:
					if (val >= maxPressing)
						val = minPressing;
					else
						val++;
					UpdateLcdParam(paramPressing, val);
				break;
				case keyUp:
					SavedDlg(0);
					SetMenu(&mParamPressing);
				return;
				case keyDown:
					if (val != oldVal)
						SetValue(paramPressing, val);
					SavedDlg(1);
					SetMenu(&mParamPressing);
				return;
			}
		}			
		if (CheckUpEditTime(TIME_FOR_SAVE) == TRUE)
			return;
	}
}
void fEditHeating()
{
	wdt_start(wdt_60ms);
	u8 val = GetValue(paramHeating);
	u8 oldVal = val;
	if (val > maxHeating)
	{
		val = minHeating;
		UpdateLcdParam(paramHeating, val);
	}
	while(1)
	{
		if (flags.scanKey)
		{
			wdt_feed();
			switch(get_key())
			{
				case keyLeft:
					if (val <= minHeating)
						val = maxHeating;
					else
						val--;
					UpdateLcdParam(paramHeating, val);
				break;
				case keyRight:
					if (val >= maxHeating)
						val = minHeating;
					else
						val++;
					UpdateLcdParam(paramHeating, val);
				break;
				case keyUp:
					SavedDlg(0);
					SetMenu(&mParamHeating);
				return;
				case keyDown:
					if (val != oldVal)
						SetValue(paramHeating, val);
					SavedDlg(1);
					SetMenu(&mParamHeating);
				return;
			}
		}			
		if (CheckUpEditTime(TIME_FOR_SAVE) == TRUE)
			return;
	}
}
void fEditForging()
{
	wdt_start(wdt_60ms);
	u8 val = GetValue(paramForging);
	u8 oldVal = val;
	if (val > maxForging)
	{
		val = minForging;
		UpdateLcdParam(paramForging, val);
	}
	while(1)
	{
		if (flags.scanKey)
		{
			wdt_feed();
			switch(get_key())
			{
				case keyLeft:
					if (val <= minForging)
						val = maxForging;
					else
						val--;
					UpdateLcdParam(paramForging, val);
				break;
				case keyRight:
					if (val >= maxForging)
						val = minForging;
					else
						val++;
					UpdateLcdParam(paramForging, val);
				break;
				case keyUp:
					SavedDlg(0);
					SetMenu(&mParamForging);
				return;
				case keyDown:
					if (val != oldVal)
						SetValue(paramForging, val);
					SavedDlg(1);
					SetMenu(&mParamForging);
				return;
			}
		}			
		if (CheckUpEditTime(TIME_FOR_SAVE) == TRUE)
			return;
	}
}
void fEditModulation()
{
	wdt_start(wdt_60ms);
	u8 val = GetValue(paramModulation);
	u8 oldVal = val;
	if (val > maxModulation)
	{
		val = minModulation;
		UpdateLcdParam(paramModulation, val);
	}
	while(1)
	{
		if (flags.scanKey)
		{
			wdt_feed();
			switch(get_key())
			{
				case keyLeft:
					if (val <= minModulation)
						val = maxModulation;
					else
						val--;
					UpdateLcdParam(paramModulation, val);
				break;
				case keyRight:
					if (val >= maxModulation)
						val = minModulation;
					else
						val++;
					UpdateLcdParam(paramModulation, val);
				break;
				case keyUp:
					SavedDlg(0);
					SetMenu(&mParamModulation);
				return;
				case keyDown:
					if (val != oldVal)
						SetValue(paramModulation, val);
					SavedDlg(1);
					SetMenu(&mParamModulation);
				return;
			}
		}
		if (CheckUpEditTime(TIME_FOR_SAVE) == TRUE)
			return;
	}
}
void fEditCurrent()
{
	wdt_start(wdt_60ms);
	u8 val = GetValue(paramCurrent);
	u8 oldVal = val;
	if (val > maxCurrent)
	{
		val = minCurrent;
		UpdateLcdParam(paramCurrent, val);
	}
	while(1)
	{
		if (flags.scanKey)
		{
			wdt_feed();
			switch(get_key())
			{
				case keyLeft:
					if (val <= minCurrent)
						val = maxCurrent;
					else
						val--;
					UpdateLcdParam(paramCurrent, val);
				break;
				case keyRight:
					if (val >= maxCurrent)
						val = minCurrent;
					else
						val++;
					UpdateLcdParam(paramCurrent, val);
				break;
				case keyUp:
					SavedDlg(0);
					SetMenu(&mParamCurrent);
				return;
				case keyDown:
					if (val != oldVal)
						SetValue(paramCurrent, val);
					SavedDlg(1);
					SetMenu(&mParamCurrent);
				return;
			}
		}			
		if (CheckUpEditTime(TIME_FOR_SAVE) == TRUE)
			return;
	}
}

void fEditStartPrg()
{
	wdt_start(wdt_60ms);
	u8 val = readByteEE(addrStartPrg);
	u8 oldVal = val;
	if (val > lastPrg)
	{
		val = firstPrg;
		UpdateLcdParam(cmnprmStartPrg, val);
	}
	while(1)
	{
		if (flags.scanKey)
		{
			wdt_feed();
			switch(get_key())
			{
				case keyLeft:
					if (val <= firstPrg)
						val = lastPrg;
					else
						val--;
					UpdateLcdParam(cmnprmStartPrg, val);
				break;
				case keyRight:
					if (val >= lastPrg)
						val = firstPrg;
					else
						val++;
					UpdateLcdParam(cmnprmStartPrg, val);
				break;
				case keyUp:
					SavedDlg(0);
					SetMenu(&mCmnPrmStartPrg);
				return;
				case keyDown:
					if (val != oldVal)
						SetValue(cmnprmStartPrg, val);
					SavedDlg(1);
					SetMenu(&mCmnPrmStartPrg);
				return;
			}
		}			
		if (CheckUpEditTime(TIME_FOR_SAVE) == TRUE)
			return;
	}
}
void fEditMode()
{
	wdt_start(wdt_60ms);
	u8 val = readByteEE(curPrg.get() * paramNum + addrMode);
	u8 oldVal = val;
	if (val > LAST_MODE)
	{
		val = SIMPLE_MODE;
		UpdateLcdParam(paramMode, val);
	}
	while(1)
	{
		if (flags.scanKey)
		{
			wdt_feed();
			switch(get_key())
			{
				case keyLeft:
					val = SIMPLE_MODE;
					UpdateLcdParam(paramMode, val);
				break;
				case keyRight:
					val = AUTO_MODE;
					UpdateLcdParam(paramMode, val);
				break;
				case keyUp:
					SavedDlg(0);
					SetMenu(&mParamMode);
				return;
				case keyDown:
					if (val != oldVal)
						SetValue(paramMode, val);
					SavedDlg(1);
					SetMenu(&mParamMode);
				return;
			}
		}			
		if (CheckUpEditTime(TIME_FOR_SAVE) == TRUE)
			return;
	}
}
void fEditPause()
{
	wdt_start(wdt_60ms);
	u8 val = readByteEE(curPrg.get() * paramNum + addrPause);
	u8 oldVal = val;
	if (val > MAX_PAUSE || val < MIN_PAUSE)
	{
		val = SIMPLE_MODE;
		UpdateLcdParam(paramPause, val);
	}
	while(1)
	{
		if (flags.scanKey)
		{
			wdt_feed();
			switch(get_key())
			{
				case keyLeft:
					if (val <= MIN_PAUSE)
						val = MAX_PAUSE;
					else
						val--;
					UpdateLcdParam(paramPause, val);
				break;
				case keyRight:
					if (val >= MAX_PAUSE)
						val = MIN_PAUSE;
					else
						val++;
					UpdateLcdParam(paramPause, val);
				break;
				case keyUp:
					SavedDlg(0);
					SetMenu(&mParamPause);
				return;
				case keyDown:
					if (val != oldVal)
						SetValue(paramPause, val);
					SavedDlg(1);
					SetMenu(&mParamPause);
				return;
			}
		}			
		if (CheckUpEditTime(TIME_FOR_SAVE) == TRUE)
			return;
	}
}

BOOL CheckUpEditTime(u32 time)
{
	if (is_time_expired(time) == TRUE)
	{
		SavedDlg(0);
		SetMenu(&mPrograms);
		return TRUE;
	}
	return FALSE;
}
