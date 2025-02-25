void buzzer(int buzzerDelay) {
    digitalWrite(buzzerPin, HIGH);
    delay(buzzerDelay);
    digitalWrite(buzzerPin, LOW);
    delay(buzzerDelay);
}

void handleLEDStatus() {
  buzzer(1);
  unsigned long currentMillis = millis();

  // Check if it's time to toggle the LED
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Toggle the LED state
    int ledState = digitalRead(LED_BUILTIN);
    digitalWrite(LED_BUILTIN, !ledState);
  }
}

void handleBuzzer() {
  if (server.hasArg("delay")) {
    int buzzDelay = server.arg("delay").toInt();
    buzzDelay = constrain(buzzDelay, 0, 5000);
    buzzer(buzzDelay);
    server.send(200, "text/plain", "Buzzer Delay: " + String(buzzDelay));
  } else {
    server.send(400, "text/plain", "Delay time not provided.");
  }
}

// Function to handle relay control
void handleRelayControl() {
    String path = server.uri();
    Serial.println("Request: " + path);

    if (path.startsWith("/relay") && path.length() > 8) {
        int relayNum = path.substring(6, path.indexOf("=")).toInt();
        String action = path.substring(path.indexOf("=") + 1);

        // Validate relay number (1-16)
        if (relayNum < 1 || relayNum > 16) {
            server.send(400, "text/plain", "Invalid relay number. Use 1-16.");
            return;
        }

        int pin = relayNum - 1; // Convert relay number (1-16) to PCF8575 pin (0-15)

        // Process action
        if (action == "on") {
            pcf8575.write(pin, LOW);
            Serial.println("Relay " + String(relayNum) + " ON");
            server.send(200, "text/plain", "Relay " + String(relayNum) + " is ON");
            buzzer(200);
        } else if (action == "off") {
            pcf8575.write(pin, HIGH);
            Serial.println("Relay " + String(relayNum) + " OFF");
            server.send(200, "text/plain", "Relay " + String(relayNum) + " is OFF");
            buzzer(200);
        } else {
            server.send(400, "text/plain", "Invalid action. Use on/off.");
        }
    } else {
        server.send(404, "text/plain", "Invalid request. Use /relayX=on or /relayX=off");
    }
}

// Function to return LDR value
void handleLDR() {
    int lightValue = analogRead(LDR_PIN);
    Serial.println("Light Sensor Value: " + String(lightValue));
    server.send(200, "text/plain", String(lightValue));  // Send value as plain text
    buzzer(100);
}
