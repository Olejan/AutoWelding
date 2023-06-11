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
void fCmnPrmModbusId();
void fEditPrePressing();
void fEditPressing();
void fEditHeating();
void fEditForging();
void fEditModulation();
void fEditCurrent();
void fEditStartPrg();
void fEditPedalNum();
void fEditBrtns();
void fEditModbusId();
void fEditMode();
void fEditPause();
BOOL CheckUpEditTime(u32 time);

//============================= eeprom ==============================
__attribute__((section(".eeprom")))u8 unused_var = 0;
__attribute__((section(".eeprom")))
u8 eeMass[programNumber][paramNum] =
{
//	{PrePressing,		Pressing,		Modulation,		Current,		Heating,		Forging,		Mode,			Pause}
	{defPrePressing,	defPressing,	defModulation,	defCurrent,		defHeating,		defForging,		SIMPLE_MODE,	DEF_PAUSE},
	{defPrePressing,	defPressing,	defModulation,	defCurrent,		defHeating,		defForging,		SIMPLE_MODE,	DEF_PAUSE},
	{defPrePressing,	defPressing,	defModulation,	defCurrent,		defHeating,		defForging,		SIMPLE_MODE,	DEF_PAUSE},
	{defPrePressing,	defPressing,	defModulation,	defCurrent,		defHeating,		defForging,		SIMPLE_MODE,	DEF_PAUSE},
	{defPrePressing,	defPressing,	defModulation,	defCurrent,		defHeating,		defForging,		SIMPLE_MODE,	DEF_PAUSE},
	{defPrePressing,	defPressing,	defModulation,	defCurrent,		defHeating,		defForging,		SIMPLE_MODE,	DEF_PAUSE},
	{defPrePressing,	defPressing,	defModulation,	defCurrent,		defHeating,		defForging,		SIMPLE_MODE,	DEF_PAUSE},
	{defPrePressing,	defPressing,	defModulation,	defCurrent,		defHeating,		defForging,		SIMPLE_MODE,	DEF_PAUSE},
	{defPrePressing,	defPressing,	defModulation,	defCurrent,		defHeating,		defForging,		SIMPLE_MODE,	DEF_PAUSE},
	{defPrePressing,	defPressing,	defModulation,	defCurrent,		defHeating,		defForging,		SIMPLE_MODE,	DEF_PAUSE}
};
__attribute__((section(".eeprom")))u8 ee_startprg = DEF_START_PRG;
__attribute__((section(".eeprom")))u8 ee_pedalnum = DEF_PEDAL_NUM;
__attribute__((section(".eeprom")))u8 ee_brtns = ON;
__attribute__((section(".eeprom")))u8 ee_modbus_id = MAX_MODBUS_ID;
//=========================== строки меню ===========================
const char PROGMEM
	_Empty[]			= "                ",
	_ModbusId[]			= "Modbus ID    247",
#ifndef _RUSSIAN_VERSION_
	_ViewInfo1[]		= "Ver.  11.06.23T ",
	_InfoAuto[]			= "Auto (Pause    )",
	_InfoSeam[]			= "Mode is Seam    ",
	_InfoSimple[]		= "Mode is Simple  ",
	_ViewParams1[]		= "S7 Pr:450*30 M:5",
	_ViewParams2[]		= "I:30 H: 30 F:110",
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
	_SignalAbscent[]	= "Synchronization ",
	_Synch[]			= "      is absent!",
	_On[]				= " On",
	_Off[]				= "Off",
	_Attention[]		= "ATTENTION       ",
	_Alarm[]			= "        ALARM!!!",
	_Checkup[]			= "Check up        ",
	_Equipnent[]		= "    an equipment",
	_PedalIsPressed[]	= "Pedal is pressed",
	_ReleaseIt[]		= "  - release it  ";
#else
#ifdef MVL
	_ViewInfo1[]		= "Версия 11.06.23M",
#else
	_ViewInfo1[]		= "Версия 11.06.23T",
#endif
	_InfoAuto[]			= "Цикл (Пауза    )",
	_InfoSeam[]			= "Режим Шовный    ",
	_InfoSimple[]		= "Режим Одиночный ",
	_ViewParams1[]		= "Ш7 С:150*150 М:9",
	_ViewParams2[]		= "Т:30 Н:150 П:150",
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
	_SignalAbscent[]	= "Нет сигнала     ",
	_Synch[]			= "  синхронизации!",
	_On[]				= " Вкл",
	_Off[]				= "Выкл",
	_Attention[]		= "ВНИМАНИЕ        ",
	_Alarm[]			= "       АВАРИЯ!!!",
	_Checkup[]			= "Проверьте       ",
	_Equipnent[]		= "    оборудование",
	_PedalIsPressed[]	= "Нажата педаль - ",
	_ReleaseIt[]		= "    отпустите е$";

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
	mCmnPrmModbusId		PROGMEM = { idChooseModbusId,	_CommonStngs,	_ModbusId,		fCmnPrmModbusId },
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
	mEditModbusId		PROGMEM = { idEditModbusId,		_Editing,		_ModbusId,		fEditModbusId },
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
extern u8 _nTaskAlarm;
extern void UpdateLcdParam(u8 a_ParamsId, u8 a_nVal);
extern u8 GetValue(u8 a_nParamId);
extern BOOL SetValue(u8 a_nParamId, u8 a_nVal);
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
void set_sqreen_menu(const MenuItem* left_m, const MenuItem* right_m, const MenuItem* up_m, const MenuItem* down_m)
{
	if (flags.scanKey)
	{
		switch(get_key())
		{
			case keyLeft:
				SetMenu(left_m);
				break;
			case keyRight:
				SetMenu(right_m);
			break;
			case keyUp:
				SetMenu(up_m);
			break;
			case keyDown:
				SetMenu(down_m);
			break;
		}
	}
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
	set_sqreen_menu(&mChooseCmnStngs, &mChooseCmnStngs, &mPrograms, &mParamMode);
}
void fChooseCmnStngs()
{
	set_sqreen_menu(&mChoosePrgStngs, &mChoosePrgStngs, &mPrograms, &mCmnPrmStartPrg);
}
void fCmnPrmStartPrg()
{
	set_sqreen_menu(&mCmnPrmModbusId, &mCmnPrmBrtns, &mChooseCmnStngs, &mEditStartPrg);
}
void fCmnPrmBrtns()
{
	set_sqreen_menu(&mCmnPrmStartPrg, &mCmnPrmPedalNum, &mChooseCmnStngs, &mEditBrtns);
}
void fCmnPrmPedalNum()
{
	set_sqreen_menu(&mCmnPrmBrtns, &mCmnPrmModbusId, &mChooseCmnStngs, &mEditPedalNum);
}
void fCmnPrmModbusId()
{
	set_sqreen_menu(&mCmnPrmPedalNum, &mCmnPrmStartPrg, &mChooseCmnStngs, &mEditModbusId);
}
void fParamMode()
{
	set_sqreen_menu(curMode.get() == AUTO_MODE ? &mParamPause : &mParamForging,
		&mParamPrePressing,
		&mChoosePrgStngs,
		&mEditMode);
}
void fParamPrePressing()
{
	set_sqreen_menu(&mParamMode, &mParamPressing, &mChoosePrgStngs, &mEditPrePressing);
}
void fParamPressing()
{
	set_sqreen_menu(&mParamPrePressing, &mParamModulation, &mChoosePrgStngs, &mEditPressing);
}
void fParamModulation()
{
	set_sqreen_menu(&mParamPressing, &mParamCurrent, &mChoosePrgStngs, &mEditModulation);
}
void fParamCurrent()
{
	set_sqreen_menu(&mParamModulation, &mParamHeating, &mChoosePrgStngs, &mEditCurrent);
}
void fParamHeating()
{
	set_sqreen_menu(&mParamCurrent, &mParamForging, &mChoosePrgStngs, &mEditHeating);
}
void fParamForging()
{
	set_sqreen_menu(&mParamHeating,
		curMode.get() == AUTO_MODE ? &mParamPause : &mParamMode,
		&mChoosePrgStngs,
		&mEditForging);
}
void fParamPause()
{
	set_sqreen_menu(&mParamForging, &mParamMode, &mChoosePrgStngs, &mEditPause);
}
void set_edit_sqreen_menu(u8 val, u8 param, u8 min, u8 max, const MenuItem* menu)
{
	wdt_start(wdt_60ms);
	u8 oldVal = val;
	//if (val > max)
	if (!(val >= min && val <= max))
	{
		val = min;
		UpdateLcdParam(param, val);
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
					if (param == cmnprmPedalNum)
						val = min;
					else if (param == cmnprmBrtns)
					{
						val = OFF;
						switchBrightness(OFF);
					}
					else if (param == paramMode)
					{
						if (val > FIRST_MODE)
							val--;
					}
					else if (val > min)
						val--;
					else
						val = max;
					UpdateLcdParam(param, val);
				break;
				case keyRight:
					if (param == cmnprmPedalNum)
						val = max;
					else if (param == cmnprmBrtns)
					{
						val = ON;
						switchBrightness(ON);
					}
					else if (param == paramMode)
					{
						if (val < LAST_MODE)
							val++;
					}
					else if (val < max)
						val++;
					else
						val = min;
					UpdateLcdParam(param, val);
				break;
				case keyUp:
					SavedDlg(0);
					SetMenu(menu);
					return;
				case keyDown:
				{
					BOOL res = TRUE;
					if (val != oldVal)
						res = SetValue(param, val);
					SavedDlg(res);
					SetMenu(menu);
				}
				return;
			}
		}
		if (CheckUpEditTime(TIME_FOR_SAVE) == TRUE)
		return;
	}
}
void fEditPrePressing()
{
	set_edit_sqreen_menu(GetValue(paramPrePressing), paramPrePressing, minPrePressing, maxPrePressing, &mParamPrePressing);
}
void fEditPressing()
{
	set_edit_sqreen_menu(GetValue(paramPressing), paramPressing, minPressing, maxPressing, &mParamPressing);
}
void fEditHeating()
{
	set_edit_sqreen_menu(GetValue(paramHeating), paramHeating, minHeating, maxHeating, &mParamHeating);
}
void fEditForging()
{
	set_edit_sqreen_menu(GetValue(paramForging), paramForging, minForging, maxForging, &mParamForging);
}
void fEditModulation()
{
	set_edit_sqreen_menu(GetValue(paramModulation), paramModulation, minModulation, maxModulation, &mParamModulation);
}
void fEditCurrent()
{
	set_edit_sqreen_menu(GetValue(paramCurrent), paramCurrent, minCurrent, maxCurrent, &mParamCurrent);
}

void fEditStartPrg()
{
	set_edit_sqreen_menu(readByteEE((u16)&ee_startprg), cmnprmStartPrg, firstPrg, lastPrg, &mCmnPrmStartPrg);
}

void fEditPedalNum()
{
	set_edit_sqreen_menu(readByteEE((u16)&ee_pedalnum), cmnprmPedalNum, minPedalNum, maxPedalNum, &mCmnPrmPedalNum);
}
void fEditBrtns()
{
	set_edit_sqreen_menu(readByteEE((u16)&ee_brtns), cmnprmBrtns, OFF, ON, &mCmnPrmBrtns);
}
void fEditModbusId()
{
	set_edit_sqreen_menu(readByteEE((u16)&ee_modbus_id), cmnprmModbusId, MIN_MODBUS_ID, MAX_MODBUS_ID, &mCmnPrmModbusId);
}
void fEditMode()
{
	set_edit_sqreen_menu(readByteEE((u16)&eeMass + curPrg.get() * paramNum + addrMode),
		paramMode, SIMPLE_MODE, LAST_MODE, &mParamMode);
}
void fEditPause()
{
	set_edit_sqreen_menu(readByteEE((u16)&eeMass + curPrg.get() * paramNum + addrPause),
		paramPause, MIN_PAUSE, MAX_PAUSE, &mParamPause);
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
