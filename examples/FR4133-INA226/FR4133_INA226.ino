///
/// @mainpage	FR4133-INA226
///
/// @details	Library for INA226 BoosterPack
/// @n
/// @n
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
///
/// @author		Rei Vilo
/// @author		http://embeddedcomputing.weebly.com
/// @date		Jan 30, 2018 15:51
/// @version	101
///
/// @copyright	(c) Rei Vilo, 2018
/// @copyright	CC = BY SA NC
///
/// @see		ReadMe.txt for references
///


///
/// @file		FR4133_INA226.ino
/// @brief		Main sketch
///
/// @details	Example for INA226 BoosterPack on MSP430FR4133 LaunchPad with LCD
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
///
/// @author		Rei Vilo
/// @author		http://embeddedcomputing.weebly.com
/// @date		Jan 30, 2018 15:51
/// @version	101
///
/// @copyright	(c) Rei Vilo, 2018
/// @copyright	CC = BY SA NC
///
/// @see		ReadMe.txt for references
/// @n
///


// Core library for code-sense - IDE-based
// !!! Help: http://bit.ly/2AdU7cu
#if defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(ROBOTIS) // Robotis specific
#include "libpandora_types.h"
#include "pandora.h"
#elif defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#elif defined(LITTLEROBOTFRIENDS) // LittleRobotFriends specific
#include "LRF.h"
#elif defined(MICRODUINO) // Microduino specific
#include "Arduino.h"
#elif defined(TEENSYDUINO) // Teensy specific
#include "Arduino.h"
#elif defined(REDBEARLAB) // RedBearLab specific
#include "Arduino.h"
#elif defined(RFDUINO) // RFduino specific
#include "Arduino.h"
#elif defined(SPARK) || defined(PARTICLE) // Particle / Spark specific
#include "application.h"
#elif defined(ESP8266) // ESP8266 specific
#include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif // end IDE

// Set parameters
#define USE_UART 1
#define USE_LCD 1


// Include application, user and local libraries
#include "Wire.h"
#include "INA226.h"
#include "stdarg.h"

#if (USE_LCD == 1)
#include "LCD_Launchpad.h"
LCD_LAUNCHPAD myLCD;
#endif


// Define structures and classes


// Define variables and constants
const uint8_t myButtonMeasure = PUSH1;
const uint8_t myButtonUnit = PUSH2;
INA226 myINA226;
char bufferOut[128];
uint8_t fsmMeasure = 0;
uint8_t fsmUnit = 0;

// Prototypes
// !!! Help: http://bit.ly/2l0ZhTa


// Utilities
String formatString(const char * format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(bufferOut, format, args);
    va_end(args);
    return String(bufferOut);
}

#if (USE_LCD == 1)
void blink(uint8_t pin, uint8_t times, uint16_t ms)
{
    for (uint8_t i = 0; i < times; i++)
    {
        myLCD.showSymbol(pin, 1);
        delay(ms >> 1);
        myLCD.showSymbol(pin, 0);
        delay(ms >> 1);
    }
}
#endif

// Functions
void myMeasureISR()
{
    // Debounce mechanism
    static uint32_t millisMeasure = 0;
    if (millis() > millisMeasure)
    {
        fsmMeasure += 1;
        fsmMeasure %= 3;
    }
    millisMeasure = millis() + 200;
}

void myUnitISR()
{
    // Debounce mechanism
    static uint32_t millisUnit = 0;
    if (millis() > millisUnit)
    {
        fsmUnit += 1;
        fsmUnit %= 2;
    }
    millisUnit = millis() + 200;
}

// Add setup code
void setup()
{
#if (USE_UART == 1)
    Serial.begin(9600);
#endif

    pinMode(myButtonMeasure, INPUT_PULLUP);
    pinMode(myButtonUnit, INPUT_PULLUP);

#if (USE_LCD == 1)
    myLCD.init();
    myLCD.clear();
    myLCD.println("INA226");
#endif
    // Wire.setModule(0);
    Wire.begin();
    //myINA226.begin(1000, INA226_AVERAGES_16 + INA226_VOLTAGE_TIME_DEFAULT + INA226_SHUNT_TIME_DEFAULT + INA226_MODE_DEFAULT);
    myINA226.begin();

#if (USE_UART == 1)
    Serial.println();
    Serial.println();
    Serial.println("* FR4133 INA226");
    Serial.println("Press PUSH1 for measures");
    Serial.println("Press PUSH2 for units");
#endif

#if (USE_LCD == 1)
    blink(LCD_SEG_MARK, 3, 333);
#endif

    // Change measure displayed
    attachInterrupt(myButtonMeasure, myMeasureISR, RISING);
    // Change unit used
    attachInterrupt(myButtonUnit, myUnitISR, RISING);
}

LCD_ICONS decimalPoint = LCD_SEG_DOT5;
LCD_ICONS lastDecimalPoint = LCD_SEG_DOT5;

// Add loop code
void loop()
{
    float value = 0.0;
    char unit = ' ';
    char unitMultiplier = ' ';
    bool flagNegative = false;
    bool flagOverflow = false;
    String display;

    myINA226.get();

    switch (fsmUnit)
    {
        case 0:
            decimalPoint = LCD_SEG_DOT1;
            unitMultiplier = ' ';
            break;

        case 1:
            decimalPoint = LCD_SEG_DOT4;
            unitMultiplier = 'm';
            break;

        default:
            break;
    }

    switch (fsmMeasure)
    {
        case 0:
            unit = 'V';
            value = myINA226.voltageBusFloat();
            break;

        case 1:
            unit = 'A';
            value = myINA226.currentFloat();
            break;

        case 2:
            unit = 'W';
            value = myINA226.powerFloat();
            break;

        default:
            break;
    }

    if (value < 0.0)
    {
        flagNegative = true;
        value *= -1.0;
    }

    if (value > 10000)
    {
        if (decimalPoint == LCD_SEG_DOT1)
        {
            decimalPoint = LCD_SEG_DOT2;
            unitMultiplier = ' ';
        }
        else
        {
            flagOverflow = true;
        }
    }

#if (USE_LCD == 1)
    myLCD.showSymbol(LCD_SEG_MARK, flagOverflow);

    if (!flagOverflow)
    {
        myLCD.showSymbol(LCD_SEG_MINUS1, flagNegative);
        if (decimalPoint != lastDecimalPoint)
        {
            myLCD.showSymbol(lastDecimalPoint, false);
            myLCD.showSymbol(decimalPoint, true);
            lastDecimalPoint = decimalPoint;
        }
#endif

        switch (decimalPoint)
        {
            case LCD_SEG_DOT1:
                display = formatString("%04i", uint32_t(value));
                break;

            case LCD_SEG_DOT2:
                display = formatString("%04i", uint32_t(value / 10.0));

                break;

            case LCD_SEG_DOT4:
                display = formatString("%4i", uint32_t(value));

                break;

            default:
                break;
        }
        display += unitMultiplier;
        display += unit;

#if (USE_UART == 1)
        Serial.println(display);
#endif

#if (USE_LCD == 1)
        myLCD.println(display);
#endif
    }
    delay(100);
}

