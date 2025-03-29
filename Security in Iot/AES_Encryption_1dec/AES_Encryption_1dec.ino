#include <WiFi.h>
#include <PubSubClient.h>
#include <Crypto.h>
#include <AES.h>

const char* ssid = "PratikOne";
const char* password = "pratik1234";
const char* mqtt_server = "192.168.52.84";
const char* topic = "esp32/aes/data";

WiFiClient espClient;
PubSubClient client(espClient);

byte aesKey[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 }; // 16-byte AES key
AES128 aes;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  client.setServer(mqtt_server, 1883);
  
  // Set buffer size for larger payloads
  client.setBufferSize(256);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Generate 8-bit binary data
  String binaryData = String(random(0, 256), BIN);
  while (binaryData.length() < 8) binaryData = "0" + binaryData;

  // Start time measurement for padding
  unsigned long startPaddingTime = micros();

  // Padding the binary data to fit AES block size (16 bytes)
  byte plaintext[16] = {0};
  binaryData.getBytes(plaintext, 16); // Pad the binary data to fit AES block size

  // End time measurement for padding
  unsigned long endPaddingTime = micros();
  unsigned long paddingTime = endPaddingTime - startPaddingTime;

  // Start time measurement for encryption
  unsigned long startEncryptionTime = micros();

  aes.setKey(aesKey, sizeof(aesKey));
  byte ciphertext[16] = {0};
  aes.encryptBlock(ciphertext, plaintext);

  // End time measurement for encryption
  unsigned long endEncryptionTime = micros();
  unsigned long encryptionTime = endEncryptionTime - startEncryptionTime;

  // Calculate the total time (padding + encryption)
  unsigned long totalTime = paddingTime + encryptionTime;

  // Output padding, encryption, and total times in microseconds
  Serial.print("Padding Time: ");
  Serial.print(paddingTime);
  Serial.println(" us");

  Serial.print("Encryption Time: ");
  Serial.print(encryptionTime);
  Serial.println(" us");

  Serial.print("Total Time (Padding + Encryption): ");
  Serial.print(totalTime);
  Serial.println(" us");

  // Publish encrypted data
  String encryptedString;
  for (int i = 0; i < 16; i++) {
    encryptedString += String(ciphertext[i], HEX);
  }
  client.publish(topic, encryptedString.c_str());
  Serial.print("Encrypted Data Sent: ");
  Serial.println(encryptedString);

  delay(2000); // Adjust as needed
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Sender")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
