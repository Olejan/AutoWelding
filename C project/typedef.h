//#define _DEBUG_
#define _RUSSIAN_VERSION_
#define WDT_ENABLE
#define LED_COMMON_CATHODE	/* ���������� ���������� � ����� ������� */

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
	pinTrans	= 6,// PORTD, // PORTD.0
	pinValve1	= 0, // PORTB.0
	pinValve2	= 1, // PORTB.1
	pinPedal1	= 6, // PORTB.6
	pinPedal2	= 7, // PORTB.7
	pinIndBrt	= 7, // PORTD.7
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
	pinPrePressingHL	= 7,
	pinPressingHL		= 6,
	pinHeatingHL		= 5,
	pinForgingHL		= 4,
	pinPauseHL			= 3,
	pinSimpleHL			= 2,
	pinAutoHL			= 1,
	pinCurrentHL		= 0,
#ifdef LED_COMMON_CATHODE
	ALL_LEDS_OFF		= 0xff,
#else
	ALL_LEDS_OFF		= 0,
#endif
};

#define PINPEDAL1	PINB	/* ��� ������ ���������������� ������ */
#define PINPEDAL2	PINB	/* ��� ������ */
#define DDRPEDAL1	DDRB	/* direct ���� ������ ���������������� ������ */
#define DDRPEDAL2	DDRB	/* direct ���� ������ */
#define PORTPEDAL1	PORTB	/* ���� ������ ��� ��������� ������������� ���������� */
#define PORTPEDAL2	PORTB

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
#define pin_CURRENT	5

#define PINBUTTONS	PINB
#define PORTBUTTONS	PORTB

#define DDRTRANS	DDRD
#define PORTTRANS	PORTD	/* ���� �������������� */
#define DDRVALVE1	DDRB	/* direct ���� ������� ���������������� ������ */
#define DDRVALVE2	DDRB	/* direct ���� ������� */
#define PORTVALVE1	PORTB	/* ���� ������� ���������������� ������ */
#define PORTVALVE2	PORTB	/* ���� ������� */
#define DDRLED		DDRC
#define PORTLED		PORTC	/* ���� ����������� */

/* ���� ����� "������" */
#define PORT_ALARM	PORTD
#define DDR_ALARM	DDRD
#define PIN_ALARM	PIND
#define pin_ALARM	4
/* ���� ���������� ��������� ���������� */
#define PORT_IND_BRT	PORTD
#define DDR_IND_BRT		DDRD
#define PIN_IND_BRT		PIND

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
	extremeParam = cmnprmBrtns,	// ������� ��������

	minPrePressing	= 0,
	minPressing		= 0,
	minModulation	= 0,
	minCurrent		= 0,
	minHeating		= 0,
	minForging		= 0,
	MIN_PAUSE		= 0,

	maxPrePressing	= 250,
	maxPressing		= 250,
	maxModulation	= 9,
	maxCurrent		= 9,
	maxHeating		= 250,
	maxForging		= 250,
	MAX_PAUSE		= 250,

	defPrePressing	= 100,
	defPressing		= 20,
	defModulation	= 9,
	defCurrent		= 5,
	defHeating		= 30,
	defForging		= 10,
	DEF_PAUSE		= 25,
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
	addrStartPrg	= 1 + paramNum * programNumber,	// ����� ������� ��������� (������������� �� ����� �����������)
	addrPedalNum,	// ���������� �������
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
