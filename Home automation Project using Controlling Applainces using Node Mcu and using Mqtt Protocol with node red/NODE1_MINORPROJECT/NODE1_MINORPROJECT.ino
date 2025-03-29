#define BLYNK_TEMPLATE_ID "TMPL3UN_0TjJ9"
#define BLYNK_TEMPLATE_NAME "Home automation"
#define BLYNK_AUTH_TOKEN "UTDuE5KMgVfR3eTz0oZdDuCKMBgHxhEI"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Wi-Fi credentials
char ssid[] = "PratikOne";
char pass[] = "pratik1234";

// Pin definitions
#define RELAY1_PIN 2
#define RELAY2_PIN 5
#define RELAY3_PIN 19
#define RELAY4_PIN 27
#define PHOTO_SENSOR_PIN 23

// Virtual pins
#define V1_BUTTON  V0 // Relay 1
#define V2_BUTTON  V1 // Relay 2
#define V3_SWITCH  V2 // Relay 3 in mode 2
#define V4_SWITCH  V3 // Relay 4 in mode 2
#define V5_MODE    V4 // Mode switch

// Variables
bool mode = false; // false: Mode 1 (sensor), true: Mode 2 (Blynk switch)

BLYNK_WRITE(V1_BUTTON) {
  digitalWrite(RELAY1_PIN, param.asInt());
}

BLYNK_WRITE(V2_BUTTON) {
  digitalWrite(RELAY2_PIN, param.asInt());
}

BLYNK_WRITE(V5_MODE) {
  mode = param.asInt();
  if (mode) {
    Blynk.virtualWrite(V3_SWITCH, 0); // Reset switches for Mode 2
    Blynk.virtualWrite(V4_SWITCH, 0);
  }
}

BLYNK_WRITE(V3_SWITCH) {
  if (mode) {
    digitalWrite(RELAY3_PIN, param.asInt());
  }
}

BLYNK_WRITE(V4_SWITCH) {
  if (mode) {
    digitalWrite(RELAY4_PIN, param.asInt());
  }
}

void setup() {
  // Serial monitor
  Serial.begin(115200);

  // Initialize relays as outputs
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
  digitalWrite(RELAY3_PIN, LOW);
  digitalWrite(RELAY4_PIN, LOW);

  // Initialize photoelectric sensor as input
  pinMode(PHOTO_SENSOR_PIN, INPUT);

  // Connect to Wi-Fi and Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();

  // Handle relays in Mode 1 (photoelectric sensor)
  if (!mode) {
    int sensorValue = digitalRead(PHOTO_SENSOR_PIN);
    if(sensorValue==1){
digitalWrite(RELAY3_PIN, LOW);
digitalWrite(RELAY4_PIN, LOW);
    }
    else if(sensorValue==0){
digitalWrite(RELAY3_PIN, HIGH);
digitalWrite(RELAY4_PIN, HIGH);
    }
    
    Serial.print(sensorValue);
    Serial.println("Mode 1: Controlling relays with photoelectric sensor");
  } else {
    Serial.println("Mode 2: Controlling relays with Blynk switches");
  }
}
