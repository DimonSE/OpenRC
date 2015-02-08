/*******************************************************************************
*  Hardware.cpp
*
*
*******************************************************************************/

#include <avr/io.h>

#include "../def.h"
#include "Hardware.h"

#include <util/delay.h>


void hwr_InitPorts()
{
    // PORTA use for menu buttons
    PORTA = 0xFF;
    DDRA  = 0x00;

    // PORTB.0, PORTB.1, PORTB.2 and PORTB.3 use by SPI in TFT display, Also use for radio OCR1A
    PORTB = 0xFF;
    DDRB  = 0xFF;

    // PORTL.0, PORTL.1 and PORTL.2 use as GPIO in TFT display, other pins for trimmers and more
    PORTL = 0xFF;
    DDRL  = 0x07;

    // PORTG.0, PORTG.1, PORTG.2 for trimmers
    PORTG = 0xFF;
    DDRG  = 0xF8;

    // PORTD.7 for trimmers
    PORTD = 0xFF;
    DDRD  = 0xBF;

    // PORTC For other keys (SW1, SW2, SW3, TCUT and other)
    PORTC = 0xFF;
    DDRC  = 0x00;

    // PORTF use in ADC
    PORTF = 0x00;
    DDRF  = 0x00;

    PORTH = 0xFF;
    DDRH  = 0xFF;
}

void hwr_InitTimers()
{
    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: CLK/8 = 2 MHz
    // Mode: Fast PWM, top = ICR1
    // OC1A output: Non-Inv.
    // OC1B output: Discon.
    // OC1C output: Discon.
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer 1 Overflow Interrupt: Off
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: On
    // Compare B Match Interrupt: Off
    // Compare C Match Interrupt: Off
    TCCR1A = (1 << WGM11) | (0 << WGM10);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (0 << CS10);
    TIMSK1 = (1 << OCIE1A);
    ICR1   = 0x1770;
    OCR1A  = 0x0268; // F_CPU / 8 * 616 = 0.308 mSec, pause between channels PWM

    hwr_SetModulation(DEFAULT_MODULATION);

    // Timer/Counter 3 initialization

    TCCR3B |= (1 << WGM32);                // Configure timer 3 for CTC mode
    TIMSK3 |= (1 << OCIE3A);               // Enable CTC interrupt
    OCR3A   = F_CPU / 64 / 50;             // Set CTC compare value to 50Hz at 16MHz AVR clock, with a prescaler of 64
    TCCR3B |= ((1 << CS30) | (1 << CS31)); // Start timer at Fcpu/64  
}

void hwr_InitUSART0(uint32_t BaudRate)
{
    // USART initialization
    // with baud rate = 9600bps
    const uint16_t ubrr_value = F_CPU / 16 / BaudRate - 1;
    UBRR0L = ubrr_value;
    UBRR0H = ubrr_value >> 8;

    // Set Frame Format:
    // Asynchronous mode, No Parity, 1 StopBit, char size 8
    UCSR0C = (3 << UCSZ00);

    //Enable The receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

const uint8_t ADC_VREF_TYPE = 0x40; // ADC Voltage Reference: AVCC with external capacitor at AREF pin

void hwr_InitADC()
{
    // ADC initialization
    // ADC Clock frequency: 250 kHz
    // ADC Voltage Reference: Internal 1.1V Voltage Reference with external capacitor at AREF pin
    ADMUX  = ADC_VREF_TYPE;
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);
}

uint16_t read_adc(uint8_t adc_input)
{
    ADMUX = adc_input | ADC_VREF_TYPE;
    _delay_us(10);
    ADCSRA |= 0x40; // Start the AD conversion
  
    while ((ADCSRA & 0x10)==0)
        ;

    ADCSRA |= 0x10; // Stop
    return ADCW;
}

void hwr_SetModulation(const Modulation modulation)
{
    switch (modulation)
    {
        case MODUL_PPM:
            TCCR1A |= (1 << COM1A1) | (0 << COM1A0);
            break;

        case MODUL_IPPM:
            TCCR1A |= (1 << COM1A1) | (1 << COM1A0);
            break;
    }
}

void hwr_SetTone(uint16_t Freq)
{
    // Таймер запускается с разными делителями для разных частот, потому как:
    // при делителе 64 невозможно получить частоту ниже ~750Гц
    // при делителе 256 на частотах > ~1500Гц высока погрешность генерации
    // Деление на 4 аргументов для того, что бы F_CPU/x не превысило word
    // Деление на два в конце, потому как нужен полупериод
    if (Freq > 1000)
    {
        // Clock source: System Clock
        // Mode: CTC top = OCR2
        // OC2 output: Toggle on compare match
        // Prescaler: clock / 64
        TCCR4A = (0 << COM4B1) | (1 << COM4B0) | (0 << WGM41) | (0 << WGM40);
        TCCR4B = (0 << WGM43)  | (1 << WGM42)  | (0 << CS42)  | (1 << CS41) | (1 << CS40);
        TCNT4  = 0;
        OCR4B  = (F_CPU / 64 / 2) / (Freq / 2) / 2;
    }
    else
    if (Freq > 0)
    {
        // Clock source: System Clock
        // Mode: CTC top = OCR2
        // OC2 output: Toggle on compare match
        // Prescaler: clock / 256
        TCCR4A = (0 << COM4B1) | (1 << COM4B0) | (0 << WGM41) | (0 << WGM40);
        TCCR4B = (0 << WGM43)  | (1 << WGM42)  | (1 << CS42)  | (0 << CS41) | (0 << CS40);
        TCNT4  = 0;
        OCR4B  = F_CPU / 256 / Freq / 2;
    }
    else
    {
        TCCR4A = 0;
        TCCR4B = 0;
        OCR4B  = 0;

        // Снятие напряжения с пищалки, т.к. по останову таймера оно может быть любым
        REGISTER_BIT(PORTH, bit4) = 0;
    }
}