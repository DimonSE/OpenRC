/*******************************************************************************
* UI_Engine.h
*
*
*
********************************************************************************/

#ifndef __UI_Engine_h__
#define __UI_Engine_h__

#include "Graphic.h"

//
// Class definitions
//

class CString
{
public:
    CString(const char* Str, FontProfile& Font = Font_8x8, ColorIdx Color = COLOR_FRONT, uint8_t VSpace = 0);

    uint8_t GetWidth() const;
    uint8_t GetHeight() const;
    uint8_t GetLength() const;
    uint8_t GetVSpace() const;

    void SetColor(const ColorIdx Color);

    void PrintString(const uint8_t x, const uint8_t y) const;

private:
    const char*  m_Str;     // String pointer
    uint8_t      m_Len;     // String length
    FontProfile* m_StrFont; // String font
    ColorIdx     m_Color;   // String color
    uint8_t      m_VSpace;  // Text vertical indent
};

class CMenu
{
public:
    struct Item
    {
        typedef void  (*OnEnterFunc)(int8_t AddParam);
        typedef char* (*GetInfoFunc)(char* Buf, int8_t AddParam);

        const char* Name;          // Название пункта меню
        const OnEnterFunc OnEnter; // Функция вызываемая при нажатии ВВОД
        const GetInfoFunc GetInfo; // Функция вывода дополнительной информации
        const int8_t AddParam;     // Дополнительный параметр, передаваемый в функции
    };

    CMenu(const char* Caption, const Item* Items, const uint8_t ItemsCount, const FontProfile& ItemFont);

    const uint8_t  GetCaptionHeight() const;
    uint8_t        GetItemsCount()    const;
    const Item*    GetItem(const uint8_t _ItemIdx) const;

    const FontProfile& GetItemFont() const;

    uint8_t GetCurrentPos() const;
    void    SetCurrentPos(uint8_t CurrentPos);

    void Run();

private:
    void DrawMenu(const uint8_t StartPos) const;

    void DrawStatusBar() const;
    void DrawHeader()    const;
    void DrawScrollBar() const;
    const FontProfile& GetStatusBarFont() const;
    uint8_t GetStatusBarHeight() const;
    uint8_t GetHeaderHeight() const;
    uint8_t GetMenuVisibleCount() const;

public:

    static const uint8_t ScrollBarWidth;

private:
    const char*        m_Caption;    // Название меню, выводимое в заголовке
    const Item*        m_Items;      // Указатель на массив описания пунктов
    const uint8_t      m_ItemsCount; // Количество пунктов меню
    const FontProfile& m_ItemFont;   // Шрифт пукта меню
    uint8_t            m_CurrentPos; // Стартовая позиция
};

enum PointerType
{
    PTR_INT8 = 0,
    PTR_UINT8,
    PTR_INT16,
    PTR_UINT16
};

//
// Public functions
//

void MsgBox(const char*        Body,
            const FontProfile& BodyFont   = Font_8x12,
            const char*        Header     = NULL,
            const FontProfile& HeaderFont = Font_8x8,
            const uint8_t      WidthMin   = 0,
            const ColorIdx     Color      = COLOR_BACKG);

int8_t SelectBox(const char** Strings, const uint8_t StrCount, uint8_t Selected, const FontProfile& BodyFont,
                 const char*  Header,  const FontProfile& HeaderFont);

// Result: hight byte From, low byte - To
int16_t FromToBox(const char** FromBody, const uint8_t FromN,
                  const char** ToBody,   const uint8_t ToN,
                  const char* Header,    const FontProfile& HeaderFont);

bool EditStrDlg(char* Str, uint8_t StrLen, const FontProfile& StrFont, const char* Header, const FontProfile& HeaderFont);

bool EditNumDlg(void* Val, const PointerType PtrType, const int16_t Min, const int16_t Max, const uint8_t WidthMin, const char* Header);

#endif // __UI_Engine_h__