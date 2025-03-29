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
  client.setCallback(callback);

  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void reconnect() {
  static unsigned long lastAttemptTime = 0;
  const unsigned long reconnectInterval = 5000;

  if (millis() - lastAttemptTime > reconnectInterval) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Receiver")) {
      Serial.println("connected");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
    lastAttemptTime = millis();
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Convert payload to ciphertext
  byte ciphertext[16] = {0};
  for (int i = 0; i < 16 && i * 2 < length; i++) {
    char hex[3] = { (char)payload[i * 2], (char)payload[i * 2 + 1], '\0' };
    ciphertext[i] = strtoul(hex, nullptr, 16);
  }

  // Start time measurement for decryption
  unsigned long startDecryptionTime = micros();

  // Decrypt the data
  byte decryptedData[16] = {0};
  aes.setKey(aesKey, sizeof(aesKey));
  aes.decryptBlock(decryptedData, ciphertext);

  // End time measurement for decryption
  unsigned long endDecryptionTime = micros();
  unsigned long decryptionTime = endDecryptionTime - startDecryptionTime;

  // Start time measurement for padding removal
  unsigned long startPaddingTime = micros();

  // Remove PKCS#7 padding
  int paddingLength = decryptedData[15];
  if (paddingLength > 0 && paddingLength <= 16) {
    decryptedData[16 - paddingLength] = '\0';
  }

  // End time measurement for padding removal
  unsigned long endPaddingTime = micros();
  unsigned long paddingTime = endPaddingTime - startPaddingTime;

  // Calculate the total time (decryption + padding removal)
  unsigned long totalDecryptionTime = decryptionTime + paddingTime;

  // Output decryption, padding removal, and total times in microseconds
  Serial.print("Decryption Time: ");
  Serial.print(decryptionTime);
  Serial.println(" us");

  Serial.print("Padding Removal Time: ");
  Serial.print(paddingTime);
  Serial.println(" us");

  Serial.print("Total Time (Decryption + Padding Removal): ");
  Serial.print(totalDecryptionTime);
  Serial.println(" us");

  // Convert decrypted data to string
  String binaryData = String((char*)decryptedData);

  Serial.print("Decrypted Data: ");
  Serial.println(binaryData);
}
