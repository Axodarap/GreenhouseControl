#include "SoilSensors.h"
#include <Arduino.h>

/**
 * constructor
 *
 * mux_sig_pin  : GPIO-Nummer, an der der gemeinsame Analog-Pin (Z) des CD74HC4067 hängt
 * mux_s_pins[] : Array mit 3 Elementen: {GPIO_S0, GPIO_S1, GPIO_S2}
 * num_sensors  : Anzahl der Sensoren (maximal 8, da wir S3 = GND festlegen)
 */
SoilSensors::SoilSensors(int mux_sig_pin, int mux_s_pins[], int num_sensors)
  : num_sensors_(num_sensors),
    mux_sig_pin_(mux_sig_pin)
{
  // Kopiere die drei Select-Pins (S0, S1, S2) in unser Member-Array
  for (int i = 0; i < 3; i++) {
    mux_s_pins_[i] = mux_s_pins[i];
  }

  // Initialisiere das Filter-Array auf 0
  for (int i = 0; i < num_sensors_; i++) {
    filtered_raw_[i] = 0.0f;
  }
}

/**
 * Initialisiert den MUX und den ADC-Pin
 */
void SoilSensors::Init() {
  // Setze S0, S1, S2 als OUTPUT
  for (int i = 0; i < 3; i++) {
    pinMode(mux_s_pins_[i], OUTPUT);
    digitalWrite(mux_s_pins_[i], LOW); // initial auf Kanal 0
  }

  // Setze den gemeinsamen MUX-Pin (Z) als Input (ADC)
  pinMode(mux_sig_pin_, INPUT);
}

/**
 * Setzt den MUX-Kanal (0…7) über die drei Select-Leitungen
 */
void SoilSensors::setMuxChannel(int channel) {
  // channel muss im Bereich 0…7 sein
  if (channel < 0)   channel = 0;
  if (channel > 7)   channel = 7;

  // Extrahiere die Bits
  // S0 = LSB, S1 = Bit 1, S2 = Bit 2
  digitalWrite(mux_s_pins_[0], (channel & 0x01) ? HIGH : LOW);       // S0
  digitalWrite(mux_s_pins_[1], (channel & 0x02) ? HIGH : LOW);       // S1
  digitalWrite(mux_s_pins_[2], (channel & 0x04) ? HIGH : LOW);       // S2

  // Hinweis: S3 ist fest auf GND gezogen (Hardware)
}

/**
 * Liest den Rohwert eines einzelnen Bodensensors (1-basiert)
 *
 * @param selected_sensor Sensorindex 1…num_sensors_ (1…8)
 * @return Rohwert (0…4095) oder -1 bei ungültigem Index
 */
int SoilSensors::GetRawValue(int selected_sensor) {
  // Prüfe gültigen Bereich
  if (selected_sensor < 1 || selected_sensor > num_sensors_) {
    return -1;
  }

  // MUX-Kanal = selected_sensor − 1 (da unser Kanalbereich 0…7 ist)
  int channel = selected_sensor - 1;
  setMuxChannel(channel);

  // Kurze kleine Verzögerung, damit der MUX-Umschaltvorgang durch ist
  delayMicroseconds(5);

  // Lese den ADC-Wert ein (ESP32: analogRead liefert 0…4095)
  int raw = analogRead(mux_sig_pin_);
  return raw;
}

/**
 * Wandelt den Rohwert in eine Feuchte-Prozentzahl um (0…100)
 *
 * @param soil_raw         Rohwert (0…4095)
 * @param selected_sensor  Sensorindex 1…num_sensors_
 * @return Feuchte 0…100 oder -1 bei Fehler
 */
int SoilSensors::DecodeMoisture(int soil_raw, int selected_sensor) {
  // Prüfe gültigen Bereich
  if (selected_sensor < 1 || selected_sensor > num_sensors_) {
    return -1;
  }

  int idx = selected_sensor - 1;
  int dry = raw_dry_[idx];
  int wet = raw_wet_[idx];

  // Absicherung: dry und wet dürfen nicht gleich sein
  if (dry == wet) {
    return -1;
  }

  // ESP32-ADC hat 12 Bit (0…4095). Wir mapen trotzdem linear auf 0…100.
  int moisture = map(soil_raw, dry, wet, 0, 100);

  // Clamp auf 0…100
  if (moisture < 0)   moisture = 0;
  if (moisture > 100) moisture = 100;

  return moisture;
}

/**
 * Filtert den Rohwert mittels Exponential Smoothing
 *
 * @param soil_raw        Rohwert (0…4095)
 * @param selected_sensor Sensorindex 1…num_sensors_
 * @return Gefilterter Rohwert (float gerundet)
 */
int SoilSensors::FilterRawValue(int soil_raw, int selected_sensor) {
  if (selected_sensor < 1 || selected_sensor > num_sensors_) {
    return soil_raw;
  }

  int idx = selected_sensor - 1;
  float alpha = 0.8f;  // Glättungsfaktor
  filtered_raw_[idx] = alpha * soil_raw + (1.0f - alpha) * filtered_raw_[idx];
  return static_cast<int>(filtered_raw_[idx]);
}

/**
 * Setzt Kalibrierwerte (trocken/nass) für einen Sensor
 */
void SoilSensors::SetCalibration(int selected_sensor, int raw_dry, int raw_wet) {
  if (selected_sensor < 1 || selected_sensor > num_sensors_) {
    return;
  }

  int idx = selected_sensor - 1;
  raw_dry_[idx] = raw_dry;
  raw_wet_[idx] = raw_wet;
}

/**
 * Neue Funktion: Liest, filtert und dekodiert den Feuchtewert in Prozent
 */
int SoilSensors::GetMoisturePercent(int selected_sensor) {
  int raw = GetRawValue(selected_sensor);
  if (raw < 0) {
    return -1;
  }
  int filt = FilterRawValue(raw, selected_sensor);
  int pct  = DecodeMoisture(filt, selected_sensor);
  return pct;
}
