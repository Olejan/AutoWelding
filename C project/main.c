﻿/*
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
static u8 _backfront = 0;
#endif
volatile tagFlags flags;
//=========================================================================
extern void DoMenu();
extern u8 DoWelding();
extern u8 getCurMenuId();
extern u8 get_key();
extern void SetMenu(const MenuItem* a_curMenu);
extern MenuItem mPrograms;
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

ISR (INT0_vect) // прерывание по синхроимпульсу
{
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
	syncpresent = SYNC_NUM; // обновляю
#endif
	//pulsCnt++;
	// можно здесь запускать таймер на время включения диода до 0, а в процедуре обработки прерывания таймера отключать его и устанавливать флаг flags.halfPeriod = 1
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
	if(flags.syncfront == _backfront) // если задний фронт
	{
		PORTTRANS |= 1<<pinTrans; // отключаем трансформатор 
		flags.transswitchoff = 1;
	}
	else
	{
		flags.halfPeriod = 1;
		//PORTTRANS &= ~(1<<pinTrans);
	}
	flags.syncfront ^= 1;
#else
	flags.halfPeriod = 1;
#endif
	//PORTTRANS ^= 1<<pinTrans;
}

ISR (INT1_vect)
{//return;
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
	TIMSK = (1<<TOIE0); // разрешаю прерывание по переполнению Т0
	TCNT0 = 0xc1; //(0x82 - 2 ms) (0xc1 - 1ms)
	TCCR0 = (1<<CS01)|(1<<CS00);    // включаю Т0 с прескаллером 64

	// INT0
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
// Узнаём, какой фронт задний по разнице между прерываниями, и фиксируем это
void initFronts()
{
	wdt_start(wdt_1s);
	begin:
	if(syncpresent == 0)
	{
		WriteMessage(_SignalAbscent, _Synch);
		_delay_ms(500);
		wdt_feed();
		WriteMessage(_Empty, _Empty);
		_delay_ms(500);
		wdt_feed();
		goto begin;
	}
	u32 first = 0;
	u32 second = 0;
	flags.syncfront = 0;
	while(flags.syncfront == 0)
	{
		if(syncpresent == 0)
			goto begin;
	}
	flags.syncfront = 0;
	while(flags.syncfront == 0 && syncpresent)
	{
		_delay_us(1);
		first++;
	}
	if(syncpresent == 0)
		goto begin;
	flags.syncfront = 0;
	while(flags.syncfront == 0 && syncpresent)
	{
		_delay_us(1);
		second++;
	}
	if(syncpresent == 0)
		goto begin;
	flags.halfPeriod = 0;
	flags.transswitchoff = 0;
	//while(flags.halfPeriod == 0 || flags.transswitchoff == 0){}
	if(first > second)
		_backfront = 1;
	else
		_backfront = 0;
	wdt_start(wdt_250ms);
}
#endif
void init()
{
	wdt_start(wdt_250ms);
	initProc();
	initVars();
	initParams();
#ifndef _DEBUG_
	lcd_init(LCD_DISP_ON);
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
	initFronts();
#endif
	//init_lcd_simbols();
//#ifdef _DEMO_VERSION_
	SplashScreen();
//#endif // _DEMO_VERSION_
#endif // _DEBUG_
	SetMenu(&mPrograms);
}

int main()
{
	init();
	//Test();
	wdt_start(wdt_60ms);
	while(1)//{}
	{
		wdt_feed();
		DoMenu();
		if (isPedal1Pressed() == TRUE && (getCurMenuId() == idPrograms)) // если нажата педаль и активное меню - "Программы"
		{
			StartTaskWelding();
			while(isPedal1Pressed())
			{
				u8 res = DoWelding();
				if (res == WELD_HAS_BROKEN)
					break;
				wdt_feed();
			}
			StopTaskWelding();
			SetMenu(&mPrograms);
		}
	}
	return 0;
}
