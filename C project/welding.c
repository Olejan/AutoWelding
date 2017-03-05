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

//=================================================================
// Data
//=================================================================

static u8 m_nPrePressing = 0; // число предварительного сжатия
static u8 m_nPressing = 0; // число сжатия
static u8 m_nHeating = 0; // число нагрева
static u8 m_nForging = 0; // число проковки
static u8 m_nModulation = 0; // число модуляции
static u8 m_nCurrent = 0; // число мощности тока
static u8 m_nCurPrg = 0; // текущая программа
static u8 m_nPedalNum = 2; // количество педалей
static u8 m_nMode = AUTO_MODE;
static u8 m_nPause = MAX_PAUSE;

static u8 m_TaskWelding_State = 0; // состояние задачи сварки
//static u8 P2cntHigh;	// кол-во зафиксированных состояний педали 2 в высоком уровне
//static u8 P2cntLow;		// кол-во зафиксированных состояний педали 2 в низком уровне
//static u8 P2state;		
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
	u16 shift = a_nPrg * paramNum;
	m_nMode = readByteEE(shift + addrMode); // читаем значение режима сварки
	if (m_nMode > LAST_MODE)
		m_nMode = SIMPLE_MODE;
	switchModeHL(m_nMode);
}

//===================================================================
// предварительное сжатие
u8 doPrePressing()
{
	BOOL bToWrite = FALSE;
	wdt_start(wdt_60ms);
	u8 prepressing = m_nPrePressing; // значение времени предварительного сжатия
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
			Wr3Dec(prepressing, 13, lcdstr1); // отправляю значение предварительного сжатия
		if (prepressing-- == 0)
			break;
		while(!flags.halfPeriod) // ждём прихода полупериода
		{
			if (isPedal1Pressed() == FALSE)
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
	u8 pressing = m_nPressing; // значение времени сжатия
	wdt_start(wdt_60ms);
	if (pressing)
	{
		setScreen(scrPressing); // инициализируем экран процедуры сжатия
		switchValve2(ON); // включаю клапан
	}
	else return TRUE;
	while(1)
	{
		WrDec(pressing, 14, lcdstr1); // отправляю туда значение сжатия
		if (pressing-- == 0)
			break;
		while(!flags.halfPeriod)
		{
			if (isPedal2Pressed() == FALSE && isPedal1Pressed() == FALSE)
				return FALSE;
		}
		flags.halfPeriod = 0;
		wdt_feed();
	}
	return TRUE;
}

void impulse()
{ // пока не установится флаг - управляем трансформатором
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
	while(!flags.transswitchoff)
#else
	while(!flags.halfPeriod)
#endif
	{
		if (isPedal2Pressed() == FALSE && isPedal1Pressed() == FALSE)
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
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
	flags.transswitchoff = 0;
#endif
	switchTrans(OFF); // отключаем трансформатор
}

u16 _CurrentPauseTCNT; // константа таймера тока текущей программы
u8 doModulation()
{
	wdt_start(wdt_60ms);
	setScreen(scrHeating); // инициализируем экран сварки
	switchHL(pinHeatingHL, ON); // включение светодиода нагрева
	//if (m_nModulation == 0) return TRUE;
	//_CurrentPauseTCNT = /*0xFFFF - */(m_nCurrent + 1) * (40000 / (maxCurrent + 1));// если максимальный ток равен 99, то не делаем +1. 40000 - это количество тактов в таймере без предделителя на 10 мс, т.е. 1 полупериод сетевого напряжения
	//_CurrentPauseTCNT = /*0xFFFF - */4000 * (maxCurrent - m_nCurrent);
	_CurrentPauseTCNT = 4000 * (m_nCurrent + 1);
	u16 step = _CurrentPauseTCNT / (m_nModulation + 1);
	//float fstep = _CurrentPauseTCNT / (m_nModulation + 1); // расчитываю шаг приращения константы для увеличения тока
	u8 cnt = /*m_nModulation;/*/ 1;
	_TCNT1 = 25535 + step * cnt; // 0xFFFF - 40000 + step * cnt
	flags.halfPeriod = 0;
	flags.useT1forHeating = 1;
	if (m_nModulation == 0)
	{
		_TCNT1 = 25535 + _CurrentPauseTCNT;
		return TRUE;
	}
	
	while(cnt <= m_nModulation)
	{
		while(!flags.halfPeriod) // и ждём передний фронт
		{
			if (isPedal2Pressed() == FALSE && isPedal1Pressed() == FALSE)
			{
				switchHL(pinHeatingHL, OFF);
				return FALSE;
			}
		}
		flags.halfPeriod = 0;
		flags.T1IsUp = 0; // сбросим используемые флаги
		flags.transswitchoff = 0;
		cnt++;
		if (cnt > m_nModulation)
			_TCNT1 = 25535 + _CurrentPauseTCNT;
		else
			_TCNT1 = 25535 + step * cnt;

		wdt_feed();
		while(flags.T1IsUp == 0 && flags.transswitchoff == 0)
		{
			if (isPedal2Pressed() == FALSE && isPedal1Pressed() == FALSE)
			{
				switchHL(pinHeatingHL, OFF);
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
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
			flags.transswitchoff = 0;
#endif
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
	wdt_start(wdt_60ms);
	u8 heating = m_nHeating;
	while(!flags.halfPeriod) // ждём прерывание int0
	{
		if (isPedal2Pressed() == FALSE && isPedal1Pressed() == FALSE)
		{
			switchHL(pinHeatingHL, OFF);
			return FALSE;
		}
	}
	flags.halfPeriod = 0; // сбросим флаг
	flags.transswitchoff = 0;
	if(m_nCurrent < 9)
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
		if (m_nCurrent < 9)
		{
			while(flags.T1IsUp == 0 && flags.transswitchoff == 0)
			{
				if (isPedal2Pressed() == FALSE && isPedal1Pressed() == FALSE)
				{
					switchHL(pinHeatingHL, OFF);
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
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
			flags.transswitchoff = 0;
#endif
		}
		while(!flags.halfPeriod) // ждём передний фронт после выключения транса по заднему фронту
		{
			if (isPedal2Pressed() == FALSE && isPedal1Pressed() == FALSE)
			{
				switchHL(pinHeatingHL, OFF);
				return FALSE;
			}
		}
		flags.halfPeriod = 0;
		flags.T1IsUp = 0;
	}
	switchHL(pinHeatingHL, OFF);
	return TRUE;
}

u8 doForging()
{
	wdt_start(wdt_60ms);
	setScreen(scrForging);
	switchHL(pinForgingHL, ON);
	u8 forging = m_nForging;
	while(1)
	{
		Wr3Dec(forging, 13, lcdstr1);
		if (forging-- == 0)
			break;
		while(!flags.halfPeriod)
		{
			if (isPedal2Pressed() == FALSE && isPedal1Pressed() == FALSE)
			{
				switchValve2(OFF);
				switchHL(pinForgingHL, OFF);
				return FALSE;
			}
		}
		flags.halfPeriod = 0;
		wdt_feed();
	}
	switchValve2(OFF);
	switchHL(pinForgingHL, OFF);
	return TRUE;
}

void doPause()
{
	wdt_start(wdt_60ms);
	u8 pause = m_nPause;
	setScreen(scrPause);
	switchHL(pinPauseHL, ON);
	while(1)
	{
		Wr3Dec(pause, 11, lcdstr1);
		if (pause-- == 0)
		break;
		while(!flags.halfPeriod)
		{
			if (isPedal2Pressed() == FALSE && isPedal1Pressed() == FALSE)
			{
				switchHL(pinPauseHL, OFF);
				WriteWeldReadiness();
				return;
			}
		}
		wdt_feed();
	}
	switchHL(pinPauseHL, OFF);
	if (isPedal2Pressed() == FALSE)
	WriteWeldReadiness();	// готовность к сварке на экран
}

u8 DoWelding()
{
	if (isPedal2Pressed())
	{
		//P2cntLow = 0;
		u8 res = doPressing(); // прижим сварных деталей
		if (res == FALSE) return WELD_HAS_BROKEN;
		res = doHeating(); // нагрев
		flags.useT1forHeating = 0;
		if (res == FALSE) return WELD_HAS_BROKEN;
		res = doForging(); // проковка сварной точки
		if (res == FALSE) return WELD_HAS_BROKEN;
		if (m_nMode == SIMPLE_MODE)
		{
			setScreen(scrWeldingCompleted);
			while(1)
			{
				if (isPedal2Pressed() == FALSE)
				{
					WriteWeldReadiness();	// готовность к сварке на экран
					_delay_ms(5);	// защита от дребезга контактов. Без этой строки при отпускании педали 2 мог возникнуть повторный цикл сварки
					return SIMPLE_WELD_HAS_DONE;
				}
				wdt_feed();
			}
		}
		doPause(); // пауза между циклами сварки
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
	switchValve1(OFF);
	switchValve2(OFF);
}
u8 TaskWelding()
{
	switch(m_TaskWelding_State)
	{
		case 0:
			break;
		case 1:
			wdt_start(wdt_1s);
			_delay_ms(500);
			wdt_start(wdt_60ms);
			if (isPedal1Pressed())
			{
				initPrgParams(m_nCurPrg); 	// читаю из еепром параметры сварки
				u8 res = doPrePressing();	// предварительный прижим сварных деталей
				if (res == FALSE)
					break;
				WriteWeldReadiness();	// готовность к сварке на экран
				m_TaskWelding_State++;
			}
			break;
		case 2:
			return DoWelding();
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
	Wr1Dec(m_nCurPrg, 0, lcdstr1); // текущая программа
	Wr3Dec(m_nPrePressing, 5, lcdstr1); // число предварительного сжатия
	WrDec(m_nPressing, 9, lcdstr1); // число сжатия
	Wr1Dec(m_nModulation, 14, lcdstr1); // число модуляции
	Wr1Dec(m_nCurrent, 2, lcdstr2); // число мощности тока
	Wr3Dec(m_nHeating, 6, lcdstr2); // число нагрева
	Wr3Dec(m_nForging, 13, lcdstr2); // число проковки
}

u8 GetValue(u8 a_nParamId)
{
	if (a_nParamId >= paramPrePressing && a_nParamId <= cmnprmPedalNum)
	{
		u16 addr = 0;
		if (a_nParamId == cmnprmStartPrg)
			addr = addrStartPrg;
		else if (a_nParamId == cmnprmPedalNum)
			addr = addrPedalNum;
		else
			addr = m_nCurPrg * paramNum + a_nParamId + 1; // a_nParamId + 1 == addr...
		/*switch(a_nParamId)
		{
			case paramPrePressing:	addr += addrPrePressing;	break;
			case paramPressing:		addr += addrPressing;		break;
			case paramHeating:		addr += addrHeating;		break;
			case paramForging:		addr += addrForging;		break;
			case paramModulation:	addr += addrModulation;		break;
			case paramCurrent:		addr += addrCurrent;		break;
			case paramMode:			addr += addrMode;			break;
			case paramPause:		addr += addrPause;			break;
			case cmnprmStartPrg:	addr = addrStartPrg;		break;
		}*/
		return readByteEE(addr);
	}	
	return 0;
}

void SetValue(u8 a_nParamId, u8 a_nVal)
{
	if (a_nParamId >= paramPrePressing && a_nParamId <= cmnprmPedalNum)
	{
		u16 addr = 0;
		if (a_nParamId == cmnprmStartPrg)
			addr = addrStartPrg;
		else if (a_nParamId == cmnprmPedalNum)
			addr = addrPedalNum;
		else
			addr = m_nCurPrg * paramNum + a_nParamId + 1; // a_nParamId + 1 == addr...
		switch(a_nParamId)
		{
			case paramPrePressing:
				if (a_nVal > maxPrePressing || a_nVal < minPrePressing)
					return;
				m_nPrePressing = a_nVal;
				//writeByteEE(m_nCurPrg * paramNum + addrPrePressing, a_nVal);
				break;
			case paramPressing:
				if (a_nVal > maxPressing || a_nVal < minPressing)
					return;
				m_nPressing = a_nVal;
				//writeByteEE(m_nCurPrg * paramNum + addrPressing, a_nVal);
				break;
			case paramHeating:
				if (a_nVal > maxHeating || a_nVal < minHeating)
					return;
				m_nHeating = a_nVal;
				//writeByteEE(m_nCurPrg * paramNum + addrHeating, a_nVal);
				break;
			case paramForging:
				if (a_nVal > maxForging || a_nVal < minForging)
					return;
				m_nForging = a_nVal;
				//writeByteEE(m_nCurPrg * paramNum + addrForging, a_nVal);
				break;
			case paramModulation:
				if (a_nVal > maxModulation || a_nVal < minModulation)
					return;
				m_nModulation = a_nVal;
				//writeByteEE(m_nCurPrg * paramNum + addrModulation, a_nVal);
				break;
			case paramCurrent:
				if (a_nVal > maxCurrent || a_nVal < minCurrent)
					return;
				m_nCurrent = a_nVal;
				//writeByteEE(m_nCurPrg * paramNum + addrCurrent, a_nVal);
				break;
			case paramMode:
				if (a_nVal > LAST_MODE)
					return;
				m_nMode = a_nVal;
				//writeByteEE(m_nCurPrg * paramNum + addrMode, a_nVal);
				switchModeHL(m_nMode);
				break;
			case paramPause:
				if (a_nVal < MIN_PAUSE || a_nVal > MAX_PAUSE)
					return;
				m_nPause = a_nVal;
				//writeByteEE(m_nCurPrg * paramNum + addrPause, a_nVal);
				break;
			case cmnprmStartPrg:
				if (a_nVal > lastPrg || a_nVal < firstPrg)
					return;
				//writeByteEE(addrStartPrg, a_nVal);
				break;
			case cmnprmPedalNum:
				if (a_nVal > maxPedalNum || a_nVal < minPedalNum)
					return;
				m_nPedalNum = a_nVal;
				//writeByteEE(addrStartPrg, a_nVal);
				break;
		}
		writeByteEE(addr, a_nVal);
	}
}

void initParams()
{
	wdt_start(wdt_250ms);
	// init common params
	m_nCurPrg = readByteEE(addrStartPrg);
	if (m_nCurPrg > lastPrg)
	{
		m_nCurPrg = firstPrg;
		writeByteEE(addrStartPrg, firstPrg);
	}
	m_nPedalNum = readByteEE(addrPedalNum);
	// init local params
	initPrgParams(m_nCurPrg); // обновляю значение параметров программы
}

// инициализация переменных из eeprom
void initPrgParams(u8 a_nPrg)
{// выполняется 95мкс
	u16 shift = a_nPrg * paramNum;
	m_nPrePressing = readByteEE(shift + addrPrePressing); // число предварительного сжатия
	if (m_nPrePressing < minPrePressing || m_nPrePressing > maxPrePressing)
		m_nPrePressing = defPrePressing;
	m_nPressing = readByteEE(shift + addrPressing); // число сжатия
	if (m_nPressing < minPressing || m_nPressing > maxPressing)
		m_nPressing = defPressing;
	m_nHeating = readByteEE(shift + addrHeating); // читаем число нагрева
	if (m_nHeating < minHeating || m_nHeating > maxHeating)
		m_nHeating = defHeating;
	m_nForging = readByteEE(shift + addrForging); // число проковки
	if (m_nForging < minForging || m_nForging > maxForging)
		m_nForging = defForging;
	m_nCurrent = readByteEE(shift + addrCurrent); // читаем число мощности тока
	if (m_nCurrent < minCurrent || m_nCurrent > maxCurrent)
		m_nCurrent = defCurrent;
	m_nModulation = readByteEE(shift + addrModulation); // читаем число модуляции
	if (m_nModulation < minModulation || m_nModulation > maxModulation)
		m_nModulation = defModulation;
	setCurMode(a_nPrg); // читаем значение режима сварки
	m_nPause = readByteEE(shift + addrPause); // читаем значение паузы между циклами сварки
	if (m_nPause < MIN_PAUSE || m_nPause > MAX_PAUSE)
		m_nPause = MAX_PAUSE;
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
