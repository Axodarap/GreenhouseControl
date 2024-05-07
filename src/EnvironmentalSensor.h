#pragma once
/*
* Class for environmental sensor that reads temperature, humidity and pressure
* sensor used: BME280
*
* 
* Init() needs to be called prior to usage
*/
class EnvironmentalSensor {
    public:
        EnvironmentalSensor();
        void Init();
        float GetTemperature();
        float GetHumidity();
        float GetPressure();
    
    private:
        // Add private member functions and variables here
};