#include "SoilSensors.h"
#include <arduino.h>
/**
 * constructor
 */
SoilSensors::SoilSensors(int select_pin_0, int select_pin_1, int select_pin_2, int select_pin_3, int analog_pin):
  select_pin_0_{select_pin_0}, select_pin_1_{select_pin_1}, select_pin_2_{select_pin_2}, select_pin_3_{select_pin_3},
  analog_pin_{analog_pin}
{
}

/**
 * initializes hardware
 */
void SoilSensors::Init(){

  pinMode(select_pin_0_, OUTPUT);
  pinMode(select_pin_1_, OUTPUT);
  pinMode(select_pin_2_, OUTPUT);
  pinMode(select_pin_3_, OUTPUT);
  pinMode(analog_pin_, INPUT);
}

/**
 * Read value from one of the MUX channels and access one individual sensor
 *
 * @param selected_sensor MUX channel / selected sensor
 * @return int (0-1023) - analog sensor value
 */
int SoilSensors::GetMoisture(int selected_sensor){   //TODO clean this Mist?
  
  switch(selected_sensor){
    case 1:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 0);
      break;
    
    case 2:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 0);
      break;

    case 3:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 0);
      break;

    case 4:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 0);
      break;

    case 5:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 0);
      break;

    case 6:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 0);
      break;

    case 7:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 0);
      break;

    case 8:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 0);
      break;

    case 9:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 1);
      break;
    
    case 10:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 1);
      break;

    case 11:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 1);
      break;

    case 12:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 1);
      break;

    case 13:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 1);
      break;

    case 14:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 1);
      break;

    case 15:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 1);
      break;

    case 16:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 1);
      break;
  }

  return analogRead(analog_pin_);

}

/**
 * Read value from one of the MUX channels and access one individual sensor
 *
 * @param selected_sensor MUX channel / selected sensor
 * @return int (0-1023) - analog sensor value
 */
int SoilSensors::GetMoisture_fancy(int selected_sensor) {
  // Ensure the selected_sensor is within the valid range (1-16)
  if (selected_sensor < 1 || selected_sensor > 16) {
      return -1; // Return an error value if out of range
  }

  // Adjust selected_sensor to zero-based index
  selected_sensor -= 1;

  // Set the pins using bitwise operations
  digitalWrite(select_pin_0_, (selected_sensor & 0x01) ? HIGH : LOW);
  digitalWrite(select_pin_1_, (selected_sensor & 0x02) ? HIGH : LOW);
  digitalWrite(select_pin_2_, (selected_sensor & 0x04) ? HIGH : LOW);
  digitalWrite(select_pin_3_, (selected_sensor & 0x08) ? HIGH : LOW);

  // Read and return the analog value
  return analogRead(analog_pin_);
}

/**
 * Decode raw sensor value to actual moisture percentage
 *
 * @param soil_raw Raw analog value from the sensor (0-1023)
 * @param selected_sensor MUX channel / selected sensor (1-16)
 * @return int (0-100) - moisture percentage
 */
int SoilSensors::DecodeMoisture(int soil_raw, int selected_sensor){
  // Ensure the selected_sensor is within the valid range (1-16)
  if (selected_sensor < 1 || selected_sensor > 16) {
      return -1; // Return error for invalid sensor numbers
  }

  // Retrieve calibration values for the selected sensor
  int raw_dry = raw_dry_[selected_sensor - 1];
  int raw_wet = raw_wet_[selected_sensor - 1];

  // Ensure raw_dry and raw_wet are valid
  if (raw_dry == raw_wet) {
      return -1; // Return error if dry and wet values are the same
  }

  // Map the raw value to a percentage (0-100)
  int moisture = map(soil_raw, raw_dry, raw_wet, 0, 100);

  // Clamp the value to ensure it's within 0-100
  if (moisture < 0) moisture = 0;
  if (moisture > 100) moisture = 100;

  return moisture;
}

/**
 * Apply a simple filter to the raw sensor value
 *
 * @param soil_raw Raw analog value from the sensor (0-1023)
 * @param selected_sensor MUX channel / selected sensor (1-16)
 * @return int - Filtered raw value
 */
int SoilSensors::FilterRawValue(int soil_raw, int selected_sensor) {
  // Ensure the selected_sensor is within the valid range (1-16)
  if (selected_sensor < 1 || selected_sensor > 16) {
      return soil_raw; // Return the raw value if the sensor number is invalid
  }

  // Apply exponential smoothing
  float alpha = 0.8; // Smoothing factor (0 < alpha <= 1)
  filtered_raw_[selected_sensor - 1] = alpha * soil_raw + (1 - alpha) * filtered_raw_[selected_sensor - 1];

  return filtered_raw_[selected_sensor - 1];
}

/**
 * Set calibration values for a specific sensor
 *
 * @param selected_sensor MUX channel / selected sensor (1-16)
 * @param raw_dry Raw value representing dry soil
 * @param raw_wet Raw value representing wet soil
 */
void SoilSensors::SetCalibration(int selected_sensor, int raw_dry, int raw_wet) {
  // Ensure the selected_sensor is within the valid range (1-16)
  if (selected_sensor < 1 || selected_sensor > 16) {
      return; // Ignore invalid sensor numbers
  }

  // Store calibration values in the arrays
  raw_dry_[selected_sensor - 1] = raw_dry;
  raw_wet_[selected_sensor - 1] = raw_wet;
}