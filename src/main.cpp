#include <Arduino.h>
#include "SoilSensors.h"
#include "EnvironmentalSensor.h"
#include "PumpControl.h"
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // I2C

SoilSensors soil_sensors(4, 5, 6, 7, A0);
EnvironmentalSensor bmp_1(0x76);
EnvironmentalSensor bmp_2(0x77);


int pump_pin = 2;
int valve_pins[] = {8, 9, 10, 11, 12, 13, 14, 15};
int num_valves = sizeof(valve_pins) / sizeof(valve_pins[0]);

PumpControl pump_control(pump_pin, valve_pins, num_valves);


void setup() {
  Serial.begin(9600);
  soil_sensors.SetCalibration(1, 1023, 597);
  soil_sensors.SetCalibration(2, 1023, 597);
  soil_sensors.SetCalibration(3, 1023, 597);
  soil_sensors.SetCalibration(4, 1023, 597);
  soil_sensors.SetCalibration(5, 1023, 597);
  soil_sensors.SetCalibration(6, 1023, 597);
  soil_sensors.SetCalibration(7, 1023, 597);
  soil_sensors.SetCalibration(8, 1023, 597);

  pump_control.Init();
  pump_control.EnableDebug(true);
  pump_control.TurnPumpOnDuration(10);
  pump_control.OpenValveDuration(0, 5); 

  /*
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
  */
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
  */
 Serial.println(millis()/1000);
 pump_control.Update();
 delay(1000); 
}