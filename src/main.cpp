#include <Arduino.h>
#include "SoilSensors.h"
#include "EnvironmentalSensor.h"
#include "PumpControl.h"
#include "DHTesp.h"
#include "ArduinoHA.h"

//SoilSensors soil_sensors(4, 5, 6, 7, A0);

//int analog_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
int num_sensors = 8;
int pump_pin = 2;
int valve_pins[] = {8, 9, 10, 11, 12, 13, 14, 15};
int num_valves = sizeof(valve_pins) / sizeof(valve_pins[0]);

PumpControl pump_control(pump_pin, valve_pins, num_valves);


void setup() {
  Serial.begin(115200);
  /*soil_sensors.SetCalibration(1, 1023, 597);
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
  pump_control.OpenValveDuration(0, 5); */

}

void loop() {
 //Serial.println(millis()/1000);
 //pump_control.Update();
 Serial.println("Hallo ");
 delay(1000); 
}