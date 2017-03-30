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
void fCmnPrmPedalNum();
void fCmnPrmBrtns();
void fEditPrePressing();
void fEditPressing();
void fEditHeating();
void fEditForging();
void fEditModulation();
void fEditCurrent();
void fEditStartPrg();
void fEditPedalNum();
void fEditBrtns();
void fEditMode();
void fEditPause();
BOOL CheckUpEditTime(u32 time);

//============================= eeprom ==============================
__attribute__((section(".eeprom")))u8 unused_var = 0;
__attribute__((section(".eeprom")))
u8 eeMass[programNumber][paramNum] =
{
// {PrePressing,	Pressing,	Modulation,	Current,	Heating,	Forging,	Mode,			Pause}
	{10,			11,			1,			0,			40,			50,			SIMPLE_MODE,	DEF_PAUSE},
	{20,			21,			2,			9,			41,			51,			SIMPLE_MODE,	DEF_PAUSE},
	{30,			31,			3,			8,			42,			52,			SIMPLE_MODE,	DEF_PAUSE},
	{40,			41,			4,			7,			43,			53,			SIMPLE_MODE,	DEF_PAUSE},
	{50,			51,			5,			6,			44,			54,			SIMPLE_MODE,	DEF_PAUSE},
	{60,			61,			6,			5,			45,			55,			SIMPLE_MODE,	DEF_PAUSE},
	{70,			71,			7,			4,			46,			56,			SIMPLE_MODE,	DEF_PAUSE},
	{80,			81,			8,			3,			47,			57,			SIMPLE_MODE,	DEF_PAUSE},
	{90,			91,			9,			2,			48,			58,			SIMPLE_MODE,	DEF_PAUSE},
	{11,			21,			0,			1,			49,			59,			SIMPLE_MODE,	DEF_PAUSE}
};
__attribute__((section(".eeprom")))u8 ee_startprg = 2;
__attribute__((section(".eeprom")))u8 ee_pedalnum = 2;
__attribute__((section(".eeprom")))u8 ee_brtns = ON;
//=========================== строки меню ===========================
const char PROGMEM
	_Empty[]			= "                ",
#ifndef _RUSSIAN_VERSION_
	_ViewInfo1[]		= "Version 17.03.17",
	_InfoAuto[]			= "Auto (Pause    )",
	_InfoSeam[]			= "Mode is Seam    ",
	_InfoSimple[]		= "Mode is Simple  ",
	_ViewParams1[]		= "S7 Pr:450*30 M:5",
	_ViewParams2[]		= "I:3 H: 30 Fg:110",
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
	_PedalNum[]			= "Pedal number   2",
	_Brightness[]		= " Brightness   On",
	_PressingIs[]		= " Pressing is    ",
	_HeatingIs[]		= " Heating is     ",
	_ForgingIs[]		= " Forging is     ",
	_Simple[]			= "is Simple",
	_Auto[]				= "  is Auto",
	_Seam[]				= "  is Seam",
	_Running[]			= "       running! ",
	_Ready[]			= "Ready! Push     ",
	_ForWelding[]		= "  pedal for weld",
	_Pause_[]			= "   Pause        ",
	_Splash1[]			= " Welcome to the ",
	_Splash2[]			= "   auto welding!",
	_Demo1[]			= " Demo version   ",
	_Demo2[]			= "   Demo version ",
	_SignalAbscent[]	= "Synchronization ",
	_Synch[]			= "      is absent!",
	_On[]				= " On",
	_Off[]				= "Off"
	_Attention[]		= "ATTENTION       ",
	_Alarm[]			= "        ALARM!!!",
	_Checkup[]			= "Check up        ",
	_Equipnent[]		= "    an equipment";
#else
	_ViewInfo1[]		= "Версия 29.03.17R",
	_InfoAuto[]			= "Цикл (Пауза    )",
	_InfoSeam[]			= "Режим Шовный    ",
	_InfoSimple[]		= "Режим Одиночный ",
	_ViewParams1[]		= "Ш7 С:150*150 М:9",
	_ViewParams2[]		= "Т:3 Н:150 Пр:150",
	_PrePressing[]		= "Предсжатие      ",
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
	_BeenSaved[]		= "     сохран$н!  ",
	_ParametrHasNot[]	= "Параметр не был ",
	_WeldingIs[]		= " Цикл сварки    ",
	_Completed[]		= "     заверш$н!  ",
	_StartPrg[]			= "Стартовая прг 01",
	_PedalNum[]			= "Всего педалей  2",
	_Brightness[]		= "Подсветка       ",
	_PressingIs[]		= " Сжатие         ",
	_HeatingIs[]		= " Нагрев         ",
	_ForgingIs[]		= " Проковка       ",
	_Simple[]			= "Одиночный",
	_Auto[]				= "   Цикл  ",
	_Seam[]				= "  Шовный ",
	_Running[]			= "    в процессе! ",
	_Ready[]			= "Готово! Нажмите ",
	_ForWelding[]		= "педальдля сварки",
	_Pause_[]			= "   Пауза        ",
	_Splash1[]			= "Да будет ваш    ",
	_Splash2[]			= "  шов крепким!!!",
	_Demo1[]			= " Демо версия    ",
	_Demo2[]			= "   Демо версия  ",
	_SignalAbscent[]	= "Нет сигнала     ",
	_Synch[]			= "  синхронизации!",
	_On[]				= " Вкл",
	_Off[]				= "Выкл",
	_Attention[]		= "ВНИМАНИЕ        ",
	_Alarm[]			= "       АВАРИЯ!!!",
	_Checkup[]			= "Проверьте       ",
	_Equipnent[]		= "    оборудование";

#endif


//=========================== элементы меню =========================
const MenuItem
	// Menu item					// menu id			// 1st string	// 2nd string	// menu's function
	mInfo				PROGMEM = { idInfo,				_ViewInfo1,		_InfoSimple,	fInfo },
	mPrograms			PROGMEM = { idPrograms,			_ViewParams1,	_ViewParams2,	fPrograms },
	mChoosePrgStngs		PROGMEM = { idChoosePrgStngs,	_ChooseMenu,	_ProgramStngs,	fChoosePrgStngs },
	mChooseCmnStngs		PROGMEM = { idChooseCmnStngs,	_ChooseMenu,	_CommonStngs,	fChooseCmnStngs },
	mCmnPrmStartPrg		PROGMEM = { idChooseStartPrg,	_CommonStngs,	_StartPrg,		fCmnPrmStartPrg },
	mCmnPrmPedalNum		PROGMEM = { idChoosePedalNum,	_CommonStngs,	_PedalNum,		fCmnPrmPedalNum },
	mCmnPrmBrtns		PROGMEM = { idChooseBrightness,	_CommonStngs,	_Brightness,	fCmnPrmBrtns	},
	mParamMode			PROGMEM = { idChooseMode,		_ChooseParam,	_Mode,			fParamMode },
	mParamPrePressing	PROGMEM = { idChoosePrePressing,_ChooseParam,	_PrePressing,	fParamPrePressing },
	mParamPressing		PROGMEM = { idChoosePressing,	_ChooseParam,	_Pressing,		fParamPressing },
	mParamModulation	PROGMEM = { idChooseModulation,	_ChooseParam,	_Modulation,	fParamModulation },
	mParamCurrent		PROGMEM = { idChooseCurrent,	_ChooseParam,	_Current,		fParamCurrent },
	mParamHeating		PROGMEM = { idChooseHeating,	_ChooseParam,	_Heating,		fParamHeating },
	mParamForging		PROGMEM = { idChooseForging,	_ChooseParam,	_Forging,		fParamForging },
	mParamPause			PROGMEM = { idChoosePause,		_ChooseParam,	_Pause,			fParamPause },
	mEditStartPrg		PROGMEM = { idEditStartPrg,		_Editing, 		_StartPrg,		fEditStartPrg },
	mEditPedalNum		PROGMEM = { idEditPedalNum,		_Editing, 		_PedalNum,		fEditPedalNum },
	mEditBrtns			PROGMEM = {	idEditBrightness,	_Editing,		_Brightness,	fEditBrtns },
	mEditMode			PROGMEM = { idEditMode,			_Editing,		_Mode,			fEditMode },
	mEditPrePressing	PROGMEM = { idEditPrePressing,	_Editing,		_PrePressing,	fEditPrePressing },
	mEditPressing		PROGMEM = { idEditPressing,		_Editing,		_Pressing,		fEditPressing },
	mEditModulation		PROGMEM = { idEditModulation,	_Editing,		_Modulation,	fEditModulation },
	mEditCurrent		PROGMEM = { idEditCurrent,		_Editing, 		_Current,		fEditCurrent },
	mEditHeating		PROGMEM = { idEditHeating,		_Editing,		_Heating,		fEditHeating },
	mEditForging		PROGMEM = { idEditForging,		_Editing,		_Forging,		fEditForging },
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
extern void switchBrightness(u8 a_state);

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
				SetMenu(&mParamMode);
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
void fCmnPrmStartPrg()
{
	if (flags.scanKey)
		switch(get_key())
		{
			case keyLeft:
				SetMenu(&mCmnPrmPedalNum);
			break;
			case keyRight:
				SetMenu(&mCmnPrmBrtns);
			break;
			case keyUp:
				SetMenu(&mChooseCmnStngs);
			break;
			case keyDown:
				SetMenu(&mEditStartPrg);
			break;
		}
}
void fCmnPrmPedalNum()
{
	if (flags.scanKey)
		switch(get_key())
		{
			case keyLeft:
				SetMenu(&mCmnPrmBrtns);
			break;
			case keyRight:
				SetMenu(&mCmnPrmStartPrg);
			break;
			case keyUp:
				SetMenu(&mChooseCmnStngs);
			break;
			case keyDown:
				SetMenu(&mEditPedalNum);
			break;
		}
}
void fCmnPrmBrtns()
{
	if (flags.scanKey)
		switch (get_key())
		{
			case keyLeft:
				SetMenu(&mCmnPrmStartPrg);
			break;
			case keyRight:
				SetMenu(&mCmnPrmPedalNum);
			break;
			case keyUp:
				SetMenu(&mChooseCmnStngs);
			break;
			case keyDown:
				SetMenu(&mEditBrtns);
			break;
		}
}
void fParamMode()
{
	if (flags.scanKey)
	{
		switch(get_key())
		{
			case keyLeft:
				if (curMode.get() == AUTO_MODE)
					SetMenu(&mParamPause);
				else
					SetMenu(&mParamForging);
			break;
			case keyRight:
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
void fParamPrePressing()
{
	if (flags.scanKey)
		switch(get_key())
		{
			case keyLeft:
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
				SetMenu(&mParamModulation);
			break;
			case keyUp:
				SetMenu(&mChoosePrgStngs);
			break;
			case keyDown:
				SetMenu(&mEditPressing);
			break;
		}
}
void fParamModulation()
{
	if (flags.scanKey)
	switch(get_key())
	{
		case keyLeft:
		SetMenu(&mParamPressing);
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
		SetMenu(&mParamHeating);
		break;
		case keyUp:
		SetMenu(&mChoosePrgStngs);
		break;
		case keyDown:
		SetMenu(&mEditCurrent);
		break;
	}
}
void fParamHeating()
{
	if (flags.scanKey)
		switch(get_key())
		{
			case keyLeft:
				SetMenu(&mParamCurrent);
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
				if (curMode.get() == AUTO_MODE)
					SetMenu(&mParamPause);
				else
					SetMenu(&mParamMode);
			break;
			case keyUp:
				SetMenu(&mChoosePrgStngs);
			break;
			case keyDown:
				SetMenu(&mEditForging);
			break;
		}
}
void fParamPause()
{
	if (flags.scanKey)
	{
		switch(get_key())
		{
			case keyLeft:
				SetMenu(&mParamForging);
			break;
			case keyRight:
				SetMenu(&mParamMode);
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
	u8 val = readByteEE((u16)&ee_startprg);
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

void fEditPedalNum()
{
	wdt_start(wdt_60ms);
	u8 val = readByteEE((u16)&ee_pedalnum);
	u8 oldVal = val;
	if (val < minPedalNum || val > maxPedalNum)
	{
		val = minPedalNum;
		UpdateLcdParam(cmnprmPedalNum, val);
	}
	while(1)
	{
		if (flags.scanKey)
		{
			wdt_feed();
			switch(get_key())
			{
				case keyLeft:
					val = minPedalNum;
					UpdateLcdParam(cmnprmPedalNum, val);
					break;
				case keyRight:
					val = maxPedalNum;
					UpdateLcdParam(cmnprmPedalNum, val);
					break;
				case keyUp:
					SavedDlg(0);
					SetMenu(&mCmnPrmPedalNum);
					return;
				case keyDown:
					if (val != oldVal)
						SetValue(cmnprmPedalNum, val);
					SavedDlg(1);
					SetMenu(&mCmnPrmPedalNum);
				return;
			}
		}
		if (CheckUpEditTime(TIME_FOR_SAVE) == TRUE)
		return;
	}
}
void fEditBrtns()
{
	wdt_start(wdt_60ms);
	u8 val = readByteEE((u16)&ee_brtns);
	u8 oldVal = val;
	if (val != ON && val != OFF)
	{
		val = ON;
		UpdateLcdParam(cmnprmBrtns, val);
	}
	while(1)
	{
		if (flags.scanKey)
		{
			wdt_feed();
			switch(get_key())
			{
				case keyLeft:
					val = OFF;
					UpdateLcdParam(cmnprmBrtns, val);
					switchBrightness(OFF);
				break;
				case keyRight:
					val = ON;
					UpdateLcdParam(cmnprmBrtns, val);
					switchBrightness(ON);
				break;
				case keyUp:
					SavedDlg(0);
					SetMenu(&mCmnPrmBrtns);
					switchBrightness(oldVal);
				return;
				case keyDown:
					if (val != oldVal)
					SetValue(cmnprmBrtns, val);
					SavedDlg(1);
					SetMenu(&mCmnPrmBrtns);
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
	u8 val = readByteEE((u16)&eeMass + curPrg.get() * paramNum + addrMode);
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
					if (val > FIRST_MODE)
						val--;
					UpdateLcdParam(paramMode, val);
				break;
				case keyRight:
					if (val < LAST_MODE)
						val++;
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
	u8 val = readByteEE((u16)&eeMass + curPrg.get() * paramNum + addrPause);
	u8 oldVal = val;
	if (val > MAX_PAUSE || val < MIN_PAUSE)
	{
		val = MIN_PAUSE;
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
