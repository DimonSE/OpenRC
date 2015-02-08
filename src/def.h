// *****************************************************************************
// ***   Общие определения   ***************************************************
// *****************************************************************************

#ifndef __def_h__
#define __def_h__

#include "Utils.h"

#define F_CPU 16000000UL
#define TIMER_DEVIDER  8            // Делитель главного таймера
#define TASK_TICK_TIME 20           // Время одного тика таймера задач(в мс)

// ***   Количество точек в кривой   *******************************************
#define CURVE_NODES   7
// ***   Количество моделей   **************************************************
#define MAX_MODELS    5
// ***   Длинна названия модели (максимум n символов + нуль-терминатор)   ******
#define MODEL_NAME_LEN (12 + 1)
// ***   Длинна названия полётного режима (n символов + нуль-терминатор)   *****
#define MODE_NAME_LEN  (10 + 1)

#define DEFAULT_MODULATION MODUL_IPPM

#define U_BAT_K 5

//
// TFT Display config
//

#define TFT_ScreenWidth  160 // pixels in width
#define TFT_ScreenHeight 128 // pixels in height

#define TFT_CS  REGISTER_BIT(PORTL, bit2)
#define TFT_RS  REGISTER_BIT(PORTL, bit0)
#define TFT_RST REGISTER_BIT(PORTL, bit1)

enum ColorIdx
{
  COLOR_BACKG = 0,
  COLOR_FRONT,
  
  COLOR_TABLE_MAX
};

extern uint16_t COLOR_IDX_TO_RGB[COLOR_TABLE_MAX];

//
// Trimer config
//

#define trim_0up        (!(PINL & 0x08))
#define trim_0down      (!(PINL & 0x20))
#define trim_1up        (!(PINL & 0x40))
#define trim_1down      (!(PINL & 0x10))
#define trim_2up        (!(PINL & 0x80))
#define trim_2down      (!(PING & 0x01))
#define trim_3up        (!(PING & 0x02))
#define trim_3down      (!(PIND & 0x80))


// *****************************************************************************
// ***   Кнопки навигации   ****************************************************
// *****************************************************************************
#define HB_UP              (!(PINA & 0x20))
#define HB_DOWN            (!(PINA & 0x04))
#define HB_LEFT            (!(PINA & 0x08))
#define HB_RIGHT           (!(PINA & 0x01))
#define HB_BACK            (!(PINA & 0x10))
#define HB_ENTER           (!(PINA & 0x02) || !(PINA & 0x80))

// *****************************************************************************
// ***   Переключатели   *******************************************************
// *****************************************************************************
#define MODE_KEY1       (!(PINC & 0x80))
#define MODE_KEY2       (!(PINC & 0x40))

// Возможно использование SW1 как трехпозиционный, так и как двухпозиционный
// Если определен SW1 - преключатель двухпозиционный
//#define SW1             !PIND.7
// Иначе(должены быть определёны SW1_1 и SW1_2) - трехпозиционный
#define SW1_1           (!(PINA & 0x40))
#define SW1_2           (!(PINC & 0x20))
#define SW2             (!(PINC & 0x10))
#define SW3             (!(PINC & 0x08))

#define Tcut_KEY        (!(PINC & 0x04))
#define DUAL_AIL        (!(PINC & 0x02))
#define DUAL_ELE        (!(PINC & 0x01))
#define DUAL_RUD        (!(PING & 0x04))

// *****************************************************************************
// ***   Каналы АЦП   **********************************************************
// *****************************************************************************
enum
{
    ADC_ELE = 0,
    ADC_AIL,
    ADC_THR,
    ADC_RUD,
    ADC_AUX1,
    ADC_BAT,
    MAX_ADC
};

// *****************************************************************************
// ***   Элементы управления   *************************************************
// *****************************************************************************
enum
{
    CTRL_AIL = 0, // <-+
    CTRL_ELE,     // <-| Должны быть первыми, потому как триммеры
    CTRL_THR,     // <-| добавляются к первым 4-м каналам
    CTRL_RUD,     // <-+
    CTRL_AUX1,
    CTRL_SW1,
    CTRL_SW2,
    CTRL_SW3,
    CTRL_V1, // Виртуальные каналы должны быть последними, CTRL_V1 должен быть первым из
    CTRL_V2, // них, т.к. всё каналы что ниже его нельзя выбрать в качестве источника.
    CTRL_TRIM,
    MAX_CONTROLS
};

// *****************************************************************************
// ***   Параметры управляющих элементов   *************************************
// *****************************************************************************
enum
{
    CTRLS_REV = 0,
    CTRLS_MAXR,
    CTRLS_MINR,
    CTRLS_MAXDR,
    CTRLS_MINDR,
    CTRLS_TCUT,
    CTRLS_FROM,
    CTRLS_CURVE,
    MAX_CTRL_SETTINGS
};

// *****************************************************************************
// ***   Trimmers   ************************************************************
// *****************************************************************************
enum
{
    TRM_AIL = 0,
    TRM_ELE,
    TRM_THR,
    TRM_RUD,
    MAX_TRIMMERS
};

// *****************************************************************************
// ***   Каналы PPM   **********************************************************
// *****************************************************************************
enum
{
    CH_AIL = 0,
    CH_ELE,
    CH_THR,
    CH_RUD,
    CH_AUX1,
    CH_AUX2,
    CH_AUX3,
    CH_BAT,
    MAX_CHANNELS
};

// *****************************************************************************
// ***   Model types   *********************************************************
// *****************************************************************************
enum
{
    TYPE_PLANE = 0,
    TYPE_HELI,
    TYPE_GLIDER,
    MAX_TYPES
};

// *****************************************************************************
// ***   Fly modes   ***********************************************************
// *****************************************************************************
enum
{
    M_NORMAL = 0,
    M_IDLEUP,
    M_THOLD,
    MAX_MODES
};

//
// Modulation types
//

enum Modulation
{
    MODUL_PPM = 0,
    MODUL_IPPM,
    MODUL_PCM,
    MAX_MODULS
};

//
// Timer defines
//

#define MSEC_IN_SEC  1000
#define USEC_IN_MSEC 1000
#define USEC_IN_SEC  1000000

#define TimerPPM_min 900  // in USec
#define TimerPPM_max 2100 // in USec
#define TimerPPM_mid 1500 // in USec

#define TimerClockPerSec(s)   ((uint32_t)(s * (F_CPU / TIMER_DEVIDER)))
#define TimerClockPerMSec(ms) ((uint16_t)(ms * (F_CPU / TIMER_DEVIDER / MSEC_IN_SEC)))
#define TimerClockPerUSec(us) ((uint16_t)(us * (F_CPU / TIMER_DEVIDER / USEC_IN_SEC)))

#define TimerSecPerClock(clock)  (float(clock) / (F_CPU / TIMER_DEVIDER))
#define TimerMSecPerClock(clock) (float(clock) / (F_CPU / TIMER_DEVIDER / MSEC_IN_SEC))
#define TimerUSecPerClock(clock) (clock / (F_CPU / TIMER_DEVIDER / USEC_IN_SEC))

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#define MAX(x, y) ((x) > (y) ? (x) : (y))

//
// Constants
//

#define ON  true
#define OFF false

#define RESULT_INCORRECT -1

#endif // __def_h__