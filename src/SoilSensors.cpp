#include "SoilSensors.h"
#include <arduino.h>

/**
 * constructor
 */
SoilSensors::SoilSensors(int analog_pins[], int num_sensors):
  num_sensors_{num_sensors}
{
  for (int i = 0; i < num_sensors_; i++) {
    analog_pins_[i] = analog_pins[i];
  }
}

/**
 * initializes hardware
 */
void SoilSensors::Init() {
  for (int i = 0; i < num_sensors_; i++) {
    pinMode(analog_pins_[i], INPUT);
  }
}

/**
 * Read value from a specific sensor
 *
 * @param selected_sensor Sensor index (1-based)
 * @return int (0-1023) - analog sensor value
 */
int SoilSensors::GetMoisture(int selected_sensor) {
  // Ensure the selected_sensor is within the valid range (1 to num_sensors_)
  if (selected_sensor < 1 || selected_sensor > num_sensors_) {
    return -1; // Return an error value if out of range
  }

  // Adjust selected_sensor to zero-based index
  return analogRead(analog_pins_[selected_sensor - 1]);
}

/**
 * Decode raw sensor value to actual moisture percentage
 *
 * @param soil_raw Raw analog value from the sensor (0-1023)
 * @param selected_sensor Sensor index (1-based)
 * @return int (0-100) - moisture percentage
 */
int SoilSensors::DecodeMoisture(int soil_raw, int selected_sensor) {
  // Ensure the selected_sensor is within the valid range (1 to num_sensors_)
  if (selected_sensor < 1 || selected_sensor > num_sensors_) {
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
 * @param selected_sensor Sensor index (1-based)
 * @return int - Filtered raw value
 */
int SoilSensors::FilterRawValue(int soil_raw, int selected_sensor) {
  // Ensure the selected_sensor is within the valid range (1 to num_sensors_)
  if (selected_sensor < 1 || selected_sensor > num_sensors_) {
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
 * @param selected_sensor Sensor index (1-based)
 * @param raw_dry Raw value representing dry soil
 * @param raw_wet Raw value representing wet soil
 */
void SoilSensors::SetCalibration(int selected_sensor, int raw_dry, int raw_wet) {
  // Ensure the selected_sensor is within the valid range (1 to num_sensors_)
  if (selected_sensor < 1 || selected_sensor > num_sensors_) {
    return; // Ignore invalid sensor numbers
  }

  // Store calibration values in the arrays
  raw_dry_[selected_sensor - 1] = raw_dry;
  raw_wet_[selected_sensor - 1] = raw_wet;
}