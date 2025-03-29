#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "PratikOne";
const char* password = "pratik1234";

// Set LED GPIO pins (change according to your setup)
const int ledPin1 = D1;  // Connect an LED to GPIO 5 (D1 on NodeMCU)
const int ledPin2 = D2;  // Connect an LED to GPIO 4 (D2 on NodeMCU)
const int ledPin3 = D3;  // Connect an LED to GPIO 0 (D3 on NodeMCU)

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Set LED pins as outputs
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  // Define server routes
  server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request){
    String ledState;
    if (request->hasParam("finger")) {
      ledState = request->getParam("finger")->value();
      
      // Control LEDs based on finger count
      int fingers = ledState.toInt();
      digitalWrite(ledPin1, fingers >= 1 ? HIGH : LOW);
      digitalWrite(ledPin2, fingers >= 2 ? HIGH : LOW);
      digitalWrite(ledPin3, fingers >= 3 ? HIGH : LOW);
    }
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
}

void loop() {
  // No code needed here
}
