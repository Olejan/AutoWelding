/*
 * Registers.h
 *
 * Created: 01.06.2023 19:16:19
 *  Author: Oleg
 */ 


#ifndef REGISTERS_H_
#define REGISTERS_H_

// Карта регистров modbus.
// В регистрах по адресу PROGRAMM_X содержатся ещё регистры настроек программ,
// которые описаны как смещение адреса в енуме tagParams (paramPrePressing - paramPause).
// Т.е. адрес регистра параметра тока третьей программы равен PROGRAMM_3 + paramCurrent.
// С адреса 0xA0 идут регистры общих настроек.
enum Modbus_hold_regs
{
	PROGRAMM_0 = 0x00,
	PROGRAMM_1 = 0x10,
	PROGRAMM_2 = 0x20,
	PROGRAMM_3 = 0x30,
	PROGRAMM_4 = 0x40,
	PROGRAMM_5 = 0x50,
	PROGRAMM_6 = 0x60,
	PROGRAMM_7 = 0x70,
	PROGRAMM_8 = 0x80,
	PROGRAMM_9 = 0x90,

	CMN_CUR_PRG = 0xA0,
	CMN_START_PRG,
	CMN_LIGTH,
	CMN_PEDAL_NUM,
	CMN_MODBUS_ID,
};



#endif /* REGISTERS_H_ */