#include <Arduino.h>
#include "SoilSensors.h"
#include "EnvironmentalSensor.h"

SoilSensors soil_sensors(4, 5, 6, 7, A0);
EnvironmentalSensor bmp_1(0x76);
EnvironmentalSensor bmp_2(0x77);

void setup() {
  Serial.begin(9600);
  if(bmp_1.Init()){
    Serial.print("Sensor with address ");
    Serial.print(bmp_1.GetAddress(), HEX);
    Serial.print(" succesfully connected.");
  }
  else{
    Serial.print("Sensor with address ");
        Serial.print(bmp_1.GetAddress(), HEX);
        Serial.println(" not found");
  }
  if(bmp_2.Init()){
    Serial.print("Sensor with address ");
    Serial.print(bmp_2.GetAddress(), HEX);
    Serial.print(" succesfully connected.");
  }
  else{
    Serial.print("Sensor with address ");
        Serial.print(bmp_2.GetAddress(), HEX);
        Serial.println(" not found");
  } 
}

void loop() {
  Serial.print("H1: ");
  Serial.print(bmp_1.ReadHumidity());
  Serial.print("\t H2: ");
  Serial.print(bmp_2.ReadHumidity());
  Serial.print("\t T1: ");
  Serial.print(bmp_1.ReadHumidity());
  Serial.print("\t T2: ");
  Serial.print(bmp_2.ReadHumidity());
  Serial.print("\t P1: ");
  Serial.print(bmp_1.ReadPressure());
  Serial.print("\t P2: ");
  Serial.print(bmp_2.ReadPressure());
  delay(1000); 
}