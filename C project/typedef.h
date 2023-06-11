// !!! Проект собирается в AtmelStudio 7.
// Выбирается нужная конфигурация Release или Debug и жмётся Build

#include "config.h"

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
#ifdef LED_COMMON_CATHODE
	ALL_LEDS_OFF		= 0xff,
#else
	ALL_LEDS_OFF		= 0,
#endif
};

#ifdef MVL
//-----------------------------------------------------------
// Версия распиновки Моя и В.Метелицы
//-----------------------------------------------------------
/* Индикатор */
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

/*Педали*/
#define PIN_PEDAL1	PINB	/* пин педали предварительного сжатия */
#define PIN_PEDAL2	PINB	/* пин педали */
#define DDR_PEDAL1	DDRB	/* direct порт педали предварительного сжатия */
#define DDR_PEDAL2	DDRB	/* direct порт педали */
#define PORT_PEDAL1	PORTB	/* порт педали для установки подтягивающих резисторов */
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

/* порт трансформатора */
#define DDR_TRANS	DDRD
#define PORT_TRANS	PORTD
#define PIN_TRANS	PIND
#define pin_TRANS	6

/*Клапаны*/
#define DDR_VALVE1	DDRB	/* direct порт клапана предварительного сжатия */
#define DDR_VALVE2	DDRB	/* direct порт клапана */
#define PORT_VALVE1	PORTB	/* порт клапана предварительного сжатия */
#define PORT_VALVE2	PORTB	/* порт клапана */
#define PIN_VALVE1	PINB
#define PIN_VALVE2	PINB
#define pin_VALVE1	0
#define pin_VALVE2	1

/*Светодиоды*/
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

/* порт входа "Ошибка" */
#define PORT_ALARM	PORTD
#define DDR_ALARM	DDRD
#define PIN_ALARM	PIND
#define pin_ALARM	4

/* порт регулятора подсветки индикатора */
#define PORT_IND_BRT	PORTD
#define DDR_IND_BRT		DDRD
#define PIN_IND_BRT		PIND
#define pin_IND_BRT		7
//-----------------------------------------------------------
#else
//-----------------------------------------------------------
// Версия распиновки от Tapman
//-----------------------------------------------------------
/* Индикатор */
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

/*Педали*/
#define PIN_PEDAL1	PINB	/* пин педали предварительного сжатия */
#define PIN_PEDAL2	PINB	/* пин педали */
#define DDR_PEDAL1	DDRB	/* direct порт педали предварительного сжатия */
#define DDR_PEDAL2	DDRB	/* direct порт педали */
#define PORT_PEDAL1	PORTB	/* порт педали для установки подтягивающих резисторов */
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

/* порт трансформатора */
#define DDR_TRANS	DDRB
#define PORT_TRANS	PORTB
#define PIN_TRANS	PINB
#define pin_TRANS	0

/*Клапаны*/
#define DDR_VALVE1	DDRB	/* direct порт клапана предварительного сжатия */
#define DDR_VALVE2	DDRB	/* direct порт клапана */
#define PORT_VALVE1	PORTB	/* порт клапана предварительного сжатия */
#define PORT_VALVE2	PORTB	/* порт клапана */
#define PIN_VALVE1	PINB
#define PIN_VALVE2	PINB
#define pin_VALVE1	3
#define pin_VALVE2	4

/*Светодиоды*/
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

/* порт входа "Ошибка" */
#define PORT_ALARM	PORTA
#define DDR_ALARM	DDRA
#define PIN_ALARM	PINA
#define pin_ALARM	0

/* порт регулятора подсветки индикатора */
#define PORT_IND_BRT	PORTB
#define DDR_IND_BRT		DDRB
#define PIN_IND_BRT		PINB
#define pin_IND_BRT		5
//-----------------------------------------------------------
#endif

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
	cmnprmModbusId,			// Modbus Id
	extremeParam = cmnprmModbusId,	// крайний параметр

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
	addrLastParam = addrPause,
	addrStartPrg	= 1 + paramNum * programNumber,	// номер текущей программы (располагается за всеми программами)
	addrPedalNum,	// количество педалей
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
	//u16 modbus_enabled	:1; // Modbus активен
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

