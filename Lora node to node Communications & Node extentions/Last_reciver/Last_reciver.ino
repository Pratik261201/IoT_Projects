#include <SPI.h>
#include <LoRa.h>

// Define the pins used by the LoRa module
#define ss 16
#define rst 14
#define dio0 26

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver Initializing");
  
  // Initialize LoRa with the specified frequency
  LoRa.setPins(ss, rst, dio0);

  // Wait until the LoRa module is initialized
  while (!LoRa.begin(866E6)) {
    Serial.print(".");
    delay(500);
  }
  
  // Set LoRa parameters
  LoRa.setSyncWord(0xF3);
  LoRa.setSpreadingFactor(8); // Adjust spreading factor as needed
  Serial.println("LoRa Receiver Initialized!");
}

void loop() {
  // Try to parse a packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Received a packet
    Serial.print("Received packet: ");

    // Read packet
    String receivedData = "";
    while (LoRa.available()) {
      receivedData += (char)LoRa.read();
    }
    
    // Print the received data
    Serial.println(receivedData);
    
    // Print the RSSI of the received packet
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
  }
}
