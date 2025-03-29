#include <HardwareSerial.h>

// Use HardwareSerial port 2 (UART2)
HardwareSerial SerialBT(2);

// Define LED pins
const int led1Pin = 19; // LED 1 connected to GPIO 13
const int led2Pin = 23; // LED 2 connected to GPIO 14

void setup() {
  // Start the built-in Serial Monitor
  Serial.begin(115200);
  
  // Start the HC-05 Serial connection at 9600 baud rate
  SerialBT.begin(9600, SERIAL_8N1, 22, 21); // RX=16, TX=17

  // Set LED pins as outputs
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  Serial.println("ESP32 & HC-05 Bluetooth LED Control Example");
}

void loop() {
  if (Serial.available()) {
    // Read from Serial Monitor and send to HC-05
    char c = Serial.read();
    SerialBT.write(c);
  }

  if (SerialBT.available()) {
    // Read from HC-05
    char c = SerialBT.read();
    Serial.write(c);

    // Control LEDs based on received character
    switch (c) {
      case '1':
        // Turn on LED 1
        digitalWrite(led1Pin, HIGH);
        Serial.println("LED 1 ON");
        break;
      case '2':
        // Turn off LED 1
        digitalWrite(led1Pin, LOW);
        Serial.println("LED 1 OFF");
        break;
      case '3':
        // Turn on LED 2
        digitalWrite(led2Pin, HIGH);
        Serial.println("LED 2 ON");
        break;
      case '4':
        // Turn off LED 2
        digitalWrite(led2Pin, LOW);
        Serial.println("LED 2 OFF");
        break;
      default:
        Serial.println("Invalid command");
        break;
    }
  }

  // Add a small delay to prevent reading the same character twice
  delay(50);
}
