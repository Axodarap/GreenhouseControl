#include "EnvironmentalSensor.h"
#include <DHTesp.h>

// Globale Instanz des DHTesp-Objekts
DHTesp dht;

/**
 * constructor
 */
EnvironmentalSensor::EnvironmentalSensor(int pin): pin_{pin}
{
}

/**
 * initializes hardware
 * 
 * @return bool - true if sensor is initialized successfully, false otherwise
 */
bool EnvironmentalSensor::Init()
{
    dht.setup(pin_, DHTesp::DHT22); // DHT22 als Standardtyp, anpassbar
    return dht.getStatus() == DHTesp::ERROR_NONE;
}

/**
 * reads temperature 
 * 
 * @return float - temperature value
 */
float EnvironmentalSensor::ReadTemperature()
{
    TempAndHumidity data = dht.getTempAndHumidity();
    if (dht.getStatus() != DHTesp::ERROR_NONE) {
        return NAN; // Fehlerfall: Rückgabe von NaN
    }
    return data.temperature;
}

/**
 * reads humidity 
 * 
 * @return float - humidity value
 */
float EnvironmentalSensor::ReadHumidity()
{
    TempAndHumidity data = dht.getTempAndHumidity();
    if (dht.getStatus() != DHTesp::ERROR_NONE) {
        return NAN; // Fehlerfall: Rückgabe von NaN
    }
    return data.humidity;
}

/**
 * reads pressure 
 * 
 * @return float - pressure value
 */
float EnvironmentalSensor::ReadPressure()
{
    // DHT-Sensoren messen keinen Druck
    return NAN; // Rückgabe von NaN, da nicht unterstützt
}

/**
 * getter for pin_
 * 
 * @return int - GPIO-Pin
 */
int EnvironmentalSensor::GetPin()
{
    return pin_;
}