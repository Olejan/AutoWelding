//#define _DEBUG_
#define _RUSSIAN_VERSION_
#define WDT_ENABLE
#define LED_COMMON_CATHODE	/* светодиоды подключены с общим катодом */

#ifdef _RUSSIAN_VERSION_
//Русские буквы
#define _B 160/*Б*/
#define _G 161/* Г*/
#define _YO 162/* Ё*/
#define _ZH 163/* Ж*/
#define _Z 164/* З*/
#define _I 165/* И*/
#define _II 166/* Й*/
#define _L 167/* Л*/
#define _P 168/* П*/
#define _U 169/* У*/
#define _F 170/* Ф*/
#define _CH 171/* Ч*/
#define _SH 172/* Ш*/
#define _SS 173/* Ъ*/
#define _Y 174/* Ы*/
#define _E 175/* Э*/
#define _YU 176/* Ю*/
#define _YA 177/* Я*/
#define _b 178/* б*/
#define _v 179/* в*/
#define _g 180/* г*/
#define _yo 181/* ё*/
#define _zh 182/* ж*/
#define _z 183/* з*/
#define _i 184/* и*/
#define _ii 185/* й*/
#define _k 186/* к*/
#define _l 187/* л*/
#define _m 188/* м*/
#define _n 189/* н*/
#define _p 190/* п*/
#define _t 191/* т*/
#define _ch 192/* ч*/
#define _sh 193/* ш*/
#define _ss 194/* ъ*/
#define _y 195/* ы*/
#define _sfts 196/* ь*/
#define _e 197/* э*/
#define _yu 198/* ю*/
#define _ya 199/* я*/
#define _D 224/* Д*/
#define _C 225/* Ц*/
#define _SSH 226/* Щ*/
#define _d 227/* д*/
#define _f 228/* ф*/
#define _c 229/* ц*/
#define _ssh 230/* щ*/
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
	firstPrg	= 0, // первая программа
	lastPrg		= 9, // крайняя программ
	minPedalNum	= 1,
	maxPedalNum	= 2,
	programNumber = lastPrg + 1,
	TIME_FOR_MENU	= 30000, // время (мс), после которого нужно выходить в главное меню
	TIME_FOR_SAVE	= 30000, // время данное для сохранения редактируемого параметра
	TIME_PER_UNIT	= 10, // время единицы измерения параметров: сжатие, проковка
	SCAN_KEY_TIME	= 20, // время сканирования кнопок, мс
	TIME_TO_WRITE	= 30, // время, после которого имеет смысл выводить что-то на экран, мс
	SYNC_NUM		= 50, // количечтво мс, после которых (на старте) появится сообщение об отсутствии синхроимпульса
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

#define PINPEDAL1	PINB	/* пин педали предварительного сжатия */
#define PINPEDAL2	PINB	/* пин педали */
#define DDRPEDAL1	DDRB	/* direct порт педали предварительного сжатия */
#define DDRPEDAL2	DDRB	/* direct порт педали */
#define PORTPEDAL1	PORTB	/* порт педали для установки подтягивающих резисторов */
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
#define PORTTRANS	PORTD	/* порт трансформатора */
#define DDRVALVE1	DDRB	/* direct порт клапана предварительного сжатия */
#define DDRVALVE2	DDRB	/* direct порт клапана */
#define PORTVALVE1	PORTB	/* порт клапана предварительного сжатия */
#define PORTVALVE2	PORTB	/* порт клапана */
#define DDRLED		DDRC
#define PORTLED		PORTC	/* порт светодиодов */

/* порт входа "Ошибка" */
#define PORT_ALARM	PORTD
#define DDR_ALARM	DDRD
#define PIN_ALARM	PIND
#define pin_ALARM	4
/* порт регулятора подсветки индикатора */
#define PORT_IND_BRT	PORTD
#define DDR_IND_BRT		DDRD
#define PIN_IND_BRT		PIND

enum tagParams
{
	paramPrePressing	= 0,// предварительное сжатие !!! 1-й параметр должен быть равен 0 !!!
	paramPressing,			// сжатие
	paramModulation,		// полупериоды модуляции
	paramCurrent,			// мощность тока
	paramHeating,			// нагрев
	paramForging,			// проковка
	paramMode,				// режим сварки
	paramPause,				// пауза между циклами сварки
	paramNum,				// количество параметров !!! должен стоять сразу после последнего параметра !!!
	cmnprmStartPrg,			// стартовая программа
	cmnprmPedalNum,			// количество педалей
	cmnprmBrtns,			// подсветка
	extremeParam = cmnprmBrtns,	// крайний параметр

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
//!!// addr... и param... взаимозависимы, смотри welding.c GetValue()
enum tagEEPROMAddr
{
	addrEmpty		= 0,
	addrPrePressing	= 0,//1, // предварительное сжатие
	addrPressing, // сжатие
	addrModulation, // полупериоды модуляции
	addrCurrent, // мощность тока
	addrHeating, // нагрев
	addrForging, // проковка
	addrMode, // текущий режим
	addrPause, // пауза между циклами сварки
	addrStartPrg	= 1 + paramNum * programNumber,	// номер текущей программы (располагается за всеми программами)
	addrPedalNum,	// количество педалей
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
	scrViewParams,	// отображение параметров программ
	scrPrePressing,	// предварительное сжатие
	scrPressing,	// процесс сжатия
	scrHeating,		// сварка
	scrForging,		// проковка
	scrWeldingCompleted,	// цикл сварки завершён
	scrPause,		// пауза
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
	//u16 dirty		:1; // данные в массиве LCD изменены
	u16 halfPeriod	:1; // флаг прихода полупериода сетевого напряжения на int0
	u16 syncfront	:1; // фронт синхроимпульса: 0 - передний, 1 - задний
	u16 transswitchoff	:1; // выключить трансформатор
	//u16 heating		:1; // flag_nagrev
	u16 scanKey		:1; // можно сканировать кнопки
	u16 currentIsEnable	:1; // выход тока открыт/закрыт (1/0)
	u16 T1IsUp	:1;	// прошло 100мкс
	u16 useT1forHeating	:1;	// сейчас задача нагрева
	u16 alarm		:1;	// зарегистрированно состояние аварии
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
	const u8 id;	// id меню
	const char* str1;	// 1-я строка
	const char* str2; // 2-я строка
	void (*pf)();	// функция данного меню
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
