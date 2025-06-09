// HomeAssistantGreenhouse.h
#pragma once

#include <WiFi.h>
#include "ArduinoHA.h"
#include <functional>

// Diese Version ist KEINE wiederverwendbare Klasse.
// Sie dient als strukturierter Container für eine einmalige Verwendung.

namespace GreenhouseHA {

void begin(IPAddress broker, const char* user, const char* pass,
           const char* deviceName,
           const char* swVersion,
           WiFiClient& client);

void loop();
void publishSensors(float soilValues[], size_t count, float hum_outside,float temp_outside);
void setPumpState(bool state);
void setFanState(bool state);
void setValveState(uint8_t index, bool state);
void setAllValvesState(bool state);

// Callback-Funktionen für Home Assistant Befehle
void onPumpCommand(std::function<void(bool)> cb);
void onFanCommand(std::function<void(bool)> cb);
void onValveCommand(std::function<void(uint8_t, bool)> cb);
void onAllValvesCommand(std::function<void(bool)> cb);
void onDurationSelected(std::function<void(int)> cb);

} // namespace GreenhouseHA
