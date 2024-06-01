#pragma once
#include <Adafruit_BME280.h>

/*
* Class for environmental sensor that reads temperature, humidity and pressure
* sensor used: BME280
*
* 
* Init() needs to be called prior to usage
*
* i2c address CANNOT be set by sofware and is purely a dummy here to keep track of the one set in hardware
*
*/
class EnvironmentalSensor {
    public:
        EnvironmentalSensor(unsigned char i2c_address);
        bool Init();
        float ReadTemperature();
        float ReadHumidity();
        float ReadPressure();
        unsigned char GetAddress();

    private:
        unsigned char i2c_address_;
        Adafruit_BME280 bme_;

};