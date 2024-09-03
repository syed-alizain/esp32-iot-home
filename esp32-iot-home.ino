#include "Arduino.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "DHT.h"        // DHT11 temperature and humidity sensor Predefined library
#include "config.h"

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

void sendTemp0() {
    server.send(200, "text/plain", getTemp0());
}

void sendTemp1() {
    server.send(200, "text/plain", getTemp1());
}

void sendHumidity0() {
    server.send(200, "text/plain", getHumidity0());
}

// Define routing
void restServerRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", "Home Automation Server");
    });
    server.on("/temperature0", HTTP_GET, sendTemp0);
    server.on("/temperature1", HTTP_GET, sendTemp1);
    server.on("/humidity0", HTTP_GET, sendHumidity0);
    server.on("/relay", HTTP_GET, handleRelays);
}

// Manage not found URL
void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void setup() {
    dht.begin();
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(relayone, OUTPUT);
    pinMode(relaytwo, OUTPUT);
    pinMode(relaythree, OUTPUT);
    pinMode(relayfour, OUTPUT);

    digitalWrite(relayone, HIGH);
    digitalWrite(relaytwo, HIGH);
    digitalWrite(relaythree, HIGH);
    digitalWrite(relayfour, HIGH);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        handleLEDStatus();
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Activate mDNS
    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }

    // Set server routing
    restServerRouting();
    // Set not found response
    server.onNotFound(handleNotFound);
    // Start server
    server.begin();
    Serial.println("HTTP server started");
}

String getTemp0() {
    handleLEDStatus();
    t = dht.readTemperature();
    Serial.print("temp: ");
    Serial.println(t);
    return String(t);
}

String getTemp1() {
    handleLEDStatus();
    int adcValue = analogRead(thermistorPin);
    float resistance = (float)(adcResolution - adcValue) * thermistorNominal / adcValue;
    Serial.print(adcValue);
    Serial.println();
    Serial.print(resistance);
    float steinhart;
    steinhart = resistance / thermistorNominal;                 // (R/Ro)
    steinhart = log(steinhart);                                // ln(R/Ro)
    steinhart /= 3950.0;                                      // 1/B * (1/T - 1/To)
    steinhart += 1.0 / (temperatureNominal + 273.15);          // + (1/To)
    steinhart = 1.0 / steinhart;                              // Invert
    steinhart -= 273.15;                                      // Convert to Celsius
    steinhart += offset;
    //steinhart *= 1;

    Serial.print("thermistor temp: ");
    Serial.println(steinhart);
    return String(steinhart);
}

String getHumidity0() {
    handleLEDStatus();
    h = dht.readHumidity();
    Serial.print("humidity: ");
    Serial.println(h);
    return String(h);
}

void handleRelays() {
    handleLEDStatus();
    String action = server.arg("action");
    int relayNum = server.arg("num").toInt();

    if (action == "on") {
        Serial.println("Turning relay " + String(relayNum) + " ON");
        digitalWrite(relayNumber(relayNum), LOW);
        server.send(200, "application/json", "Relay " + String(relayNum) + " is ON");
    } else if (action == "off") {
        digitalWrite(relayNumber(relayNum), HIGH);
        server.send(200, "application/json", "Relay " + String(relayNum) + " is OFF");
    } else {
        server.send(400, "application/json", "Bad Request");
    }
    handleLEDStatus();
}

int relayNumber(int relayNum) {
    if (relayNum == 1) {
        return relayone;
    } else if (relayNum == 2) {
        return relaytwo;
    } else if (relayNum == 3) {
        return relaythree;
    } else if (relayNum == 4) {
        return relayfour;
    } else {
        return -1;
    }
}

// Function to handle LED blinking
void handleLEDStatus() {
  unsigned long currentMillis = millis();

  // Check if it's time to toggle the LED
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Toggle the LED state
    int ledState = digitalRead(LED_BUILTIN);
    digitalWrite(LED_BUILTIN, !ledState);
  }
}

void loop() {
    server.handleClient();
    digitalWrite(LED_BUILTIN, 0);
}
