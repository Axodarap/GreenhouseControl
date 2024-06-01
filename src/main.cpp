#include <Arduino.h>
#include "SoilSensors.h"
#include "EnvironmentalSensor.h"

#include <Adafruit_BME280.h>
Adafruit_BME280 bme; // I2C

SoilSensors soil_sensors(4, 5, 6, 7, A0);
EnvironmentalSensor bmp_1(0x76);
EnvironmentalSensor bmp_2(0x77);

void setup() {
  Serial.begin(9600);
  if(bmp_1.Init()){
    Serial.print("Sensor with address ");
    Serial.print(bmp_1.GetAddress());
    Serial.print(" succesfully connected.");
  }
  else{
    Serial.print("Sensor with address ");
        Serial.print(bmp_1.GetAddress());
        Serial.println(" not found");
  }
  if(bmp_2.Init()){
    Serial.print("Sensor with address ");
    Serial.print(bmp_2.GetAddress());
    Serial.print(" succesfully connected.");
  }
  else{
    Serial.print("Sensor with address ");
        Serial.print(bmp_2.GetAddress());
        Serial.println(" not found");
  } 

  if (!bme.begin()) {  
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void loop() {
/*   Serial.print("Humidity Sensor 1:\t");
  Serial.println(bmp_1.ReadHumidity());
  Serial.print("Humidity Sensor 2: \t");
  Serial.println(bmp_2.ReadHumidity());
  Serial.print("Temperature Sensor 1: \t");
  Serial.println(bmp_1.ReadHumidity());
  Serial.print("Temperature Sensor 2: \t");
  Serial.println(bmp_2.ReadHumidity());
  Serial.print("Pressure Sensor 1: \t");
  Serial.println(bmp_1.ReadPressure());
  Serial.print("Pressure Sensor 2: \t");
  Serial.println(bmp_2.ReadPressure());
  Serial.println("----------------------------------------------");
  delay(1000); */
}