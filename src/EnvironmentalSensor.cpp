#include "EnvironmentalSensor.h"

/**
 * constructor
 */
EnvironmentalSensor::EnvironmentalSensor(unsigned char i2c_address):i2c_address_{i2c_address}
{
}

/**
 * initializes hardware
 * 
 * @return bool - true if sensor is found, false if not
 */
bool EnvironmentalSensor::Init()
{
    if (!bme_.begin(i2c_address_)) 
    {  
        return false;   //sensor not found
    }
    return true;    //sensor found
}

/**
 * reads temperature 
 * 
 * @return float - temperature value
 */
float EnvironmentalSensor::ReadTemperature()
{
    return bme_.readTemperature();
}

/**
 * reads humidity 
 * 
 * @return float - humidity value
 */
float EnvironmentalSensor::ReadHumidity()
{
   // return bme_.readHumidity(); //TODO -->BMP no humidity
}

/**
 * reads pressure 
 * 
 * @return float - pressure value
 */
float EnvironmentalSensor::ReadPressure()
{
    return bme_.readPressure();
}

/**
 * getter for i2c_address_
 * 
 * @return unsigned char - i2c address
 */
unsigned char EnvironmentalSensor::GetAddress()
{
    return i2c_address_;
}