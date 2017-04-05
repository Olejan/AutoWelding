/*
 * AutoWelding.c
 *
 * Created: 15.09.2013 11:12:04
 *  Author: Punka
 */ 

#include "IDE.h"
#include "lcd_drv.h"
//===================================================================
// MCU ATmega16A
// Fclk = 4MHz
//=========================================================================
/*
//!!// - Места, требующие пристального внимания разработчика
*/
//=========================================================================
volatile u32 nTimeMs = 0; // глобальный счётчик микросекунд
volatile u32 nStartTime = 0; // буфер для засекания отсчёта времени
volatile u32 waitTime; // декриментируемый счётчик для жёстких задержек (без возможности выхода из функции задержки до её окончания)
volatile u8 couScanKeys; // счётчик для сканирования кнопок
volatile u8 syncpresent = SYNC_NUM; // детектор пропадания синхроимпульса - если == 0, значит синхроимпульс пропал.
volatile tagFlags flags;
u8 _nTaskAlarm = 0;
//=========================================================================
extern void DoMenu();
extern u8 TaskWelding();
extern u8 getCurMenuId();
extern u8 get_key();
extern void SetMenu(const MenuItem* a_curMenu);
extern MenuItem mPrograms;
extern MenuItem mParamMode;
extern MenuItem mParamPause;
extern MenuItem mParamPrePressing;
extern MenuItem mParamPressing;
extern MenuItem mParamHeating;
extern MenuItem mParamForging;
extern MenuItem mParamModulation;
extern MenuItem mParamCurrent;

extern void setParamsFromEE();
extern void switchValve1(u8 state);
extern void switchValve2(u8 state);
extern void switchTrans(u8 state);
extern BOOL CheckUpMenuTime(u32 time);
//extern void init_lcd_simbols();
extern void Test();
extern void SplashScreen();
extern void StartTaskWelding();
extern void StopTaskWelding();
extern void initParams();
extern void switchHL(u8 line, u8 state);
extern void AlarmTask();
//=========================================================================

//u8 cnt = 0;
BOOL did = FALSE;
ISR(TIMER0_OVF_vect)
{
	//static u8 i = 0;
	TCNT0 = 0xc1; // 1ms for 4MHz & prescaller = 64
	nTimeMs++;
	if (waitTime) // если есть задача задержки,
		waitTime--; // декриментируем счётчик
	/*else
	{
		waitTime = 1000;
		PORTC ^= 1<<i;
		if (++i == 8)
			i = 0;
	}*/
	if (couScanKeys)
		couScanKeys--;
	else
	{
		couScanKeys = SCAN_KEY_TIME; // t = 0.1 s
		flags.scanKey = 1;

		
		if (!(PIN_BUTTON_CURRENT & (1 << pin_CURRENT)))
		{
			if (/*cnt++ > 10 && */did == FALSE)
			{
				if (flags.currentIsEnable == 1)// если ток был разрешён
				{// запрещаем его
					flags.currentIsEnable = 0; // запрещаю ток
					PORTTRANS |= 1<<pinTrans; // если был включён, выключаю трансформатор
				#ifdef LED_COMMON_CATHODE
					PORTLED |= 1 << pinCurrentHL;
				#else
					PORTLED &= ~(1 << pinCurrentHL);
				#endif
				}
				else
				{// разрешаем его
					flags.currentIsEnable = 1; // разрешаю ток
				#ifdef LED_COMMON_CATHODE
					PORTLED &= ~(1 << pinCurrentHL);
				#else
					PORTLED |= 1 << pinCurrentHL;
				#endif
				}
			did = TRUE;
			}
		}
		else
		{
			did = FALSE;
			//cnt = 0;
		}
	}
	if(syncpresent)
		syncpresent--;
	if (!(PIN_ALARM & (1<<pin_ALARM)))
	{
		flags.alarm = 1;
		_nTaskAlarm = 1;
	}
}

volatile u16 _TCNT1;
ISR (TIMER1_OVF_vect)
{
	//TCNT1 = _TCNT1;
	flags.T1IsUp = 1;
	//PORTTRANS ^= 1<<pinTrans;
	TCCR1B = 0; // выключаю T1
}

ISR (INT0_vect) // прерывание по синхроимпульсу
{
	syncpresent = SYNC_NUM; // обновляю
	// можно здесь запускать таймер на время включения диода до 0,
	// а в процедуре обработки прерывания таймера отключать его и устанавливать флаг flags.halfPeriod = 1
	flags.syncfront = 1;
	flags.halfPeriod = 1;
	if(flags.useT1forHeating == 1)
	{
		TCNT1 = _TCNT1;
		TCCR1B = 1; // включаю T1
	}
}

ISR (INT1_vect)
{
	flags.syncfront = 0;
	PORTTRANS |= 1<<pinTrans; // отключаем трансформатор
	flags.transswitchoff = 1;
}

#if 0
ISR (INT2_vect)
{
	if (flags.currentIsEnable == 1)// если ток был разрешён
	{// запрещаем его
		flags.currentIsEnable = 0; // запрещаю ток
		PORTTRANS |= 1<<pinTrans; // если был включён, выключаю трансформатор
#ifdef LED_COMMON_CATHODE
		PORTLED |= 1 << pinCurrentHL;
#else
		PORTLED &= ~(1 << pinCurrentHL);
#endif
	}		
	else
	{// разрешаем его
		flags.currentIsEnable = 1; // разрешаю ток
#ifdef LED_COMMON_CATHODE
		PORTLED &= ~(1 << pinCurrentHL);
#else
		PORTLED |= 1 << pinCurrentHL;
#endif
	}
}
#endif

ISR(BADISR_vect)
{// если зашли по неправильному вектору
	// значит рестартуем процессор
	WDTCR = 1<<WDTOE | 1<<WDE; // запускаю wdt с наименьшим интервалом (16мс)
	for(;;);
}
void initProc()
{
	// T0
	TIMSK = (1<<TOIE0) | (1<<TOIE1); // разрешаю прерывание по переполнению Т0
	TCNT0 = 0xc1; //(0x82 - 2 ms) (0xc1 - 1ms)
	TCCR0 = (1<<CS01)|(1<<CS00);    // включаю Т0 с прескаллером 64
	// T1
	TCNT1H = 0xFE;
	TCNT1L = 0x70;
	//TCCR1B = 1;//<<CS10;

	PORTD = 3 << 2; // подтяжка входов прерывания
	MCUCR = (1 << ISC11) | (3 << ISC00); // int0 по переднему фронту (синхроимпульс) и int1 по заднему (си)
	GICR = (1 << INT0) | (1 << INT1)/* | (1 << INT2)*/; // разрешаю внешние прерывания

	//SFIOR |= 1 << PUD; // отключаю внутреннюю подтяжку портов
	DDRTRANS |= 1<<pinTrans;
	PORTTRANS |= 1<<pinTrans;
	DDRVALVE1 |= 1<<pinValve1;
	PORTVALVE1 |= 1<<pinValve1;
	DDRVALVE2 |= 1<<pinValve2;
	PORTVALVE2 |= 1<<pinValve2;
	DDRLED = 0xff;
	PORTLED = ALL_LEDS_OFF; // выключить все светодиоды
	//PORTBUTTONS |= 0xf<<2; // устанавливаю подтягивающие резисторы на кнопки
	DDR_BUTTON_CURRENT &= ~(1 << pin_CURRENT);
	DDR_BUTTON_UP &= ~(1 << pin_UP);
	DDR_BUTTON_LEFT &= ~(1 << pin_LEFT);
	DDR_BUTTON_RIGHT &= ~(1 << pin_RIGHT);
	DDR_BUTTON_DOWN &= ~(1 << pin_DOWN);
	PORT_BUTTON_CURRENT |= 1 << pin_CURRENT;
	PORT_BUTTON_UP |= 1 << pin_UP;
	PORT_BUTTON_LEFT |= 1 << pin_LEFT;
	PORT_BUTTON_RIGHT |= 1 << pin_RIGHT;
	PORT_BUTTON_DOWN |= 1 << pin_DOWN;

	PORTPEDAL1 |= 1 << pinPedal1; // и педали
	PORTPEDAL2 |= 1 << pinPedal2;

	DDR_ALARM &= ~(1 << pin_ALARM);
	PORT_ALARM |= 1 << pin_ALARM; // подтягивающий резистор
	DDR_IND_BRT |= 1 << pinIndBrt;
	PORT_IND_BRT |= 1 << pinIndBrt;
	
	asm("sei");
}

void initVars()
{
	couScanKeys = SCAN_KEY_TIME;
	//waitTime = 1000;
	flags.currentIsEnable = 1; // разрешаю ток
	switchHL(pinCurrentHL, ON);
}
extern const char _SignalAbscent[], _Synch[], _Empty[],
	_Attention[],
	_Alarm[],
	_Checkup[],
	_Equipnent[],
	_PedalIsPressed[],
	_ReleaseIt[];
extern void WriteMessage(const char* str1, const char* str2);
void CheckSynchroImpulse()
{
	wdt_start(wdt_1s);
	u8 _tm, cnt;
	st:
	_tm = 10;
	cnt = 0;
	while(_tm > 0) // выжидаю первые 10 импульсов
	{
		flags.syncfront = 0;
		while(flags.syncfront == 0)
		{
			if(syncpresent == 0 || cnt > 150)
			{
				
				while (syncpresent == 0)
				{
					wdt_feed();
					WriteMessage(_SignalAbscent, _Synch);
					_delay_ms(500);
					wdt_feed();
					WriteMessage(_Empty, _Empty);
					_delay_ms(500);
				}
				goto st;
			}
			_delay_us(100);
			cnt++;
		}
		_tm--;
		cnt = 0;
	}
}
/*void WriteMsgAndBlinkLED(u8 a_var)
{
	wdt_feed();
	if (a_var == 0)
		WriteMessage(_Attention, _Alarm);
	else
		WriteMessage(_Checkup, _Equipnent);
	for (u8 i = 0; i < 4; i++)
	{
		SwitchAllLED(i%2);
		_delay_ms(500);
		wdt_feed();
	}
}*/

void StartTaskAlarm()
{
	_nTaskAlarm = 1;
}
void StopTaskAlarm()
{
	flags.alarm = 0;
	_nTaskAlarm = 0;
	SwitchAllLED(OFF);
	if (flags.currentIsEnable == 1)// если ток был разрешён
	{// разрешаем его
#ifdef LED_COMMON_CATHODE
		PORTLED &= ~(1 << pinCurrentHL);
#else
		PORTLED |= 1 << pinCurrentHL;
#endif
	}
	else
	{// запрещаем его
		PORTTRANS |= 1<<pinTrans; // если был включён, выключаю трансформатор
#ifdef LED_COMMON_CATHODE
		PORTLED |= 1 << pinCurrentHL;
#else
		PORTLED &= ~(1 << pinCurrentHL);
#endif
	}

	switchModeHL(curMode.get());
	//SetMenu(&mPrograms);
	wdt_start(wdt_60ms);
}
BOOL NextCase()
{
	if ((flags.scanKey && get_key() != keyEmpty) || !(PIN_BUTTON_CURRENT & (1 << pin_CURRENT)))
	{
		StopTaskAlarm();
		return FALSE;
	}
	if (waitTime > 0)
		return FALSE;
	return TRUE;
}
void TaskAlarm()
{
	u8 i = 0;
	while (1)
	{
		switch (_nTaskAlarm)
		{
			case 0:
				return;
			case 1:
				wdt_start(wdt_2s);
				//StopTaskWelding();
				i = 0;
				waitTime = 0;
				_nTaskAlarm++;
				break;
			case 2:
				if (NextCase() == FALSE)
					break;
				wdt_feed();
				waitTime = 500;
				WriteMessage(_Attention, _Alarm);
				SwitchAllLED(i%2);
				i++;
				if (i > 3)
				{
					_nTaskAlarm++;
					i = 0;
				}
				break;
			case 3:
				if (NextCase() == FALSE)
					break;
				wdt_feed();
				waitTime = 500;
				WriteMessage(_Checkup, _Equipnent);
				SwitchAllLED(i%2);
				i++;
				if (i > 3)
				{
					_nTaskAlarm--;
					i = 0;
				}
				break;
		}
	}
}
/*u8 cnt = 0;
BOOL did = FALSE;
void TaskCurrent()
{
	if (!(PIN_BUTTON_CURRENT & (1 << pin_CURRENT)))
	{
		if (cnt++ > 10 && did == FALSE)
		{
			switchCurrent();
			did = TRUE;
		}
	}
	else
	{
		did = FALSE;
		cnt = 0;
	}
}*/
void MainTask()
{
	if (flags.alarm == 1)
		TaskAlarm();
	//TaskCurrent();
}
void init()
{
	//Test();
	wdt_start(wdt_250ms);
	initProc();
	initVars();
	initParams();
	lcd_init(LCD_DISP_ON);
	if (isPedal1Pressed() == TRUE || isPedal2Pressed() == TRUE)
	{
		WriteMessage(_PedalIsPressed, _ReleaseIt);
		while (isPedal1Pressed() == TRUE || isPedal2Pressed() == TRUE)
		{
			wdt_feed();
		}
	}
#ifndef _DEBUG_
	CheckSynchroImpulse();
	//init_lcd_simbols();
	SplashScreen();
#endif // _DEBUG_
	SetMenu(&mPrograms);
}

void RefreshSqreen(u8 a_menu)
{
	const MenuItem * addr;
	switch (a_menu)
	{
		case idPrograms: addr = &mPrograms; break;
		case idChooseMode: addr = &mParamMode; break;
		case idChoosePause: addr = &mParamPause; break;
		case idChoosePrePressing: addr = &mParamPrePressing; break;
		case idChoosePressing: addr = &mParamPressing; break;
		case idChooseHeating: addr = &mParamHeating; break;
		case idChooseForging: addr = &mParamForging; break;
		case idChooseModulation: addr = &mParamModulation; break;
		case idChooseCurrent: addr = &mParamCurrent; break;
		default: addr = &mPrograms; break;
	}
	SetMenu(addr);
}

int main()
{
	init();
	wdt_start(wdt_60ms);
	while(1)//{}
	{
		wdt_feed();
		DoMenu();
		u8 menu = getCurMenuId();
		if (isPedal1Pressed() == TRUE // если нажата педаль
			&& (
					menu == idPrograms // и активное меню - "Программы" или меню выбора параметров
					|| (menu >= idBeginChooseParams && menu <= idEndChooseParams)
				)
			)
		{
			StartTaskWelding();
			while(isPedal1Pressed())
			{
				/*u8 res = */TaskWelding();
				if (flags.alarm == 1)
					break;
				wdt_feed();
			}
			StopTaskWelding();
			RefreshSqreen(menu);
		}
		if (flags.alarm == 1)
		{
			TaskAlarm();
			RefreshSqreen(menu);
		}
	}
	return 0;
}
