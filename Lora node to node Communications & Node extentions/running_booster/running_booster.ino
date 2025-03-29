#include <SPI.h>
#include <LoRa.h>

// Define the pins used by the LoRa module
#define ss 16
#define rst 14
#define dio0 26
String receivedData = "";
int counter = 0;

void setupLoRaReceiver() {
  // Initialize the LoRa module for receiving
  Serial.println("LoRa Receiver Initializing");
  LoRa.setPins(ss, rst, dio0);

  while (!LoRa.begin(866E6)) {
    Serial.print(".");
    delay(500);
  }
  LoRa.setSyncWord(0xF3);
  LoRa.setSpreadingFactor(7);  // Set spreading factor for the receiver
  Serial.println("LoRa Receiver Initialized!");
}

void setupLoRaSender() {
  // Initialize the LoRa module for sending
  Serial.println("LoRa Sender Initializing");
  LoRa.setPins(ss, rst, dio0);

  while (!LoRa.begin(866E6)) {
    Serial.print(".");
    delay(500);
  }
  LoRa.setSyncWord(0xF3);
  LoRa.setSpreadingFactor(8);  // Set spreading factor for the sender
  Serial.println("LoRa Sender Initialized!");
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  setupLoRaReceiver();  // Initialize the LoRa module for receiving
}

void loop() {
  // Check for incoming packets
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Received a packet
    receivedData = "";
    while (LoRa.available()) {
      receivedData += (char)LoRa.read();
    }
    Serial.print("Received packet: ");
    Serial.println(receivedData);
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());

    // Reconfigure for sending mode
    LoRa.end();
    setupLoRaSender();
    
    // Send the received data
    Serial.print("Sending packet: ");
    Serial.println(counter);

    LoRa.beginPacket();
    LoRa.print(receivedData);
    LoRa.endPacket();

    counter++;
    receivedData = "";  // Clear the data after sending

    // Reconfigure for receiving mode
    LoRa.end();
    setupLoRaReceiver();

    delay(5000);  // Delay before checking for new packets
  }
}
