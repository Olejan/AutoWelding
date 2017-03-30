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
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
		if (flags.halfPeriod || flags.transswitchoff) // ���� ������ ����� ��������� �����
#else
		if (flags.halfPeriod) // ���� ������ ���������� �� int0
#endif
			return;
		_delay_us(8);
	}
}
// �������� ����� 40��� � ��������� ����� int0
BOOL wait_100us()
{
	u8 i = 10;
	while(i--)
	{
		_delay_us(7);
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
		if (flags.halfPeriod || flags.transswitchoff) // ���� ������ ����� ��������� �����
#else
		if (flags.halfPeriod) // ���� ������ ���������� �� int0
#endif
			return TRUE;
	}
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	return FALSE; // ���������� �� ����� ���������� ������� �� ���������
}
// �������� ����� 200��� � ��������� ����� int0
BOOL wait_300us()
{
	u8 i = 30;
	while(i--)
	{
		_delay_us(8);
#ifdef SWITCH_OFF_TRANS_BY_BACK_FRONT
		if (flags.halfPeriod || flags.transswitchoff) // ���� ������ ����� ��������� �����
#else
		if (flags.halfPeriod) // ���� ������ ���������� �� int0
#endif
			return TRUE;
	}
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	return FALSE; // ���������� �� ����� ���������� ������� �� ���������
}
// ������ ������� � ��������� 1��
BOOL is_time_expired(u32 time)
{
	i32 tmp = nTimeMs - nStartTime;
	if (tmp >= time || nStartTime > nTimeMs)
		return TRUE;
	return FALSE;
}

// �������� �����
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
	WDTCR = 1<<WDTOE | 1<<WDE; // �������� wdt � ���������� ���������� (16��)
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
		GICR |= 1<<INT0; // ��������� INT0
	else
		GICR &= ~(1<<INT0); // ���������� INT0
}*/

void switchTrans(u8 a_state)
{
	if (flags.currentIsEnable == 0)
	{
		PORTTRANS |= 1<<pinTrans; // ��������� �������������
		return; // � �������
	}		
	if (a_state == ON)
		PORTTRANS &= ~(1<<pinTrans); // �������� �������������
	else
		PORTTRANS |= 1<<pinTrans; // ��������� �������������
}

void switchValve1(u8 state)
{
	if (state == ON)
		PORTVALVE1 &= ~(1<<pinValve1);
	else
		PORTVALVE1 |= 1<<pinValve1;
	switchHL(pinPrePressingHL, state);
}
void switchValve2(u8 state)
{
	if (state == ON)
		PORTVALVE2 &= ~(1<<pinValve2);
	else
		PORTVALVE2 |= 1<<pinValve2;
	switchHL(pinPressingHL, state);
}

void switchHL(u8 line, u8 state)
{
#ifdef LED_COMMON_CATHODE
	if (state == ON)
		PORTLED &= ~(1 << line);
	else
		PORTLED |= 1 << line;
#else
	if (state == ON)
		PORTLED |= 1 << line;
	else
		PORTLED &= ~(1 << line);
#endif
}

void SwitchAllLED(u8 a_state)
{
	if(a_state == ON)
	{
		PORTLED = 0;
	}
	else
	{
		PORTLED = 0xff;
	}
}

void switchModeHL(u8 a_mode)
{
	if (a_mode == SIMPLE_MODE)
	{
		switchHL(pinAutoHL, OFF);
		switchHL(pinSimpleHL, ON);
	}
	else
	{
		switchHL(pinSimpleHL, OFF);
		switchHL(pinAutoHL, ON);
	}
}

void switchBrightness(u8 a_state)
{
	if (a_state == ON)
	{
		PORT_IND_BRT |= 1 << pinIndBrt;
	}
	else
	{
		PORT_IND_BRT &= ~(1 << pinIndBrt);
	}
}
