#include <SPI.h>
#include <LoRa.h>

// Define the pins used by the LoRa module
#define ss 10
#define rst 9
#define dio0 2

// Define the pin used by the push button
#define buttonPin 3

bool lastButtonState = HIGH; // Stores the last button state

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);

  // Setup button pin
  pinMode(buttonPin, INPUT_PULLUP); // Using internal pull-up resistor

  // Setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);

  // Attempt to start LoRa module
  while (!LoRa.begin(866E6)) { // Use 433E6, 866E6, or 915E6 based on your region
    Serial.println("LoRa initialization failed. Trying again...");
    delay(500);
  }

  // Change sync word (0xF3) to match the receiver
  LoRa.setSyncWord(0xF3);
  LoRa.setSpreadingFactor(7);

  Serial.println("LoRa Initialized Successfully!");
  Serial.println("Press the button to toggle the LED.");
}

void loop() {
  // Read the button state
  int buttonState = digitalRead(buttonPin);

  // Send a message when the button is pressed
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      // Toggle message
      String message = "1"; // Send '1' when button is pressed

      // Send data over LoRa
      Serial.print("Sending packet: ");
      Serial.println(message);
      LoRa.beginPacket();
      LoRa.print(message);
      LoRa.endPacket();
    } else {
      String message = "0"; // Send '0' when button is released

      // Send data over LoRa
      Serial.print("Sending packet: ");
      Serial.println(message);
      LoRa.beginPacket();
      LoRa.print(message);
      LoRa.endPacket();
    }

    // Debounce the button
    delay(50);
  }

  lastButtonState = buttonState; // Update lastButtonState
}
