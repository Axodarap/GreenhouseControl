#pragma once
#include <Arduino.h>

class PumpControl {
  public:
    PumpControl(int pump_pin, const int* valve_pins, int num_valves);
    void Init();
    void TurnPumpOn();
    void TurnPumpOff();
    void OpenValve(int valve_index);
    void CloseValve(int valve_index);
    void CloseAllValves();
    bool IsPumpOn();
    bool IsValveOpen(int valve_index);
    bool Update(); // Call this periodically to enforce maximum on times
    void TurnPumpOnDuration(int time_on_seconds);
    void OpenValveDuration(int valve_index, int time_on_seconds);
    void EnableDebug(bool debug); // Enable or disable debug mode
    void setPumpMaxOnTime(int max_pump_on);
    void setValveMaxOnTime(int max_valve_on);
  private:
    int pump_pin_;
    int valve_pins_[16]; // Fixed-size array for up to 16 valves
    int num_valves_;
    bool pump_state_;
    bool valve_states_[16]; // Fixed-size array for valve states

    unsigned long pump_on_time_; // Time when the pump was turned on
    unsigned long pump_duration_; // Duration the pump should stay on
    unsigned long valve_on_times_[16]; // Times when each valve was turned on
    unsigned long valve_durations_[16]; // Durations each valve should stay open

    unsigned long max_pump_on_time_; // Maximum allowed pump on time in milliseconds
    unsigned long max_valve_on_time_; // Maximum allowed valve on time in milliseconds

    bool debug_; // Debug mode flag
};