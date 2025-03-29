#include <WiFi.h>
#include <PubSubClient.h>
#include <AESLib.h>

// WiFi credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// MQTT broker details
const char* mqtt_server = "broker.hivemq.com"; // Replace with your broker address
const char* mqtt_topic = "esp32/sensor/data";

WiFiClient espClient;
PubSubClient client(espClient);

// AES configuration
AESLib aesLib;
byte aes_key[16] = {
    0x75, 0x66, 0x09, 0x05,0x44, 0x96, 0x30, 0x14,0x75, 0x66, 0x09, 0x05,0x44, 0x96, 0x30, 0x14};// 16-byte key for AES-128
byte aes_iv[16] = { 
    0x96, 0x30, 0x14, 0x01, 0x05, 0x75, 0x66, 0x09, 0x96, 0x30, 0x14, 0x01, 0x05, 0x75, 0x66, 0x09 
}; // 16-byte IV

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("WiFi connected");

    client.setServer(mqtt_server, 1883);

    while (!client.connected()) {
        Serial.println("Connecting to MQTT broker...");
        if (client.connect("ESP32Client")) {
            Serial.println("Connected to MQTT broker");
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            delay(5000);
        }
    }
}

void loop() {
    if (!client.connected()) {
        client.connect("ESP32Client");
    }
    client.loop();

    // Example plaintext data
    String plainText = "Temperature:25.5,Humidity:60";

    // Encrypt data
    char encryptedText[64];
    aesLib.encrypt64(encryptedText, plainText.c_str(), aes_key, aes_iv);

    // Send encrypted data to MQTT topic
    client.publish(mqtt_topic, encryptedText);

    delay(5000); // Send data every 5 seconds
}
