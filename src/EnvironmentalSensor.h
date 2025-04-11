#pragma once
#include <Adafruit_BMP280.h>

/*
* Class for environmental sensor that reads temperature, humidity and pressure
* sensor used: DHT22 (AM2302)
*
* NOT UPDATED YET! --> see DHTTester from adafruit library for usage
*   
* Init() needs to be called prior to usage
*
* 
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
        Adafruit_BMP280 bme_;

};