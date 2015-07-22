#include "IDE.h"
#include "lcd_drv.h"

extern const u8
	_Empty[],
	_ViewParams1[],
	_ViewParams2[],
	_PrePressing[],
	_Pressing[],
	_Heating[],
	_Forging[],
	_Modulation[],
	_Current[],
	_Editing[],
	_ChooseParam[],
	_Splash1[],
	_Splash2[],
	_Demo1[],
	_Demo2[],
	_Ready[],
	_ForWelding[];


//-------------------------------------------------------------------
extern volatile tagFlags flags;
extern volatile u32 waitTime;
extern u8 get_key();
//===================================================================
//===================  Утилитки вывода на экран  ====================
//=========================  рисую символ  ==========================
void WrChar(u8 a_char, u8 a_x, u8 a_y)
{
#ifndef _DEBUG_
	lcd_gotoxy(a_x, a_y);
	lcd_putc(a_char);
#endif
}// WrChar //
//===============  рисую десимальное число (0..99) ==================
void Wr3Dec(u8 a_data, u8 a_x, u8 a_y)
{
#ifndef _DEBUG_
	u8 h = (a_data / 100) + 0x30;
	u8 m = ((a_data % 100) / 10) + 0x30;
	u8 l = (a_data % 10) + 0x30;
	lcd_gotoxy(a_x, a_y);
	if (h == 0x30)
	{
		lcd_data(' ');
		if (m == 0x30)
			lcd_data(' ');
		else
			lcd_data(m);
	}
	else
	{
		lcd_data(h);
		lcd_data(m);
	}
	lcd_data(l);
#endif
}
//===============  рисую десимальное число (0..99) ==================
void WrDec(u8 a_data, u8 a_x, u8 a_y)
{
#ifndef _DEBUG_
	u8 l = (a_data % 10) + 0x30;
	u8 h = (a_data / 10) + 0x30;
	lcd_gotoxy(a_x, a_y);
	if (h == 0x30)
		lcd_data(' ');
	else
		lcd_data(h);
	lcd_data(l);
#endif
}
//===============  рисую десимальное число (0..9) ==================
void Wr1Dec(u8 a_data, u8 a_x, u8 a_y)
{
#ifndef _DEBUG_
	u8 l = (a_data % 10) + 0x30;
	lcd_gotoxy(a_x, a_y);
	lcd_data(l);
#endif
}
//===================================================================
#ifdef _DEMO_VERSION_
void SendStr(u8 * str, char num)
{
	if (num == 2)
	{
		lcd_putc(*str);
		lcd_putc(*(str+1));
	}
	else
		for(u8 i = 0; i < num; i++)
			lcd_putc(*(str + i));
}// SendStr //

void SplashScreen()
{
	wdt_start(wdt_250ms);
	lcd_home();
	u8 str1[16], str2[16];

	memcpy_P(str1, _Splash1, 16);
	memcpy_P(str2, _Splash2, 16);
	
	// въезжаю словами в экран
	for(u8 i = 0; i <= 16; i+=2)
	{
		lcd_clrscr();
		lcd_gotoxy(16-i, 0);
		SendStr(str1, i + 1);
		lcd_gotoxy(0, 1);
		SendStr(str2 + 16 - i, i + 1);
		_delay_ms(150);
		wdt_feed();
	}

	lcd_puts_p((const char *)_Splash1);
	lcd_gotoxy(0, 1);
	lcd_puts_p((const char *)_Splash2);
	
	for(u8 i = 0; i < 20; i++) // жду 2с
	{
		_delay_ms(100);
		wdt_feed();
	}	
	
	for(u8 i = 0; i <= 16; i+=2)
	{
		lcd_clrscr();
		SendStr(str1 + i, 16 - i);
		lcd_gotoxy(i, 1);
		SendStr(str2, 16);
		_delay_ms(150);
		wdt_feed();
	}
}
#endif

void WriteWeldReadiness()
{
	lcd_gotoxy(0, lcdstr1);
	lcd_puts_p((const char *)_Ready);
	lcd_gotoxy(0, lcdstr2);
	lcd_puts_p((const char *)_ForWelding);
}
