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
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
volatile u8 syncpresent = SYNC_NUM; // детектор пропадания синхроимпульса - если == 0, значит синхроимпульс пропал.
#endif
volatile tagFlags flags;
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
//=========================================================================

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
	}
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
	if(syncpresent)
		syncpresent--;
#endif
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
	MCUCR ^= 1<<ISC00; // следующее прерывание по другому фронту
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
	syncpresent = SYNC_NUM; // обновляю
#endif
	// можно здесь запускать таймер на время включения диода до 0, а в процедуре обработки прерывания таймера отключать его и устанавливать флаг flags.halfPeriod = 1
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
	if (MCUCR & (1<<ISC00)) // если задний фронт (ранее изменил фронт срабатывания, поэтому смотрю другое значение бита)
	{
		flags.syncfront = 0;
		PORTTRANS |= 1<<pinTrans; // отключаем трансформатор 
		flags.transswitchoff = 1;
	}
	else
	{
		flags.syncfront = 1;
		flags.halfPeriod = 1;
		if(flags.useT1forHeating == 1)
		{
			TCNT1 = _TCNT1;
			TCCR1B = 1; // включаю T1
		}
	}
#else
	flags.halfPeriod = 1;
#endif
}

ISR (INT1_vect)
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

	MCUCR = (2 << ISC10); // int1 по заднему фронту (кнопка)
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
	MCUCR |= (1 << ISC00); // int0 по любому фронту (синхроимпульс)
#else
	MCUCR |= (3 << ISC00); // int0 по переднему фронту (синхроимпульс)
#endif
	GICR = (1 << INT0) | (1 << INT1); // разрешаю внешние прерывания

	SFIOR |= 1 << PUD; // отключаю внутреннюю подтяжку портов
	DDRTRANS |= 1<<pinTrans;
	PORTTRANS |= 1<<pinTrans;
	DDRVALVE1 |= 1<<pinValve1;
	PORTVALVE1 |= 1<<pinValve1;
	DDRVALVE2 |= 1<<pinValve2;
	PORTVALVE2 |= 1<<pinValve2;
	DDRLED = 0xff;
	PORTLED = ALL_LEDS_OFF; // выключить все светодиоды
	
	GICR |= 1<<INT0; // включение INT0
	asm("sei");
}

void initVars()
{
	couScanKeys = SCAN_KEY_TIME;
	//waitTime = 1000;
	flags.currentIsEnable = 1; // разрешаю ток
	switchHL(pinCurrentHL, ON);
}
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
extern const char _SignalAbscent[], _Synch[], _Empty[];
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
#endif
void init()
{
	//Test();
	wdt_start(wdt_250ms);
	initProc();
	initVars();
	initParams();
#ifndef _DEBUG_
	lcd_init(LCD_DISP_ON);
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
	CheckSynchroImpulse();                                          
#endif
	//init_lcd_simbols();
//#ifdef _DEMO_VERSION_
	//SplashScreen();
//#endif // _DEMO_VERSION_
#endif // _DEBUG_
	SetMenu(&mPrograms);
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
				u8 res = TaskWelding();
				if (res == WELD_HAS_BROKEN)
					break;
				wdt_feed();
			}
			StopTaskWelding();
			const MenuItem * addr;
			switch (menu)
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
	}
	return 0;
}
