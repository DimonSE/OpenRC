/*******************************************************************************
*  User_Interface.cpp
*
*******************************************************************************/

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include "def.h"
#include "Global_Variables.h"
#include "Graphic.h"
#include "UI_Engine.h"
#include "User_Interface.h"
#include "Tasks.h"
#include "System.h"
#include "Sound.h"
#include "Modulation.h"
#include "hardware/Hardware.h"

#include <util/delay.h>

//
// Public Functions
//

void DrawTrimmers();
void DrawRadio(const uint8_t X, const uint8_t Y);

void DrawSticks(int16_t Min[], int16_t Max[], int16_t Val[], bool IsDrawTrimmers)
{
    const uint8_t OFFSETX = 16;
    const uint8_t OFFSETY = 26;

    const uint8_t RECT_SIZE = TFT_ScreenWidth / 2 - 32;
    
    const uint8_t RECT_LEFT_POSX  = OFFSETX;
    const uint8_t RECT_LEFT_POSY  = OFFSETY + 4;
    const uint8_t RECT_RIGHT_POSX = TFT_ScreenWidth - OFFSETX - RECT_SIZE - 1;
    const uint8_t RECT_RIGHT_POSY = OFFSETY + 4;

    gfx_DrawLine(RECT_LEFT_POSX, RECT_LEFT_POSY + RECT_SIZE / 2, RECT_LEFT_POSX + RECT_SIZE, RECT_LEFT_POSY + RECT_SIZE / 2,     COLOR_FRONT, true);
    gfx_DrawLine(RECT_LEFT_POSX + RECT_SIZE / 2, RECT_LEFT_POSY, RECT_LEFT_POSX + RECT_SIZE / 2, RECT_LEFT_POSY + RECT_SIZE,     COLOR_FRONT, true);
    gfx_DrawLine(RECT_RIGHT_POSX, RECT_RIGHT_POSY + RECT_SIZE / 2, RECT_RIGHT_POSX + RECT_SIZE, RECT_RIGHT_POSY + RECT_SIZE / 2, COLOR_FRONT, true);
    gfx_DrawLine(RECT_RIGHT_POSX + RECT_SIZE / 2, RECT_RIGHT_POSY, RECT_RIGHT_POSX + RECT_SIZE / 2, RECT_RIGHT_POSY + RECT_SIZE, COLOR_FRONT, true);

    gfx_DrawRectangle(RECT_LEFT_POSX,      RECT_LEFT_POSY,      RECT_LEFT_POSX  + RECT_SIZE,     RECT_LEFT_POSY  + RECT_SIZE,     COLOR_FRONT);
    gfx_DrawRectangle(RECT_LEFT_POSX - 1,  RECT_LEFT_POSY - 1,  RECT_LEFT_POSX  + RECT_SIZE + 1, RECT_LEFT_POSY  + RECT_SIZE + 1, COLOR_FRONT);
    gfx_DrawRectangle(RECT_RIGHT_POSX,     RECT_RIGHT_POSY,     RECT_RIGHT_POSX + RECT_SIZE,     RECT_RIGHT_POSY + RECT_SIZE,     COLOR_FRONT);
    gfx_DrawRectangle(RECT_RIGHT_POSX - 1, RECT_RIGHT_POSY - 1, RECT_RIGHT_POSX + RECT_SIZE + 1, RECT_RIGHT_POSY + RECT_SIZE + 1, COLOR_FRONT);

    uint8_t x =             uint16_t(Val[CTRL_RUD] - Min[CTRL_RUD]) * RECT_SIZE / (Max[CTRL_RUD] - Min[CTRL_RUD] + 1);
    uint8_t y = RECT_SIZE - uint16_t(Val[CTRL_THR] - Min[CTRL_THR]) * RECT_SIZE / (Max[CTRL_THR] - Min[CTRL_THR] + 1);

    gfx_DrawPixel(RECT_LEFT_POSX + x, RECT_LEFT_POSY - 2 + y, COLOR_FRONT);
    gfx_DrawPixel(RECT_LEFT_POSX + x, RECT_LEFT_POSY + 2 + y, COLOR_FRONT);
    gfx_DrawLine(RECT_LEFT_POSX - 1 + x, RECT_LEFT_POSY - 1 + y, RECT_LEFT_POSX + 1 + x, RECT_LEFT_POSY - 1 + y, COLOR_FRONT);
    gfx_DrawLine(RECT_LEFT_POSX - 2 + x, RECT_LEFT_POSY + y,     RECT_LEFT_POSX + 2 + x, RECT_LEFT_POSY + y,     COLOR_FRONT);
    gfx_DrawLine(RECT_LEFT_POSX - 1 + x, RECT_LEFT_POSY + 1 + y, RECT_LEFT_POSX + 1 + x, RECT_LEFT_POSY + 1 + y, COLOR_FRONT);

    x =             uint16_t(Val[CTRL_AIL] - Min[CTRL_AIL]) * RECT_SIZE / (Max[CTRL_AIL] - Min[CTRL_AIL] + 1);
    y = RECT_SIZE - uint16_t(Val[CTRL_ELE] - Min[CTRL_ELE]) * RECT_SIZE / (Max[CTRL_ELE] - Min[CTRL_ELE] + 1);

    gfx_DrawPixel(RECT_RIGHT_POSX + x, RECT_RIGHT_POSY - 2 + y, COLOR_FRONT);
    gfx_DrawPixel(RECT_RIGHT_POSX + x, RECT_RIGHT_POSY + 2 + y, COLOR_FRONT);
    gfx_DrawLine(RECT_RIGHT_POSX - 1 + x, RECT_RIGHT_POSY - 1 + y, RECT_RIGHT_POSX + 1 + x, RECT_RIGHT_POSY - 1 + y, COLOR_FRONT);
    gfx_DrawLine(RECT_RIGHT_POSX - 2 + x, RECT_RIGHT_POSY + y,     RECT_RIGHT_POSX + 2 + x, RECT_RIGHT_POSY + y,     COLOR_FRONT);
    gfx_DrawLine(RECT_RIGHT_POSX - 1 + x, RECT_RIGHT_POSY + 1 + y, RECT_RIGHT_POSX + 1 + x, RECT_RIGHT_POSY + 1 + y, COLOR_FRONT);

    x = uint16_t(Val[CTRL_AUX1] - Min[CTRL_AUX1]) * RECT_SIZE / (Max[CTRL_AUX1] - Min[CTRL_AUX1] + 1);

    gfx_DrawRectangle(RECT_RIGHT_POSX,     RECT_RIGHT_POSY - 5, RECT_RIGHT_POSX + RECT_SIZE, RECT_RIGHT_POSY - 3, COLOR_FRONT);
    gfx_FillRectangle(RECT_RIGHT_POSX + x, RECT_RIGHT_POSY - 5, RECT_RIGHT_POSX + x + 1,     RECT_RIGHT_POSY - 3, COLOR_FRONT);

    if (IsDrawTrimmers)
    {
        const uint8_t TRIM_SIZE = 32;
        const uint8_t TRIM_HEIGHT = 8;

        const uint8_t TRIM0_X = RECT_RIGHT_POSX + RECT_SIZE / 2 - TRIM_SIZE / 2;
        const uint8_t TRIM0_Y = RECT_RIGHT_POSY + RECT_SIZE + TRIM_HEIGHT;
        const uint8_t TRIM1_X = RECT_RIGHT_POSX - TRIM_HEIGHT;
        const uint8_t TRIM1_Y = RECT_RIGHT_POSY + RECT_SIZE / 2 - TRIM_SIZE / 2;
        const uint8_t TRIM2_X = RECT_LEFT_POSX + RECT_SIZE + TRIM_HEIGHT;
        const uint8_t TRIM2_Y = RECT_LEFT_POSY + RECT_SIZE / 2 - TRIM_SIZE / 2;
        const uint8_t TRIM3_X = RECT_LEFT_POSX + RECT_SIZE / 2 - TRIM_SIZE / 2;
        const uint8_t TRIM3_Y = RECT_LEFT_POSY + RECT_SIZE + TRIM_HEIGHT;

        int8_t tr[4];

        tr[0] = CurModel.Mode[FLY_MODE].trimmers[0] / 6 + TRIM0_X + 16;

        gfx_DrawLine(TRIM0_X, TRIM0_Y, TRIM0_X + TRIM_SIZE, TRIM0_Y, COLOR_FRONT);
        
        gfx_DrawPixel(TRIM0_X,      TRIM0_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM0_X,      TRIM0_Y - 2, COLOR_FRONT);
        gfx_DrawPixel(TRIM0_X +  4, TRIM0_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM0_X +  8, TRIM0_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM0_X + 12, TRIM0_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM0_X + 16, TRIM0_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM0_X + 16, TRIM0_Y - 2, COLOR_FRONT);
        gfx_DrawPixel(TRIM0_X + 20, TRIM0_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM0_X + 24, TRIM0_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM0_X + 28, TRIM0_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM0_X + 32, TRIM0_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM0_X + 32, TRIM0_Y - 2, COLOR_FRONT);
        
        gfx_DrawPixel(tr[0],     TRIM0_Y - 3, COLOR_FRONT);
        gfx_DrawLine (tr[0] - 1, TRIM0_Y - 4, tr[0] + 1, TRIM0_Y - 4, COLOR_FRONT);
        gfx_DrawLine (tr[0] - 2, TRIM0_Y - 5, tr[0] + 2, TRIM0_Y - 5, COLOR_FRONT);

        tr[1] = CurModel.Mode[FLY_MODE].trimmers[1] / -6 + TRIM1_Y + 16;

        gfx_DrawLine(TRIM1_X, TRIM1_Y, TRIM1_X, TRIM1_Y + TRIM_SIZE, COLOR_FRONT);

        gfx_DrawPixel(TRIM1_X + 1, TRIM1_Y,      COLOR_FRONT);
        gfx_DrawPixel(TRIM1_X + 2, TRIM1_Y,      COLOR_FRONT);
        gfx_DrawPixel(TRIM1_X + 1, TRIM1_Y +  4, COLOR_FRONT);
        gfx_DrawPixel(TRIM1_X + 1, TRIM1_Y +  8, COLOR_FRONT);
        gfx_DrawPixel(TRIM1_X + 1, TRIM1_Y + 12, COLOR_FRONT);
        gfx_DrawPixel(TRIM1_X + 1, TRIM1_Y + 16, COLOR_FRONT);
        gfx_DrawPixel(TRIM1_X + 2, TRIM1_Y + 16, COLOR_FRONT);
        gfx_DrawPixel(TRIM1_X + 1, TRIM1_Y + 20, COLOR_FRONT);
        gfx_DrawPixel(TRIM1_X + 1, TRIM1_Y + 24, COLOR_FRONT);
        gfx_DrawPixel(TRIM1_X + 1, TRIM1_Y + 28, COLOR_FRONT);
        gfx_DrawPixel(TRIM1_X + 1, TRIM1_Y + 32, COLOR_FRONT);
        gfx_DrawPixel(TRIM1_X + 2, TRIM1_Y + 32, COLOR_FRONT);

        gfx_DrawPixel(TRIM1_X + 3, tr[1], COLOR_FRONT);
        gfx_DrawLine (TRIM1_X + 4, tr[1] - 1, TRIM1_X + 4, tr[1] + 1, COLOR_FRONT);
        gfx_DrawLine (TRIM1_X + 5, tr[1] - 2, TRIM1_X + 5, tr[1] + 2, COLOR_FRONT);

        tr[2] = CurModel.Mode[FLY_MODE].trimmers[2]/ -6 + TRIM1_Y + 16;

        gfx_DrawLine(TRIM2_X, TRIM1_Y, TRIM2_X, TRIM1_Y + TRIM_SIZE, COLOR_FRONT);

        gfx_DrawPixel(TRIM2_X - 1, TRIM1_Y,      COLOR_FRONT);
        gfx_DrawPixel(TRIM2_X - 2, TRIM1_Y,      COLOR_FRONT);
        gfx_DrawPixel(TRIM2_X - 1, TRIM1_Y +  4, COLOR_FRONT);
        gfx_DrawPixel(TRIM2_X - 1, TRIM1_Y +  8, COLOR_FRONT);
        gfx_DrawPixel(TRIM2_X - 1, TRIM1_Y + 12, COLOR_FRONT);
        gfx_DrawPixel(TRIM2_X - 1, TRIM1_Y + 16, COLOR_FRONT);
        gfx_DrawPixel(TRIM2_X - 2, TRIM1_Y + 16, COLOR_FRONT);
        gfx_DrawPixel(TRIM2_X - 1, TRIM1_Y + 20, COLOR_FRONT);
        gfx_DrawPixel(TRIM2_X - 1, TRIM1_Y + 24, COLOR_FRONT);
        gfx_DrawPixel(TRIM2_X - 1, TRIM1_Y + 28, COLOR_FRONT);
        gfx_DrawPixel(TRIM2_X - 1, TRIM1_Y + 32, COLOR_FRONT);
        gfx_DrawPixel(TRIM2_X - 2, TRIM1_Y + 32, COLOR_FRONT);
        
        gfx_DrawPixel(TRIM2_X - 3, tr[2], COLOR_FRONT);
        gfx_DrawLine (TRIM2_X - 4, tr[2] - 1, TRIM2_X - 4, tr[2] + 1, COLOR_FRONT);
        gfx_DrawLine (TRIM2_X - 5, tr[2] - 2, TRIM2_X - 5, tr[2] + 2, COLOR_FRONT);

        tr[3] = CurModel.Mode[FLY_MODE].trimmers[3] / 6 + TRIM3_X + 16;

        gfx_DrawLine(TRIM3_X, TRIM3_Y, TRIM3_X + TRIM_SIZE, TRIM3_Y, COLOR_FRONT);
        
        gfx_DrawPixel(TRIM3_X,      TRIM3_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM3_X,      TRIM3_Y - 2, COLOR_FRONT);
        gfx_DrawPixel(TRIM3_X +  4, TRIM3_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM3_X +  8, TRIM3_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM3_X + 12, TRIM3_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM3_X + 16, TRIM3_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM3_X + 16, TRIM3_Y - 2, COLOR_FRONT);
        gfx_DrawPixel(TRIM3_X + 20, TRIM3_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM3_X + 24, TRIM3_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM3_X + 28, TRIM3_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM3_X + 32, TRIM3_Y - 1, COLOR_FRONT);
        gfx_DrawPixel(TRIM3_X + 32, TRIM3_Y - 2, COLOR_FRONT);

        gfx_DrawPixel(tr[3], TRIM3_Y - 3, COLOR_FRONT);
        gfx_DrawLine(tr[3] - 1, TRIM3_Y - 4, tr[3] + 1, TRIM3_Y - 4, COLOR_FRONT);
        gfx_DrawLine(tr[3] - 2, TRIM3_Y - 5, tr[3] + 2, TRIM3_Y - 5, COLOR_FRONT);
    }
}

void MainScreen()
{
    char str[32];

    gfx_ClearBuf();

    // Номер модели
    sprintf(str, "%d", Settings.ModelNum + 1);
    gfx_PrintString(2, 2, str, COLOR_FRONT, Font_8x12);
    
    if(CurModel.type == TYPE_HELI)
    {
        // Значек верта
        gfx_PrintChar(12, 5, 125, COLOR_FRONT, Font_8x8);
        gfx_PrintChar(20, 5, 126, COLOR_FRONT, Font_8x8);
    }
    if(CurModel.type == TYPE_PLANE)
    {
        // Значек самолета
        gfx_PrintChar(12, 5, 127, COLOR_FRONT, Font_8x8);
        gfx_PrintChar(20, 5, 128, COLOR_FRONT, Font_8x8);
    }

    // Название модели
    gfx_PrintString(32, 2, CurModel.name, COLOR_FRONT, Font_8x12);

    if(Cut_enable)
    {
        // Значек зачеркнутого двигателя
        gfx_PrintChar(90, 1, 129, COLOR_FRONT, Font_8x8);
        gfx_PrintChar(98, 1, 130, COLOR_FRONT, Font_8x8);
        gfx_PrintChar(90, 9, 131, COLOR_FRONT, Font_8x8);
        gfx_PrintChar(98, 9, 132, COLOR_FRONT, Font_8x8);
    }

    // Draw battery status
    int16_t U_bat = GetBatteryStatus();
    sprintf(str, "%2i.%02iV", U_bat / 100, U_bat % 100);
    
    const uint8_t BatWidth  = 7 * Font_4x6.W + 3;
    const uint8_t BatHeight = Font_4x6.H + 3;
    gfx_DrawRectangle(TFT_ScreenWidth - BatWidth, 1, TFT_ScreenWidth - 2, BatHeight, COLOR_FRONT);
    gfx_DrawRectangle(TFT_ScreenWidth - BatWidth - 2, 1 + div2(BatHeight) - 2, TFT_ScreenWidth - BatWidth - 1, 1 + div2(BatHeight) + 2, COLOR_FRONT);
    gfx_PrintString(TFT_ScreenWidth - BatWidth + 2, 3, str, COLOR_FRONT, Font_4x6);


    // Print fly mode
    gfx_PrintString(2, Font_8x12.H + 4, CurModel.Mode[FLY_MODE].name, COLOR_FRONT, Font_8x8);

    // Draw timer
    const int16_t t    = GetTimerValue();
    const int16_t subt = GetTimerSubValue();
    
    int8_t min[2], sec[2], ms;
    
    min[0] = t / 600;     // Минуты - старший разряд
    min[1] = t / 60 % 10; // Минуты - младший разряд
    sec[0] = t % 60 / 10; // Секунды - старший разряд
    sec[1] = t % 10;      // Секунды - младший разряд
    ms     = subt / 100;  // Миллисекунды

    const uint8_t TimerPosX = ((TFT_ScreenWidth - 7 * Font_12x16.W) / 2);
    const uint8_t TimerPosY = (TFT_ScreenHeight - 30);

    sprintf(str, "%d%d:%d%d.%d", min[0], min[1], sec[0], sec[1], ms);
    gfx_PrintString(TimerPosX, TimerPosY, str, COLOR_FRONT, Font_12x16);
    
    enum { Stick_Min = 0, Stick_Max, Stick_Val, StickGroup_Count };
    
    int16_t Sticks[StickGroup_Count][5];
    Sticks[Stick_Min][CTRL_AIL]  = OUTPUT_MIN / 10;
    Sticks[Stick_Min][CTRL_ELE]  = OUTPUT_MIN / 10;
    Sticks[Stick_Min][CTRL_THR]  = OUTPUT_MIN / 10;
    Sticks[Stick_Min][CTRL_RUD]  = OUTPUT_MIN / 10;
    Sticks[Stick_Min][CTRL_AUX1] = OUTPUT_MIN / 10;

    Sticks[Stick_Max][CTRL_AIL]  = OUTPUT_MAX / 10;
    Sticks[Stick_Max][CTRL_ELE]  = OUTPUT_MAX / 10;
    Sticks[Stick_Max][CTRL_THR]  = OUTPUT_MAX / 10;
    Sticks[Stick_Max][CTRL_RUD]  = OUTPUT_MAX / 10;
    Sticks[Stick_Max][CTRL_AUX1] = OUTPUT_MAX / 10;

    Sticks[Stick_Val][CTRL_AIL]  = output[CTRL_AIL] / 10;
    Sticks[Stick_Val][CTRL_ELE]  = output[CTRL_ELE] / 10;
    Sticks[Stick_Val][CTRL_THR]  = output[CTRL_THR] / 10;
    Sticks[Stick_Val][CTRL_RUD]  = output[CTRL_RUD] / 10;
    Sticks[Stick_Val][CTRL_AUX1] = output[CTRL_AUX1] / 10;
    
    DrawSticks(Sticks[Stick_Min], Sticks[Stick_Max], Sticks[Stick_Val], true);

    DrawRadio(6, TFT_ScreenHeight - 6);

    gfx_Refresh();
}

//
// Service Functions
//

void DrawRadio(const uint8_t X, const uint8_t Y)
{
    uint8_t PosX1 = X, PosX2 = X;

    const uint8_t Pause  = 2;
    const uint8_t Height = 6;
    const uint8_t EndPos = 160;
    
    for(uint8_t i = 0; i < CurModel.num_ch; ++i)
    {
        PosX2 += Pause;
        gfx_DrawLine(PosX1, Y, PosX2, Y, COLOR_FRONT);
        
        PosX1 = PosX2;
        gfx_DrawLine(PosX1, Y, PosX2, Y - Height, COLOR_FRONT);
        
        PosX2 += output[i] / TimerClockPerUSec(100); // Длинна импульса в пикселях
        gfx_DrawLine(PosX1, Y - Height, PosX2, Y - Height, COLOR_FRONT);
        
        PosX1 = PosX2;
        gfx_DrawLine(PosX1, Y - Height, PosX2, Y, COLOR_FRONT);
    }

    PosX1 = PosX2;
    PosX2 = X + EndPos;

    gfx_DrawLine(PosX1, Y, PosX2, Y, COLOR_FRONT);
}

//
// Model menu handlers
//

void ModelCopy(int8_t dummy)
{
    char        ModelsStr[MAX_MODELS][MODEL_NAME_LEN];
    const char* ModelsStrPtrs[MAX_MODELS];
    for(uint8_t i = 0; i < MAX_MODELS; ++i)
    {
        for(uint8_t j = 0; j < ARRAY_SIZE(EEPROM_MODEL[i].name); j++)
            ModelsStr[i][j] = eeprom_read_byte((uint8_t*)&EEPROM_MODEL[i].name[j]);
        
        ModelsStrPtrs[i] = ModelsStr[i];
    }

    int16_t result = FromToBox(ModelsStrPtrs, MAX_MODELS, ModelsStrPtrs, MAX_MODELS, "Model Copy:", Font_8x8);

    if(result == RESULT_INCORRECT)
        return;
    
    int8_t From = int8_t(result >> 8);
    int8_t To   = int8_t(result & 0xFF);
    if(From != To)
    {
        MODEL_Copy(From, To);
        
        // Если мы скопировали другую модель в текущюю - её нужно переинициализировать
        if(To == Settings.ModelNum)
            MODEL_Init(Settings.ModelNum);
    }
}

void ModeCopy(int8_t AddParam)
{
    char        ModesStr[MAX_MODES][MODE_NAME_LEN];
    const char* ModesStrPtrs[MAX_MODES];
    
    for(uint8_t i = 0; i < MAX_MODES; ++i)
    {
        strcpy(ModesStr[i], CurModel.Mode[i].name);
        ModesStrPtrs[i] = ModesStr[i];
    }

    int16_t result = FromToBox(ModesStrPtrs, ARRAY_SIZE(ModesStrPtrs), ModesStrPtrs, ARRAY_SIZE(ModesStrPtrs), "Mode Copy:", Font_8x8);

    if(result == RESULT_INCORRECT)
        return;
    
    int8_t From = int8_t(result >> 8);
    int8_t To   = int8_t(result & 0xFF);
    if(From != To)
    {
        MODE_Copy(From, To);
        MODEL_Init(Settings.ModelNum);
    }
}

void ModelSave(int8_t AddParam)
{
    MsgBox("Are you sure?", Font_8x12, "Model Save");

    WaitEmptyButtons();
    if(WaitButtonPress(B_ENTER | B_BACK) == B_ENTER)
    {
        Beep(1000, 100);
        _delay_ms(100);

        MsgBox("Please wait...", Font_8x12, "Model Save");
        MODEL_Save(Settings.ModelNum);
        
        Beep(1000, 100);
        _delay_ms(100);
    }
}

void ModelReset(int8_t AddParam)
{
    MsgBox("Are you sure?", Font_8x12, "Model Reset");

    
    WaitEmptyButtons();
    if(WaitButtonPress(B_ENTER | B_BACK) == B_ENTER)
    {
        Beep(1000, 100);
        _delay_ms(100);
        
        MsgBox("Please wait...", Font_8x12, "Model Reset");
        MODEL_Reset(Settings.ModelNum);
        Recalc_EEPROM_CRC();

        MODEL_Init(Settings.ModelNum);
        
        Beep(1000, 100);
        _delay_ms(100);
    }
}

char* ModelTypeGetStr(char* Buf, int8_t AddParam)
{
    const char* ModelTypes[] = {
            "Plane",
            "Heli"
        };
    
    sprintf(Buf, "%s", ModelTypes[CurModel.type]);
    return Buf;
}

void ModeTypeSet(int8_t AddParam)
{
    const char* ModelTypes[] = {
            "Plane",
            "Heli"
        };

    int8_t result = SelectBox(ModelTypes, ARRAY_SIZE(ModelTypes), CurModel.type, Font_8x12, "Model Type:", Font_8x8);

    if(result != RESULT_INCORRECT)
        CurModel.type = result;
}

char* ModulationTypeGetStr(char* Buf, int8_t AddParam)
{
    sprintf(Buf, "%s", GetCurrentModulationName());
    return Buf;
}

void ModulationTypeSet(int8_t AddParam)
{
    int8_t result = SelectBox(ModulationTypeName, ARRAY_SIZE(ModulationTypeName), CurModel.modulation, Font_8x12, "Modulation Type:", Font_8x8);

    if(result != RESULT_INCORRECT)
        UseModulation((ModulationType)result);
}

char* ModelChannelsNumGetStr(char* Buf, int8_t AddParam)
{
    sprintf(Buf, "%d", CurModel.num_ch);
    return Buf;
}

void ModelChannelsNumSet(int8_t AddParam)
{
    EditNumDlg(&CurModel.num_ch, PTR_INT8, 2, MAX_CHANNELS, 100, "Channels");
}

char* ModelNameGetStr(char* Buf, int8_t AddParam)
{
    sprintf(Buf, "%s", CurModel.name);
    return Buf;
}

void ModelNameSet(int8_t AddParam)
{
    EditStrDlg(CurModel.name, ARRAY_SIZE(CurModel.name) - 1, Font_8x12, "Model Name:", Font_8x8);
}

char* ModeNameGetStr(char* Buf, int8_t AddParam)
{
    sprintf(Buf, "%s", CurModel.Mode[FLY_MODE].name);
    return Buf;
}

void ModeNameSet(int8_t AddParam)
{
    EditStrDlg(CurModel.Mode[FLY_MODE].name, ARRAY_SIZE(CurModel.Mode[FLY_MODE].name) - 1, Font_8x12, "Mode Name:", Font_8x8);
}

void ModelSelect(int8_t AddParam)
{
    char        ModelsStr[MAX_MODELS][MODEL_NAME_LEN];
    const char* ModelsStrPtrs[MAX_MODELS];
    for(uint8_t i = 0; i < MAX_MODELS; ++i)
    {
        for(uint8_t j = 0; j < MODEL_NAME_LEN; ++j)
            ModelsStr[i][j] = eeprom_read_byte((uint8_t*)&EEPROM_MODEL[i].name[j]);
        ModelsStrPtrs[i] = ModelsStr[i];
    }

    int8_t result = SelectBox(ModelsStrPtrs, MAX_MODELS, Settings.ModelNum, Font_8x12, "Model:", Font_8x8);

    if(result == RESULT_INCORRECT)
        return;
    
    cli();
    
    Settings.ModelNum = result;
    TX_SaveSettings();
    MODEL_Init(Settings.ModelNum);
    
    sei();
}

void ModelMenu(int8_t AddParam)
{
    CMenu::Item ModelMenuItems[] =
    {
        {"Name",       ModelNameSet,        ModelNameGetStr,        0 },
        {"Type",       ModeTypeSet,         ModelTypeGetStr,        0 },
        {"Modulation", ModulationTypeSet,   ModulationTypeGetStr,   0 },
        {"Channels",   ModelChannelsNumSet, ModelChannelsNumGetStr, 0 },
        {"Mode Name",  ModeNameSet,         ModeNameGetStr,         0 },
        {"Save",       ModelSave,           NULL,                   0 },
        {"Select",     ModelSelect,         NULL,                   0 },
        {"Copy",       ModelCopy,           NULL,                   0 },
        {"Modes Copy", ModeCopy,            NULL,                   0 },
        {"Reset",      ModelReset,          NULL,                   0 }
    };
    
    CMenu Menu("Model", ModelMenuItems, ARRAY_SIZE(ModelMenuItems), Font_8x8);
    
    Menu.Run();
}

//
// Controls menu handlers
//

static int8_t CurrentControl;

char* ControlsReverseGetStr(char* Buf, int8_t AddParam)
{
    const char* str;
    
    if(CurModel.Mode[FLY_MODE].Control[CurrentControl].reverse == 1)
        str = "OFF";
    else
    if(CurModel.Mode[FLY_MODE].Control[CurrentControl].reverse == -1)
        str = "ON";
    else
        str = "ERROR";

    sprintf(Buf, str);
    return Buf;
}

void ControlsReverseSet(int8_t AddParam)
{
    const uint8_t Selected = CurModel.Mode[FLY_MODE].Control[CurrentControl].reverse == 1 ? 0 : 1;
    const char*   Strings[] = {"OFF", "ON"};

    int8_t result = SelectBox(Strings, ARRAY_SIZE(Strings), Selected, Font_8x12, "Reverse:", Font_8x8);
    
    if(result == RESULT_INCORRECT)
        return;
    
    CurModel.Mode[FLY_MODE].Control[CurrentControl].reverse =  result ? -1 : 1;
}


uint8_t* ControlsRateGetPointer(int8_t Control, uint8_t param)
{
    switch(param)
    {
        case CTRLS_MINR:
            return &CurModel.Mode[FLY_MODE].Control[Control].minRates;

        case CTRLS_MAXR:
            return &CurModel.Mode[FLY_MODE].Control[Control].maxRates;

        case CTRLS_MINDR:
        case CTRLS_TCUT:
            return &CurModel.Mode[FLY_MODE].Control[Control].minDRates;

        case CTRLS_MAXDR:
            return &CurModel.Mode[FLY_MODE].Control[Control].maxDRates;
    }

    return NULL;
}

char* ControlsRateGetStr(char* Buf, int8_t AddParam)
{
    uint8_t* val = ControlsRateGetPointer(CurrentControl, AddParam);

    if(val)
        sprintf(Buf, "%d", *val);
    else
        sprintf(Buf, "ERROR");
    
    return Buf;
}


void ControlsRateSet(int8_t AddParam)
{
    EditNumDlg(ControlsRateGetPointer(CurrentControl, AddParam), PTR_UINT8, 0, 150, 100, "Control:");
}

void ControlsCurveChange(int8_t AddParam)
{
    const uint8_t RECT_SIZE = 108;
    const uint8_t RECT_POSX = TFT_ScreenWidth / 2 - RECT_SIZE / 2;
    const uint8_t RECT_POSY = 2;

    int8_t* nodes = CurModel.Mode[FLY_MODE].Control[CurrentControl].nodes;
    
    int8_t tmp[CURVE_NODES];
    for(uint8_t i = 0; i < CURVE_NODES; ++i)
        tmp[i] = nodes[i];

    uint8_t Kbd = 0;
    uint8_t sel = 0;
    do
    {
        gfx_ClearBuf();
        gfx_DrawLine(RECT_POSX, RECT_POSY + RECT_SIZE / 2, RECT_POSX + RECT_SIZE, RECT_POSY + RECT_SIZE / 2, COLOR_FRONT, true);

        for(uint8_t i = 0; i < CURVE_NODES - 2; ++i)
        {
            const uint8_t x1 = RECT_POSX + (RECT_SIZE / (CURVE_NODES - 1) * int16_t(i+1));
            gfx_DrawLine(x1, RECT_POSY, x1, RECT_POSY + RECT_SIZE, COLOR_FRONT, true);
        }

        // Передов точек кривой в координаты на экране (Y)
        int8_t Y[CURVE_NODES];
        for(uint8_t i = 0; i < CURVE_NODES; ++i)
        {
            const uint8_t RECT_MIDY = RECT_POSY + RECT_SIZE / 2;
            Y[i] = RECT_MIDY - int16_t(RECT_SIZE / 2) * tmp[i] / 100;
        }

        // Рисуем кривую из отрезков
        for(uint8_t i = 0; i < CURVE_NODES - 1; ++i)
        {
            const uint8_t x1 = RECT_POSX + RECT_SIZE / (CURVE_NODES - 1) * int16_t(i);
            const uint8_t x2 = RECT_POSX + RECT_SIZE / (CURVE_NODES - 1) * int16_t(i + 1);
            gfx_DrawLine(x1, Y[i], x2, Y[i+1], COLOR_FRONT);
        }

        // Рисуем квадратик на выбранной точке
        const uint8_t x1 = RECT_POSX + RECT_SIZE / (CURVE_NODES - 1) * int16_t(sel);
        gfx_DrawRectangle(x1 - 2, Y[sel] - 2, x1 + 2, Y[sel] + 2, COLOR_FRONT);

        // Рисуем рамку вокруг области выбора
        gfx_DrawRectangle(RECT_POSX, RECT_POSY, RECT_POSX + RECT_SIZE, RECT_POSY + RECT_SIZE, COLOR_FRONT);

        char str[10];
        sprintf(str, "%d", tmp[sel]);
        
        const uint8_t     len  = strlen(str);
        const FontProfile font = Font_8x8;
        
        gfx_DrawLine(x1 + 2, Y[sel] + 2, x1 + 5, Y[sel] + 3, COLOR_FRONT);
        gfx_DrawRectangle(x1 + 5, Y[sel] + 3, x1 + 5 + font.W * len + 2, Y[sel] + 3 + font.H + 2, COLOR_BACKG);
        gfx_DrawRectangle(x1 + 6, Y[sel] + 4, x1 + 5 + font.W * len + 1, Y[sel] + 3 + font.H + 1, COLOR_FRONT);
        gfx_PrintString(x1 + 7, Y[sel] + 5, str, COLOR_FRONT, font);

        gfx_Refresh();

        Kbd = WaitButtonPress(B_ANY);
        WaitEmptyButtons();
        
        if((Kbd & B_LEFT) && (sel > 0))
            sel--;
        
        if((Kbd & B_RIGHT) && (sel < CURVE_NODES - 1))
            sel++;

        if (Kbd & (B_DOWN | B_UP | B_ENTER))
        {
            char str[12];
            sprintf(str, "Curve %d", sel + 1);
            EditNumDlg(&tmp[sel], PTR_INT8, -100, 100, 120, str);
        }
    }
    while(!(Kbd & B_BACK));

    MsgBox("Save changes?", Font_8x12, "Curve save");

    WaitEmptyButtons();
    if(WaitButtonPress(B_ENTER | B_BACK) == B_ENTER)
    {
        for(uint8_t i = 0; i < CURVE_NODES; ++i)
            nodes[i] = tmp[i];
    }
}

char* ControlsVirtualGetStr(char* Buf, int8_t AddParam);

void ControlsVirtualSet(int8_t AddParam);


void ControlsSettingsMenu(int8_t AddParam)
{
    // Запоминаем номер текущего управляющего элемента
    CurrentControl = AddParam;

    // Устанавливаем настройки в зависимости от настраеваемиго канала
    switch(AddParam)
    {
        case CTRL_AIL:
        case CTRL_ELE:
        case CTRL_RUD:
        {
            CMenu::Item ControlsMainSettings[] =
            {
                {"Reverse",       ControlsReverseSet,  ControlsReverseGetStr, CTRLS_REV   },
                {"Min Rate",      ControlsRateSet,     ControlsRateGetStr,    CTRLS_MINR  },
                {"Max Rate",      ControlsRateSet,     ControlsRateGetStr,    CTRLS_MAXR  },
                {"Min Dual Rate", ControlsRateSet,     ControlsRateGetStr,    CTRLS_MAXDR },
                {"Max Dual Rate", ControlsRateSet,     ControlsRateGetStr,    CTRLS_MINDR },
                {"Curve",         ControlsCurveChange, NULL,                  CTRLS_CURVE }
            };

            CMenu Menu("Controls", ControlsMainSettings, ARRAY_SIZE(ControlsMainSettings), Font_8x8);
            Menu.Run();
            break;
        }

        case CTRL_THR:
        {
            CMenu::Item ControlsThrottleSettings[] =
            {
                {"Reverse",  ControlsReverseSet,  ControlsReverseGetStr, CTRLS_REV   },
                {"Min Rate", ControlsRateSet,     ControlsRateGetStr,    CTRLS_MINR  },
                {"Max Rate", ControlsRateSet,     ControlsRateGetStr,    CTRLS_MAXR  },
                {"T. Cut",   ControlsRateSet,     ControlsRateGetStr,    CTRLS_TCUT  },
                {"Curve",    ControlsCurveChange, NULL,                  CTRLS_CURVE }
            };

            CMenu Menu("Controls", ControlsThrottleSettings, ARRAY_SIZE(ControlsThrottleSettings), Font_8x8);
            Menu.Run();
            break;
        }
            
        case CTRL_AUX1:
        {
            CMenu::Item ControlsAuxSettings[] =
            {
                {"Reverse",  ControlsReverseSet,  ControlsReverseGetStr, CTRLS_REV   },
                {"Min Rate", ControlsRateSet,     ControlsRateGetStr,    CTRLS_MINR  },
                {"Max Rate", ControlsRateSet,     ControlsRateGetStr,    CTRLS_MAXR  },
                {"Curve",    ControlsCurveChange, NULL,                  CTRLS_CURVE }
            };

            CMenu Menu("Controls", ControlsAuxSettings, ARRAY_SIZE(ControlsAuxSettings), Font_8x8);
            Menu.Run();
            break;
        }

        case CTRL_SW1:
        case CTRL_SW2:
        case CTRL_SW3:
        {
            CMenu::Item ControlsSwSettings[] =
            {
                {"Reverse",  ControlsReverseSet, ControlsReverseGetStr, CTRLS_REV  },
                {"Min Rate", ControlsRateSet,    ControlsRateGetStr,    CTRLS_MINR },
                {"Max Rate", ControlsRateSet,    ControlsRateGetStr,    CTRLS_MAXR }
            };

            CMenu Menu("Controls", ControlsSwSettings, ARRAY_SIZE(ControlsSwSettings), Font_8x8);
            Menu.Run();
            break;
        }

        case CTRL_V1:
        case CTRL_V2:
        {
            CMenu::Item ControlsVSettings[] =
            {
                {"Reverse",  ControlsReverseSet,  ControlsReverseGetStr, CTRLS_REV   },
                {"Min Rate", ControlsRateSet,     ControlsRateGetStr,    CTRLS_MINR  },
                {"Max Rate", ControlsRateSet,     ControlsRateGetStr,    CTRLS_MAXR  },
                {"Curve",    ControlsCurveChange, NULL,                  CTRLS_CURVE },
                {"From",     ControlsVirtualSet,  ControlsVirtualGetStr, CTRLS_FROM  }
            };

            CMenu Menu("Controls", ControlsVSettings, ARRAY_SIZE(ControlsVSettings), Font_8x8);
            Menu.Run();
            break;
        }
            
        default:
        {
            CMenu::Item ControlsMainSettings[] =
            {
                {"Reverse",       ControlsReverseSet,  ControlsReverseGetStr, CTRLS_REV   },
                {"Min Rate",      ControlsRateSet,     ControlsRateGetStr,    CTRLS_MINR  },
                {"Max Rate",      ControlsRateSet,     ControlsRateGetStr,    CTRLS_MAXR  },
                {"Min Dual Rate", ControlsRateSet,     ControlsRateGetStr,    CTRLS_MAXDR },
                {"Max Dual Rate", ControlsRateSet,     ControlsRateGetStr,    CTRLS_MINDR },
                {"Curve",         ControlsCurveChange, NULL,                  CTRLS_CURVE }
            };

            CMenu Menu("Controls", ControlsMainSettings, ARRAY_SIZE(ControlsMainSettings), Font_8x8);
            Menu.Run();
            break;
        }
    }
}

CMenu::Item ControlsMenuItems[10] =
{
    {"Ailerons",  ControlsSettingsMenu, NULL, CTRL_AIL  },
    {"Elevator",  ControlsSettingsMenu, NULL, CTRL_ELE  },
    {"Throttle",  ControlsSettingsMenu, NULL, CTRL_THR  },
    {"Rudder",    ControlsSettingsMenu, NULL, CTRL_RUD  },
    {"SW 1",      ControlsSettingsMenu, NULL, CTRL_SW1  },
    {"SW 2",      ControlsSettingsMenu, NULL, CTRL_SW2  },
    {"SW 3",      ControlsSettingsMenu, NULL, CTRL_SW3  },
    {"Aux 1",     ControlsSettingsMenu, NULL, CTRL_AUX1 },
    {"Virtual 1", ControlsSettingsMenu, NULL, CTRL_V1   },
    {"Virtual 2", ControlsSettingsMenu, NULL, CTRL_V2   }
};

char* ControlsVirtualGetStr(char* Buf, int8_t AddParam)
{
    sprintf(Buf, ControlsMenuItems[ CurModel.Mode[FLY_MODE].Control[CurrentControl].from ].Name);
    return Buf;
}

void ControlsVirtualSet(int8_t AddParam)
{
    const char* Strings[MAX_CONTROLS];
    for(uint8_t i = 0; i < MAX_CONTROLS; ++i)
        Strings[i] = ControlsMenuItems[i].Name;

    const uint8_t Selected = CurModel.Mode[FLY_MODE].Control[CurrentControl].from;

    int8_t result = SelectBox(Strings, CTRL_V1, Selected, Font_8x12, "From:", Font_8x8);
    
    if(result != RESULT_INCORRECT)
        CurModel.Mode[FLY_MODE].Control[CurrentControl].from = result;
}

void ControlsMenu(int8_t AddParam)
{
    CMenu Menu("Controls", ControlsMenuItems, ARRAY_SIZE(ControlsMenuItems), Font_8x8);
    Menu.Run();
}

// 
// Menu Mixer handlers
// 

static char MixerCurrentChannel;

void MixerChangeSetting(int8_t AddParam)
{
    EditNumDlg(&CurModel.Mode[FLY_MODE].Chanels[MixerCurrentChannel][AddParam], PTR_INT8, -100, 100, 120, "Mixer");
}

char* MixerGetStr(char* Buf, int8_t AddParam)
{
    sprintf(Buf, "%d", CurModel.Mode[FLY_MODE].Chanels[MixerCurrentChannel][AddParam]);
    return Buf;
}

void ChannelsMixerMenu(int8_t AddParam)
{
    CMenu::Item MixerMenuItems[] =
    {
        {"Ailerons",  MixerChangeSetting, MixerGetStr, CTRL_AIL},
        {"Elevator",  MixerChangeSetting, MixerGetStr, CTRL_ELE},
        {"Throttle",  MixerChangeSetting, MixerGetStr, CTRL_THR},
        {"Rudder",    MixerChangeSetting, MixerGetStr, CTRL_RUD},
        {"SW 1",      MixerChangeSetting, MixerGetStr, CTRL_SW1},
        {"SW 2",      MixerChangeSetting, MixerGetStr, CTRL_SW2},
        {"SW 3",      MixerChangeSetting, MixerGetStr, CTRL_SW3},
        {"Aux 1",     MixerChangeSetting, MixerGetStr, CTRL_AUX1},
        {"Virtual 1", MixerChangeSetting, MixerGetStr, CTRL_V1},
        {"Virtual 2", MixerChangeSetting, MixerGetStr, CTRL_V2},
        {"Trim",      MixerChangeSetting, MixerGetStr, CTRL_TRIM}
    };

    MixerCurrentChannel = AddParam;

    char Name[12];
    snprintf(Name, sizeof(Name), "CH %n Mixer", AddParam);

    CMenu Menu(Name, MixerMenuItems, ARRAY_SIZE(MixerMenuItems), Font_8x8);
    
    Menu.Run();
}

void ChannelsMenu(int8_t AddParam)
{
    static CMenu::Item ChannelsMenuItems[] =
    {
        {"CH 1", ChannelsMixerMenu, NULL, 0},
        {"CH 2", ChannelsMixerMenu, NULL, 1},
        {"CH 3", ChannelsMixerMenu, NULL, 2},
        {"CH 4", ChannelsMixerMenu, NULL, 3},
        {"CH 5", ChannelsMixerMenu, NULL, 4},
        {"CH 6", ChannelsMixerMenu, NULL, 5},
        {"CH 7", ChannelsMixerMenu, NULL, 6},
        {"CH 8", ChannelsMixerMenu, NULL, 7}
    };
    
    const uint8_t ChannelsCount = (ARRAY_SIZE(ChannelsMenuItems) < CurModel.num_ch) ? ARRAY_SIZE(ChannelsMenuItems) : CurModel.num_ch;

    CMenu Menu("Channels", ChannelsMenuItems, ChannelsCount, Font_8x8);
    
    Menu.Run();
}

//
// Menu Timer handlers
// 

char* TimerModeGetStr(char* Buf, int8_t AddParam)
{
    sprintf(Buf, (CurModel.timer_mode & 0x02) ? "DOWN" : "UP");
    return Buf;
}
// ***   Установка режима таймера   ********************************************
void TimerModeSet(int8_t AddParam)
{
    const char* Strings[2] = {"UP", "DOWN"};

    // Вызываем функцию выбора значмения
    int8_t result = SelectBox(Strings, ARRAY_SIZE(Strings), (CurModel.timer_mode & 0x02) ? 1 : 0, Font_8x12, "Timer Mode:", Font_8x8);

    if(result == RESULT_INCORRECT)
        return;
    
    if(result)
        CurModel.timer_mode |=  0x02;
    else
        CurModel.timer_mode &= ~0x02;
}

char* TimerSoundGetStr(char* Buf, int8_t AddParam)
{
    sprintf(Buf, (CurModel.timer_mode & 0x01) ? "OFF" : "ON");
    return Buf;
}

void TimerSoundSet(int8_t AddParam)
{
    const char* Strings[2] = {"OFF", "ON"};

    int8_t result = SelectBox(Strings, ARRAY_SIZE(Strings), (CurModel.timer_mode & 0x01) ? 0 : 1, Font_8x12, "Timer Sound:", Font_8x8);

    if(result == RESULT_INCORRECT)
        return;
        
    if(result)
        CurModel.timer_mode &= ~0x01;
    else
        CurModel.timer_mode |=  0x01;
}

void TimerMenu(int8_t AddParam)
{
    CMenu::Item TimerMenuItems[] =
    {
        {"Set",   NULL,          NULL,             0},
        {"Mode",  TimerModeSet,  TimerModeGetStr,  0},
        {"Sound", TimerSoundSet, TimerSoundGetStr, 0}
    };
    
    CMenu Menu("Timer", TimerMenuItems, ARRAY_SIZE(TimerMenuItems), Font_8x8);
    
    Menu.Run();
}

char* OptionsSoundGetStr(char* Buf, int8_t AddParam)
{
    sprintf(Buf, Settings.SoundFlag ? "ON" : "OFF");
    return Buf;
}

void OptionsSoundSet(int8_t AddParam)
{
    const char* Strings[2] = {"OFF", "ON"};

    int8_t result = SelectBox(Strings, ARRAY_SIZE(Strings), Settings.SoundFlag, Font_8x12, "Sound:", Font_8x8);

    if(result == RESULT_INCORRECT)
        return;

    Settings.SoundFlag = result;
    TX_SaveSettings();
}


void Calibration_AP(int8_t AddParam)
{
    Calibration();
}

void OptionsMenu(int8_t AddParam)
{
    CMenu::Item OptionsMenuItems[] =
    {
        {"Sound",       OptionsSoundSet,     OptionsSoundGetStr,     0},
        {"Calibration", Calibration_AP,      NULL,                   0}
    };
    
    CMenu Menu("Options", OptionsMenuItems, ARRAY_SIZE(OptionsMenuItems), Font_8x8);
    
    Menu.Run();
}

//
// Main menu handlers
//

void DummyMenu(int8_t AddParam)
{

}

void MainMenu()
{
    CMenu::Item MainMenuItems[] =
    {
        {"Model",    ModelMenu,    NULL, 0},
        {"Controls", ControlsMenu, NULL, 0},
        {"Channels", ChannelsMenu, NULL, 0},
        {"Timer",    TimerMenu,    NULL, 0},
        {"Options",  OptionsMenu,  NULL, 0}
    };
    
    CMenu Menu("Main", MainMenuItems, ARRAY_SIZE(MainMenuItems), Font_8x8);
    
    Menu.Run();
}
