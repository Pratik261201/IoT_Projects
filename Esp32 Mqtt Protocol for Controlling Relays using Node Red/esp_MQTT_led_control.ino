#define BLYNK_TEMPLATE_ID "TMPL3ZZLZHvep"
#define BLYNK_TEMPLATE_NAME "Esp"
#define BLYNK_AUTH_TOKEN "0FKYK5lXc8BCkiFkb6BpAMW2cQnTZaMF"

// Include required libraries
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <PubSubClient.h>

// Replace these with your Wi-Fi credentials
const char* ssid = "PratikOne";
const char* password = "pratik1234";

// Define MQTT broker details
const char* mqtt_server = "test.mosquitto.org";

// Blynk authentication token
char auth[] = BLYNK_AUTH_TOKEN;

// Define pin connections
#define KY018_PIN 34      // Analog pin for KY-018
#define FIRE_SENSOR_PIN 2 // Analog pin for fire sensor
#define DHT_PIN 21         // Digital pin for DHT11
#define DHT_TYPE DHT11     // Specify DHT type
#define LED1 22           // Pin for first relay
#define LED2 23            // Pin for second relay

DHT dht(DHT_PIN, DHT_TYPE);

WiFiClient espClient;
PubSubClient client(espClient);

// Variables to store sensor data
float temperature = 0.0;
float humidity = 0.0;
int lightLevel = 0;
int fireLevel = 0;

void setup_wifi() { // Setting up Wi-Fi connection
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); // Wi-Fi station mode
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Control relays based on payload
  if ((char)payload[0] == '1') {
    digitalWrite(LED1, HIGH);
  } else if ((char)payload[0] == '0') {
    digitalWrite(LED1, LOW);
  } else if ((char)payload[0] == '2') {
    digitalWrite(LED2, HIGH);
  } else if ((char)payload[0] == '3') {
    digitalWrite(LED2, LOW);
  } else if ((char)payload[0] == '4') {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
  } else if ((char)payload[0] == '5') {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("HOME/BLOCK/SENSOR");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  // Set relay pins as output
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  // Initialize Wi-Fi and MQTT
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Start Blynk connection
  Blynk.begin(auth, ssid, password);
}

void loop() {
  // Reconnect MQTT if disconnected
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read sensor data
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  lightLevel = analogRead(KY018_PIN);
  fireLevel = digitalRead(FIRE_SENSOR_PIN);

  // Log data to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C, Humidity: ");
  Serial.print(humidity);
  Serial.print("%, Light Level: ");
  Serial.print(lightLevel);
  Serial.print(", Fire Level: ");
  Serial.println(fireLevel);

  // Send data to Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V3, lightLevel);
  Blynk.virtualWrite(V2, fireLevel);

  // Allow Blynk processing
  Blynk.run();

  delay(1000); // Delay for stability
}
