#pragma once

/*
* wrapper for 4 bit analog multiplexer
*
* select_pin_i is used to access the channels 1-16 of the multiplexer
* Init() needs to be called prior to usage
*/
class Multiplexer{
  private:
    int select_pin_0_;
    int select_pin_1_;
    int select_pin_2_;
    int select_pin_3_;
    int analog_pin_;

  public:
    Multiplexer(int select_pin_0, int select_pin_1, int select_pin_2, int select_pin_3, int analog_pin);
    void Init();
    int ReadValue(int selected_input);
};