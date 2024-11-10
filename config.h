const char* ssid = "SSID";
const char* password = "PASSWORD";

#define DHTTYPE DHT11   // DHT 11
#define dht_dpin  21    // GPIO-21 for DHT sensor (update as per your ESP32 board pinout)

unsigned long previousMillis = 0; // Stores the last time LED was updated
const long interval = 100;        // Interval at which to blink (milliseconds)

const int relayone = 22;  // GPIO pins for relays (update as per your ESP32 board pinout)
const int relaytwo = 17;
const int relaythree = 16;
const int relayfour = 4;

// Thermistor configuration
const int thermistorPin = 34; // GPIO pin connected to the thermistor
const float thermistorNominal = 10000; // Nominal resistance of thermistor at 25°C
const float temperatureNominal = 25.0; // Nominal temperature in °C
const int adcResolution = 4095; // ADC resolution for ESP32 (12-bit)
const float offset = 18.9;

DHT dht(dht_dpin, DHTTYPE);

WebServer server(80);

float h = 0.0;  // Humidity level
float t = 0.0;  // Temperature in Celsius