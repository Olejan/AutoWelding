// !!! ������ ���������� � AtmelStudio 7.
// ���������� ������ ������������ Release ��� Debug � ����� Build

#include "config.h"

#ifdef _RUSSIAN_VERSION_
//������� �����
#define _B 160/*�*/
#define _G 161/* �*/
#define _YO 162/* �*/
#define _ZH 163/* �*/
#define _Z 164/* �*/
#define _I 165/* �*/
#define _II 166/* �*/
#define _L 167/* �*/
#define _P 168/* �*/
#define _U 169/* �*/
#define _F 170/* �*/
#define _CH 171/* �*/
#define _SH 172/* �*/
#define _SS 173/* �*/
#define _Y 174/* �*/
#define _E 175/* �*/
#define _YU 176/* �*/
#define _YA 177/* �*/
#define _b 178/* �*/
#define _v 179/* �*/
#define _g 180/* �*/
#define _yo 181/* �*/
#define _zh 182/* �*/
#define _z 183/* �*/
#define _i 184/* �*/
#define _ii 185/* �*/
#define _k 186/* �*/
#define _l 187/* �*/
#define _m 188/* �*/
#define _n 189/* �*/
#define _p 190/* �*/
#define _t 191/* �*/
#define _ch 192/* �*/
#define _sh 193/* �*/
#define _ss 194/* �*/
#define _y 195/* �*/
#define _sfts 196/* �*/
#define _e 197/* �*/
#define _yu 198/* �*/
#define _ya 199/* �*/
#define _D 224/* �*/
#define _C 225/* �*/
#define _SSH 226/* �*/
#define _d 227/* �*/
#define _f 228/* �*/
#define _c 229/* �*/
#define _ssh 230/* �*/
#endif


typedef signed char			i8;
typedef unsigned char		u8;
typedef signed short		i16;
typedef unsigned short		u16;
typedef signed int			i32;
typedef unsigned long		u32;
typedef signed long long	i64;
typedef unsigned long long	u64;
typedef u8					BOOL;

#define FALSE				0
#define TRUE				1

enum tag
{
	OFF,
	ON,
	firstPrg	= 0, // ������ ���������
	lastPrg		= 9, // ������� ��������
	minPedalNum	= 1,
	maxPedalNum	= 2,
	programNumber = lastPrg + 1,
	TIME_FOR_MENU	= 30000, // ����� (��), ����� �������� ����� �������� � ������� ����
	TIME_FOR_SAVE	= 30000, // ����� ������ ��� ���������� �������������� ���������
	TIME_PER_UNIT	= 10, // ����� ������� ��������� ����������: ������, ��������
	SCAN_KEY_TIME	= 20, // ����� ������������ ������, ��
	TIME_TO_WRITE	= 30, // �����, ����� �������� ����� ����� �������� ���-�� �� �����, ��
	SYNC_NUM		= 50, // ���������� ��, ����� ������� (�� ������) �������� ��������� �� ���������� ��������������
};

enum tagHL
{
#ifdef LED_COMMON_CATHODE
	ALL_LEDS_OFF		= 0xff,
#else
	ALL_LEDS_OFF		= 0,
#endif
};

#ifdef MVL
//-----------------------------------------------------------
// ������ ���������� ��� � �.��������
//-----------------------------------------------------------
/* ��������� */
#define LCD_PORT         PORTA        /**< port for the LCD lines   */
#define LCD_DATA0_PORT   LCD_PORT     /**< port for 4bit data bit 0 */
#define LCD_DATA1_PORT   LCD_PORT     /**< port for 4bit data bit 1 */
#define LCD_DATA2_PORT   LCD_PORT     /**< port for 4bit data bit 2 */
#define LCD_DATA3_PORT   LCD_PORT     /**< port for 4bit data bit 3 */
#define LCD_DATA0_PIN    4            /**< pin for 4bit data bit 0  */
#define LCD_DATA1_PIN    5            /**< pin for 4bit data bit 1  */
#define LCD_DATA2_PIN    6            /**< pin for 4bit data bit 2  */
#define LCD_DATA3_PIN    7            /**< pin for 4bit data bit 3  */
#define LCD_RS_PORT      LCD_PORT     /**< port for RS line         */
#define LCD_RS_PIN       1            /**< pin  for RS line         */
#define LCD_RW_PORT      LCD_PORT     /**< port for RW line         */
#define LCD_RW_PIN       2            /**< pin  for RW line         */
#define LCD_E_PORT       LCD_PORT     /**< port for Enable line     */
#define LCD_E_PIN        3            /**< pin  for Enable line     */

/*������*/
#define PIN_PEDAL1	PINB	/* ��� ������ ���������������� ������ */
#define PIN_PEDAL2	PINB	/* ��� ������ */
#define DDR_PEDAL1	DDRB	/* direct ���� ������ ���������������� ������ */
#define DDR_PEDAL2	DDRB	/* direct ���� ������ */
#define PORT_PEDAL1	PORTB	/* ���� ������ ��� ��������� ������������� ���������� */
#define PORT_PEDAL2	PORTB
#define pin_PEDAL1	6
#define pin_PEDAL2	7

/* buttons*/
#define DDR_BUTTON_UP	DDRB
#define DDR_BUTTON_LEFT	DDRB
#define DDR_BUTTON_RIGHT	DDRB
#define DDR_BUTTON_DOWN	DDRB
#define PIN_BUTTON_UP	PINB
#define PIN_BUTTON_LEFT	PINB
#define PIN_BUTTON_RIGHT	PINB
#define PIN_BUTTON_DOWN	PINB
#define PORT_BUTTON_UP	PORTB
#define PORT_BUTTON_LEFT	PORTB
#define PORT_BUTTON_RIGHT	PORTB
#define PORT_BUTTON_DOWN	PORTB
#define pin_UP		2
#define pin_LEFT	3
#define pin_RIGHT	4
#define pin_DOWN	5

#define DDR_BUTTON_CURRENT	DDRD
#define PIN_BUTTON_CURRENT	PIND
#define PORT_BUTTON_CURRENT	PORTD
#define pin_BUTTON_CURRENT	5

/* ���� �������������� */
#define DDR_TRANS	DDRD
#define PORT_TRANS	PORTD
#define PIN_TRANS	PIND
#define pin_TRANS	6

/*�������*/
#define DDR_VALVE1	DDRB	/* direct ���� ������� ���������������� ������ */
#define DDR_VALVE2	DDRB	/* direct ���� ������� */
#define PORT_VALVE1	PORTB	/* ���� ������� ���������������� ������ */
#define PORT_VALVE2	PORTB	/* ���� ������� */
#define PIN_VALVE1	PINB
#define PIN_VALVE2	PINB
#define pin_VALVE1	0
#define pin_VALVE2	1

/*����������*/
#define DDR_LED			DDRC
#define PORT_LED		PORTC
#define PIN_LED			PINC
#define pin_PRE_PRESSING_HL	7
#define pin_PRESSING_HL		6
#define pin_HEATING_HL		5
#define pin_FORGING_HL		4
#define pin_PAUSE_HL		3
#define pin_SIMPLE_HL		2
#define pin_AUTO_HL			1
#define pin_CURRENT_HL		0

/* ���� ����� "������" */
#define PORT_ALARM	PORTD
#define DDR_ALARM	DDRD
#define PIN_ALARM	PIND
#define pin_ALARM	4

/* ���� ���������� ��������� ���������� */
#define PORT_IND_BRT	PORTD
#define DDR_IND_BRT		DDRD
#define PIN_IND_BRT		PIND
#define pin_IND_BRT		7
//-----------------------------------------------------------
#else
//-----------------------------------------------------------
// ������ ���������� �� Tapman
//-----------------------------------------------------------
/* ��������� */
#define LCD_PORT         PORTA        /**< port for the LCD lines   */
#define LCD_DATA0_PORT   LCD_PORT     /**< port for 4bit data bit 0 */
#define LCD_DATA1_PORT   LCD_PORT     /**< port for 4bit data bit 1 */
#define LCD_DATA2_PORT   LCD_PORT     /**< port for 4bit data bit 2 */
#define LCD_DATA3_PORT   LCD_PORT     /**< port for 4bit data bit 3 */
#define LCD_DATA0_PIN    4            /**< pin for 4bit data bit 0  */
#define LCD_DATA1_PIN    3            /**< pin for 4bit data bit 1  */
#define LCD_DATA2_PIN    2            /**< pin for 4bit data bit 2  */
#define LCD_DATA3_PIN    1            /**< pin for 4bit data bit 3  */
#define LCD_RS_PORT      LCD_PORT     /**< port for RS line         */
#define LCD_RS_PIN       7            /**< pin  for RS line         */
#define LCD_RW_PORT      LCD_PORT     /**< port for RW line         */
#define LCD_RW_PIN       6            /**< pin  for RW line         */
#define LCD_E_PORT       LCD_PORT     /**< port for Enable line     */
#define LCD_E_PIN        5            /**< pin  for Enable line     */

/*������*/
#define PIN_PEDAL1	PINB	/* ��� ������ ���������������� ������ */
#define PIN_PEDAL2	PINB	/* ��� ������ */
#define DDR_PEDAL1	DDRB	/* direct ���� ������ ���������������� ������ */
#define DDR_PEDAL2	DDRB	/* direct ���� ������ */
#define PORT_PEDAL1	PORTB	/* ���� ������ ��� ��������� ������������� ���������� */
#define PORT_PEDAL2	PORTB
#define pin_PEDAL1	1
#define pin_PEDAL2	2

/* buttons*/
#define DDR_BUTTON_UP		DDRD
#define DDR_BUTTON_LEFT		DDRD
#define DDR_BUTTON_RIGHT	DDRD
#define DDR_BUTTON_DOWN		DDRD
#define PIN_BUTTON_UP		PIND
#define PIN_BUTTON_LEFT		PIND
#define PIN_BUTTON_RIGHT	PIND
#define PIN_BUTTON_DOWN		PIND
#define PORT_BUTTON_UP		PORTD
#define PORT_BUTTON_LEFT	PORTD
#define PORT_BUTTON_RIGHT	PORTD
#define PORT_BUTTON_DOWN	PORTD
#define pin_UP				7
#define pin_LEFT			6
#define pin_RIGHT			5
#define pin_DOWN			4

#define DDR_BUTTON_CURRENT	DDRB
#define PIN_BUTTON_CURRENT	PINB
#define PORT_BUTTON_CURRENT	PORTB
#define pin_BUTTON_CURRENT	6

/* ���� �������������� */
#define DDR_TRANS	DDRB
#define PORT_TRANS	PORTB
#define PIN_TRANS	PINB
#define pin_TRANS	0

/*�������*/
#define DDR_VALVE1	DDRB	/* direct ���� ������� ���������������� ������ */
#define DDR_VALVE2	DDRB	/* direct ���� ������� */
#define PORT_VALVE1	PORTB	/* ���� ������� ���������������� ������ */
#define PORT_VALVE2	PORTB	/* ���� ������� */
#define PIN_VALVE1	PINB
#define PIN_VALVE2	PINB
#define pin_VALVE1	3
#define pin_VALVE2	4

/*����������*/
#define DDR_LED			DDRC
#define PORT_LED		PORTC
#define PIN_LED			PINC
#define pin_PRE_PRESSING_HL	1
#define pin_PRESSING_HL		2
#define pin_HEATING_HL		3
#define pin_FORGING_HL		4
#define pin_PAUSE_HL		5
#define pin_SIMPLE_HL		7
#define pin_AUTO_HL			6
#define pin_CURRENT_HL		0

/* ���� ����� "������" */
#define PORT_ALARM	PORTA
#define DDR_ALARM	DDRA
#define PIN_ALARM	PINA
#define pin_ALARM	0

/* ���� ���������� ��������� ���������� */
#define PORT_IND_BRT	PORTB
#define DDR_IND_BRT		DDRB
#define PIN_IND_BRT		PINB
#define pin_IND_BRT		5
//-----------------------------------------------------------
#endif

enum tagParams
{
	paramPrePressing	= 0,// ��������������� ������ !!! 1-� �������� ������ ���� ����� 0 !!!
	paramPressing,			// ������
	paramModulation,		// ����������� ���������
	paramCurrent,			// �������� ����
	paramHeating,			// ������
	paramForging,			// ��������
	paramMode,				// ����� ������
	paramPause,				// ����� ����� ������� ������
	paramNum,				// ���������� ���������� !!! ������ ������ ����� ����� ���������� ��������� !!!
	cmnprmStartPrg,			// ��������� ���������
	cmnprmPedalNum,			// ���������� �������
	cmnprmBrtns,			// ���������
	cmnprmModbusId,			// Modbus Id
	extremeParam = cmnprmModbusId,	// ������� ��������

	minPrePressing	= 0,
	minPressing		= 0,
	minModulation	= 0,
	minCurrent		= 0,
	minHeating		= 0,
	minForging		= 0,
	MIN_PAUSE		= 0,
	MIN_MODBUS_ID	= 1,

	maxPrePressing	= 250,
	maxPressing		= 250,
	maxModulation	= 99,//24,//9,
	maxCurrent		= 99,//24,//9,
	maxHeating		= 250,
	maxForging		= 250,
	MAX_PAUSE		= 250,
	MAX_MODBUS_ID	= 247,

	defPrePressing	= 100,
	defPressing		= 20,
	defModulation	= 99,//24,//9,
	defCurrent		= 50,//12,//5,
	defHeating		= 30,
	defForging		= 10,
	DEF_PAUSE		= 25,

	DEF_PEDAL_NUM	= 2,
	DEF_START_PRG	= 0,
};
//!!// addr... � param... ��������������, ������ welding.c GetValue()
enum tagEEPROMAddr
{
	addrEmpty		= 0,
	addrPrePressing	= 0,//1, // ��������������� ������
	addrPressing, // ������
	addrModulation, // ����������� ���������
	addrCurrent, // �������� ����
	addrHeating, // ������
	addrForging, // ��������
	addrMode, // ������� �����
	addrPause, // ����� ����� ������� ������
	addrLastParam = addrPause,
	addrStartPrg	= 1 + paramNum * programNumber,	// ����� ������� ��������� (������������� �� ����� �����������)
	addrPedalNum,	// ���������� �������
	addrModbusId,	// modbus id
};

enum tagLCD
{
	lcdstr1	= 0, // first string
	lcdstr2, // second string
	lcdstr3, // third string
	lcdstr4, // fourth string
};

enum tagScreens
{
	scrViewParams,	// ����������� ���������� ��������
	scrPrePressing,	// ��������������� ������
	scrPressing,	// ������� ������
	scrHeating,		// ������
	scrForging,		// ��������
	scrWeldingCompleted,	// ���� ������ ��������
	scrPause,		// �����
};

enum tagButtons
{
	keyEmpty	= 0,
	keyUp,
	keyDown,
	keyLeft,
	keyRight,
};

enum tagDimentionLCD
{
	numOfStrings	= 2,
	numOfColumn		= 16,
};

typedef struct
{
	//u16 dirty		:1; // ������ � ������� LCD ��������
	u16 halfPeriod	:1; // ���� ������� ����������� �������� ���������� �� int0
	u16 syncfront	:1; // ����� ��������������: 0 - ��������, 1 - ������
	u16 transswitchoff	:1; // ��������� �������������
	//u16 heating		:1; // flag_nagrev
	u16 scanKey		:1; // ����� ����������� ������
	u16 currentIsEnable	:1; // ����� ���� ������/������ (1/0)
	u16 T1IsUp	:1;	// ������ 100���
	u16 useT1forHeating	:1;	// ������ ������ �������
	u16 alarm		:1;	// ����������������� ��������� ������
	//u16 modbus_enabled	:1; // Modbus �������
	u16 t3_5_started	:1;
}tagFlags;

enum tagIds
{
	idInfo			= 0,
	idPrograms,
	idChoosePrgStngs,
	idChooseCmnStngs,
	idBeginChooseParams,
	idChooseMode = idBeginChooseParams,
	idChoosePrePressing,
	idChoosePressing,
	idChooseModulation,
	idChooseCurrent,
	idChooseHeating,
	idChooseForging,
	idChoosePause,
	idEndChooseParams = idChoosePause,
	idChooseStartPrg,
	idEditPrePressing,
	idEditPressing,
	idEditModulation,
	idEditCurrent,
	idEditHeating,
	idEditForging,
	idEditPause,
	idEditMode,
	idEditStartPrg,
	idChoosePedalNum,
	idEditPedalNum,
	idChooseBrightness,
	idEditBrightness,
	idChooseModbusId,
	idEditModbusId,
};

enum tagMode
{
	FIRST_MODE = 0,
	SIMPLE_MODE	= FIRST_MODE,
	AUTO_MODE,
	SEAM_MODE,
	LAST_MODE = SEAM_MODE,
	
	WELD_IS_RUNNIG,
	SIMPLE_WELD_HAS_DONE,
	WELD_HAS_BROKEN,
	WELD_IS_PAUSED,
};

typedef struct PROGMEM
{
	const u8 id;	// id ����
	const char* str1;	// 1-� ������
	const char* str2; // 2-� ������
	void (*pf)();	// ������� ������� ����
} MenuItem;

typedef struct
{
	void (*set)(u8);
	u8 (*get)();
}CURPRG, CURMODE;

enum tagWDT
{
	wdt_15ms	= 0,
	wdt_30ms	= 1,
	wdt_60ms	= 2,
	wdt_120ms	= 3,
	wdt_250ms	= 4,
	wdt_500ms	= 5,
	wdt_1s		= 6,
	wdt_2s		= 7,
};

