#include <Arduino.h>
#include <WiFi.h>
#include "SoilSensors.h"
#include "EnvironmentalSensor.h"
#include "PumpControl.h"
#include "DHTesp.h"
#include "ArduinoHA.h"
#include "pin_config.h"
#include "HomeAssistantGreenhouse.h"
#include "passwort.h"

EnvironmentalSensor humtemp_outside(32); // DHT22 an GPIO 32

// Array mit den drei Select-Pins (S0, S1, S2)
int muxSelectPins[3] = {MUX_S0, MUX_S1, MUX_S2 };

// SoilSensors-Objekt instanziieren
SoilSensors soil(MUX_SIG_PIN, muxSelectPins, NUM_SOIL_SENSORS);

int num_sensors = 8;
int pump_pin = 13;
int valve_pins[] = {16, 17, 18, 19, 20, 21, 22, 23};
int num_valves = sizeof(valve_pins) / sizeof(valve_pins[0]);
const int fan_pin = 4; 

bool pumpOn = false;
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 3000;


// Zentrale Definition der Optionen
const int wateringOptions[] = {30, 60, 90, 120, 180};
const int numWateringOptions = sizeof(wateringOptions) / sizeof(wateringOptions[0]);

PumpControl pump_control(pump_pin, valve_pins, num_valves);


const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
IPAddress brokerAddr(172,20,10,10);   // Studgard
//IPAddress brokerAddr(192,168,68,62);   // Huedde
const char* mqttUser = MQTT_USER;
const char* mqttPassword = MQTT_PASSWORD;


WiFiClient wifiClient;

// Initialisierung der Sensoren und Pumpensteuerung
void setupGreenhouseCallbacks(); 


void setup() {
  Serial.begin(115200);
  humtemp_outside.Init();
  soil.Init();
  /*
  soil_sensors.SetCalibration(1, 1023, 597);
  soil_sensors.SetCalibration(2, 1023, 597);
  soil_sensors.SetCalibration(3, 1023, 597);
  soil_sensors.SetCalibration(4, 1023, 597);
  soil_sensors.SetCalibration(5, 1023, 597);
  soil_sensors.SetCalibration(6, 1023, 597);
  soil_sensors.SetCalibration(7, 1023, 597);
  soil_sensors.SetCalibration(8, 1023, 597);
  */

  // Lüfter-Pin initialisieren
  pinMode(fan_pin, OUTPUT);
  digitalWrite(fan_pin, LOW);

  pump_control.Init();
  pump_control.EnableDebug(true);
  pump_control.setPumpMaxOnTime(60); // Maximal 60 Sekunden Pumpenlaufzeit
  pump_control.setValveMaxOnTime(60); // Maximal 60 Sekunden Ventile-Offen. Macht das überhaupt Sinn?

  // WLAN verbinden
  WiFi.begin(ssid, password);
  Serial.print("Verbinde mit WLAN");
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("\nWLAN verbunden. IP: " + WiFi.localIP().toString());

  GreenhouseHA::begin(brokerAddr, mqttUser, mqttPassword, "ESP32-Greenhouse", "1.0.0", wifiClient);
  setupGreenhouseCallbacks();
}

void loop() {
 //Serial.println(millis()/1000);
  pumpOn = pump_control.Update();
  GreenhouseHA::loop();

  if (millis() - lastUpdate > updateInterval) {
      float values_soilsensor[8] = {0};
      for (int i = 1; i <= NUM_SOIL_SENSORS; i++) {
        values_soilsensor[i] = soil.GetMoisturePercent(i);  // Roh → Filter → Prozent
        Serial.printf("Sensor %d: Feuchte=%3d%%\n", i, values_soilsensor[i]);
        delay(50);
      }

      GreenhouseHA::publishSensors(values_soilsensor, 8);

      // Zustand an Home Assistant zurueckmelden
      GreenhouseHA::setPumpState(pumpOn);
      GreenhouseHA::setFanState(digitalRead(fan_pin) == HIGH);
      for (int i = 0; i < 8; ++i) {
        GreenhouseHA::setValveState(i, pump_control.GetValveState(i));
      }
      GreenhouseHA::setAllValvesState(pump_control.AreAllValvesOpen());

      lastUpdate = millis();
      Serial.printf("Update ");
  }
}

void setupGreenhouseCallbacks() {
    GreenhouseHA::onPumpCommand([](bool state) {
        Serial.println(state ? "Pumpe EIN" : "Pumpe AUS");
        if (state) pump_control.TurnPumpOn();
        else pump_control.TurnPumpOff();
    });

    GreenhouseHA::onFanCommand([](bool state) {
        digitalWrite(fan_pin, state ? HIGH : LOW);
        Serial.println(state ? "Luefter EIN" : "Luefter AUS");
    });

    GreenhouseHA::onValveCommand([](uint8_t idx, bool state) {
        Serial.printf("Ventil %d %s\n", idx+1, state ? "AN" : "AUS");
        if (state) pump_control.OpenValve(idx);
        else pump_control.CloseValve(idx);
    });

    GreenhouseHA::onAllValvesCommand([](bool state) {
        Serial.printf("Alle Ventile %s\n", state ? "AN" : "AUS");
        pump_control.SetAllValves(state);
    });

    GreenhouseHA::onDurationSelected([](int duration) {
        Serial.printf("Dauer gewaehlt: %d Sekunden\n", duration);
        pump_control.TurnPumpOnDuration(duration);
        pump_control.OpenAllValvesDuration(duration);
    });
}