#ifndef SOIL_SENSORS_H
#define SOIL_SENSORS_H

#include <Arduino.h>

class SoilSensors {
public:
  /**
   * @param mux_sig_pin    GPIO-Nummer des gemeinsamen Analogeingangs (Z-Pin) des CD74HC4067
   * @param mux_s_pins     Integer-Array mit genau 3 Einträgen: {S0, S1, S2}-GPIOs am ESP32
   * @param num_sensors    Anzahl der angeschlossenen Bodenfeuchtesensoren (hier: 8)
   */
  SoilSensors(int mux_sig_pin, int mux_s_pins[], int num_sensors);

  /** Initialisiert die MUX-Pins und bereitet den ADC vor */
  void Init();

  /**
   * Liest den Rohwert (0–4095 bei ESP32-ADC 12 Bit) von einem bestimmten Sensor (1-basiert von 1…num_sensors)
   *
   * @param selected_sensor Index des Sensors (1…num_sensors)
   * @return Rohwert 0–4095 (ESP32-ADC), oder -1 bei ungültigem Index
   */
  int GetRawValue(int selected_sensor);

  /**
   * Wandelt einen Rohwert (0–4095) in einen Feuchte-Prozentsatz 0–100 um
   * unter Verwendung von Kalibrierwerten pro Sensor.
   *
   * @param soil_raw         Rohwert des Sensors
   * @param selected_sensor  Index des Sensors (1-basiert)
   * @return Feuchte in Prozent (0–100) oder -1 bei Fehler
   */
  int DecodeMoisture(int soil_raw, int selected_sensor);

  /**
   * Einfache Filterung des Rohwerts (Exponential Smoothing)
   *
   * @param soil_raw         Rohwert des Sensors
   * @param selected_sensor  Index des Sensors (1-basiert)
   * @return Gefilterter Rohwert (float, auf int gerundet)
   */
  int FilterRawValue(int soil_raw, int selected_sensor);

  /**
   * Setzt die Kalibrierwerte (trocken/nass) für einen Sensor
   *
   * @param selected_sensor  Index des Sensors (1-basiert)
   * @param raw_dry          Rohwert, der „trocken“ repräsentiert
   * @param raw_wet          Rohwert, der „nass“ repräsentiert
   */
  void SetCalibration(int selected_sensor, int raw_dry, int raw_wet);

  /**
   * Neue Funktion: Liest, filtert und dekodiert den Feuchtewert in Prozent
   * @param selected_sensor Sensorindex (1-basiert)
   * @return Feuchte in Prozent (0–100) oder -1 bei Fehler
   */
  int GetMoisturePercent(int selected_sensor);

private:
  int num_sensors_;           // Anzahl der Sensoren (8)
  int mux_sig_pin_;           // ADC-Eingang (Z) des MUX
  int mux_s_pins_[3];         // GPIOs für S0, S1, S2
  int raw_dry_[8]   = {0};    // Kalibrierwerte „trocken“ pro Sensor
  int raw_wet_[8]   = {0};    // Kalibrierwerte „nass“ pro Sensor
  float filtered_raw_[8] = {0}; // internes Filter-Array

  /** 
   * Setzt die Select-Leitungen S0…S2 auf den Binärwert (0…7) 
   * @param channel Binärwert 0…7 für Y0…Y7 
   */
  void setMuxChannel(int channel);
};

#endif // SOIL_SENSORS_H
