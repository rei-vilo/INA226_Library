///
/// @file		INA226.h
/// @brief		Class library header
/// @details	Library for INA226 BoosterPack
/// @n
/// @n @b		Project FR4133-INA226
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
///
/// @author		Rei Vilo
/// @author		http://embeddedcomputing.weebly.com
///
/// @date		Jan 30, 2018 15:55
/// @version	101
///
/// @copyright	(c) Rei Vilo, 2018
/// @copyright	CC = BY SA NC
///
/// @see		ReadMe.txt for references
///


// Core library - IDE-based
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
#elif defined(SPARK) // Spark specific
#include "application.h"
#elif defined(ARDUINO) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif // end IDE

#ifndef INA226__RELEASE
///
/// @brief    	Release
///
#define INA226_RELEASE

#include "Wire.h"

///
/// @brief      Debug flag
/// @details    verbosity 1 or 2 for debugging messages, 0 = no
#define INA226_DEBUG 0


#define INA226_I2C_ADDRESS 0x40

#define INA226_RESET                    1 << 15

// Averages [11:9]
#define INA226_AVERAGES_1               0b000 << 9 // default
#define INA226_AVERAGES_4               0b001 << 9
#define INA226_AVERAGES_16              0b010 << 9
#define INA226_AVERAGES_64              0b011 << 9
#define INA226_AVERAGES_128             0b010 << 9
#define INA226_AVERAGES_256             0b101 << 9
#define INA226_AVERAGES_512             0b110 << 9
#define INA226_AVERAGES_1024            0b111 << 9
#define INA226_AVERAGES_DEFAULT INA226_AVERAGES_1

// Bus Voltage Conversion time [8:6]
#define INA226_VOLTAGE_TIME_140         0b000 << 6
#define INA226_VOLTAGE_TIME_204         0b001 << 6
#define INA226_VOLTAGE_TIME_332         0b010 << 6
#define INA226_VOLTAGE_TIME_588         0b011 << 6
#define INA226_VOLTAGE_TIME_1100        0b100 << 6 // default
#define INA226_VOLTAGE_TIME_2116        0b101 << 6
#define INA226_VOLTAGE_TIME_4156        0b110 << 6
#define INA226_VOLTAGE_TIME_8244        0b111 << 6
#define INA226_VOLTAGE_TIME_DEFAULT INA226_VOLTAGE_TIME_1100

// Shunt Voltage Conversion time [5:3]
#define INA226_SHUNT_TIME_140           0b000 << 3
#define INA226_SHUNT_TIME_204           0b001 << 3
#define INA226_SHUNT_TIME_332           0b010 << 3
#define INA226_SHUNT_TIME_588           0b011 << 3
#define INA226_SHUNT_TIME_1100          0b100 << 3 // default
#define INA226_SHUNT_TIME_2116          0b101 << 3
#define INA226_SHUNT_TIME_4156          0b110 << 3
#define INA226_SHUNT_TIME_8244          0b111 << 3
#define INA226_SHUNT_TIME_DEFAULT INA226_SHUNT_TIME_1100

// Operating Mode [2:0]
#define INA226_SHUTDOWN                 0b000
#define INA226_TRIGGERED_SHUNT_VOLTAGE  0b001
#define INA226_TRIGGERED_BUS_VOLTAGE    0b010
#define INA226_TRIGGERED_SHUNT_BUS      0b011
#define INA226_SHUTDOWN                 0b100
#define INA226_CONTINUOUS_SHUNT_VOLTAGE 0b101
#define INA226_CONTINUOUS_BUS_VOLTAGE   0b110
#define INA226_CONTINUOUS_SHUNT_BUS     0b111 // default
#define INA226_MODE_DEFAULT INA226_CONTINUOUS_SHUNT_BUS

#define INA226_DEFAULT (INA226_AVERAGES_DEFAULT | INA226_VOLTAGE_TIME_DEFAULT | INA226_SHUNT_TIME_DEFAULT | INA226_MODE_DEFAULT)

// Registers
#define INA226_REGISTER_CONFIGURATION   0x00
#define INA226_REGISTER_SHUNT_VOLTAGE   0x01
#define INA226_REGISTER_BUS_VOLTAGE     0x02
#define INA226_REGISTER_POWER           0x03
#define INA226_REGISTER_CURRENT         0x04
#define INA226_REGISTER_CALIBRATION     0x05
#define INA226_REGISTER_MANUFACTURER_ID 0xfe
#define INA226_REGISTER_DIE_ID          0xff


///
/// @class		<#Description#>
///
class INA226
{

  public:
    ///
    /// @brief		Constructor
    ///
    INA226();

    ///
    /// @brief	    Initialisation
    /// @param      maxCurrent maximum expected current, in mA
    /// @param      parameter default parameters
    ///
    void begin(int32_t maxCurrent = 1000, uint16_t parameter = INA226_DEFAULT);

    ///
    ///	@brief  	Who am I?
    ///	@return 	Who am I? string
    ///
    String WhoAmI();

    ///
    ///	@brief		Read all measures
    /// @details    Read voltage shunt, voltage, current and power
    ///
    void get();

    ///
    /// @brief      Get shunt voltage
    /// @return     shunt voltage in mV
    ///
    int32_t voltageShunt();
    float voltageShuntFloat();

    ///
    /// @brief      Get bus voltage
    /// @return     bus voltage in mV
    ///
    int32_t voltageBus();
    float voltageBusFloat();

    ///
    /// @brief      Get bus current
    /// @return     bus voltage in mA
    ///
    int32_t current();
    float currentFloat();

    ///
    /// @brief      Get bus power
    /// @return     bus voltage in mW
    ///
    int32_t power();
    float powerFloat();

  private:
    int16_t read16(uint8_t address);
    void write16(uint8_t address, uint16_t value);

    int16_t iShunt;
    int16_t iBus;
    int16_t iPower;
    int16_t iCurrent;

    int32_t _voltageShunt;
    int32_t _voltageBus;
    int32_t _current;
    int32_t _power;
    float _currentLSB;
    float _powerLSB;
    float _resistor;
};

#endif // INA226_RELEASE

