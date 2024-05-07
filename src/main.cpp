#include <Arduino.h>
#include "SoilSensors.h"

SoilSensors soil_sensors(4, 5, 6, 7, A0);

void setup() {
  Serial.begin(9600);
  soil_sensors.Init();
}

void loop() {
  
for(int i=1; i<17; i++){
  Serial.print((String)soil_sensors.ReadMoisture(i) + "\t");  
 }
 Serial.println();
 delay(500);
}