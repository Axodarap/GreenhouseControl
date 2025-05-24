#include <Arduino.h>
#include <WiFi.h>
#include "SoilSensors.h"
#include "EnvironmentalSensor.h"
#include "PumpControl.h"
#include "DHTesp.h"
#include "ArduinoHA.h"
#include "pin_config.h"

//Mögliche Pins
// 1 Pin: Pumpe, Digital IN/OUT
// 1 Pin: FAN, Digital IN/OUT
// 2 Pins: Fenster, Digital IN/OUT
// 8 Pins: Ventil, Digital IN/OUT
// 2 Pins: Temp/Feuchte Innen/Außen DHT22, Digital IN/OUT
// 8 Pins: Feuchtesensoren, Analog IN


EnvironmentalSensor humtemp_outside(32); // DHT22 an GPIO 32

//int soilsensor_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
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


#define SOIL_PIN 34          // Analog-Pin für Bodenfeuchte


const char* ssid = "Marcell’s iPhone";
const char* password = "b17kx09azkmjk";
IPAddress brokerAddr(172,20,10,10);   // MQTT-Broker-IP
const char* mqttUser = "mqtt-user";
const char* mqttPassword = "1q2w3e4r5t";

/*const char* ssid = "OWP-Mesh";
const char* password = "jbmaitk_106";
IPAddress brokerAddr(192,168,68,62);   // MQTT-Broker-IP
const char* mqttUser = "mqtt-user";
const char* mqttPassword = "jbmaitk_106";*/

WiFiClient wifiClient;
HADevice device("Greenhouse");
HAMqtt mqtt(wifiClient, device);

// Home Assistant Komponenten
HASwitch pumpSwitch("greenhouse_pump");
HABinarySensor pumpStatus("greenhouse_pump_status");
HASwitch fanSwitch("greenhouse_fan");

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

HASwitch valveSwitches[8] = {
    HASwitch("greenhouse_valve_1"),
    HASwitch("greenhouse_valve_2"),
    HASwitch("greenhouse_valve_3"),
    HASwitch("greenhouse_valve_4"),
    HASwitch("greenhouse_valve_5"),
    HASwitch("greenhouse_valve_6"),
    HASwitch("greenhouse_valve_7"),
    HASwitch("greenhouse_valve_8")
};

HASwitch all_valves("greenhouse_all_valves");

uint8_t numberSoilSensors = sizeof(soilSensors) / sizeof(soilSensors[0]);
uint8_t numberValves = sizeof(valveSwitches) / sizeof(valveSwitches[0]);

HASelect durationSelect("watering_duration");


void pumpSwitchCallback(bool state, HASwitch* sender);
void onDurationSelected(int8_t index, HASelect* sender);
void valveSwitchCallback(bool state, HASwitch* sender);
void fanSwitchCallback(bool state, HASwitch* sender);

String getWateringOptionsString();

void setup() {
  Serial.begin(115200);
  humtemp_outside.Init();

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
  durationSelect.setOptions(getWateringOptionsString().c_str());
  durationSelect.onCommand(onDurationSelected);

  pumpStatus.setName("Pumpenstatus");
  pumpStatus.setDeviceClass("running");

  // Lüfter-Schalter konfigurieren
    fanSwitch.setName("Lüfter");
    fanSwitch.onCommand(fanSwitchCallback);

  for (int i = 0; i < numberSoilSensors; i++) {
        char name[20];
        snprintf(name, sizeof(name), "Bodenfeuchte_%d", i+1);
        soilSensors[i].setName(name);
        soilSensors[i].setUnitOfMeasurement("%");
        soilSensors[i].setDeviceClass("humidity");
        Serial.println(name);
  }

  for (int i = 0; i < numberValves; i++) {
        char name[20];
        snprintf(name, sizeof(name), "Ventil_%d", i+1);
        valveSwitches[i].setName(name);
        valveSwitches[i].onCommand(valveSwitchCallback);
        Serial.println(name);
  }

  all_valves.setName("Alle Ventile");
  all_valves.onCommand(valveSwitchCallback);

  

  // Komponenten registrieren
  mqtt.begin(brokerAddr,mqttUser,mqttPassword);

}

void loop() {
 //Serial.println(millis()/1000);
 pumpOn = pump_control.Update();

 mqtt.loop();

if (millis() - lastUpdate > updateInterval) {
    int raw = 2000;
    float soilPercent = map(raw, 4095, 0, 0, 100); // Anpassen falls nötig
    for (int i = 0; i < numberSoilSensors; i++) {
        soilSensors[i].setValue(soilPercent);
    }
    pumpSwitch.setState(pumpOn); 
    pumpStatus.setState(pumpOn);  // zur Sicherheit aktualisieren
    // Ventil-Status synchronisieren
    for (int i = 0; i < numberValves; i++) {
        valveSwitches[i].setState(pump_control.GetValveState(i));
    }
    all_valves.setState(pump_control.AreAllValvesOpen());
    fanSwitch.setState(digitalRead(fan_pin) == HIGH);
    lastUpdate = millis();

    Serial.print("feuchte: ");
    Serial.print(soilPercent);
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
    if (index >= 0 && index < numWateringOptions) {
        int wateringDurationSeconds = wateringOptions[index];
        Serial.print("Dauer gewählt: ");
        Serial.println(wateringDurationSeconds);
        pump_control.TurnPumpOnDuration(wateringDurationSeconds);
        pump_control.OpenAllValvesDuration(wateringDurationSeconds);
    } else {
        Serial.println("Ungültiger Index für Dauer");
    }
}

void valveSwitchCallback(bool state, HASwitch* sender) {
    if (sender == &all_valves) {
        // Alle Ventile schalten
        Serial.printf("Alle Ventile %s\n", state ? "AN" : "AUS");
        pump_control.SetAllValves(state);
        sender->setState(state); // Rückmeldung für den "Alle Ventile"-Schalter
    } else {
        // Einzelnes Ventil schalten
        for (int i = 0; i < numberValves; i++) {
            if (sender == &valveSwitches[i]) {
                Serial.printf("Ventil %d %s\n", i+1, state ? "AN" : "AUS");
                if (state) {
                    pump_control.OpenValve(i);
                } else {
                    pump_control.CloseValve(i);
                }
                sender->setState(state); // Rückmeldung für das einzelne Ventil
                break;
            }
        }
    }
}

// Callback für Lüfter-Schalter
void fanSwitchCallback(bool state, HASwitch* sender) {
    digitalWrite(fan_pin, state ? HIGH : LOW);
    sender->setState(state); // Rückmeldung an Home Assistant
    Serial.print("Lüfter ");
    Serial.println(state ? "EIN" : "AUS");
}

// Hilfsfunktion, um das Options-Array als String zu erzeugen
String getWateringOptionsString() {
    String result;
    for (int i = 0; i < numWateringOptions; i++) {
        result += String(wateringOptions[i]);
        if (i < numWateringOptions - 1) result += ";";
    }
    return result;
}
