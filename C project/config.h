/*
 * config.h
 *
 * Created: 07.06.2023 21:13:09
 *  Author: gos
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

//#define _DEBUG_
#define _RUSSIAN_VERSION_	// ������� ������ ����������
#define WDT_ENABLE			// ������� watchdog timer
#define LED_COMMON_CATHODE	// ���������� ���������� � ����� �������
//#define MVL				// ������ ���������� ��� � �.��������
#define _CHECK_SI_			// ��������� �� �� ������

//#define SEDMAN
#ifdef SEDMAN
	#define BEFORE_WELDING_DELAY	10 // �������� ����� ������� ������ ����� ������� ������
	#define VALVE_ON_IS_LOW		// ��������� �������� ���������� ������ �������
#else
	#define BEFORE_WELDING_DELAY	500	// �������� ����� ������� ������ ����� ������� ������
	//#define VALVE_ON_IS_LOW		// ��������� �������� ���������� ������ �������
#endif


#endif //CONFIG_H_
