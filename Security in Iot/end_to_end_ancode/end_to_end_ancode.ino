#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <mbedtls/md.h> // For SHA-256 hash generation
#include "base64.h"     // Include base64 encoding library

// WiFi parameters to be configured
const char* ssid = "PratikOne"; // Write here your router's username
const char* password = "pratik1234"; // Write here your router's password

// MQTT broker settings
const char* mqtt_server = "broker.hivemq.com";
char port[6] = "1883";
char topic[40] = "Hm3";

WiFiClient espClient;
PubSubClient client(espClient);
// DHT11 sensor settings
#define DHTPIN 4        // Pin connected to the DHT11 data pin
#define DHTTYPE DHT11   // DHT11 sensor
DHT dht(DHTPIN, DHTTYPE);



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "espClient";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected to broker: ");
      Serial.println(mqtt_server);
      // Once connected, publish an announcement...
      // client.publish(topic, "hello world");
      // ... and resubscribe
      // client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_server);
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void mqtt_setup() {
  client.setServer(mqtt_server, atoi(port));
}

// Function to generate SHA-256 hash
String generateHash(String data) {
  unsigned char hash[32];  // SHA-256 produces 32-byte (256-bit) hashes
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char*)data.c_str(), data.length());
  mbedtls_md_finish(&ctx, hash);
  mbedtls_md_free(&ctx);

  // Convert hash to a readable hex string
  String hashString = "";
  for (int i = 0; i < 32; i++) {
    if (hash[i] < 16) hashString += '0'; // Add leading zero if necessary
    hashString += String(hash[i], HEX);
  }

  return hashString;
}

// Base64 encode the data
String base64Encode(String data) {
  String encodedData = base64::encode((unsigned char*)data.c_str(), data.length());
  return encodedData;
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
 WiFi.begin(ssid, password);
// then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 Serial.println("WiFi connected");

  // Print the IP address
  Serial.println(WiFi.localIP());
  delay(1000);

  dht.begin();


  // Initialize MQTT
  mqtt_setup();
  reconnect();
}

void loop() {
   // Ensure the client is connected
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read sensor data
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Prepare data string
  String sensorData = "Temperature: " + String(t) + "C, Humidity: " + String(h) + "%";
  Serial.println("Sensor Data: " + sensorData);

  // Generate SHA-256 hash of the sensor data
  String hash = generateHash(sensorData);
  Serial.println("SHA-256 Hash: " + hash);

  // Encode both the data and hash in base64
  String encodedData = base64Encode(sensorData);
  String encodedHash = base64Encode(hash);
  Serial.println("Base64 Encoded Data: " + encodedData);
  Serial.println("Base64 Encoded Hash: " + encodedHash);

  // Prepare JSON payload with encoded sensor data and hash
  String payload = "{\"encoded_data\": \"" + encodedData + "\", \"encoded_hash\": \"" + encodedHash + "\"}";
  Serial.println("Payload: " + payload);

  // Publish the payload to MQTT
  client.publish("sensor/data", payload.c_str());

  // Wait before next reading
  delay(1000);  // Delay 5 seconds between reads
}