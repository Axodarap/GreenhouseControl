#pragma once
/*
* Wrapper for up to 16 soil moisture sensors using direct analog pin readings.
*
* Each sensor is connected to a dedicated analog pin.
* Init() needs to be called prior to usage.
*/
class SoilSensors {
  public:
    SoilSensors(int analog_pins[], int num_sensors);
    void Init();
    int GetMoisture(int selected_sensor);
    int DecodeMoisture(int soil_raw, int selected_sensor);
    int FilterRawValue(int soil_raw, int selected_sensor);
    void SetCalibration(int selected_sensor, int raw_dry, int raw_wet);

  private:
    int num_sensors_; // Number of sensors
    int analog_pins_[16]; // Array to store analog pins for each sensor
    int raw_dry_[16]; // Array to store dry calibration values for each sensor
    int raw_wet_[16]; // Array to store wet calibration values for each sensor
    int filtered_raw_[16]; // Array to store filtered raw values for each sensor
};