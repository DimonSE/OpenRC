/*******************************************************************************
*  System.cpp
*
*******************************************************************************/

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include "def.h"
#include "Crc.h"
#include "Global_Variables.h"
#include "hardware/Hardware.h"
#include "Graphic.h"
#include "Tasks.h"
#include "UI_Engine.h"
#include "System.h"
#include "User_Interface.h"

#include <util/delay.h>

//
// Global defines
//

#define EEPROM_TRASH (0x0000)                    // Мусорная переменная - нулевая ячейка подвержена самопорче
#define EEPROM_CRC   (0x1000 - sizeof(uint16_t)) // Переменная для хранения CRC, размещена в конце EEPROM

// Адрес начала области EEPROM по которой будет считатся CRC. Равен нулевому адресу EEPROM(0) + размер мусорной переменной
#define EEPROM_CRC_START ((uint8_t*)(0x0000 + sizeof(uint8_t)))

// Размер области EEPROM по которой будет считатся CRC. Равен размеру EEPROM - размер мусорной переменной - размер переменной с CRC
#define EEPROM_CRC_SIZE (0x1000 - sizeof(uint8_t) - sizeof(uint16_t))

//
// Functions
//

void Recalc_EEPROM_CRC()
{
    eeprom_write_word((uint16_t*)EEPROM_CRC, Crc16_eeprom(EEPROM_CRC_START, EEPROM_CRC_SIZE));
}

inline uint16_t Get_EEPROM_CRC()
{
    return Crc16_eeprom(EEPROM_CRC_START, EEPROM_CRC_SIZE);
}

bool Is_EEPROM_CRC_Correct()
{
    return eeprom_read_word((uint16_t*)EEPROM_CRC) == Crc16_eeprom(EEPROM_CRC_START, EEPROM_CRC_SIZE);
}

bool TX_IsFirstStart()
{
    return !eeprom_read_byte((uint8_t*)&EEPROM_SETTINGS.FirstStartComplete);
}

void TX_Init()
{
    uint8_t* PtrDst = (uint8_t*)(&Settings);
    uint8_t* PtrSrc = (uint8_t*)(&EEPROM_SETTINGS);

    eeprom_read_block(PtrDst, PtrSrc, sizeof(TRANSMITTER_SETTINGS));

    MODEL_Init(Settings.ModelNum);
}

void TX_SaveSettings()
{
    cli();

    uint8_t* PtrSrc = (uint8_t*)(&Settings);
    uint8_t* PtrDst = (uint8_t*)(&EEPROM_SETTINGS);

    eeprom_write_block(PtrSrc, PtrDst, sizeof(TRANSMITTER_SETTINGS));

    Recalc_EEPROM_CRC();

    sei();
}

void MODEL_Init(int8_t ModelNum)
{
    cli();

    uint8_t* PtrDst = (uint8_t*)(&CurModel);
    uint8_t* PtrSrc = (uint8_t*)(&EEPROM_MODEL[ModelNum]);

    eeprom_read_block(PtrDst, PtrSrc, sizeof(MODEL_SETTINGS));

    hwr_SetModulation(CurModel.modulation);
    Timer_Init();

    sei();
}

void MODEL_Save_Trimmers()
{
    for(int8_t i = 0; i < MAX_MODES; ++i)
        for(int8_t j = 0; j < MAX_TRIMMERS; ++j)
            eeprom_write_byte((uint8_t*)&EEPROM_MODEL[Settings.ModelNum].Mode[i].trimmers[j], CurModel.Mode[i].trimmers[j]);

    Recalc_EEPROM_CRC();
}


void MODEL_Save(int8_t To)
{
    cli();

    uint8_t* PtrSrc = (uint8_t*)(&CurModel);
    uint8_t* PtrDst = (uint8_t*)(&EEPROM_MODEL[To]);

    eeprom_write_block(PtrSrc, PtrDst, sizeof(MODEL_SETTINGS));
    
    Recalc_EEPROM_CRC();

    sei();
}

void MODEL_Copy(int8_t From, int8_t To)
{
    cli();

    uint8_t* PtrFrom = (uint8_t*)(&EEPROM_MODEL[From]);
    uint8_t* PtrTo   = (uint8_t*)(&EEPROM_MODEL[To]);

    for(uint8_t i = 0; i < sizeof(MODEL_SETTINGS); ++i)
        eeprom_write_byte(PtrTo++, eeprom_read_byte(PtrFrom++));
    
    Recalc_EEPROM_CRC();

    sei();
}

void MODEL_Reset(int8_t ModelNum)
{
    static const char* DefModeNames[MAX_MODES] =
    {
        "Normal",
        "Pilotage",
        "Landing"
    };

    uint8_t MOD, CH, CTRL;

    MODEL_SETTINGS Model;

    for(uint8_t MOD = 0; MOD < MAX_MODES; ++MOD)
    {
        for(uint8_t CTRL = 0; CTRL < MAX_CONTROLS; ++CTRL)
        {
            // Устанавливаем 1 - нет реверса
            Model.Mode[MOD].Control[CTRL].reverse = 1;

            // Устанавливаем расходы
            Model.Mode[MOD].Control[CTRL].maxRates = (CTRL == CTRL_THR) ? 100 : 50; // EPA val in 2% unit
            Model.Mode[MOD].Control[CTRL].minRates = (CTRL == CTRL_THR) ? 100 : 50;

            // Устанавливаем двойные расходы
            Model.Mode[MOD].Control[CTRL].maxDRates = 100; // EPA val in 2% unit
            Model.Mode[MOD].Control[CTRL].minDRates = 100;

            // Устанавливаем кривую
            for(uint8_t i = 0; i < CURVE_NODES; ++i)
            {
                Model.Mode[MOD].Control[CTRL].nodes[i] = (200 * (int)i) / (CURVE_NODES - 1) - 100;
            }

            // Устанавливаем источник каналов(для виртуальных)
            Model.Mode[MOD].Control[CTRL].from = CTRL;
        }

        // Нулим микшер
        for(uint8_t CH = 0; CH < MAX_CHANNELS; ++CH)
            for(uint8_t CTRL = 0; CTRL < MAX_CONTROLS; ++CTRL)
                Model.Mode[MOD].Chanels[CH][CTRL] = 0;

        // Устанавливаем микшер по умолчанию
        for(uint8_t CH = 0; CH < MAX_CHANNELS; ++CH)
            Model.Mode[MOD].Chanels[CH][CH] = 100;

        // Нулим тримерры
        for(uint8_t i = 0; i < 4; ++i)
            Model.Mode[MOD].trimmers[i] = 0;

        // Задаем название полётного режима
        {
            uint8_t i = 0;
            for(; i < MODE_NAME_LEN; ++i)
            {
                Model.Mode[MOD].name[i] = DefModeNames[MOD][i];
                if(DefModeNames[MOD][i] == 0)
                    break;
            }
            
            // Заполняем оставшееся место имени полётного режима нулями
            for(; i < MODE_NAME_LEN; ++i)
                Model.Mode[MOD].name[i] = 0;
        }
    }

    // Устанавливаем имя
    char str[10];
    sprintf(str, "Model %d", ModelNum + 1);
    uint8_t len = uint8_t(strlen(str));
    if(len > ARRAY_SIZE(Model.name) - 1) //DEBUG TODO, WTF???
        len = ARRAY_SIZE(Model.name) - 1;

    {
        uint8_t i = 0;
        for(; i < len; ++i)
            Model.name[i] = str[i];
        for(; i < ARRAY_SIZE(Model.name); ++i)
            Model.name[i] = 0;
    }

    // Устанавливаем тип модуляции
    Model.modulation = DEFAULT_MODULATION;

    // Устанавливаем тип модели
    Model.type = TYPE_PLANE;

    // Устанавливаем настройки таймера
    Model.timer = 0;
    Model.timer_mode = 0;

    // Устанавливаем количество каналов
    Model.num_ch = 6;

    // Устанавливаем размер структуры
    Model.size = sizeof(Model);

    uint8_t* PtrSrc = (uint8_t*)(&Model);
    uint8_t* PtrDst = (uint8_t*)(&EEPROM_MODEL[ModelNum]);

    eeprom_write_block(PtrSrc, PtrDst, Model.size);
}

void MODE_Copy(int8_t From, int8_t To)
{
    cli();

    for(uint8_t i = 0; i < MODE_NAME_LEN; i++)
    {
        eeprom_write_byte(
                (uint8_t*)&EEPROM_MODEL[Settings.ModelNum].Mode[To].name[i],
                eeprom_read_byte((uint8_t*)&EEPROM_MODEL[Settings.ModelNum].Mode[From].name[i])
            );
    }

    uint8_t* PtrFrom = (uint8_t*)(&EEPROM_MODEL[Settings.ModelNum].Mode[From]);
    uint8_t* PtrTo   = (uint8_t*)(&EEPROM_MODEL[Settings.ModelNum].Mode[To]);

    for(uint8_t i = 0; i < sizeof(DEF_MODE); ++i)
        eeprom_write_byte(PtrTo++, eeprom_read_byte(PtrFrom++));

    
    Recalc_EEPROM_CRC();

    sei();
}

void Calibration()
{
    cli();

    for(uint8_t i = 0; i < MAX_ADC; ++i)
    {
        AD[i] = read_adc(i);

        Settings.ADmin[i] = AD[i];
        Settings.ADmax[i] = AD[i];
    }

    gfx_ClearBuf();
    gfx_PrintString(30, TFT_ScreenHeight / 2, "Calibrating", COLOR_FRONT, Font_8x12);
    gfx_Refresh();
    _delay_ms(1000);

    enum { Stick_Min = 0, Stick_Max, Stick_Val, StickGroup_Count };

    int16_t Sticks[StickGroup_Count][5];
    
    while(!HB_ENTER)
    {
        gfx_ClearBuf();

        for (uint8_t i = 0; i < MAX_ADC; ++i)
        {
            AD[i] = read_adc(i);
            
            if(Settings.ADmin[i] > AD[i])
                Settings.ADmin[i] = AD[i];
            
            if(Settings.ADmax[i] < AD[i])
                Settings.ADmax[i] = AD[i];
        }

        Sticks[Stick_Min][CTRL_AIL]  = Settings.ADmin[ADC_AIL];
        Sticks[Stick_Min][CTRL_ELE]  = Settings.ADmin[ADC_ELE];
        Sticks[Stick_Min][CTRL_THR]  = Settings.ADmin[ADC_THR];
        Sticks[Stick_Min][CTRL_RUD]  = Settings.ADmin[ADC_RUD];
        Sticks[Stick_Min][CTRL_AUX1] = Settings.ADmin[ADC_AUX1];

        Sticks[Stick_Max][CTRL_AIL]  = Settings.ADmax[ADC_AIL];
        Sticks[Stick_Max][CTRL_ELE]  = Settings.ADmax[ADC_ELE];
        Sticks[Stick_Max][CTRL_THR]  = Settings.ADmax[ADC_THR];
        Sticks[Stick_Max][CTRL_RUD]  = Settings.ADmax[ADC_RUD];
        Sticks[Stick_Max][CTRL_AUX1] = Settings.ADmax[ADC_AUX1];

        Sticks[Stick_Val][CTRL_AIL]  = AD[ADC_AIL];
        Sticks[Stick_Val][CTRL_ELE]  = AD[ADC_ELE];
        Sticks[Stick_Val][CTRL_THR]  = AD[ADC_THR];
        Sticks[Stick_Val][CTRL_RUD]  = AD[ADC_RUD];
        Sticks[Stick_Val][CTRL_AUX1] = AD[ADC_AUX1];
        
        DrawSticks(Sticks[Stick_Min], Sticks[Stick_Max], Sticks[Stick_Val]);

        gfx_Refresh();
    }

    MsgBox("Center all\nsticks and\npress Enter", Font_8x12, "Calibration");
    
    // Используется прямой доступ к кнопкам потому, что во время калибровки драйвер клавиатуры не работатает.
    while(HB_ENTER);  // Ждем отпускания кнопок
    while(!HB_ENTER); // Ждем нажатия кнопоки ВВОД

    for(uint8_t i = 0; i < MAX_ADC; ++i)
    {
        // Читаем и записываем центральное положение аналоговых каналов
        Settings.ADmid[i] = read_adc(i);
        eeprom_write_word((uint16_t*)&EEPROM_SETTINGS.ADmid[i], Settings.ADmid[i]);
        eeprom_write_word((uint16_t*)&EEPROM_SETTINGS.ADmin[i], Settings.ADmin[i]);
        eeprom_write_word((uint16_t*)&EEPROM_SETTINGS.ADmax[i], Settings.ADmax[i]);
        
        // Коэффициент нормализации умножается на 10, что бы он был больше.
        // В математике при рассчётах результат делится на 10.
        // Читаем и записываем коэффициент нормализации при отклонении
        // управляющего элемента в большую сторону относительно центра
        Settings.Kmax[i] = (TimerClockPerUSec(500) * 10) / (Settings.ADmax[i] - Settings.ADmid[i]);
        eeprom_write_word((uint16_t*)&EEPROM_SETTINGS.Kmax[i], Settings.Kmax[i]); 
        // Читаем и записываем коэффициент нормализации при отклонении
        // управляющего элемента в меньшую сторону относительно центра
        Settings.Kmin[i] = (TimerClockPerUSec(500) * 10) / (Settings.ADmid[i] - Settings.ADmin[i]);
        eeprom_write_word((uint16_t*)&EEPROM_SETTINGS.Kmin[i], Settings.Kmin[i]); 
    }

    Recalc_EEPROM_CRC();

    sei();
}

void TX_Reset()
{
    Settings.SoundFlag = ON;

    gfx_ClearBuf();

    const FontProfile& Font = Font_8x12;
    const uint8_t StartPos = (TFT_ScreenHeight - Font.H) / 2 - Font.H;

    gfx_PrintString((TFT_ScreenWidth - Font.W * 10) / 2, StartPos,          "Please" , COLOR_FRONT, Font);
    gfx_PrintString((TFT_ScreenWidth - Font.W * 5 ) / 2, StartPos + Font.H, "wait...", COLOR_FRONT, Font);
    gfx_DrawRectangle(0, StartPos + Font.H*2, TFT_ScreenWidth - 1, StartPos + Font.H * 3, COLOR_FRONT);
    gfx_Refresh();

    const uint8_t Step = TFT_ScreenWidth / MAX_MODELS;
    for(uint8_t i = 0, x = Step; i < MAX_MODELS; ++i, x += Step)
    {
        Settings.ModelNum = i;
        MODEL_Reset(Settings.ModelNum);
        gfx_FillRectangle(0, StartPos + Font.H * 2, x, StartPos + Font.H * 3, COLOR_FRONT);
        gfx_Refresh();
    }

    _delay_ms(500);

    eeprom_write_word((uint16_t*)&EEPROM_SETTINGS.size, sizeof(EEPROM_SETTINGS));
    eeprom_write_byte((uint8_t* )&EEPROM_SETTINGS.ModelNum, 0);
    eeprom_write_byte((uint8_t* )&EEPROM_SETTINGS.SoundFlag, true);
    
    Calibration();

    eeprom_write_byte((uint8_t*)&EEPROM_SETTINGS.FirstStartComplete, true);

    Recalc_EEPROM_CRC();
}
