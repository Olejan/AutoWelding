#include "IDE.h"

//=================================================================
// Extern section
//=================================================================
extern void writeByteEE(u16 addr, u8 data);
extern u8 readByteEE(u16 addr);
extern BOOL wait_100us();
extern BOOL wait_300us();
//extern void wait_x10us(u8 a_i);
extern void WrDec(u8 a_data, u8 a_x, u8 a_y);
extern void Wr3Dec(u8 a_data, u8 a_x, u8 a_y);
extern void Wr1Dec(u8 a_data, u8 a_x, u8 a_y);
extern void WrChar(u8 a_char, u8 a_x, u8 a_y);
extern void setScreen(u8 a_scr);
extern void switchTrans(u8 a_state);
extern void switchValve1(u8 state);
extern void switchValve2(u8 state);
extern void switchHL(u8 line, u8 state);
extern void SetProgressBar(u8 a_value, u8 a_maxvalue, BOOL a_bShowPercents);
extern u8 get_key(void);
extern void WriteWeldReadiness();
extern void switchModeHL(u8 a_mode);

extern volatile tagFlags flags;
extern volatile u32 waitTime;
extern volatile u16 _TCNT1;

extern u8 eeMass[programNumber][paramNum];
extern __attribute__((section(".eeprom")))u8 ee_startprg;
extern __attribute__((section(".eeprom")))u8 ee_pedalnum;
extern __attribute__((section(".eeprom")))u8 ee_brtns;
extern __attribute__((section(".eeprom")))u8 ee_modbus_id;
//=================================================================
// Data
//=================================================================

/*static*/ u8 m_nPrePressing = minPrePressing; // число предварительного сжатия
/*static*/ u8 m_nPressing = minPressing; // число сжатия
/*static*/ u8 m_nModulation = minModulation; // число модуляции
/*static*/ u8 m_nCurrent = minCurrent; // число мощности тока
/*static*/ u8 m_nHeating = minHeating; // число нагрева
/*static*/ u8 m_nForging = minForging; // число проковки
/*static*/ u8 m_nMode = AUTO_MODE;
/*static*/ u8 m_nPause = MAX_PAUSE;
/*static*/ u8 m_nCurPrg = firstPrg; // текущая программа
/*static*/ u8 m_nPedalNum = maxPedalNum; // количество педалей
/*static*/ u8 m_nBrtns = ON;
extern unsigned char m_nModbusId;

static u8 m_TaskWelding_State = 0; // состояние задачи сварки		
//===================================================================
// Prototypes
//==================================================================
void setCurMode(u8 a_nPrg);
void initPrgParams(u8 a_nPrg);
//===================================================================
void setCurPrg(u8 a_nPrg){ m_nCurPrg = a_nPrg; setCurMode(a_nPrg); }
u8 getCurPrg(){ return m_nCurPrg; }
CURPRG curPrg = {setCurPrg, getCurPrg}; // методы get и set для обслуживания static m_nCurPrg

void setMode(u8 a_nMode){ m_nMode = a_nMode; }
u8 getMode(){ return m_nMode; }
CURMODE curMode = {setMode, getMode}; // методы get и set для обслуживания static m_nMode


void setCurMode(u8 a_nPrg)
{
	u16 shift = (u16)&eeMass + (int)a_nPrg * paramNum;
	m_nMode = readByteEE(shift + addrMode); // читаем значение режима сварки
	if (m_nMode > LAST_MODE)
		m_nMode = SIMPLE_MODE;
	switchModeHL(m_nMode);
}
//=============================== Pedals ===================================
// Write here pedals functions
//==========================================================================
BOOL isPedal1Pressed()
{
	if (m_nPedalNum == 1 || m_nPrePressing == 0)
		return isPedal2Pressed();
	if(!(PIN_PEDAL1 & (1<<pin_PEDAL1)))
		return TRUE; // педаль предварительного сжатия нажата
	return FALSE; // педаль предварительного сжатия отжата
}
BOOL isPedal2Pressed()
{
	if(!(PIN_PEDAL2 & (1<<pin_PEDAL2)))
		return TRUE; // педаль нажата
	return FALSE; // педаль отжата
}
BOOL isImpossibleToWork()
{// если педали нажаты и нет аварии
	return (isPedal2Pressed() == FALSE || isPedal1Pressed() == FALSE) || flags.alarm == 1;
}
//==========================================================================

//===================================================================
// предварительное сжатие
u8 doPrePressing()
{
	BOOL bToWrite = FALSE;
	wdt_start(wdt_60ms);
	u16 prepressing = (u16)m_nPrePressing << 1; // значение времени предварительного сжатия
	u8 towrite = m_nPrePressing; // значение для вывода на экран
	if (prepressing)
	{
		if (prepressing > TIME_TO_WRITE)
		{
			setScreen(scrPrePressing); // инициализируем экран процедуры предварительного сжатия
			bToWrite = TRUE;
		}			
		switchValve1(ON); // включаю клапан предварительного сжатия
	}
	else return TRUE;
	flags.halfPeriod = 0;
	while(1)
	{
		if (bToWrite)
		{
			if (!(prepressing & (1 << 0)))
			{
				Wr3Dec(towrite, 13, lcdstr1); // отправляю значение предварительного сжатия
				towrite--;
			}
		}
		if (prepressing-- == 0)
			break;
		while(!flags.halfPeriod) // ждём прихода полупериода
		{
			if (isPedal1Pressed() == FALSE  || flags.alarm == 1)
				return FALSE;
		}
		flags.halfPeriod = 0;
		wdt_feed();
	}
	return TRUE;
}

// сжатие
u8 doPressing()
{
	BOOL bToWrite = FALSE;
	u16 pressing = (u16)m_nPressing << 1; // значение времени сжатия
	u8 towrite = m_nPressing; // значение для вывода на экран
	wdt_start(wdt_60ms);
	if (pressing)
	{
		if (pressing > TIME_TO_WRITE)
		{
			setScreen(scrPressing); // инициализируем экран процедуры сжатия
			bToWrite = TRUE;
		}
		switchValve2(ON); // включаю клапан
	}
	else return TRUE;
	flags.halfPeriod = 0;
	while(1)
	{
		if (bToWrite)
		{
			if (!(pressing & (1 << 0)))
			{
				Wr3Dec(towrite, 13, lcdstr1); // отправляю туда значение сжатия
				towrite--;
			}
		}
		if (pressing-- == 0)
			break;
		while(!flags.halfPeriod)
		{
			if (isImpossibleToWork())
				return FALSE;
		}
		flags.halfPeriod = 0;
		wdt_feed();
	}
	return TRUE;
}

void impulse()
{ // пока не установится флаг - управляем трансформатором
	while(!flags.transswitchoff)
	{
		if (isImpossibleToWork())
			break; // если педали отпустили - значит аварийная остановка
		switchTrans(ON); // включаем трансформатор
		if (!wait_100us()) // если прерывания на int0 не было
		{
			switchTrans(OFF); // отключаем трансформатор
			if (wait_300us()) // если было прерывание, выходим
				break;
		}
		else
			break;
	}
	flags.transswitchoff = 0;
	switchTrans(OFF); // отключаем трансформатор
}

u16 _CurrentPauseTCNT; // константа таймера тока текущей программы
u8 doModulation()
{
	wdt_start(wdt_60ms);
	//u16 heating = m_nHeating << 1;
	//if (heating > TIME_TO_WRITE)
	if (m_nHeating > 0)
		setScreen(scrHeating); // инициализируем экран сварки
	//if (m_nModulation == 0) return TRUE;
	//_CurrentPauseTCNT = 4000 * (m_nCurrent + 1); // 0..9
	//_CurrentPauseTCNT = 1600 * (m_nCurrent + 1); // 0..24
	_CurrentPauseTCNT = 400 * (m_nCurrent + 1); // 0..99
	u8 nMod = m_nModulation << 1;// умножаем на 2, т.к. полупериодов в 2 раза больше
	u16 step = _CurrentPauseTCNT / (nMod + 1);
	//float fstep = _CurrentPauseTCNT / (m_nModulation + 1); // расчитываю шаг приращения константы для увеличения тока
	u8 cnt = /*m_nModulation;/*/ 1;
	// В данном случае 40000 - это константа для таймера Т1 задающая время 10мс
	// Для установки тока мы заполняем импульсами промежуток от 0 до 10мс.
	// Одим импульс равен 100мкс вкл и 300мкс выкл, т.е. в сумме 400мкс.
	// Значит диапазон тока может быть 10мс / 0.4мс = 25, т.е. [0, 24]
	_TCNT1 = 25535 + step * cnt; // 0xFFFF - 40000 + step * cnt
	switchHL(pin_HEATING_HL, ON); // включение светодиода нагрева
	flags.halfPeriod = 0;
	flags.useT1forHeating = 1;
	if (m_nModulation == 0)
	{
		if (m_nHeating > 0)
			_TCNT1 = 25535 + _CurrentPauseTCNT;
		return TRUE;
	}
	
	while(cnt <= nMod)
	{
		while(!flags.halfPeriod) // и ждём передний фронт
		{
			if (isImpossibleToWork())
			{
				switchHL(pin_HEATING_HL, OFF);
				return FALSE;
			}
		}
		flags.halfPeriod = 0;
		flags.T1IsUp = 0; // сбросим используемые флаги
		flags.transswitchoff = 0;
		cnt++;
		if (cnt > nMod)
			_TCNT1 = 25535 + _CurrentPauseTCNT;
		else
			_TCNT1 = 25535 + step * cnt;

		wdt_feed();
		while(flags.T1IsUp == 0 && flags.transswitchoff == 0)
		{
			if (isImpossibleToWork())
			{
				switchHL(pin_HEATING_HL, OFF);
				TCCR1B = 0;
				return FALSE;
			}
		}
		if (flags.transswitchoff == 0)
		{
			impulse();
		}
		else
		{
			 // если пересидели в паузе, а уже пришёл задний фронт
			flags.transswitchoff = 0;
		}
	}
	return TRUE;
}

/*u16 times[]=
{
	0x735F,	// 9ms I = 0
	0x82FF, // 8ms I = 1
	0x929F, // 7ms I = 2
	0xA23F, // 6ms I = 3
	0xB1DF, // 5ms I = 4
	0xC17F, // 4ms I = 5
	0xD11F, // 3ms I = 6
	0xE0BF, // 2ms I = 7
	0xF05F, // 1ms I = 8
	0xFFFF, // 0ms I = 9
};*/
u8 doHeating()
{
	u8 res = doModulation();
	if (res == FALSE) return FALSE;
	if (m_nHeating == 0) return TRUE;
	wdt_start(wdt_60ms);
	u16 heating = (u16)m_nHeating << 1;// делаю полупериоды из периодов
	while(!flags.halfPeriod) // ждём прерывание int0
	{
		if (isImpossibleToWork())
		{
			switchHL(pin_HEATING_HL, OFF);
			return FALSE;
		}
	}
	flags.halfPeriod = 0; // сбросим флаг
	flags.transswitchoff = 0;
	if (m_nCurrent < maxCurrent/*9*/)
	{
//		_TCNT1 = times[m_nCurrent];
//		TCNT1 = _TCNT1;
//		TCCR1B = 1;
//		flags.useT1forHeating = 1;
		flags.T1IsUp = 0;
	}
	else
	{
		flags.useT1forHeating = 0;
		TCCR1B = 0;
	}
	while(heating--)
	{
		//while(!flags.halfPeriod) // ждём прерывание int0
		if (m_nCurrent < maxCurrent/*9*/)
		{
			while(flags.T1IsUp == 0 && flags.transswitchoff == 0)
			{
				if (isImpossibleToWork())
				{
					switchHL(pin_HEATING_HL, OFF);
					TCCR1B = 0;
					return FALSE;
				}
			}
		}
		wdt_feed();
		if (flags.transswitchoff == 0)
		{
			//flags.halfPeriod = 0; // сбросим флаг
			//flags.T1IsUp = 0;
			impulse();
		}
		else
		{
			 // если пересидели в паузе, а уже пришёл задний фронт
			flags.transswitchoff = 0;
		}
		while(!flags.halfPeriod) // ждём передний фронт после выключения транса по заднему фронту
		{
			if (isImpossibleToWork())
			{
				switchHL(pin_HEATING_HL, OFF);
				return FALSE;
			}
		}
		flags.halfPeriod = 0;
		flags.T1IsUp = 0;
	}
	switchHL(pin_HEATING_HL, OFF);
	return TRUE;
}

u8 doForging()
{
	if (m_nForging == 0) return TRUE;
	BOOL bToWrite = FALSE;
	wdt_start(wdt_60ms);
	setScreen(scrForging);
	switchHL(pin_FORGING_HL, ON);
	u16 forging = m_nForging << 1;
	u8 towrite = m_nForging;
	//if (forging > TIME_TO_WRITE)
		bToWrite = TRUE;
	while(1)
	{
		if (bToWrite)
		{
			if (!(forging & (1 << 0)))
			{
				Wr3Dec(towrite, 13, lcdstr1); // отправляю туда значение проковки
				towrite--;
			}
		}
		if (forging-- == 0)
			break;
		while(!flags.halfPeriod)
		{
			if (isImpossibleToWork())
			{
				switchHL(pin_FORGING_HL, OFF);
				return FALSE;
			}
		}
		flags.halfPeriod = 0;
		wdt_feed();
	}
	switchHL(pin_FORGING_HL, OFF);
	return TRUE;
}

u8 doPause()
{
	if (m_nPause == 0) return TRUE;
	BOOL bToWrite = FALSE;
	wdt_start(wdt_60ms);
	u16 pause = m_nPause << 1;
	u8 towrite = m_nPause;
	//if (pause > TIME_TO_WRITE)
		bToWrite = TRUE;
	setScreen(scrPause);
	switchHL(pin_PAUSE_HL, ON);
	while(1)
	{
		if (bToWrite)
		{
			if (!(pause & (1 << 0)))
			{
				Wr3Dec(towrite, 11, lcdstr1); // отправляю туда значение паузы
				towrite--;
			}
		}
		if (pause-- == 0)
			break;
		while(!flags.halfPeriod)
		{
			if (isImpossibleToWork())
			{
				switchHL(pin_PAUSE_HL, OFF);
				WriteWeldReadiness();
				return FALSE;
			}
		}
		flags.halfPeriod = 0;
		wdt_feed();
	}
	switchHL(pin_PAUSE_HL, OFF);
	if (isPedal2Pressed() == FALSE)
		WriteWeldReadiness();	// готовность к сварке на экран
	return TRUE;
}

u8 GetWeldingState()
{
	if (isPedal1Pressed() == FALSE || flags.alarm == 1)
		return WELD_HAS_BROKEN;
	if (isPedal2Pressed() == FALSE)
		return WELD_IS_PAUSED;
	return WELD_IS_RUNNIG;
}

u8 DoWelding()
{
	if (isPedal2Pressed())
	{
		u8 res = doPressing(); // прижим сварных деталей
		if (res == FALSE)
			return GetWeldingState();
		res = doHeating(); // нагрев
		flags.useT1forHeating = 0;
		if (res == FALSE)
			return GetWeldingState();
		res = doForging(); // проковка сварной точки
		//switchValve2(OFF);
		switchHL(pin_FORGING_HL, OFF);
		if (res == FALSE)
			return GetWeldingState();
		if (m_nMode == SIMPLE_MODE)
		{
			switchValve2(OFF);
			setScreen(scrWeldingCompleted);
			while(1)
			{
				if (isPedal2Pressed() == FALSE)
				{
					WriteWeldReadiness();	// готовность к сварке на экран
					_delay_ms(5);	// защита от дребезга контактов. Без этой строки при отпускании педали 2 мог возникнуть повторный цикл сварки
					return SIMPLE_WELD_HAS_DONE;
				}
				if (isPedal1Pressed() == FALSE || flags.alarm == 1)
					return GetWeldingState();
				wdt_feed();
			}
		}
		if (m_nPause > 0)
		{
			switchValve2(OFF);
			doPause(); // пауза между циклами сварки
		}
	}
	return WELD_IS_RUNNIG;
}

u8 DoSeamWelding()
{
	if (isPedal2Pressed())
	{
		u8 res = doHeating(); // нагрев
		flags.useT1forHeating = 0;
		if (res == FALSE)
			return GetWeldingState();
		res = doForging(); // проковка сварной точки
		if (res == FALSE)
			return GetWeldingState();
	}
	return WELD_IS_RUNNIG;
}

void StartTaskWelding()
{
	m_TaskWelding_State = 1;
}
void StopTaskWelding()
{
	//setScreen(scrWeldingCompleted);
	m_TaskWelding_State = 0;
	switchTrans(OFF);
	switchValve2(OFF);
	switchValve1(OFF);
}
u8 TaskWelding()
{
	u8 res;
	switch(m_TaskWelding_State)
	{
		case 0:
			break;
		case 1:
			wdt_start(wdt_1s);
			_delay_ms(10/*500*/);
			wdt_start(wdt_60ms);
			if (isPedal1Pressed())
			{
				initPrgParams(m_nCurPrg); 	// читаю из еепром параметры сварки
				res = doPrePressing();	// предварительный прижим сварных деталей
				if (res == FALSE)
					break;
				WriteWeldReadiness();	// готовность к сварке на экран
				if (m_nMode == SEAM_MODE)
					m_TaskWelding_State = 3;
				else
					m_TaskWelding_State = 2;
			}
			break;
		case 2:
			res = DoWelding();
			if (res != WELD_IS_RUNNIG)
			{
				switchValve2(OFF);
			}
			if (res == WELD_IS_PAUSED)
			{
				WriteWeldReadiness();	// готовность к сварке на экран
			}
			break;
		case 3:
			if (isPedal2Pressed())
			{
				res = doPressing();
				if (res == FALSE)
					break;
				m_TaskWelding_State++;
			}
			break;
		case 4:
			res = DoSeamWelding();
			if (res != WELD_IS_RUNNIG)
			{
				switchValve2(OFF);
				m_TaskWelding_State--;
			}
			if (res == WELD_IS_PAUSED)
			{
				WriteWeldReadiness();
			}
			break;
	}
	return WELD_IS_RUNNIG;
}

//==================================================================
//////////////////////////////////////////////////////////////////////////
// Читает из EEPROM параметры и обновляет их на экране меню mPrograms
//////////////////////////////////////////////////////////////////////////
void UpdateParams()
{
	initPrgParams(m_nCurPrg);
	char ch = ' ';
#ifdef _RUSSIAN_VERSION_
	if (m_nMode == SIMPLE_MODE)
		ch = 'О';
	else if (m_nMode == AUTO_MODE)
		ch = 'Ц';
	else if (m_nMode == SEAM_MODE)
		ch = 'Ш';
#else
	if (m_nMode == SIMPLE_MODE)
		ch = 'S';
	else if (m_nMode == AUTO_MODE)
		ch = 'C';
	else if (m_nMode == SEAM_MODE)
		ch = 'E';
#endif // _RUSSIAN_VERSION_
	WrChar(ch, 0, lcdstr1); // текущий режим
	Wr1Dec(m_nCurPrg, 1, lcdstr1); // текущая программа
	if(m_nPrePressing < 100 || m_nPressing < 100)
	{
#ifdef _RUSSIAN_VERSION_
		WrChar('ж', 4, lcdstr1);
#else
		WrChar('r', 4, lcdstr1);
#endif
		WrChar(':', 5, lcdstr1);
		if(m_nPrePressing < 99)
		{
			WrDec(m_nPrePressing, 6, lcdstr1);
			WrChar('*', 8, lcdstr1);
			Wr3Dec(m_nPressing, 9, lcdstr1);
		}
		else
		{
			Wr3Dec(m_nPrePressing, 6, lcdstr1);
			WrChar('*', 9, lcdstr1);
			WrDec(m_nPressing, 10, lcdstr1);
		}
	}
	else
	{
		Wr3Dec(m_nPrePressing, 5, lcdstr1);
		WrChar('*', 8, lcdstr1);
		Wr3Dec(m_nPressing, 9, lcdstr1);
	}
	/*Wr3Dec(m_nPrePressing, 5, lcdstr1); // число предварительного сжатия
	Wr3Dec(m_nPressing, 9, lcdstr1); // число сжатия*/
	if (m_nModulation < 10)
	{
		WrChar(':', 14, lcdstr1);
		Wr1Dec(m_nModulation, 15, lcdstr1); // число модуляции
	}
	else
	{
		WrDec(m_nModulation, 14, lcdstr1); // число модуляции
	}
	WrDec(m_nCurrent, 2, lcdstr2); // число мощности тока
	Wr3Dec(m_nHeating, 7, lcdstr2); // число нагрева
	Wr3Dec(m_nForging, 13, lcdstr2); // число проковки
}

u8 GetValue(i8 a_nParamId)
{
	if (a_nParamId < paramPrePressing || a_nParamId > extremeParam)
		return 0;
	u16 addr = 0;
	if (a_nParamId == cmnprmStartPrg)
		addr = (u16)&ee_startprg;
	else if (a_nParamId == cmnprmPedalNum)
		addr = (u16)&ee_pedalnum;
	else if (a_nParamId == cmnprmBrtns)
		addr = (u16)&ee_brtns;
	else if (a_nParamId == cmnprmModbusId)
		addr = (u16)&ee_modbus_id;
	else
		addr = (u16)&eeMass + m_nCurPrg * paramNum + a_nParamId; // a_nParamId == addr...
	return readByteEE(addr);
}

BOOL SetValue(i8 a_nParamId, u8 a_nVal)
{
	if (a_nParamId < paramPrePressing || a_nParamId > extremeParam)
		return FALSE;
	u16 addr = 0;
	if (a_nParamId == cmnprmStartPrg)
		addr = (u16)&ee_startprg;
	else if (a_nParamId == cmnprmPedalNum)
		addr = (u16)&ee_pedalnum;
	else if (a_nParamId == cmnprmBrtns)
		addr = (u16)&ee_brtns;
	else if (a_nParamId == cmnprmModbusId)
		addr = (u16)&ee_modbus_id;
	else
		addr = (u16)&eeMass + m_nCurPrg * paramNum + a_nParamId; // a_nParamId == addr...
	switch(a_nParamId)
	{
		case paramPrePressing:
			if (a_nVal > maxPrePressing || a_nVal < minPrePressing)
				return FALSE;
			m_nPrePressing = a_nVal;
			break;
		case paramPressing:
			if (a_nVal > maxPressing || a_nVal < minPressing)
				return FALSE;
			m_nPressing = a_nVal;
			break;
		case paramModulation:
			if (a_nVal > maxModulation || a_nVal < minModulation)
				return FALSE;
			m_nModulation = a_nVal;
			break;
		case paramCurrent:
			if (a_nVal > maxCurrent || a_nVal < minCurrent)
				return FALSE;
			m_nCurrent = a_nVal;
			break;
		case paramHeating:
			if (a_nVal > maxHeating || a_nVal < minHeating)
				return FALSE;
			m_nHeating = a_nVal;
			break;
		case paramForging:
			if (a_nVal > maxForging || a_nVal < minForging)
				return FALSE;
			m_nForging = a_nVal;
			break;
		case paramMode:
			if (a_nVal > LAST_MODE)
				return FALSE;
			m_nMode = a_nVal;
			switchModeHL(m_nMode);
			break;
		case paramPause:
			if (a_nVal < MIN_PAUSE || a_nVal > MAX_PAUSE)
				return FALSE;
			m_nPause = a_nVal;
			break;
		case cmnprmStartPrg:
			if (a_nVal > lastPrg || a_nVal < firstPrg)
				return FALSE;
			break;
		case cmnprmPedalNum:
			if (a_nVal > maxPedalNum || a_nVal < minPedalNum)
				return FALSE;
			m_nPedalNum = a_nVal;
			break;
		case cmnprmBrtns:
			if (a_nVal != ON && a_nVal != OFF)
				return FALSE;
			m_nBrtns = a_nVal;
			break;
		case cmnprmModbusId:
			if (a_nVal == 0)
				return FALSE;
			m_nModbusId = a_nVal;
			break;
		default:
			return FALSE;
	}
	writeByteEE(addr, a_nVal);
	return TRUE;
}

void initParams()
{
	wdt_start(wdt_250ms);
	// init common params
	m_nCurPrg = readByteEE((u16)&ee_startprg);
	if (m_nCurPrg > lastPrg)
	{
		m_nCurPrg = firstPrg;
		writeByteEE((u16)&ee_startprg, firstPrg);
	}
	// init local params
	initPrgParams(m_nCurPrg); // обновляю значение параметров программы
	m_nPedalNum = readByteEE((u16)&ee_pedalnum);
	m_nBrtns = readByteEE((u16)&ee_brtns);
	m_nModbusId = readByteEE((u16)&ee_modbus_id);
}

// инициализация переменных из eeprom
void initPrgParams(u8 a_nPrg)
{// выполняется 95мкс
	u16 shift = (u16)&eeMass + ((int)a_nPrg * paramNum);
	m_nPrePressing = readByteEE(shift + addrPrePressing); // число предварительного сжатия
	if (m_nPrePressing < minPrePressing || m_nPrePressing > maxPrePressing)
		m_nPrePressing = defPrePressing;
	m_nPressing = readByteEE(shift + addrPressing); // число сжатия
	if (m_nPressing < minPressing || m_nPressing > maxPressing)
		m_nPressing = defPressing;
	m_nModulation = readByteEE(shift + addrModulation); // читаем число модуляции
	if (m_nModulation < minModulation || m_nModulation > maxModulation)
		m_nModulation = defModulation;
	m_nCurrent = readByteEE(shift + addrCurrent); // читаем число мощности тока
	if (m_nCurrent < minCurrent || m_nCurrent > maxCurrent)
		m_nCurrent = defCurrent;
	m_nHeating = readByteEE(shift + addrHeating); // читаем число нагрева
	if (m_nHeating < minHeating || m_nHeating > maxHeating)
		m_nHeating = defHeating;
	m_nForging = readByteEE(shift + addrForging); // число проковки
	if (m_nForging < minForging || m_nForging > maxForging)
		m_nForging = defForging;
	setCurMode(a_nPrg); // читаем значение режима сварки
	m_nPause = readByteEE(shift + addrPause); // читаем значение паузы между циклами сварки
	if (m_nPause < MIN_PAUSE || m_nPause > MAX_PAUSE)
		m_nPause = MAX_PAUSE;
}

#include "Registers.h"
// инициализация переменных из eeprom
int get_param(int reg)
{// выполняется 95мкс
	switch (reg)
	{
	case CMN_CUR_PRG:
		return curPrg.get();
	case CMN_START_PRG:
		return readByteEE((u16)&ee_startprg);
	case CMN_LIGTH:
		return readByteEE((u16)&ee_brtns);
	case CMN_PEDAL_NUM:
		return readByteEE((u16)&ee_pedalnum);
	}
	u8 prg = reg / 0x10;
	u8 param = reg % 0x10;
	if (prg > lastPrg || param > addrLastParam)
		return -1;
	return readByteEE((u16)&eeMass + ((int)prg * paramNum + param));
}

bool check_prg(int prg)
{
	return prg >= firstPrg && prg <= lastPrg;
}
bool check_light(int val)
{
	return val == ON || val == OFF;
}
bool check_pedal_num(int val)
{
	return val == minPedalNum || val == maxPedalNum;
}
bool check_prg_param(uint8_t param, int val)
{
	switch (param)
	{
		case paramPrePressing:
			return val >= minPrePressing && val <= maxPrePressing;
		case paramPressing:
			return val >= minPressing && val <= maxPressing;
		case paramModulation:
			return val >= minModulation && val <= maxModulation;
		case paramCurrent:
			return val >= minCurrent && val <= maxCurrent;
		case paramHeating:
			return val >= minHeating && val <= maxHeating;
		case paramForging:
			return val >= minForging && val <= maxForging;
		case paramMode:
			return val == SIMPLE_MODE || val == AUTO_MODE || val == SEAM_MODE;
		case paramPause:
			return val >= MIN_PAUSE && val <= MAX_PAUSE;
	}
	return false;
}


// Проверка входных данных на валидность
int check_param(int reg, int val)
{
	switch (reg)
	{
	case CMN_CUR_PRG:
		if (!check_prg(val))
			return MB_EX_ILLEGAL_DATA_VALUE;
		return MB_EX_NONE;
	case CMN_START_PRG:
		if (!check_prg(val))
			return MB_EX_ILLEGAL_DATA_VALUE;
		return MB_EX_NONE;
	case CMN_LIGTH:
		if (!check_light(val))
			return MB_EX_ILLEGAL_DATA_VALUE;
		return MB_EX_NONE;
	case CMN_PEDAL_NUM:
		if (!check_pedal_num(val))
			return MB_EX_ILLEGAL_DATA_VALUE;
		return MB_EX_NONE;
	}
	u8 prg = reg / 0x10;
	u8 param = reg % 0x10;
	if (prg > lastPrg || param > addrLastParam)
		return MB_EX_ILLEGAL_DATA_ADDRESS;
	if (!check_prg_param(param, val))
		return MB_EX_ILLEGAL_DATA_VALUE;
	return MB_EX_NONE;
}

// Установка изменяемых извне параметров
int set_param(int reg, int val)
{
	switch (reg)
	{
	case CMN_CUR_PRG:
		if (!check_prg(val))
			return MB_EX_ILLEGAL_DATA_VALUE;
		curPrg.set(val);
		UpdateParams();
		return MB_EX_NONE;
	case CMN_START_PRG:
		if (!check_prg(val))
			return MB_EX_ILLEGAL_DATA_VALUE;
		writeByteEE((u16)&ee_startprg, val);
		return MB_EX_NONE;
	case CMN_LIGTH:
		if (!check_light(val))
			return MB_EX_ILLEGAL_DATA_VALUE;
		writeByteEE((u16)&ee_brtns, val);
		return MB_EX_NONE;
	case CMN_PEDAL_NUM:
		if (!check_pedal_num(val))
			return MB_EX_ILLEGAL_DATA_VALUE;
		writeByteEE((u16)&ee_pedalnum, val);
		return MB_EX_NONE;
	}
	u8 prg = reg / 0x10;
	u8 param = reg % 0x10;
	if (prg > lastPrg || param > addrLastParam)
		return MB_EX_ILLEGAL_DATA_ADDRESS;
	if (!check_prg_param(param, val))
		return MB_EX_ILLEGAL_DATA_VALUE;
	writeByteEE((u16)&eeMass + ((int)prg * paramNum + param), val);
	if (prg == curPrg.get())
		UpdateParams();
	return MB_EX_NONE;
}

#if 0
void Test()
{
	for(;;)// делать всегда
	{
		doModulation();
	}
}
#endif
