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

DHTesp dht1;

//int soilsensor_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
int num_sensors = 8;
int pump_pin = 13;
int valve_pins[] = {16, 17, 18, 19, 20, 21, 22, 23};
int num_valves = sizeof(valve_pins) / sizeof(valve_pins[0]);

bool pumpOn = false;
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 3000;
unsigned long pumpStartTime = 0;
unsigned long pumpDurationMs = 60000; // Standard: 60s


PumpControl pump_control(pump_pin, valve_pins, num_valves);


#define SOIL_PIN 34          // Analog-Pin für Bodenfeuchte

/*
const char* ssid = "Marcell’s iPhone";
const char* password = "b17kx09azkmjk";
IPAddress brokerAddr(172,20,10,10);   // MQTT-Broker-IP
const char* mqttUser = "mqtt-user";
const char* mqttPassword = "1q2w3e4r5t";*/

const char* ssid = "OWP-Mesh";
const char* password = "jbmaitk_106";
IPAddress brokerAddr(192,168,68,62);   // MQTT-Broker-IP
const char* mqttUser = "mqtt-user";
const char* mqttPassword = "jbmaitk_106";

WiFiClient wifiClient;
HADevice device("Greenhouse");
HAMqtt mqtt(wifiClient, device);

// Home Assistant Komponenten
HASwitch pumpSwitch("greenhouse_pump");
HABinarySensor pumpStatus("greenhouse_pump_status");

// Array für 8 Bodenfeuchte-Sensoren
HASensorNumber soilSensors[8] = {
    HASensorNumber("greenhouse_soil_moisture_1", HASensorNumber::PrecisionP0),
    HASensorNumber("greenhouse_soil_moisture_2", HASensorNumber::PrecisionP0),
    HASensorNumber("greenhouse_soil_moisture_3", HASensorNumber::PrecisionP0),
    HASensorNumber("greenhouse_soil_moisture_4", HASensorNumber::PrecisionP0),
    HASensorNumber("greenhouse_soil_moisture_5", HASensorNumber::PrecisionP0),
    HASensorNumber("greenhouse_soil_moisture_6", HASensorNumber::PrecisionP0),
    HASensorNumber("greenhouse_soil_moisture_7", HASensorNumber::PrecisionP0),
    HASensorNumber("greenhouse_soil_moisture_8", HASensorNumber::PrecisionP0)
};
uint8_t numberSoilSensors = sizeof(soilSensors) / sizeof(soilSensors[0]);

HASelect durationSelect("watering_duration");
const char* durations = {"30;60;90"};

void pumpSwitchCallback(bool state, HASwitch* sender);
void onDurationSelected(int8_t index, HASelect* sender);


void setup() {
  Serial.begin(115200);
  dht1.setup(32, DHTesp::DHT22); // Connect DHT sensor to GPIO D2
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

  pump_control.Init();
  pump_control.EnableDebug(true);
  pump_control.setPumpMaxOnTime(60); // Maximal 60 Sekunden Pumpenlaufzeit
  pump_control.setValveMaxOnTime(60); // Maximal 60 Sekunden Ventile-Offen. Macht das überhaupt Sinn?
  pump_control.TurnPumpOnDuration(10);
  pump_control.OpenValveDuration(0, 5);

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

  for (int i = 0; i < numberSoilSensors; i++) {
        char name[20];
        snprintf(name, sizeof(name), "Bodenfeuchte_%d", i+1);
        soilSensors[i].setName(name);
        soilSensors[i].setUnitOfMeasurement("%");
        soilSensors[i].setDeviceClass("humidity");
        Serial.println(name);
  }

  // Komponenten registrieren
  mqtt.begin(brokerAddr,mqttUser,mqttPassword);

}

void loop() {
 //Serial.println(millis()/1000);
 pumpOn = pump_control.Update();

 mqtt.loop();

if (millis() - lastUpdate > updateInterval) {

    float h = dht1.getHumidity();
    float t = dht1.getTemperature();

    int raw = 2000;
    float soilPercent = map(raw, 4095, 0, 0, 100); // Anpassen falls nötig
    for (int i = 0; i < numberSoilSensors; i++) {
        soilSensors[i].setValue(soilPercent);
    }
    pumpSwitch.setState(pumpOn); 
    pumpStatus.setState(pumpOn);  // zur Sicherheit aktualisieren
    lastUpdate = millis();

    Serial.print("feuchte: ");
    Serial.print(h);
    Serial.println(" %");
}
}

void pumpSwitchCallback(bool state, HASwitch* sender) {
    Serial.print(state ? "Pumpe EIN " : "Pumpe AUS ");
    Serial.println(state);

    sender->setState(state);  // Rückmeldung an Home Assistant
    
    if (state) {
      pump_control.TurnPumpOn();
    } else {
      pump_control.TurnPumpOff();
    }
}

void onDurationSelected(int8_t index, HASelect* sender) {
    const int options[] = {30, 60, 90};
    if (index >= 0 && index < sizeof(options)/sizeof(options[0])) {
        int wateringDurationSeconds = options[index];
        Serial.print("Dauer gewählt: ");
        Serial.println(wateringDurationSeconds);
        pump_control.TurnPumpOnDuration(wateringDurationSeconds);
    } else {
        Serial.println("Ungültiger Index für Dauer");
    }
}

