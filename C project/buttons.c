/** \file buttons.c
 * \brief Модуль обработки кнопок
 * содержит функции опроса состояния кнопок
 * \par
 * \author ARV
 * Copyright 2008 © ARV. All rights reserved. </b>
 * \par
 * Для сборки проекта требуется:
 * 	-# WinAVR-20080411 или более новая версия
 *
 */

#include "IDE.h"
#include "buttons.h"

/** Возврат кодов нажатых кнопок
 * @return - возвращает биты, соответствующие нажатым кнопкам или 0, если не нажато ни одной
 * \note Эта функция должна быть реализована для конкретной аппаратной платформы. Это может
 * быть сканирование матрицы, ввод состояния пинов порта и т.п. Никаких мер по подавлению дребезга
 * в этой функции быть не должно!
 */
static u8 get_key_code(void)
{
	u8 buf;
	buf = ~PINBUTTONS;
	buf >>= 2;
	/*if (!(PIND & (1 << 4)))// PIND.0 - кнопка Вверх
		buf |= 1;
	else
		buf &= ~1;*/
	/*if (!(PINB & (1 << 0)))
		buf |= 1 << 3;*/

	switch(buf)
	{
		case 1: buf = keyUp; break;
		case 2: buf = keyLeft; break;
		case 4: buf = keyRight; break;
		case 8: buf = keyDown; break;
		//case 0x0f: reset(); break;  // LEFT + DOWN + RIGHT + UP
		default: buf = keyEmpty; break;
	}
	return buf;
}

/** Ожидает удержания кнопок заданное время
 *
 * @param key - код кнопки, удержание которой должно контролироваться
 * @param delay10ms - длительность удержания в ДЕСЯТКАХ миллисекунд
 * @return - код удержанной кнопки или 0, если кнопку отпустили до истечения задержки
 */
static u8 wait_key(u8 key, u16 delay10ms)
{
	delay10ms *= 10;
	for(; delay10ms > 0; delay10ms--)
	{
		_delay_ms(1);
		if (key != get_key_code())
			return keyEmpty;
		wdt_feed();
	}
	return key;
}

/** получение кода нажатых кнопок с функциями автоповтора
 * если нажать и удерживать комбинацию кнопок,
 * то функция сначала выжидает FIRST_RPT десятков миллисекунд, а затем
 * возвращает результат через каждые RPT_DELAY десятков миллисекунд
 * @return - возвращаемое значение: биты, соответствующие нажатым кнопкам
 */
u8 get_key(void)
{
//return keyLeft;
	static u8 oldkey = keyEmpty;
	static u8 first = 1;
	static u8 repeat = 0;
	u8 key, delay;

	key = wait_key(get_key_code(), 1);		// ждем 10 мс для подавления дребезга
	if (key)
	{								// если есть нажатие
		if (oldkey == keyEmpty)
		{						// если предыдущий код нулевой - это новая комбинация
			oldkey = key;					// запоминаем комбинацию и на выход
			repeat = 0;
		}
		else
		{							// если предыдущая не ноль - это автоповтор
			if (first)
			{						// первый раз перед повтором долгая выдержка
				delay = FIRST_RPT;
				first = 0;					// чтобы следующий раз отработали иначе
				repeat = 1;
			}
			else
			{
				if (repeat++ >= 30)
				{
					repeat = 30;
					delay = FAST_RPT_DELAY;		// после 30 повторов уменьшаем задержку
				}					
				else
					delay = RPT_DELAY;			// каждый не первый повтор с другой задержкой
			}
			key = wait_key(oldkey, delay);	// ждем удержание нужное время
			if (key == keyUp || key == keyDown) // автоповтор кнопок ВВЕРХ и ВНИЗ не нужен)
				return keyEmpty;
		}
	}
	if (!key)
	{								// если в конце концов вышел ноль
		first = 1;							// все начинаем заново
		oldkey = 0;
	}
	return key;								// врозвращаем результирующий код
}
