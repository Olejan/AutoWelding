#include "IDE.h"
#include "lcd_drv.h"


/*const u8 ProgressBarChars_1[] PROGMEM =
{
	0x00,0x1F,0x00,0x00,0x00,0x00,0x1F,0x00,  // Empty
	0x00,0x1F,0x10,0x10,0x10,0x10,0x1F,0x00,  // 1/5
	0x00,0x1F,0x18,0x18,0x18,0x18,0x1F,0x00,  // 2/5
	0x00,0x1F,0x1C,0x1C,0x1C,0x1C,0x1F,0x00,  // 3/5
	0x00,0x1F,0x1E,0x1E,0x1E,0x1E,0x1F,0x00,  // 4/5
	0x00,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x00,  // Full
};*/

const u8 ProgressBarChars_2[] PROGMEM =
{
	0x00,0x00,0x1F,0x00,0x00,0x1F,0x00,0x00,  // 0/5
	0x00,0x10,0x1F,0x10,0x10,0x1F,0x10,0x00,  // 1/5
	0x00,0x18,0x1F,0x18,0x18,0x1F,0x18,0x00,  // 2/5
	0x00,0x1C,0x1F,0x1C,0x1C,0x1F,0x1C,0x00,  // 3/5
	0x00,0x1E,0x1F,0x1E,0x1E,0x1F,0x1E,0x00,  // 4/5
	0x00,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x00,  // 5/5
};

/*const u8 ProgressBarChars_3[] PROGMEM =
{
	0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,  // 0/5
	0x1F,0x00,0x10,0x10,0x10,0x10,0x00,0x1F,  // 1/5
	0x1F,0x00,0x18,0x18,0x18,0x18,0x00,0x1F,  // 2/5
	0x1F,0x00,0x1C,0x1C,0x1C,0x1C,0x00,0x1F,  // 3/5
	0x1F,0x00,0x1E,0x1E,0x1E,0x1E,0x00,0x1F,  // 4/5
	0x1F,0x00,0x1F,0x1F,0x1F,0x1F,0x00,0x1F,  // 5/5
};*/

//======================== Prototipes =========================
void SetProgressBar(u8 a_value, u8 a_maxvalue, BOOL a_bShowPercents);
//=============================================================

/*
a_pc - указатель на начало данных в массиве пользовательских символов
a_ddramaddr - адрес в DDRAM, по которому будет лежать пользовательский символ
*/
void SetUserChar(const u8 *a_pc,u8 a_ddramaddr/*0-7*/)
{
	u8 buf;
	u8 cmd = (1<<LCD_CGRAM) | (a_ddramaddr<<3);
	lcd_command(cmd); // используем автоинкремент адреса в LCD
	for(u8 i = 0; i < 8; i++)
	{
		buf = pgm_read_byte(a_pc + i);
		//lcd_command(cmd++);
		lcd_data(buf);
	}
}

void SetStatusBarChars(const u8 *a_pc)
{
	for(u8 i = 0; i < 6; i++)
		SetUserChar(a_pc + i * 8, i);
}

void init_lcd_simbols()
{
	SetStatusBarChars(ProgressBarChars_2);
}
extern volatile u32 waitTime;
#if 0
void PBTest()
{
	/*lcd_gotoxy(0, 1);
	for(u8 i = 0; i < 6; i++)
		lcd_putc(i);*/
	for(u8 i = 0; i <= 73; i++)
	{
		SetProgressBar(i, 73, FALSE);
		waitTime = 100;
		while(waitTime);
	}
}
#endif

enum tagProgressBar
{
	PixelsInChar		= 5,
	CharsInBar			= 16,//13,
	FullScaleCharCnt	= CharsInBar * PixelsInChar,
	EMPTY_SIGN			= 0,
	FULL_SIGN			= 5,
};
extern void switchTrans(u8 a_state);
void SetProgressBar(u8 a_value, u8 a_maxvalue, BOOL a_bShowPercents)
{
//switchTrans(ON);
	lcd_gotoxy(0, 1);
	u16 buf = a_value * CharsInBar;
	u8 fullcou = (u8)(buf / a_maxvalue);
	u8 emptycou = CharsInBar - fullcou - 1;
	if(emptycou > CharsInBar)
		emptycou = 0;
	u8 residue = (u8)(buf % a_maxvalue);
	residue = (residue * PixelsInChar) / a_maxvalue;
	for(u8 i = 0; i < fullcou; i++)
		lcd_data(FULL_SIGN);
	lcd_data(residue);
	for(u8 i = 0; i < emptycou; i++)
		lcd_data(EMPTY_SIGN);
	if(a_bShowPercents == TRUE)
	{
		u8 percent = (a_value * 100) / a_maxvalue;
		if(percent >= 100)
		{
			lcd_gotoxy(CharsInBar - 1, 1);
			lcd_data('1');
			lcd_data('0');
			lcd_data('0');
		}
		else
		{
			lcd_data((percent / 10) + 0x30);
			lcd_data((percent % 10) + 0x30);
		}
		lcd_data('%');
	}
//switchTrans(OFF);
}
