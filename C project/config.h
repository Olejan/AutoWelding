/*
 * config.h
 *
 * Created: 07.06.2023 21:13:09
 *  Author: gos
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

//#define _DEBUG_
#define _RUSSIAN_VERSION_	// Русская версия индикатора
#define WDT_ENABLE			// Включён watchdog timer
#define LED_COMMON_CATHODE	// Светодиоды подключены с общим катодом
//#define MVL				// Версия распиновки Моя и В.Метелицы
#define _CHECK_SI_			// Проверяем СИ на старте

//#define SEDMAN
#ifdef SEDMAN
	#define BEFORE_WELDING_DELAY	10 // Задержка перед началом сварки после нажатия педали
	#define VALVE_ON_IS_LOW		// Включение клапанов происходит низким уровнем
#else
	#define BEFORE_WELDING_DELAY	500	// Задержка перед началом сварки после нажатия педали
	//#define VALVE_ON_IS_LOW		// Включение клапанов происходит низким уровнем
#endif


#endif //CONFIG_H_
