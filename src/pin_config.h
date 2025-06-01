// Benötigte Pins
// 1 Pin:   Pumpe,       Digital IN/OUT
// 1 Pin:   FAN,         Digital IN/OUT
// 2 Pins:  Fenster,     Digital IN/OUT
// 8 Pins:  Ventile,     Digital IN/OUT
// 2 Pins:  DHT22,       Digital IN/OUT
// 8 Kanäle: Feuchtesensoren über CD74HC4067 (analog)

// ===============================
// Digital Outputs (Relais)
// ===============================
#define PIN_PUMPE         2   // Relais: Pumpe
#define PIN_LUEFTER       4   // Relais: Lüfter
#define PIN_FENSTER_AUF   16  // Relais: Fenster öffnen
#define PIN_FENSTER_ZU    17  // Relais: Fenster schließen

#define PIN_VENTIL_1      18  // Relais: Ventil 1
#define PIN_VENTIL_2      19  // Relais: Ventil 2
#define PIN_VENTIL_3      21  // Relais: Ventil 3
#define PIN_VENTIL_4      22  // Relais: Ventil 4
#define PIN_VENTIL_5      23  // Relais: Ventil 5
#define PIN_VENTIL_6      25  // Relais: Ventil 6
#define PIN_VENTIL_7      26  // Relais: Ventil 7
#define PIN_VENTIL_8      27  // Relais: Ventil 8

#define NUM_VENTILE       8   // Anzahl der Ventile

// ===============================
// DHT22 Sensoren (Digital IN)
// ===============================
#define PIN_DHT_INNEN     13  // DHT22 Innen
#define PIN_DHT_AUSSEN    32  // Eigentlich 14 aber dawal is 32 anglötet DHT22 Außen

// ===============================
// Bodenfeuchte-Sensoren (8 Kanäle über CD74HC4067)
// ===============================
// CD74HC4067: S0→32, S1→33, S2→5, (S3=GND, E=GND), Z→34; Y0…Y7 → Sensoren

#define MUX_S0            14  // 32 eigenltich CD74HC4067 S0 → GPIO32
#define MUX_S1            33  // CD74HC4067 S1 → GPIO33
#define MUX_S2             5  // CD74HC4067 S2 → GPIO5
#define MUX_SIG_PIN       34  // CD74HC4067 Z  → GPIO34 (ADC)

// Anzahl der angeschlossenen Sensoren
const int NUM_SOIL_SENSORS = 8;
