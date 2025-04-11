#pragma once
/*
* wrapper for up to 16 soil moisture sensors using a 4 bit analog multiplexer
*
* select_pin_i is used to access the channels 1-16 of the multiplexer --> select a sesnor
* Init() needs to be called prior to usage
*/
class SoilSensors{
  public:
    SoilSensors(int select_pin_0, int select_pin_1, int select_pin_2, int select_pin_3, int analog_pin);
    void Init();
    int GetMoisture(int selected_sensor);
    int GetMoisture_fancy(int selected_sensor);
    int DecodeMoisture(int soil_raw, int selected_sensor);
    int FilterRawValue(int soil_raw, int selected_sensor);
    void SetCalibration(int selected_sensor, int raw_dry, int raw_wet);
  private:
    int select_pin_0_;
    int select_pin_1_;
    int select_pin_2_;
    int select_pin_3_;
    int analog_pin_;
    int raw_dry_[16]; // Array to store dry calibration values for each sensor
    int raw_wet_[16]; // Array to store wet calibration values for each sensor
    int filtered_raw_[16]; // Array to store filtered raw values for each sensor
};