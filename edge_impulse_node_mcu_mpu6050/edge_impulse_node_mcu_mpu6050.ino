#include <Wire.h>
#include <MPU6050.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// Replace with your network credentials
const char* ssid = "PratikOne";
const char* password = "pratik1234";

// Edge Impulse API Key and project URL
const char* apiKey = "ei_51c7aaf6e062ad682340f3ea5106e2ca1becb014159b9513";
const char* projectURL = "https://ingestion.edgeimpulse.com/api/ingestion/data";
";

// MPU6050 setup
MPU6050 mpu;

// Wi-Fi client for sending data
WiFiClientSecure client;

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to Wi-Fi");

  // Initialize MPU6050
  Wire.begin(D2, D1); // Initialize I2C with custom pins for ESP8266
  mpu.initialize();  // Use initialize() instead of begin()

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not found");
    while (1);  // Halt program if sensor is not found
  }

  Serial.println("MPU6050 initialized successfully");

  // Connect to Edge Impulse
  client.setInsecure();
}

void loop() {
  // Variables to hold accelerometer and gyroscope data
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  // Get accelerometer and gyroscope data
  mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);

  // Convert the data to float for easier processing
  float ax_f = ax / 16384.0;  // Convert to g (assuming default sensitivity)
  float ay_f = ay / 16384.0;
  float az_f = az / 16384.0;
  float gx_f = gx / 131.0;    // Convert to dps (assuming default sensitivity)
  float gy_f = gy / 131.0;
  float gz_f = gz / 131.0;

  // Create JSON payload to send to Edge Impulse
  String payload = createJSONPayload(ax_f, ay_f, az_f, gx_f, gy_f, gz_f);

  // Send data to Edge Impulse via API
  sendDataToEdgeImpulse(payload);

  // Wait before sending the next data packet
  delay(100);  // Adjust delay as per your requirements
}

String createJSONPayload(float ax, float ay, float az, float gx, float gy, float gz) {
  // Create a JSON object to hold the data
  DynamicJsonDocument doc(1024);
  doc["sensor"] = "MPU6050";
  doc["acceleration"]["x"] = ax;
  doc["acceleration"]["y"] = ay;
  doc["acceleration"]["z"] = az;
  doc["gyro"]["x"] = gx;
  doc["gyro"]["y"] = gy;
  doc["gyro"]["z"] = gz;

  // Convert the JSON object to string
  String payload;
  serializeJson(doc, payload);
  return payload;
}

void sendDataToEdgeImpulse(String payload) {
  // Open connection to Edge Impulse API
  if (client.connect("ingestion.edgeimpulse.com", 443)) {  // Use host name for HTTPS
    // Set HTTP POST headers
    client.println("POST /api/ingestion/data HTTP/1.1");
    client.println("Host: ingestion.edgeimpulse.com");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(payload.length());
    client.println();
    
    // Send JSON data
    client.println(payload);

    // Check the response
    int statusCode = client.parseInt();
    Serial.print("Response code: ");
    Serial.println(statusCode);
    if (statusCode == 200) {
      Serial.println("Data sent successfully!");
    } else {
      Serial.println("Failed to send data.");
    }
  } else {
    Serial.println("Failed to connect to Edge Impulse.");
  }
  client.stop();  // Close the connection
}
