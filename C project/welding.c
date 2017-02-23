#include "IDE.h"

extern void writeByteEE(u16 addr, u8 data);
extern u8 readByteEE(u16 addr);
extern BOOL wait_100us();
extern BOOL wait_300us();
extern void wait_x10us(u8 a_i);
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


static u8 m_nPrePressing = 0; // число предварительного сжатия
static u8 m_nPressing = 0; // число сжатия
static u8 m_nHeating = 0; // число нагрева
static u8 m_nForging = 0; // число проковки
static u8 m_nModulation = 0; // число модуляции
static u8 m_nCurrent = 0; // число мощности тока
static u8 m_cntModPeriod = 0; // счётчик периодов модуляции
static u16 m_nModDelay = 0; // задержка модуляции
static u8 m_nCurPrg = 0; // текущая программа
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
	//while(!flags.halfPeriod)
	//PORTTRANS &= ~(1<<pinTrans);
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
	//PORTTRANS |= (1<<pinTrans);
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
	flags.transswitchoff = 0;
#endif
	switchTrans(OFF); // отключаем трансформатор
}

/*
 * За 10 проходов проедуры impuls() в doPreHeating() реально трансформатор включается только в последних 6-ти
 * заходах в процедуру. Если технология процесса сварки позволяет, нужно уменьшить задержку с 330 до 270,
 * тогда в каждом заходе в impuls() будет включаться трансформатор
*/
u8 doPreHeating()
{
	wdt_start(wdt_60ms);
	setScreen(scrHeating); // инициализируем экран сварки
	switchHL(pinHeatingHL, ON); // включение светодиода нагрева
	u8 cntPreHeating = 10;
	u8 tmpCnt = 0; // временный счётчик
	flags.halfPeriod = 0; // сбросим флаг прихода полупериода
	while(1)
	{
		while(!flags.halfPeriod) // ждём прихода полупериода
		{
			if (isPedal2Pressed() == FALSE && isPedal1Pressed() == FALSE)
			{
				switchHL(pinHeatingHL, OFF);
				return FALSE;
			}				
		}
		wdt_feed();
		flags.halfPeriod = 0;
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
		flags.transswitchoff = 0;
#endif
		tmpCnt = cntPreHeating;
		while(tmpCnt--)
			_delay_us(270/*330*/);//!!//
		_delay_ms(7);//!!//
		
		impulse();
		
		cntPreHeating--;
		if (!cntPreHeating)
			break;
	}
	return TRUE;
}

u8 doHeating()
{
	wdt_start(wdt_60ms);
	if (m_nModulation)// если модуляция не 0
	{ // расчитываю её параметры
		if ((m_nModulation < 10) && (m_nHeating < m_cntModPeriod))
			m_cntModPeriod = m_nHeating;
		else
			m_cntModPeriod = m_nModulation;
		if (m_nCurrent)
			m_nModDelay = 190 * m_nCurrent / m_cntModPeriod;
	}
	u8 heating = m_nHeating;
	while(heating--)
	{
		while(!flags.halfPeriod) // ждём прерывание int0
		{
			if (isPedal2Pressed() == FALSE && isPedal1Pressed() == FALSE)
			{
				switchHL(pinHeatingHL, OFF);
				return FALSE;
			}				
		}
		wdt_feed();
		flags.halfPeriod = 0; // сбросим флаг
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
		flags.transswitchoff = 0;
#endif
		//WrDec(heating, 14, lcdstr1); // и отправляю туда значение сжатия (~160us)
		u8 cntDelayON = 9 - m_nCurrent;
		while(cntDelayON--)
			_delay_us(777);
		if (m_nCurrent)
		{
			if (m_cntModPeriod)
			{
				u8 tmp = m_cntModPeriod;
				while(tmp--)
					wait_x10us((m_nModDelay / 10) << 2);
				if (!flags.heating)
					m_cntModPeriod--;
			}
		}
		impulse();
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
u8 DoWelding()
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
			if (isPedal2Pressed())
			{
				//P2cntLow = 0;
				u8 res = doPressing(); // прижим сварных деталей
				if (res == FALSE) return WELD_HAS_BROKEN;
				res = doPreHeating(); // прогрев
				if (res == FALSE) return WELD_HAS_BROKEN;
				res = doHeating(); // нагрев
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
	Wr1Dec(m_nCurPrg, 0, lcdstr1); // текущая программа
	Wr3Dec(m_nHeating, 4, lcdstr1); // число нагрева
	Wr1Dec(m_nModulation, 10, lcdstr1); // число модуляции
	Wr1Dec(m_nCurrent, 14, lcdstr1); // число мощности тока
	Wr3Dec(m_nPrePressing, 3, lcdstr2); // число предварительного сжатия
	WrDec(m_nPressing, 7, lcdstr2); // число сжатия
	Wr3Dec(m_nForging, 13, lcdstr2); // число проковки
}

u8 GetValue(u8 a_nParamId)
{
	if (a_nParamId >= paramPrePressing && a_nParamId <= cmnprmStartPrg)
	{
		u16 addr = 0;
		if (a_nParamId == cmnprmStartPrg)
			addr = addrStartPrg;
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
	if (a_nParamId >= paramPrePressing && a_nParamId <= cmnprmStartPrg)
	{
		u16 addr = 0;
		if (a_nParamId == cmnprmStartPrg)
			addr = addrStartPrg;
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

/*void Test()
{
	for(;;)
	{
		initPrgParams(3);
		switchTrans(ON);
		initPrgParams(3);
		switchTrans(OFF);
	}
}*/
