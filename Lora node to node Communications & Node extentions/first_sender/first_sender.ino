#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"


#define DHTPIN 4      // Selecting the pin at which we have connected DHT11
#define DHTTYPE DHT11 
 
 DHT dht ( DHTPIN, DHTTYPE );

//define the pins used by the transceiver module
#define ss 16
#define rst 14
#define dio0 26

//#define ss 10
//#define rst 9
//#define dio0 2

int counter = 0;

void setup() {
  //initialize Serial Monitor

  dht.begin();
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");
   Serial.println();
    Serial.println();
   Serial.println(" Temperature");
    Serial.println("And Humidity");
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(866E6)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
  LoRa.setSpreadingFactor(7);           // ranges from 6-12,default 7 see API docs
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);
float humidity = dht.readHumidity();
 float temprature = dht.readTemperature();
 
Serial.print("current humidity=");
  Serial.print(humidity);
  Serial.print("%,");
  Serial.print("current temperature=");
  Serial.print(temprature);
  Serial.println("C");

 
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(temprature);
   LoRa.print(" ");
  LoRa.print("C");
   LoRa.print("  ");
   
//LoRa.endPacket();
//
//  LoRa.beginPacket();
    LoRa.print(humidity);
  LoRa.endPacket();

  counter++;

  delay(2000);
}