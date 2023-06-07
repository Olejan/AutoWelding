#include "IDE.h"
#include <avr/wdt.h>	 // wdt

extern volatile u32 waitTime;
extern volatile tagFlags flags;
extern void updateLCD();
extern volatile u32 nTimeMs;
extern volatile u32 nStartTime;
//============================================================================
void switchHL(u8 line, u8 state);
//============================================================================

void wait_x10us(u8 a_i)
{
	while(a_i--)
	{
		if (flags.halfPeriod || flags.transswitchoff) // если срочно нужно выключить транс
			return;
		_delay_us(8);
	}
}
// задержка ровно 40мкс с проверкой флага int0
BOOL wait_100us()
{
	u8 i = 10;
	while(i--)
	{
		_delay_us(7);
		if (flags.halfPeriod || flags.transswitchoff) // если срочно нужно выключить транс
			return TRUE;
	}
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	return FALSE; // прерывание во время выполнения функции не приходило
}
// задержка ровно 200мкс с проверкой флага int0
BOOL wait_300us()
{
	u8 i = 30;
	while(i--)
	{
		_delay_us(8);
		if (flags.halfPeriod || flags.transswitchoff) // если срочно нужно выключить транс
			return TRUE;
	}
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	return FALSE; // прерывание во время выполнения функции не приходило
}
// Отсчёт времени с точностью 1мс
BOOL is_time_expired(u32 time)
{
	i32 tmp = nTimeMs - nStartTime;
	if (tmp >= time || nStartTime > nTimeMs)
		return TRUE;
	return FALSE;
}

// Засекаем время
void NoteTime()
{
	nStartTime = nTimeMs;
}
//===================================================================
//	WDT
//===================================================================
void wdt_start(u8 level)
{
#ifdef WDT_ENABLE
	if (level > wdt_2s)
		level = wdt_2s;
	wdt_enable(level);
#endif
}
void wdt_stop()
{
#ifdef WDT_ENABLE
	wdt_disable();
#endif
}
void wdt_feed()
{
#ifdef WDT_ENABLE
	wdt_reset();
#endif
}
/*void ResetProc()
{
	WDTCR = 1<<WDTOE | 1<<WDE; // запускаю wdt с наименьшим интервалом (16мс)
	for(;;);
}*/
//===================================================================
//  EEPROM
//===================================================================
void writeByteEE(u16 addr, u8 data)
{
//return;
	while(EECR&(1<<EEWE));
	asm("cli");
	//while(SPMCR&(1<<SPMEN));
	EEAR=addr;
	EEDR=data;
	EECR|=1<<EEMWE;
	EECR|=1<<EEWE;
	asm("sei");
}

u8 readByteEE(u16 addr)
{
//return 0;
	while(EECR&(1<<EEWE));
	EEAR=addr;
	EECR|=1<<EERE;
	return EEDR;
}
//===================================================================
/*void switchInt0(u8 a_state)
{
	if (a_state == ON)
		GICR |= 1<<INT0; // включение INT0
	else
		GICR &= ~(1<<INT0); // выключение INT0
}*/

void switchTrans(u8 a_state)
{
	if (flags.currentIsEnable == 0)
	{
		PORT_TRANS |= 1<<pin_TRANS; // отключаем трансформатор
		return; // и выходим
	}		
	if (a_state == ON)
		PORT_TRANS &= ~(1<<pin_TRANS); // включаем трансформатор
	else
		PORT_TRANS |= 1<<pin_TRANS; // отключаем трансформатор
}

void switchValve1(u8 state)
{
	if (state == ON)
		PORT_VALVE1 &= ~(1<<pin_VALVE1);
	else
		PORT_VALVE1 |= 1<<pin_VALVE1;
	switchHL(pin_PRE_PRESSING_HL, state);
}
void switchValve2(u8 state)
{
	if (state == ON)
		PORT_VALVE2 &= ~(1<<pin_VALVE2);
	else
		PORT_VALVE2 |= 1<<pin_VALVE2;
	switchHL(pin_PRESSING_HL, state);
}

void switchHL(u8 line, u8 state)
{
#ifdef LED_COMMON_CATHODE
	if (state == ON)
		PORT_LED &= ~(1 << line);
	else
		PORT_LED |= 1 << line;
#else
	if (state == ON)
		PORT_LED |= 1 << line;
	else
		PORT_LED &= ~(1 << line);
#endif
}

void led_on(u8 pin)
{
#ifdef LED_COMMON_CATHODE
	PORT_LED &= ~(1 << pin);
#else
	PORT_LED |= 1 << pin;
#endif
}

void led_off(u8 pin)
{
#ifdef LED_COMMON_CATHODE
	PORT_LED |= 1 << pin;
#else
	PORT_LED &= ~(1 << pin);
#endif
}

void led_switch(u8 line)
{
	if (!(PORT_LED & (1 << line)))
		PORT_LED |= 1 << line;
	else
		PORT_LED &= ~(1 << line);
}

void SwitchAllLED(u8 a_state)
{
	if(a_state == ON)
	{
		PORT_LED = 0;
	}
	else
	{
		PORT_LED = 0xff;
	}
}

void switchModeHL(u8 a_mode)
{
	if (a_mode == SIMPLE_MODE)
	{
		switchHL(pin_AUTO_HL, OFF);
		switchHL(pin_SIMPLE_HL, ON);
	}
	else if (a_mode == AUTO_MODE)
	{
		switchHL(pin_SIMPLE_HL, OFF);
		switchHL(pin_AUTO_HL, ON);
	}
	else if (a_mode)
	{
		switchHL(pin_SIMPLE_HL, ON);
		switchHL(pin_AUTO_HL, ON);
	}
}

void switchBrightness(u8 a_state)
{
	if (a_state == ON)
	{
		PORT_IND_BRT |= 1 << pin_IND_BRT;
	}
	else
	{
		PORT_IND_BRT &= ~(1 << pin_IND_BRT);
	}
}
/*
void switchCurrent()
{
	if (flags.currentIsEnable == 1)// если ток был разрешён
	{// запрещаем его
		flags.currentIsEnable = 0; // запрещаю ток
		PORT_TRANS |= 1<<pinTrans; // если был включён, выключаю трансформатор
#ifdef LED_COMMON_CATHODE
		PORT_LED |= 1 << pinCurrentHL;
#else
		PORT_LED &= ~(1 << pinCurrentHL);
#endif
	}		
	else
	{// разрешаем его
		flags.currentIsEnable = 1; // разрешаю ток
#ifdef LED_COMMON_CATHODE
		PORT_LED &= ~(1 << pinCurrentHL);
#else
		PORT_LED |= 1 << pinCurrentHL;
#endif
	}
}*/
