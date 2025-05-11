#pragma once

/*
* Class for environmental sensor that reads temperature and humidity.
* Sensor used: DHT22 (AM2302) or compatible DHT sensors.
*
* Init() needs to be called prior to usage.
* Pressure reading is not supported by DHT sensors.
*/

class EnvironmentalSensor {
    public:
        EnvironmentalSensor(int pin); // Constructor to initialize with GPIO pin
        bool Init();                  // Initializes the sensor
        float ReadTemperature();      // Reads temperature in Celsius
        float ReadHumidity();         // Reads humidity in percentage
        float ReadPressure();         // Returns NAN (not supported by DHT sensors)
        int GetPin();                 // Getter for the GPIO pin

    private:
        int pin_;                     // GPIO pin connected to the DHT sensor
};