/*******************************************************************************
*  UI_Engine.cpp
*
*******************************************************************************/
#include <avr/io.h>
#include <stdio.h>
#include <string.h>

#include "def.h"
#include "UI_Engine.h"
#include "Tasks.h"
#include "Global_Variables.h"

//
// CString
//

CString::CString(const char* Str, FontProfile& Font, ColorIdx Color, uint8_t VSpace)
{
    m_Str     = Str;
    m_Len     = strlen(Str);
    m_StrFont = &Font;
    m_Color   = Color;
    m_VSpace  = VSpace;
}

inline uint8_t CString::GetWidth() const
{
    return m_StrFont->W * m_Len;
}
inline uint8_t CString::GetHeight() const
{
    return m_StrFont->H;
}
inline uint8_t CString::GetLength() const
{
    return m_Len;
}

inline uint8_t CString::GetVSpace() const
{
    return m_VSpace;
}

inline void CString::SetColor(const ColorIdx Color)
{
    m_Color = Color;
}

inline void CString::PrintString(const uint8_t x, const uint8_t y) const
{
    gfx_PrintString(x, y, m_Str, m_Color, *m_StrFont);
}

//
// CMenu
//

const uint8_t CMenu::ScrollBarWidth = 3;

CMenu::CMenu(const char* Caption, const Item* Items, const uint8_t ItemsCount, const FontProfile& ItemFont):
    m_Caption(Caption),
    m_Items(Items),
    m_ItemsCount(ItemsCount),
    m_ItemFont(ItemFont),
    m_CurrentPos(0)
{
}

const uint8_t CMenu::GetCaptionHeight() const
{
    return Font_8x12.H;
}
uint8_t CMenu::GetItemsCount() const
{
    return m_ItemsCount;
}
const CMenu::Item* CMenu::GetItem(const uint8_t _ItemIdx) const
{
    return &m_Items[_ItemIdx];
}

const FontProfile& CMenu::GetItemFont() const
{
    return m_ItemFont;
}

uint8_t CMenu::GetCurrentPos() const
{
    return m_CurrentPos;
}
void CMenu::SetCurrentPos(uint8_t CurrentPos)
{
    m_CurrentPos = CurrentPos;
}

void CMenu::Run()
{
    uint8_t Kbd = 0;
    do
    {
        uint8_t StartPos = 0;
        do
        {
            DrawMenu(StartPos);

            WaitEmptyButtons();
            
            Kbd = WaitButtonPress(B_ANY);

            if((Kbd & B_UP) && (GetCurrentPos() > 0))
            {
                SetCurrentPos(GetCurrentPos() - 1);
            }
            if((Kbd & B_DOWN) && (GetCurrentPos() < GetItemsCount() - 1))
            {
                SetCurrentPos(GetCurrentPos() + 1);
            }

            // Если курсор вышел за границы экрана - сдигаем меню на один пункт
            if(GetCurrentPos() < StartPos)
                StartPos--;
            if(GetCurrentPos() > StartPos + GetMenuVisibleCount() - 1)
                StartPos++;
        }
        while(!(Kbd & (B_BACK | B_ENTER)));

        // Если нажали ВВОД и у выбранного пункта установлен обработчик
        if((Kbd & B_ENTER) && (GetItem(GetCurrentPos())->OnEnter != NULL))
        {
            WaitEmptyButtons();
            
            GetItem(GetCurrentPos())->OnEnter(GetItem(GetCurrentPos())->AddParam);
        }
    }
    while(!(Kbd & B_BACK));

    // Ждем отпускания кнопок
    WaitEmptyButtons();
}

void CMenu::DrawMenu(const uint8_t StartPos) const
{
    gfx_ClearBuf();

    DrawHeader();

    DrawStatusBar();
    DrawScrollBar();

    // Draw items
    for(uint8_t i = StartPos; i < StartPos + GetMenuVisibleCount(); ++i)
    {
        const Item*    item  = GetItem(i);
        const uint8_t  x1    = ScrollBarWidth + 2;
        const uint8_t  x2    = TFT_ScreenWidth - 2;
        const uint8_t  y     = GetHeaderHeight() + 2 + GetItemFont().H * (i - StartPos);
        const ColorIdx Color = (GetCurrentPos() == i) ? COLOR_BACKG : COLOR_FRONT;

        if (GetCurrentPos() == i)
        {
            gfx_FillRectangle(x1, y, x2, y + GetItemFont().H, COLOR_FRONT);
            gfx_PrintChar(x1, y, '>', Color, GetItemFont());
        }
        
        gfx_PrintString(GetItemFont().W + x1, y, item->Name, Color, GetItemFont(), ALIGN_LEFT);
        if(item->GetInfo != NULL)
        {
            char buf[32];
            item->GetInfo(buf, item->AddParam);

            gfx_PrintString(x2, y, buf, Color, GetItemFont(), ALIGN_RIGHT);
        }
    }
    
    gfx_Refresh();
}

void CMenu::DrawStatusBar() const
{
    const FontProfile& Font = GetStatusBarFont();
    static char str[10];

    gfx_FillRectangle(0, 0, TFT_ScreenWidth - 1, Font.H, COLOR_BACKG);

    // Номер модели
    sprintf(str, "%d", Settings.ModelNum + 1);
    gfx_PrintString(0, 0, str, COLOR_FRONT, Font);
    
    if(CurModel.type == TYPE_HELI)
    {
        sprintf(str, "%c%c", 125, 126); // Значек верта
        gfx_PrintString(Font.W * 2, 0, str, COLOR_FRONT, Font);
    }
    else
    if(CurModel.type == TYPE_PLANE)
    {
        sprintf(str, "%c%c", 127, 128); // Значек самолета
        gfx_PrintString(Font.W * 2, 0, str, COLOR_FRONT, Font);
    }
    
    gfx_PrintString(TFT_ScreenWidth / 2, 0, CurModel.name, COLOR_FRONT, Font, ALIGN_CENTER);

    // Название режима
    gfx_PrintString(TFT_ScreenWidth, 0, CurModel.Mode[FLY_MODE].name, COLOR_FRONT, Font_6x8, ALIGN_RIGHT);
}

void CMenu::DrawHeader() const
{
    const FontProfile & Font = Font_8x12;

    gfx_FillRectangle(0, GetStatusBarHeight(), TFT_ScreenWidth - 1, GetStatusBarHeight() + Font.H, COLOR_FRONT);
    gfx_PrintString(TFT_ScreenWidth / 2, GetStatusBarHeight(), m_Caption, COLOR_BACKG, Font, ALIGN_CENTER);
}

void CMenu::DrawScrollBar() const
{
    const uint8_t HeaderHeight     = GetHeaderHeight();
    const uint8_t ScrollAreaHeight = TFT_ScreenHeight - (HeaderHeight + 2 + 2);

    // Высота индикатора полосы прокрутки:
    // (количество отображаемых пунктов меню * количество доступных точек) / количество пунктов меню
    uint8_t ScrollBarHeight = (uint16_t(GetMenuVisibleCount()) * ScrollAreaHeight) / GetItemsCount();
    // Если размер индикатора больше половины полосы прокрутки - приравниваем к половине
    if(ScrollBarHeight > ScrollAreaHeight / 2)
         ScrollBarHeight = ScrollAreaHeight / 2;

    // высота заголовка меню + (текущяя позицию в меню * количество доступных точек для индикатора) / количество пунктов меню
    uint8_t ScrollBarStart = HeaderHeight + 2 + (GetCurrentPos() * uint16_t(ScrollAreaHeight - ScrollBarHeight)) / (GetItemsCount() - 1);
    if(GetCurrentPos() == GetItemsCount() - 1)
    {
        // Если выбран последний пункт меню устанавливаем индикатор в самый низ, т.к. при подсчете может потерятся точность и появится дырка
        ScrollBarStart = ScrollAreaHeight - ScrollBarHeight + HeaderHeight + 2;
    }

    gfx_FillRectangle(0, HeaderHeight + 2, ScrollBarWidth - 1, TFT_ScreenHeight - 1,             COLOR_FRONT); // Очищаем место под полосу прокрутки
    gfx_DrawRectangle(0, HeaderHeight + 2, ScrollBarWidth - 1, TFT_ScreenHeight - 1,             COLOR_BACKG); // Отрисовываем полосу прокрутки
    gfx_FillRectangle(0, ScrollBarStart,   ScrollBarWidth - 1, ScrollBarStart + ScrollBarHeight, COLOR_FRONT); // Отрисовываем индикатор полосы прокрутки
}

inline const FontProfile& CMenu::GetStatusBarFont() const
{
    return Font_8x8;
}
inline uint8_t CMenu::GetStatusBarHeight() const
{
    return GetStatusBarFont().H;
}

inline uint8_t CMenu::GetHeaderHeight() const
{
    return GetStatusBarHeight() + GetCaptionHeight();
}

inline uint8_t CMenu::GetMenuVisibleCount() const
{
    uint8_t MenuVisibleCount = (TFT_ScreenHeight - GetHeaderHeight() - 2) / GetItemFont().H;
    if(MenuVisibleCount > GetItemsCount())
        MenuVisibleCount = GetItemsCount();

    return MenuVisibleCount;
}

//
// Public function
//

void MsgBox(const char*        Body,
            const FontProfile& BodyFont,
            const char*        Header,
            const FontProfile& HeaderFont,
            const uint8_t      WidthMin,
            const ColorIdx     Color)
{
    const uint8_t MSG_BOX_MAX_LINES = 5;

    char*   Line[MSG_BOX_MAX_LINES];
    uint8_t LineLength[MSG_BOX_MAX_LINES];
    uint8_t LineCount = 0;

    char Str[100];
    strcpy(Str, Body);

    uint8_t H = 0;
    uint8_t W = WidthMin;

    Line[0] = Str;
    // Clip Str to Lines and calc W and H
    for (uint8_t count = 0; count < MSG_BOX_MAX_LINES; ++count)
    {
        char* p = strchr(Line[count], '\n');
        
        LineLength[count] = p ? (p - Line[count]) : strlen(Line[count]);

        const uint8_t StrW = BodyFont.W * LineLength[count];
        if(W < StrW)
            W = StrW;
        H += BodyFont.H;

        if(p == NULL)
        {
            LineCount = count;
            break;
        }
        else
            *p = '\0';

        Line[count + 1] = p + 1;
    }

    if(Header != NULL)
    {
        const uint8_t StrW = HeaderFont.W * strlen(Header);
        if(W < StrW)
            W = StrW;
    }

    // Additional MsgBox padding
    W += BodyFont.W * 2;
    H += BodyFont.H;

    uint8_t X = TFT_ScreenWidth  / 2 - W / 2;
    uint8_t Y = TFT_ScreenHeight / 2 - H / 2;

    if(Header != NULL)
    {
        // Draw header
        Y += HeaderFont.H / 2;
        gfx_DrawRectangle(X - 1, Y - HeaderFont.H - 2, X + W + 1, Y + 1, COLOR_FRONT);
        gfx_FillRectangle(X, Y - HeaderFont.H - 1, X + W, Y, COLOR_BACKG);
        gfx_PrintString(X + 1, Y - HeaderFont.H, Header, COLOR_FRONT, HeaderFont);
    }

    // Clear MsgBox area
    gfx_FillRectangle(X - 1, Y, X + W + 1, Y + H + 1, COLOR_FRONT);
    gfx_DrawRectangle(X, Y, X + W, Y + H, COLOR_BACKG);

    // Draw Lines
    for(uint8_t i = 0; i < LineCount + 1; ++i)
    {
        if(LineLength[i] == 0)
            continue;
        
        const uint8_t x = (TFT_ScreenWidth - BodyFont.W * LineLength[i]) / 2;
        const uint8_t y = Y + BodyFont.H/2 + i * BodyFont.H;
        
        gfx_PrintString(x, y, Line[i], Color, BodyFont);
    }

    gfx_Refresh();
}

int8_t SelectBox(const char** Strings, const uint8_t StrCount, uint8_t Selected, const FontProfile& BodyFont,
                 const char*  Header,  const FontProfile& HeaderFont)
{
    const uint8_t PADDING_H = BodyFont.W * 2;
    const uint8_t PADDING_V = BodyFont.H / 2;

    uint8_t StrVisibleCount = (TFT_ScreenHeight - HeaderFont.H - PADDING_V) / BodyFont.H;

    uint8_t W = 0;
    uint8_t H = 0;
    for(uint8_t i = 0; i < StrCount; ++i)
    {
        const uint8_t StrW = BodyFont.W * strlen(Strings[i]);
        
        if(W < StrW)
            W = StrW;
        
        if(i < StrVisibleCount) H += BodyFont.H;
    }

    if (StrVisibleCount > StrCount)
        StrVisibleCount = StrCount;

    if(Header != NULL)
    {
        const uint8_t StrW = HeaderFont.W * strlen(Header);
        if(W < StrW)
            W = StrW;
    }

    // Additional padding
    W += PADDING_H;
    H += PADDING_V;

    uint8_t X = (TFT_ScreenWidth  - W) / 2;
    uint8_t Y = (TFT_ScreenHeight - H) / 2;

    if(Header != NULL)
    {
        // Draw header
        Y += HeaderFont.H / 2;
        gfx_DrawRectangle(X - 1, Y - HeaderFont.H - 2, X + W + 1, Y + 1, COLOR_BACKG);
        gfx_FillRectangle(X, Y - HeaderFont.H - 1, X + W, Y, COLOR_FRONT);
        gfx_PrintString(X + 1, Y - HeaderFont.H, Header, COLOR_BACKG, HeaderFont);
    }

    uint8_t Pos = 0;
    if(Selected > Pos + StrVisibleCount - 1)
        Pos = Selected + 1 - StrVisibleCount;

    uint8_t Kbd = 0;
    do
    {
        // Clear Msg area
        gfx_FillRectangle(X - 1, Y, X + W + 1, Y + H + 1, COLOR_BACKG);
        gfx_DrawRectangle(X, Y, X + W, Y + H, COLOR_FRONT);

        // Draw Strings
        for(uint8_t i = Pos; i < Pos + StrVisibleCount; ++i)
        {
            const uint8_t x = (TFT_ScreenWidth - BodyFont.W * strlen(Strings[i])) / 2;
            const uint8_t y = Y + BodyFont.H / 4 + (i - Pos) * BodyFont.H;

            if (i == Selected)
            {
                gfx_FillRectangle(X + 2, y, X + W - 4, y + BodyFont.H, COLOR_FRONT);
                gfx_PrintString(x, y, Strings[i], COLOR_BACKG, BodyFont);
            }
            else
            {
                gfx_PrintString(x, y, Strings[i], COLOR_FRONT, BodyFont);
            }
        }

        gfx_Refresh();

        WaitEmptyButtons(100);
        
        Kbd = WaitButtonPress(B_ANY);

        if((Kbd & B_UP) && (Selected > 0))
        {
            Selected--;
        }
        if((Kbd & B_DOWN) && (Selected < StrCount - 1))
        {
            Selected++;
        }

        if(Selected > Pos + StrVisibleCount - 1)
            Pos++;
        if(Selected < Pos)
            Pos--;
    }
    while(!(Kbd & (B_BACK | B_ENTER)));

    if(Kbd & B_ENTER)
        return Selected;
    
    return RESULT_INCORRECT;
}

bool EditNumDlg(void* Val, const PointerType PtrType, const int16_t Min, const int16_t Max, const uint8_t WidthMin, const char* Header)
{
    if(Val == NULL)
        return false;

    int16_t BackUp;
    switch(PtrType)
    {
        case (PTR_INT8):  { BackUp = *((int8_t*)Val);  break; }
        case (PTR_UINT8): { BackUp = *((uint8_t*)Val); break; }
        case (PTR_INT16): { BackUp = *((int16_t*)Val); break; }

        default:        
            return false;
    }

    int16_t CurVal = BackUp;
    uint8_t Kbd = 0;
    char    str[10];
    do
    {
        sprintf(str, "%d", CurVal);
        
        MsgBox(str, Font_12x16, Header, Font_8x8, WidthMin, COLOR_BACKG);
        
        WaitEmptyButtons();
        Kbd = WaitButtonPress(B_ANY);
        
        if((Kbd & B_DOWN) && (CurVal > Min))
            CurVal--;
        
        if((Kbd & B_UP) && (CurVal < Max))
            CurVal++;
        
        if(Kbd & B_LEFT)
            CurVal = (CurVal - 10 > Min) ? (CurVal - 10) : Min;
        
        if(Kbd & B_RIGHT)
            CurVal = (CurVal + 10 < Max) ? (CurVal + 10) : Max;
        

        // Изменяем значение по указателю потому, что от него может что-либо зависеть.
        // Например при изменении сабтриммера можно будет визуально наблюдать результат
        // не выходя из диалога изменения значения.
        switch(PtrType)
        {
            case (PTR_INT8):  { *((int8_t*)Val)  = CurVal; break; }
            case (PTR_UINT8): { *((uint8_t*)Val) = CurVal; break; }
            case (PTR_INT16): { *((int16_t*)Val) = CurVal; break; }
        }

    } while((Kbd != B_ENTER) && (Kbd != B_BACK));

    if(Kbd == B_ENTER)
        return BackUp != CurVal;
    else
    {
        // Restore Val
        switch(PtrType)
        {
            case (PTR_INT8):  { *((int8_t*)Val)  = BackUp; break; }
            case (PTR_UINT8): { *((uint8_t*)Val) = BackUp; break; }
            case (PTR_INT16): { *((int16_t*)Val) = BackUp; break; }
        }

        return false;
    }
}

static const char EditStrDlgTable[] =
{
    ' ','_','-','0','1','2','3','4','5','6','7','8','9','A','a','B','b','C','c','D',
    'd','E','e','F','f','G','g','H','h','I','i','J','j','K','k','L','l','M','m','N',
    'n','O','o','P','p','Q','q','R','r','S','s','T','t','U','u','V','v','W','w','X',
    'x','Y','y','Z','z'
};


bool EditStrDlg(char* Str, uint8_t StrLen, const FontProfile& StrFont, const char* Header, const FontProfile& HeaderFont)
{
    const uint8_t  STR_LEN_MAX = 15;
    const uint8_t  CHAR_INDENT = 1;
    const uint16_t BUTTON_SENSIVITY = 100;

    uint8_t StrTableIndex[STR_LEN_MAX + 1] = {0};

    if(StrLen > STR_LEN_MAX)
        StrLen = STR_LEN_MAX;

    // Перевод из строки в указатели позиции в таблице
    for(uint8_t i = 0; i < StrLen; ++i)
    {
        for(uint8_t j = 0; j < ARRAY_SIZE(EditStrDlgTable); ++j)
        {
            if(Str[i] == EditStrDlgTable[j])
            {
                StrTableIndex[i] = j;
                break;
            }
        }
    }

    // Set width and height of window
    uint8_t W = (StrFont.W + CHAR_INDENT) * StrLen;
    uint8_t H = StrFont.H;

    if(Header != NULL)
    {
        const uint8_t HeaderWidth = HeaderFont.W * strlen(Header);
        if(W < HeaderWidth)
            W = HeaderWidth;
    }

    // Additional window padding
    W += StrFont.W * 2;
    H += StrFont.H;

    uint8_t X = (TFT_ScreenWidth - W)  / 2;
    uint8_t Y = (TFT_ScreenHeight - H) / 2;

    // Render Header
    if(Header != NULL)
    {
        Y += HeaderFont.H / 2;                                                         // Сдвигаем окно вниз на половину величины заголовка
        gfx_DrawRectangle(X - 1, Y - HeaderFont.H - 2, X + W + 1, Y + 1, COLOR_BACKG); // Очищаем рамку вокруг заголовка
        gfx_FillRectangle(X, Y - HeaderFont.H - 1, X + W, Y, COLOR_BACKG);             // Очищаем место под заголовок
        gfx_PrintString(X + 1, Y - HeaderFont.H, Header, COLOR_FRONT, HeaderFont);     // Выводим заголовок
    }

    uint8_t Kbd = 0;
    uint8_t Sel = 0;
    do
    {   
        // Clear rect and draw border
        gfx_FillRectangle(X - 1, Y, X + W + 1, Y + H + 1, COLOR_FRONT);
        gfx_DrawRectangle(X, Y, X + W, Y + H, COLOR_BACKG);
        
        const uint8_t x = (TFT_ScreenWidth - (StrFont.W + CHAR_INDENT) * StrLen) / 2;

        for(uint8_t i = 0; i < StrLen; ++i)
        {
            gfx_PrintChar(
                    x + i * (StrFont.W + CHAR_INDENT),
                    Y + StrFont.H / 2,
                    EditStrDlgTable[StrTableIndex[i]],
                    COLOR_BACKG,
                    StrFont
                );
        }

        gfx_DrawRectangle(x + Sel * (StrFont.W + 1) - 2,          Y + StrFont.H/2 - 1,
                          x + Sel * (StrFont.W + 1) + StrFont.W, Y + StrFont.H/2 + StrFont.H, COLOR_BACKG);

        gfx_Refresh();

        Kbd = WaitButtonPress(B_ANY);

        if((Kbd == B_LEFT) && (Sel > 0))
            Sel--;
        if((Kbd == B_RIGHT) && (Sel < StrLen - 1))
            Sel++;

        if((Kbd == B_UP) && (StrTableIndex[Sel] > 0))
        {
            StrTableIndex[Sel]--;
        }
        if((Kbd == B_DOWN) && (StrTableIndex[Sel] < ARRAY_SIZE(EditStrDlgTable) - 1))
        {
            StrTableIndex[Sel]++;
        }

        WaitEmptyButtons(BUTTON_SENSIVITY);
    }
    while(!(Kbd & (B_BACK | B_ENTER)));

    if(Kbd & B_ENTER)
    {
        // Return entered string
        for(uint8_t i = 0; i < StrLen; i++)
            Str[i] = EditStrDlgTable[StrTableIndex[i]];
        
        Str[StrLen] = '\0';
        
        for(uint8_t i = StrLen - 1; i >= 0; i--)
        {
            if(Str[i] == ' ')
              Str[i] = '\0';
            else
              break;
        }
        
        return true;
    }
    
    return false;
}

// Service function
void MsgBoxStrEx(const CString* Strings, const uint8_t StrCount, const char* Header, const FontProfile& HeaderFont, uint8_t Color)
{
    const uint8_t MSG_BOX_PADDING = 10;

    uint8_t W = 0, H = 0;
    for(uint8_t i = 0; i < StrCount; ++i)
    {
        const uint8_t StrW = Strings[i].GetWidth();
        if(W < StrW)
            W = StrW;
        
        H += Strings[i].GetHeight() + Strings[i].GetVSpace();
    }

    // Check Header width
    if(Header != NULL)
    {
        // Вычисляем ширину заголовка в пикселях
        const uint8_t StrW = HeaderFont.W * strlen(Header);
        if(W < StrW)
            W = StrW;
    }

    H += MSG_BOX_PADDING * 2;
    W += MSG_BOX_PADDING * 2;

    uint8_t X = (TFT_ScreenWidth - W) / 2;
    uint8_t Y = (TFT_ScreenHeight - H) / 2;

    // Draw Header
    if(Header != NULL)
    {
        Y += HeaderFont.H / 2;                                                         // Сдвигаем окно вниз на половину величины заголовка
        gfx_DrawRectangle(X - 1, Y - HeaderFont.H - 2, X + W + 1, Y + 1, COLOR_FRONT); // Очищаем рамку вокруг заголовка
        gfx_FillRectangle(X, Y - HeaderFont.H - 1, X + W, Y, COLOR_BACKG);             // Очищаем место под заголовок
        gfx_PrintString(X + 1, Y - HeaderFont.H, Header, COLOR_FRONT, HeaderFont);     // Выводим заголовок
    }

    // Clear rect
    gfx_FillRectangle(X - 1, Y, X + W + 1, Y + H + 1, COLOR_FRONT);
    gfx_DrawRectangle(X, Y, X + W, Y + H, COLOR_BACKG);

    // Render strings
    uint8_t y = Y + Strings[0].GetHeight() / 2;
    for(uint8_t i = 0; i < StrCount; ++i)
    {
        const uint8_t x = (TFT_ScreenWidth / 2 - Strings[i].GetWidth()) / 2;
        
        Strings[i].PrintString(x, y);

        y += Strings[i].GetHeight() + Strings[i].GetVSpace();
    }

    gfx_Refresh();
}


// Result: hight byte From, low byte - To
int16_t FromToBox(const char** FromBody, const uint8_t FromN,
                  const char** ToBody,   const uint8_t ToN,
                  const char* Header,    const FontProfile& HeaderFont)
{
    char FromStr[16], ToStr[16];

    CString Strings[] = {
            CString("From",  Font_6x8, COLOR_BACKG, 3),
            CString(FromStr, Font_8x8, COLOR_BACKG, 3),
            CString("To:",   Font_6x8, COLOR_BACKG, 3),
            CString(ToStr,   Font_8x8, COLOR_BACKG, 3)
        };

    uint8_t Kbd = 0;
    uint8_t From = 0, To = 0, pos = 0;
    do
    {
        sprintf(FromStr, pos == 0 ? "> %s" : "%s", FromBody[From]);
        sprintf(ToStr,   pos == 0 ? "%s" : "> %s", ToBody[To]);

        MsgBoxStrEx(Strings, 4, Header, HeaderFont, COLOR_BACKG);

        WaitEmptyButtons();
        Kbd = WaitButtonPress(B_ANY);

        if((Kbd == B_DOWN) && (pos < 1)) pos++;
        if((Kbd == B_UP)   && (pos > 0)) pos--;
        if(Kbd == B_LEFT)
        {
            if((pos == 0) && (From > 0)) From--;
            if((pos == 1) &&   (To > 0))   To--;
        }
        if(Kbd == B_RIGHT)
        {
            if((pos == 0) && (From < FromN - 1)) From++;
            if((pos == 1) &&   (To <   ToN - 1))   To++;
        }
    } while((Kbd != B_ENTER) && (Kbd != B_BACK));

    // Если нажали ВВОД - сохраняем введённое значение
    if(Kbd == B_ENTER)
        return ((int16_t)From) << 8 | To;
    
    return -1;
}