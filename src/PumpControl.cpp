#include "PumpControl.h"

PumpControl::PumpControl(int pump_pin, const int* valve_pins, int num_valves)
  : pump_pin_(pump_pin), num_valves_(num_valves), pump_state_(false), pump_on_time_(0), pump_duration_(0),
    max_pump_on_time_(60000), max_valve_on_time_(30000), debug_(false) { // Default max times: 60s for pump, 30s for valves
    for (int i = 0; i < num_valves_ && i < 16; i++) {
        valve_pins_[i] = valve_pins[i];
        valve_states_[i] = false;
        valve_on_times_[i] = 0;
        valve_durations_[i] = 0;
    }
}

void PumpControl::EnableDebug(bool debug) {
    debug_ = debug;
}

void PumpControl::Init() {
    pinMode(pump_pin_, OUTPUT);
    digitalWrite(pump_pin_, LOW);
    for (int i = 0; i < num_valves_; i++) {
        pinMode(valve_pins_[i], OUTPUT);
        digitalWrite(valve_pins_[i], LOW);
    }
    if (debug_) {
        Serial.println("PumpControl initialized.");
    }
}

void PumpControl::TurnPumpOn() {
    if (!pump_state_) {
        digitalWrite(pump_pin_, HIGH);
        pump_state_ = true;
        pump_on_time_ = millis();
        pump_duration_ = 0;
        if (debug_) {
            Serial.println("Pump turned ON.");
        }
    }
}

void PumpControl::TurnPumpOnDuration(int time_on_seconds) {
    if (!pump_state_) {
        digitalWrite(pump_pin_, HIGH);
        pump_state_ = true;
        pump_on_time_ = millis();
        pump_duration_ = time_on_seconds * 1000;
        if (debug_) {
            Serial.print("Pump turned ON for ");
            Serial.print(time_on_seconds);
            Serial.println(" seconds.");
        }
    }
}

void PumpControl::TurnPumpOff() {
    digitalWrite(pump_pin_, LOW);
    pump_state_ = false;
    pump_on_time_ = 0;
    pump_duration_ = 0;
    if (debug_) {
        Serial.println("Pump turned OFF.");
    }
}

void PumpControl::OpenValve(int valve_index) {
    if (valve_index >= 0 && valve_index < num_valves_ && pump_state_ == true) {
        if (!valve_states_[valve_index]) {
            digitalWrite(valve_pins_[valve_index], HIGH);
            valve_states_[valve_index] = true;
            valve_on_times_[valve_index] = millis();
            valve_durations_[valve_index] = 0;
            if (debug_) {
                Serial.print("Valve ");
                Serial.print(valve_index);
                Serial.println(" opened.");
            }
        }
    }
}

void PumpControl::OpenValveDuration(int valve_index, int time_on_seconds) {
    if (valve_index >= 0 && valve_index < num_valves_ && pump_state_ == true) {
        if (!valve_states_[valve_index]) {
            digitalWrite(valve_pins_[valve_index], HIGH);
            valve_states_[valve_index] = true;
            valve_on_times_[valve_index] = millis();
            valve_durations_[valve_index] = time_on_seconds * 1000;
            if (debug_) {
                Serial.print("Valve ");
                Serial.print(valve_index);
                Serial.print(" opened for ");
                Serial.print(time_on_seconds);
                Serial.println(" seconds.");
            }
        }
    }
}

void PumpControl::CloseValve(int valve_index) {
    if (valve_index >= 0 && valve_index < num_valves_) {
        digitalWrite(valve_pins_[valve_index], LOW);
        valve_states_[valve_index] = false;
        valve_on_times_[valve_index] = 0;
        valve_durations_[valve_index] = 0;
        if (debug_) {
            Serial.print("Valve ");
            Serial.print(valve_index);
            Serial.println(" closed.");
        }
    }
}

void PumpControl::CloseAllValves() {
    for (int i = 0; i < num_valves_; i++) {
        CloseValve(i);
    }
    if (debug_) {
        Serial.println("All valves closed.");
    }
}

bool PumpControl::Update() {
    unsigned long current_time = millis();

    if (pump_state_) {
        if ((pump_duration_ > 0 && (current_time - pump_on_time_ >= pump_duration_)) ||
            (current_time - pump_on_time_ >= max_pump_on_time_)) {
            TurnPumpOff();
            CloseAllValves();
        }
    }

    for (int i = 0; i < num_valves_; i++) {
        if (valve_states_[i]) {
            if ((valve_durations_[i] > 0 && (current_time - valve_on_times_[i] >= valve_durations_[i])) ||
                (current_time - valve_on_times_[i] >= max_valve_on_time_)) {
                CloseValve(i);
            }
        }
    }
    return pump_state_;
}


void PumpControl::setPumpMaxOnTime(int max_pump_on) {
    // Set the maximum allowed pump on time in milliseconds
    max_pump_on_time_ = max_pump_on*1000; // 60 seconds
}

void PumpControl::setValveMaxOnTime(int max_valve_on) {
    // Set the maximum allowed pump on time in milliseconds
    max_valve_on_time_ = max_valve_on*1000; // 60 seconds
}