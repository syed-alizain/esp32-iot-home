#include <driver/ledc.h>

const int pwmPin = 25;    // GPIO pin connected to the fan's PWM control input (blue wire)
const int tachPin = 26;   // GPIO pin connected to the fan's tachometer output (yellow wire)

// PWM settings
const int freq = 25000;     // PWM frequency (25 kHz typical for fans)
const int resolution = 8;   // PWM resolution (8-bit = 0-255)
const int pwmChannel = 0;   // PWM channel number

// Fan and RPM tracking
volatile unsigned long pulseCount = 0;
const int pulsesPerRevolution = 2;  // Adjust this according to your fan's spec (often 2 pulses per revolution)
unsigned long lastMillis = 0;       // Time of last RPM calculation

// Function prototypes
void countPulses();
float calculateRPM();
void handleGetRPM();
void handleSetDuty();

void fanControl() {

  // Set up PWM for fan control
  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(pwmPin, pwmChannel);
  ledcWrite(pwmChannel, 50);  // Default to low duty cycle

  // Tachometer input with interrupt
  pinMode(tachPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(tachPin), countPulses, RISING);

  // Define HTTP routes
  server.on("/get_rpm", HTTP_GET, handleGetRPM);
  server.on("/set_duty", HTTP_GET, handleSetDuty);

}

// Function to handle the /get_rpm request
void handleGetRPM() {
  float rpm = calculateRPM();
  server.send(200, "text/plain", String(rpm));
}

// Function to handle the /set_duty request
void handleSetDuty() {
  if (server.hasArg("value")) {
    int dutyCycle = server.arg("value").toInt();
    dutyCycle = constrain(dutyCycle, 0, 255); // Clamp duty cycle within 0-255
    ledcWrite(pwmChannel, dutyCycle);
    server.send(200, "text/plain", "Duty cycle set to: " + String(dutyCycle));
  } else {
    server.send(400, "text/plain", "Duty cycle value not provided");
  }
}

// Function to calculate RPM based on pulse count
float calculateRPM() {
  unsigned long currentMillis = millis();
  unsigned long elapsedMillis = currentMillis - lastMillis;

  if (elapsedMillis >= 1000) {  // Calculate every 1 second
    lastMillis = currentMillis;

    // Calculate RPM: (pulseCount / pulsesPerRevolution) * (60 seconds / elapsed time in seconds)
    float rpm = (pulseCount * 60000.0) / (pulsesPerRevolution * elapsedMillis);
    pulseCount = 0;  // Reset pulse count for the next interval
    return rpm;
  }
  return -1;  // Return -1 to indicate RPM wasn't updated this cycle
}

// Interrupt function to count pulses from tachometer
void IRAM_ATTR countPulses() {
  pulseCount++;
}
