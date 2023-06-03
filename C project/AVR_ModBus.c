//ATmega8, ATmega16
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/crc16.h>
#include "AVR_ModBus.h"


/*unsigned char RcCount, TrCount;  //счетчик принятых/переданных данных
bool StartRec = false;// false/true начало/прием посылки
bool bModBus = false;  //флаг обработки посылки
unsigned char cNumRcByte0; //кол-во принятых байт
unsigned char cNumTrByte0;  //кол-во передаваемых байт*/
extern unsigned char cmRcBuf0[MAX_LENGHT_REC_BUF]; //буфер принимаемых данных
extern unsigned char cmTrBuf0[MAX_LENGHT_TR_BUF]; //буфер передаваемых данных


//unsigned char ModBus(unsigned char NumByte);
//char Func01(void);
//char Func02(void);
char Func03(void);
//char Func04(void);
//char Func05(void);
char Func06(void);
char Func16(void);
char ErrorMessage(char Error);
unsigned int GetCRC16(unsigned char* buf, unsigned char bufsize);


extern unsigned char m_nModbusId;
unsigned char ModBus(unsigned char NumByte)
{
	int CRC16;
	unsigned char i;

	if (cmRcBuf0[0] != m_nModbusId) return 0x00; //broadcast запрос, ответ не нужен

	CRC16 = GetCRC16(cmRcBuf0, NumByte);
	if (CRC16) return 0;  //контрольная сумма не совпадает, ответ не нужен

	//проверки прошли успешно начинаем формировать ответ на запрос
	//данные для ответа заносятся в массив cmTrBuf0[]
	//!!!!! адреса и данные двубайтные заносятся старшим байтом вперед (Hi, Low)
	//!!!!! CRC заноситься младшим байтом вперед (Low, Hi)

	cmTrBuf0[0] = m_nModbusId;//адрес устройства

	for (i = 1; i < (MAX_LENGHT_TR_BUF); i++) //очищаем буфер для данных
	{
		cmTrBuf0[i] = 0;
	}//end for()

	//код команды
	switch (cmRcBuf0[1])
	{
	/*case 0x01:
	{
		if (!QUANTITY_REG_0X) return ErrorMessage(0x01);
		return Func01();
	}

	case 0x02:
	{
		if (!QUANTITY_REG_1X) return ErrorMessage(0x01);
		return Func02();
	}*/

	case 0x03:
	{
		if (!QUANTITY_REG_4X) return ErrorMessage(0x01);
		return Func03();
	}

	/*case 0x04:
	{
		if (!QUANTITY_REG_3X) return ErrorMessage(0x01);
		return Func04();
	}

	case 0x05:
	{
		if (!QUANTITY_REG_0X) return ErrorMessage(0x01);
		return Func05();
	}*/

	case 0x06:
	{
		if (!QUANTITY_REG_4X) return ErrorMessage(0x01);
		return Func06();
	}

	case 0x10:
	{
		if (!QUANTITY_REG_4X) return ErrorMessage(0x01);
		return Func16();
	}

	default:   return ErrorMessage(0x01); //Принятый код функции не может быть обработан
	} //end switch
} //end ModBus()



//функция вычисляет код CRC-16
//на входе указатель на начало буфера
//и количество байт сообщения (без принятого кода CRC-16)
unsigned int GetCRC16(unsigned char* buf, unsigned char bufsize)
{
	unsigned int crc = 0xFFFF;
	unsigned char i;

	for (i = 0; i < bufsize; i++)
		crc = _crc16_update(crc, buf[i]);//подсчет CRC в принятой посылке
	return crc;
} //end GetCRC16()



//формирование ответа об ошибке
char ErrorMessage(char Error)
{
	int CRC16;

	cmTrBuf0[1] = cmRcBuf0[1] + 0x80;//команда с ошибкой
	cmTrBuf0[2] = Error;
	CRC16 = GetCRC16(cmTrBuf0, 3);//подсчет КС посылки
	cmTrBuf0[3] = Low(CRC16);
	cmTrBuf0[4] = Hi(CRC16);

	return 5;
} //end ErrorMessage()



//---------------------------Функции ModBus-------------------------------

#define start_adress ((cmRcBuf0[2]<<8) + cmRcBuf0[3])
#define quantity_registers ((cmRcBuf0[4]<<8) + cmRcBuf0[5])
#define data_byte ((quantity_registers+7)/8)
#define data_2byte (2*quantity_registers) //данные двухбайтные
#define max_adress (start_adress + quantity_registers)
#define reg_adress (start_adress)
#define value (quantity_registers)
#define num_to_write (cmRcBuf0[6]) // количество байт данных для записи в функции 0x10
#define data_pointer (cmRcBuf0 + 7) // указатель на данные для записи в регистры
/*//побитовая запись
#define I_byte (reg_adress/8)
#define I_bit (reg_adress%8)
//побитовая запись с учетом смещения адреса
#define I_byte_ofset(x) ((start_adress+x)/8)
#define I_bit_ofset(x) ((start_adress+x)%8)
#define NextBit(y,x) RegNum##y[I_byte_ofset(x)] & (1<<I_bit_ofset(x))
#define ClrBitOfByte(ByteA, x) (ByteA &= ~(1<<(x)))*/


#if 0
//реализация функции 0х01
//чтение значений из нескольких регистров флагов (Read Coil Status)
char Func01(void)
{
	unsigned int CRC16;
	unsigned char i;

	//проверка корректного адреса в запросе
	if (!(max_adress <= QUANTITY_REG_0X)) return ErrorMessage(0x02); //Адрес данных, указанный в запросе, недоступен
	if (!quantity_registers) return 0;//проверяем что количество регистров на чтение > 0

	//формируем ответ
	cmTrBuf0[1] = 0x01;//команда
	cmTrBuf0[2] = data_byte;//количество байт данных в ответе

	for (i = 0; i < quantity_registers; i++)
	{
		cmTrBuf0[3 + i / 8] |= ((bool)(NextBit(0x, i)) << (i % 8)); //данные
	}//end for()
	CRC16 = GetCRC16(cmTrBuf0, data_byte + 3);//подсчет КС посылки
	cmTrBuf0[3 + data_byte] = Low(CRC16);
	cmTrBuf0[4 + data_byte] = Hi(CRC16);
	return (5 + data_byte);
} //end Func01(void)




//реализация функции 0х02
//чтение значений из нескольких регистров флагов (Read Input Status)
char Func02(void)
{
	unsigned int CRC16;
	unsigned char i;

	//проверка корректного адреса в запросе
	if (!(max_adress <= QUANTITY_REG_1X)) return ErrorMessage(0x02); //Адрес данных, указанный в запросе, недоступен
	if (!quantity_registers) return 0;//проверяем что количество регистров на чтение > 0

	//формируем ответ
	cmTrBuf0[1] = 0x02;//команда
	cmTrBuf0[2] = data_byte;//количество байт данных в ответе

	for (i = 0; i < quantity_registers; i++)
	{
		cmTrBuf0[3 + i / 8] |= ((bool)(NextBit(1x, i)) << (i % 8)); //данные
	}//end for()
	CRC16 = GetCRC16(cmTrBuf0, data_byte + 3);//подсчет КС посылки
	cmTrBuf0[3 + data_byte] = Low(CRC16);
	cmTrBuf0[4 + data_byte] = Hi(CRC16);
	return (5 + data_byte);
} //end Func02(void)

#endif

#include "IDE.h"
#include "Registers.h"
void led_switch(unsigned char line);
int get_param(int reg);

/*
* Проверяет входные данные на валидность и возвращает код ошибки:
* - 0 - данные валидны
* 2 - ILLEGAL_DATA_ADDRESS
* 3 - ILLEGAL_DATA_VALUE
*/
uint8_t check_request(int reg, int num)
{
	if (reg > CMN_LAST_PARAM)
		return MB_EX_ILLEGAL_DATA_ADDRESS;
	if (reg >= CMN_CUR_PRG && reg <= CMN_LAST_PARAM)
	{
		if ((reg + num) <= CMN_LAST_PARAM)
			return MB_EX_NONE;
	}
	u8 prg = reg / 0x10;
	u8 param = reg % 0x10;
	if (prg <= lastPrg && (param + num - 1) <= addrLastParam)
		return MB_EX_NONE;
	if (num > paramNum)
		return MB_EX_ILLEGAL_DATA_VALUE;
	return MB_EX_ILLEGAL_DATA_ADDRESS;
}

//реализация функции 0х03
//чтение значений из нескольких регистров хранения (Read Holding Registers)
char Func03(void)
{
	unsigned int CRC16;
	unsigned char i;

	led_switch(pin_PRE_PRESSING_HL);

	//проверка корректного адреса в запросе и количество регистров на чтение
	char err = check_request(start_adress, quantity_registers);
	if (err != MB_EX_NONE)
		return ErrorMessage(err); //Адрес данных, указанный в запросе, недоступен

	for (int i = 0; i < quantity_registers; i++)
	{
		RegNum4x[i] = get_param(start_adress + i);
	}

	//формируем ответ
	cmTrBuf0[1] = 0x03;//команда
	cmTrBuf0[2] = data_2byte;//количество байт данных в ответе
	for (i = 0; i < quantity_registers; i++)
	{
		cmTrBuf0[3 + 2 * i] = Hi(RegNum4x[/*start_adress + */i]);//данные старший байт
		cmTrBuf0[4 + 2 * i] = Low(RegNum4x[/*start_adress + */i]);//данные младший байт
	} //end for()
	CRC16 = GetCRC16(cmTrBuf0, data_2byte + 3);//подсчет КС посылки
	cmTrBuf0[3 + data_2byte] = Low(CRC16);
	cmTrBuf0[4 + data_2byte] = Hi(CRC16);
	return (5 + data_2byte);
} //end Func03(void)



#if 0
//реализация функции 0х04
//чтение значения аналоговых входов (Read Input Registers)
char Func04(void)
{
	unsigned int CRC16;
	unsigned char i;

	//проверка корректного адреса в запросе
	if (!(max_adress <= QUANTITY_REG_3X)) return ErrorMessage(0x02); //Адрес данных, указанный в запросе, недоступен
	if (!quantity_registers) return 0;//проверяем что количество регистров на чтение > 0 *******************************************************************************проверить работу***********************

	//формируем ответ
	cmTrBuf0[1] = 0x04;//команда
	cmTrBuf0[2] = data_2byte;//количество байт данных в ответе
	for (i = 0; i < quantity_registers; i++)
	{
		cmTrBuf0[3 + 2 * i] = Hi(RegNum3x[start_adress + i]);//данные старший байт
		cmTrBuf0[4 + 2 * i] = Low(RegNum3x[start_adress + i]);//данные младший байт
	} //end for()
	CRC16 = GetCRC16(cmTrBuf0, data_2byte + 3);//подсчет КС посылки
	cmTrBuf0[3 + data_2byte] = Low(CRC16);
	cmTrBuf0[4 + data_2byte] = Hi(CRC16);
	return (5 + data_2byte);
} //end Func04(void)




//реализация функции 0х05
//запись значения одного флага (Force Single Coil)
char Func05(void)
{
	//проверка корректного адреса в запросе
	if (!(reg_adress <= QUANTITY_REG_0X)) return ErrorMessage(0x02); //Адрес данных, указанный в запросе, недоступен

	//Значение FF 00 hex устанавливает флаг(1).
	//Значение 00 00 hex сбрасывает флаг (0).
	//Другие значения не меняют флаг
	//формируем ответ, возвращая полученное сообщение
	if ((value == 0xFF00) || (value == 0x0000))
	{
		if (value)
		{
			RegNum0x[I_byte] |= (1 << I_bit);
		}
		else
		{
			RegNum0x[I_byte] &= ~(1 << I_bit);
		}
		cmTrBuf0[1] = cmRcBuf0[1];
		cmTrBuf0[2] = cmRcBuf0[2];
		cmTrBuf0[3] = cmRcBuf0[3];
		cmTrBuf0[4] = cmRcBuf0[4];
		cmTrBuf0[5] = cmRcBuf0[5];
		cmTrBuf0[6] = cmRcBuf0[6];
		cmTrBuf0[7] = cmRcBuf0[7];
		return 8;
	} //end if()
	return ErrorMessage(0x03); //Значение, содержащееся в поле данных запроса, является недопустимой величиной
} //end Func05(void)

#endif


int set_param(int reg, int val);
//реализация функции 0х06
//запись значения в один регистр хранения (Preset Single Register)
char Func06(void)
{
	//проверка корректного адреса
	uint8_t err = check_request(start_adress, 1);
	if (err != MB_EX_NONE)
		return ErrorMessage(err); //Адрес данных, указанный в запросе, недоступен
	// Установка заданного параметра
	err = set_param(reg_adress, value);
	if (err != MB_EX_NONE)
		return ErrorMessage(MB_EX_ILLEGAL_DATA_VALUE);

	//формируем ответ, возвращая полученное сообщение
	RegNum4x[reg_adress] = value;
	cmTrBuf0[1] = cmRcBuf0[1];
	cmTrBuf0[2] = cmRcBuf0[2];
	cmTrBuf0[3] = cmRcBuf0[3];
	cmTrBuf0[4] = cmRcBuf0[4];
	cmTrBuf0[5] = cmRcBuf0[5];
	cmTrBuf0[6] = cmRcBuf0[6];
	cmTrBuf0[7] = cmRcBuf0[7];
	return 8;
} //end Func06(void)

void WrHex(u8 a_data, u8 a_x, u8 a_y);
//реализация функции 0х10
//Запись значений в несколько регистров хранения (Write Holding Registers)
char Func16(void)
{
	unsigned int CRC16;

	//проверка корректного адреса в запросе и количество регистров на чтение
	char err = check_request(start_adress, quantity_registers);
	if (err != MB_EX_NONE)
		return ErrorMessage(err); //Адрес данных, указанный в запросе, недоступен
	
	int res;
	for (int i = 0; i < num_to_write / 2; i++)
	{
		res = check_param(start_adress + i, *(data_pointer + i * 2 + 1));
		if (res != MB_EX_NONE)
			return ErrorMessage(res);
	}
	int cnt = 0;
	for (int i = 0; i < num_to_write / 2; i++)
	{
		set_param(start_adress + i, *(data_pointer + i * 2 + 1));
	}
	//WrHex(res, 0, lcdstr1);

	//формируем ответ
	cmTrBuf0[1] = cmRcBuf0[1];
	cmTrBuf0[2] = cmRcBuf0[2];
	cmTrBuf0[3] = cmRcBuf0[3];
	cmTrBuf0[4] = cmRcBuf0[4];
	cmTrBuf0[5] = cmRcBuf0[5];
	CRC16 = GetCRC16(cmTrBuf0, 6);//подсчет КС посылки
	cmTrBuf0[6] = Low(CRC16);
	cmTrBuf0[7] = Hi(CRC16);
	return 8;
} //end Func16(void)
