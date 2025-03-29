#include <SoftwareSerial.h>

SoftwareSerial RS232Serial(10, 11); // RX, TX

const int ledPin = 8;

void setup() {
 pinMode(ledPin, OUTPUT);
RS232Serial.begin(9600); // Start the RS232 Serial communication
 Serial.begin(9600); // Start the built-in Serial communication for debugging
}

void loop() {
 if (RS232Serial.available()) {
 String message = RS232Serial.readStringUntil('\n'); // Read the incoming message
 message.trim(); // Remove any whitespace or newline characters

 Serial.print("Received message: ");
Serial.println(message); // Print the received message to the Serial Monitor

 if (message == "ON") {
digitalWrite(ledPin, HIGH); // Turn the LED on
 } else {
 digitalWrite(ledPin, LOW); // Turn the LED off
 }
}
}