/*
 * AutoWelding.c
 *
 * Created: 15.09.2013 11:12:04
 *  Author: gos
 */ 

#include "IDE.h"
#include "lcd_drv.h"
//#include "usart/usart.h"
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

extern void led_switch(u8 line);
void SendStr(u8 * str, char num);
//=========================================================================

//char msg[24];

#include "AVR_ModBus.h"

#if 0
// 8 bytes tx fifo buffer,
// 16 bytes rx fifo buffer
// interrupt driven USART
//typedef Usart<16, 16> usart;

//extern struct GyverFIFO _uart_struct;

ISR(USART_UDRE_vect)
{
	//usart::TxHandler();
	/*if (_uart_struct.available())
		_uart_struct.write(UDR);*/
}

ISR(USART_RXC_vect)
{
	//usart::RxHandler();
	/*if (_uart_struct.available())
		UDR = _uart_struct.read();*/
}
#endif// 0



//============================ uart ================================

unsigned char RcCount, TrCount;  //счетчик принятых/переданных данных
bool StartRec = false;// false/true начало/прием посылки
bool bModBus = false;  //флаг обработки посылки
unsigned char cNumRcByte0; //кол-во принятых байт
unsigned char cNumTrByte0;  //кол-во передаваемых байт
unsigned char cmRcBuf0[MAX_LENGHT_REC_BUF]; //буфер принимаемых данных
unsigned char cmTrBuf0[MAX_LENGHT_TR_BUF]; //буфер передаваемых данных
unsigned char m_nModbusId = MAX_MODBUS_ID; // Modbus Id

extern u8 m_nPrePressing; // число предварительного сжатия
extern u8 m_nPressing; // число сжатия
extern u8 m_nModulation; // число модуляции
extern u8 m_nCurrent; // число мощности тока
extern u8 m_nHeating; // число нагрева
extern u8 m_nForging; // число проковки
extern u8 m_nMode;
extern u8 m_nPause;
extern u8 m_nCurPrg; // текущая программа
extern u8 m_nPedalNum; // количество педалей
extern u8 m_nBrtns;

//настройка UART
void InitModBus(void)
{
	UBRRHi = Hi(BAUD_DIVIDER);
	UBRRLow = Low(BAUD_DIVIDER);
	/*UCSRA_CLR();
	UCSRB_SET();
	UCSRC_SET();*/
	UCSRA = 0;
	UCSRB = /*1 << RXEN | 1 << TXEN | */1 << RXCIE | 0 << TXCIE;
	UCSRC = 1 << URSEL | 1 << UCSZ0 | 1 << UCSZ1; //8bit, None, 1stop.bit
	//останавливаем таймер0
//	StopTimer0();
	//разрешаем прерывание по переполнению таймера0
//	Timer0InterruptEnable();
}//end InitModBus()


i8 t3_5_1ms_num = 0;
inline void start_t3_5_watching()
{
	flags.t3_5_started = true;
	t3_5_1ms_num = 0;
}
inline void stop_t3_5_watching()
{
	flags.t3_5_started = false;
	t3_5_1ms_num = 0;
}


// Включает uart
void modbus_on()
{
	UCSRB |= 1 << RXEN;
	UCSRB |= 1 << TXEN;
	stop_t3_5_watching();
}

// Выключает uart
void modbus_off()
{
	UCSRB &= ~(1 << RXEN);
	UCSRB &= ~(1 << TXEN);
	stop_t3_5_watching();
}

// Проверяет, активен ли uart
inline bool is_modbus_active()
{
	return UCSRB & (1 << RXEN);
}

void CheckModBus(void)
{
	if (bModBus)
	{
		cNumTrByte0 = ModBus(cNumRcByte0); //обработка принятого соообщения ModBus
		if (cNumTrByte0 != 0)
		{
			TrCount = 0;
			StartTrans();
		} //end if (cNumTrByte0!=0)
		bModBus = false;
	} //end if (bModBus)
} //end CheckModBus(void)
//-----------------------------Прерывания-------------------------------

//прерывание по завершению получения данных
//получение запроса от мастера
ISR(USART_RXC_vect)
{
	char cTempUART;

	cTempUART = UDR;
	
	if (UCSRA & (1 << FE)) //FE-ошибка кадра   ***может не работать на других МК****
	{
		UDR = 0xFE;
		return;
	}

	if (!StartRec)//если это первый байт, то начинаем прием
	{
		StartRec = true;
		RcCount = 0;
	}
	if (RcCount < MAX_LENGHT_REC_BUF) //если еще не конец буфера
	{
		cmRcBuf0[RcCount++] = cTempUART;
	}
	else //буфер переполнен
	{
		cmRcBuf0[MAX_LENGHT_REC_BUF - 1] = cTempUART;
	}
	start_t3_5_watching();
} //end ISR(USART_RXC_vect)


//прерывание по опустошению регистра данных (UDR)
//отправка ответа
ISR(USART_UDRE_vect)
{
	if (TrCount < cNumTrByte0)
	{
		UDR = cmTrBuf0[TrCount];
		TrCount++;
	} //end if
	else
	{
		StopTrans();
		TrCount = 0;
	} //end else
}//end ISR(USART_UDRE_vect)

//==================================================================


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

		
		if (!(PIN_BUTTON_CURRENT & (1 << pin_BUTTON_CURRENT)))
		{
			if (/*cnt++ > 10 && */did == FALSE)
			{
				if (flags.currentIsEnable == 1)// если ток был разрешён
				{// запрещаем его
					flags.currentIsEnable = 0; // запрещаю ток
					PORT_TRANS |= 1<<pin_TRANS; // если был включён, выключаю трансформатор
				#ifdef LED_COMMON_CATHODE
					PORT_LED |= 1 << pin_CURRENT_HL;
				#else
					PORT_LED &= ~(1 << pin_CURRENT_HL);
				#endif
				}
				else
				{// разрешаем его
					flags.currentIsEnable = 1; // разрешаю ток
				#ifdef LED_COMMON_CATHODE
					PORT_LED &= ~(1 << pin_CURRENT_HL);
				#else
					PORT_LED |= 1 << pin_CURRENT_HL;
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
	
	//if (flags.modbus_enabled == 1)
	if (is_modbus_active())
	{
		if (flags.t3_5_started)
		{
			if (++t3_5_1ms_num >= 4) // 4ms
			{
				flags.t3_5_started = false; // Время 3,5мс истекло
				t3_5_1ms_num = 0; // сбрасываю счётчик миллисекунд для 3,5мс
				if (StartRec)
				{
					StartRec = false; //посылка принята
					cNumRcByte0 = RcCount;  //кол-во принятых байт
					bModBus = true;//
					//StopTimer0();//остановим таймер
					stop_t3_5_watching();
				}
			}
		}
	}
}

volatile u16 _TCNT1;
ISR (TIMER1_OVF_vect)
{
	//TCNT1 = _TCNT1;
	flags.T1IsUp = 1;
	//PORT_TRANS ^= 1<<pin_TRANS;
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
	PORT_TRANS |= 1<<pin_TRANS; // отключаем трансформатор
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
		PORT_LED &= ~(1 << pin_CURRENT_HL);
#else
		PORT_LED |= 1 << pin_CURRENT_HL;
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
	// T1 - 0xFE70 = 100 мкс
	TCNT1H = 0xFE;
	TCNT1L = 0x70;
	//TCCR1B = 1;//<<CS10;

	PORTD = 3 << 2; // подтяжка входов прерывания
	MCUCR = (1 << ISC11) | (3 << ISC00); // int0 по переднему фронту (синхроимпульс) и int1 по заднему (си)
	GICR = (1 << INT0) | (1 << INT1)/* | (1 << INT2)*/; // разрешаю внешние прерывания

	//SFIOR |= 1 << PUD; // отключаю внутреннюю подтяжку портов
	DDR_TRANS |= 1<<pin_TRANS;
	PORT_TRANS |= 1<<pin_TRANS;
	DDR_VALVE1 |= 1<<pin_VALVE1;
	PORT_VALVE1 |= 1<<pin_VALVE1;
	DDR_VALVE2 |= 1<<pin_VALVE2;
	PORT_VALVE2 |= 1<<pin_VALVE2;
	DDR_LED = 0xff;
	PORT_LED = ALL_LEDS_OFF; // выключить все светодиоды
	// настраиваю порты кнопок на вход
	DDR_BUTTON_CURRENT &= ~(1 << pin_BUTTON_CURRENT);
	DDR_BUTTON_UP &= ~(1 << pin_UP);
	DDR_BUTTON_LEFT &= ~(1 << pin_LEFT);
	DDR_BUTTON_RIGHT &= ~(1 << pin_RIGHT);
	DDR_BUTTON_DOWN &= ~(1 << pin_DOWN);
	// устанавливаю подтягивающие резисторы на кнопки
	PORT_BUTTON_CURRENT |= 1 << pin_BUTTON_CURRENT;
	PORT_BUTTON_UP |= 1 << pin_UP;
	PORT_BUTTON_LEFT |= 1 << pin_LEFT;
	PORT_BUTTON_RIGHT |= 1 << pin_RIGHT;
	PORT_BUTTON_DOWN |= 1 << pin_DOWN;

	PORT_PEDAL1 |= 1 << pin_PEDAL1; // и педали
	PORT_PEDAL2 |= 1 << pin_PEDAL2;

	DDR_ALARM &= ~(1 << pin_ALARM);
	PORT_ALARM |= 1 << pin_ALARM; // подтягивающий резистор
	DDR_IND_BRT |= 1 << pin_IND_BRT;
	PORT_IND_BRT |= 1 << pin_IND_BRT;
	
	InitModBus();

	asm("sei");
}

void initVars()
{
	couScanKeys = SCAN_KEY_TIME;
	//waitTime = 1000;
	flags.currentIsEnable = 1; // разрешаю ток
	switchHL(pin_CURRENT_HL, ON);
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
#ifndef _CHECK_SI_
	return;
#endif
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
		PORT_LED &= ~(1 << pin_CURRENT_HL);
#else
		PORT_LED |= 1 << pin_CURRENT_HL;
#endif
	}
	else
	{// запрещаем его
		PORT_TRANS |= 1<<pin_TRANS; // если был включён, выключаю трансформатор
#ifdef LED_COMMON_CATHODE
		PORT_LED |= 1 << pin_CURRENT_HL;
#else
		PORT_LED &= ~(1 << pin_CURRENT_HL);
#endif
	}

	switchModeHL(curMode.get());
	//SetMenu(&mPrograms);
	wdt_start(wdt_60ms);
}
BOOL NextCase()
{
	if ((flags.scanKey && get_key() != keyEmpty) || !(PIN_BUTTON_CURRENT & (1 << pin_BUTTON_CURRENT)))
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
	if (!(PIN_BUTTON_CURRENT & (1 << pin_BUTTON_CURRENT)))
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
	modbus_on();
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

uint32_t time_to_set_modbus_on = 0;
int main()
{
	init();
	/*for (int i=0;i<25;i++)
	{
		cmTrBuf0[i] = i + 0x30;
	}
	TrCount = 0;
	cNumTrByte0 = 25;
	StartTrans();*/
	//usart::Init<115200>();
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
			if (is_modbus_active())
				modbus_off();
			else
				time_to_set_modbus_on = 0;
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
		else
		{
			if (!is_modbus_active())
			{
				if (!time_to_set_modbus_on)
					time_to_set_modbus_on = nTimeMs + 1000;
				else if (nTimeMs >= time_to_set_modbus_on)
				{
					time_to_set_modbus_on = 0;
					modbus_on();
				}
			}
			else
			{
				CheckModBus();
			}
		}
		if (flags.alarm == 1)
		{
			TaskAlarm();
			RefreshSqreen(menu);
		}
	}
	return 0;
}

void tst()
{
	for(;;)
	{
		if (!is_modbus_active())
		{
			if (!time_to_set_modbus_on)
			time_to_set_modbus_on = nTimeMs + 1000;
			else if (nTimeMs >= time_to_set_modbus_on)
			{
				time_to_set_modbus_on = 0;
				modbus_on();
			}
		}
	}
}
