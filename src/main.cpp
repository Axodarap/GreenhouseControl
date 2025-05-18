#include <Arduino.h>
#include <WiFi.h>
#include "SoilSensors.h"
#include "EnvironmentalSensor.h"
#include "PumpControl.h"
#include "DHTesp.h"
#include "ArduinoHA.h"


//Mögliche Pins
// 1: Pumpe
// 1: FAN
// 2: Fenster
// 8: Ventil
// 2: Temp/Feuchte Innen/Außen
// 8: Feuchtesensoren

//int soilsensor_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
int num_sensors = 8;
int pump_pin = 2;
int valve_pins[] = {8, 9, 10, 11, 12, 13, 14, 15};
int num_valves = sizeof(valve_pins) / sizeof(valve_pins[0]);

bool pumpOn = false;
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 5000;
unsigned long pumpStartTime = 0;
unsigned long pumpDurationMs = 60000; // Standard: 60s


PumpControl pump_control(pump_pin, valve_pins, num_valves);

#define PUMP_PIN 5           // GPIO zur Steuerung der Pumpe
#define SOIL_PIN 34          // Analog-Pin für Bodenfeuchte

const char* ssid = "Marcell’s iPhone";
const char* password = "b17kx09azkmjk";

IPAddress brokerAddr(172,20,10,10);   // MQTT-Broker-IP
const char* mqttUser = "mqtt-User";
const char* mqttPassword = "1q2w3e4r5t";

WiFiClient wifiClient;
HADevice device("Greenhouse");
HAMqtt mqtt(wifiClient, device);

// Home Assistant Komponenten
HASwitch pumpSwitch("greenhouse_pump");
HABinarySensor pumpStatus("greenhouse_pump_status");
HASensorNumber soilSensor("greenhouse_soil_moisture", HASensorNumber::PrecisionP1);

HASelect durationSelect("watering_duration");
const char* durations = {"30;60;90"};

void pumpSwitchCallback(bool state, HASwitch* sender);
void onDurationSelected(int8_t index, HASelect* sender);


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

  Serial.begin(115200);
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW);

    // WLAN verbinden
    WiFi.begin(ssid, password);
    Serial.print("Verbinde mit WLAN");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWLAN verbunden. IP: " + WiFi.localIP().toString());

    // Gerätedetails
    device.setName("ESP32-Greenhouse");
    device.setSoftwareVersion("1.0.0");

    // Konfiguration der Komponenten
    pumpSwitch.setName("Pumpe");
    pumpSwitch.onCommand(pumpSwitchCallback);

     // Select konfigurieren
    durationSelect.setName("Dauer");
    durationSelect.setOptions(durations);
    durationSelect.onCommand(onDurationSelected);

    pumpStatus.setName("Pumpenstatus");
    pumpStatus.setDeviceClass("running");

    soilSensor.setName("Bodenfeuchte");
    soilSensor.setUnitOfMeasurement("%");
    soilSensor.setDeviceClass("humidity");

    // Komponenten registrieren
    mqtt.begin(brokerAddr,mqttUser,mqttPassword);

}

void loop() {
 //Serial.println(millis()/1000);
 //pump_control.Update();

 mqtt.loop();

if (millis() - lastUpdate > updateInterval) {
    int raw = 2000;
    float soilPercent = map(raw, 4095, 0, 0, 100); // Anpassen falls nötig
    soilSensor.setValue(soilPercent);
    pumpStatus.setState(pumpOn);  // zur Sicherheit aktualisieren
    lastUpdate = millis();

    Serial.print("Bodenfeuchte: ");
    Serial.print(soilPercent);
    Serial.println(" %");
}
}

void pumpSwitchCallback(bool state, HASwitch* sender) {
    pumpOn = state;
    digitalWrite(PUMP_PIN, pumpOn ? HIGH : LOW);
    sender->setState(pumpOn);  // Rückmeldung an Home Assistant
    Serial.println(pumpOn ? "Pumpe EIN" : "Pumpe AUS");
}

void onDurationSelected(int8_t index, HASelect* sender) {
    const int options[] = {30, 60, 90};
    if (index >= 0 && index < sizeof(options)/sizeof(options[0])) {
        int wateringDurationSeconds = options[index];
        Serial.print("Dauer gewählt: ");
        Serial.println(wateringDurationSeconds);
    } else {
        Serial.println("Ungültiger Index für Dauer");
    }
}