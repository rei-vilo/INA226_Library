//
// INA226.cpp
// Class library C++ code
// ----------------------------------
// Developed with embedXcode+
// http://embedXcode.weebly.com
//
// Project 		FR4133-INA226
//
// Created by 	Rei Vilo, Jan 30, 2018 15:55
// 				http://embeddedcomputing.weebly.com
//
// Copyright 	(c) Rei Vilo, 2018
// License		CC = BY SA NC
//
// See 			INA226.h and ReadMe.txt for references
//


// Library header
#include "INA226.h"

// Code
INA226::INA226()
{

}

void INA226::begin(int32_t maxCurrent, uint16_t parameter)
{
    _voltageShunt = 0;
    _voltageBus = 0;
    _current = 0;
    _power = 0;

    write16(INA226_REGISTER_CONFIGURATION, parameter);
    _currentLSB = 1.0 * maxCurrent / 1000 / 32768; // 2^15 = 32768
    //_currentLSB = 0.0001; // A/bit 1/10000

#if (INA226_DEBUG > 0)
    Serial.print("Configuration = 0x");
    Serial.println(parameter, HEX);

    Serial.print("_currentLSB = ");
    Serial.print(_currentLSB, DEC);
    Serial.print("\t");
    Serial.println(_currentLSB, DEC);
#endif

    _resistor = 0.1;
    uint16_t calibration = uint16_t((float) 0.00512 / _currentLSB / _resistor); // 512/100000

    _currentLSB *= 1000.0; // mA/bit
    _powerLSB = _currentLSB * 25.0; // mW/bit

#if (INA226_DEBUG > 0)
    Serial.print("calibration=");
    Serial.println(calibration, DEC);
#endif

    write16(INA226_REGISTER_CALIBRATION, calibration);

#if (INA226_DEBUG > 0)
    Serial.print("calibration = ");
    Serial.println(read16(INA226_REGISTER_CALIBRATION), DEC); // 5449
    Serial.print("5449 ? ");
    Serial.println(read16(INA226_REGISTER_MANUFACTURER_ID), HEX); // 5449
    Serial.print("2260 ? ");
    Serial.println(read16(INA226_REGISTER_DIE_ID), HEX); // 2260
#endif
}

String INA226::WhoAmI()
{
    return "INA226";
}

// maxShuntVoltage = 81.92 mV
// maxCurrent = maxShuntVoltage / _resistor
// maxPossibleCurrent = min(_currentLSB * 32767, maxCurrent)
// maxPossibleShuntVoltage = maxPossibleCurrent * _resistor
// maxBusVoltage = 36 V


void INA226::get()
{
    iShunt   = read16(INA226_REGISTER_SHUNT_VOLTAGE);
    iBus     = read16(INA226_REGISTER_BUS_VOLTAGE);
    iPower   = read16(INA226_REGISTER_POWER);
    iCurrent = read16(INA226_REGISTER_CURRENT);

#if (INA226_DEBUG > 0)
    Serial.print("Raw        Vs Vb P C\t");
    Serial.print(iShunt);    Serial.print(" \t");
    Serial.print(iBus);      Serial.print("\t");
    Serial.print(iPower);    Serial.print("\t");
    Serial.print(iCurrent);  Serial.println();
#endif

#if (INA226_DEBUG > 1)
    _voltageShunt = (int32_t)((float)(iShunt) * 2.5E-3); // fixed 2,5 uV/bit = 2.5E-3 mV/bit
    _voltageBus   = (int32_t)((float)(iBus) * 1.25); // fixed 1,25 mV/bit
    _current      = (int32_t)((float)(iCurrent) * _currentLSB);
    _power        = (int32_t)((float)(iPower) * _powerLSB);

    Serial.print("Values Vs Vb P C\t");
    Serial.print(_voltageShunt);   Serial.print("\t");
    Serial.print(_voltageBus);     Serial.print("\t");
    Serial.print(_current);        Serial.print("\t");
    Serial.print(_power);          Serial.println();
#endif
}

int32_t INA226::voltageShunt()
{
    //    return _voltageShunt;
    //    return (int32_t)((float)(iShunt) * 2.5E-3); // fixed 2,5 uV/bit = 2.5E-3 mV/bit
    return (int32_t)voltageShuntFloat();
}

int32_t INA226::voltageBus()
{
    //    return _voltageBus;
    //    return (int32_t)((float)(iBus) * 1.25); // fixed 1,25 mV/bit
    return (int32_t)voltageBusFloat();
}

int32_t INA226::current()
{
    //    return _current;
    //    return (int32_t)((float)(iCurrent) * _currentLSB);
    return (int32_t)currentFloat();
}

int32_t INA226::power()
{
    //    return _power;
    //    return (int32_t)((float)(iPower) * _powerLSB);
    return (int32_t)powerFloat();
}

float INA226::voltageShuntFloat()
{
    return ((float)(iShunt) * 2.5E-3); // fixed 2,5 uV/bit = 2.5E-3 mV/bit
}

float INA226::voltageBusFloat()
{
    return ((float)(iBus) * 1.25); // fixed 1,25 mV/bit
}

float INA226::currentFloat()
{
    return ((float)(iCurrent) * _currentLSB);
}

float INA226::powerFloat()
{
    return ((float)(iPower) * _powerLSB);
}

// Utilities
int16_t INA226::read16(uint8_t address)
{
    int16_t value = 0;

    Wire.beginTransmission(INA226_I2C_ADDRESS);
    Wire.write(address);
    Wire.endTransmission();

    Wire.requestFrom(INA226_I2C_ADDRESS, 2);
    while (Wire.available() < 2)
    {
        Serial.print(".");
    }

    value = Wire.read();
    value <<= 8;
    value += Wire.read();

    return value;
}

void INA226::write16(uint8_t address, uint16_t value)
{
    Wire.beginTransmission(INA226_I2C_ADDRESS);
    Wire.write(address);
    Wire.write(value >> 8);
    Wire.write(value & 0xff);
    Wire.endTransmission();
}
