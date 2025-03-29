#include <ESP8266WiFi.h> // import WiFi library
#include <PubSubClient.h> // Import mqtt library

const char* ssid = "PratikOne";
const char* password = "pratik1234";
const char* mqtt_server = "broker.hivemq.com"; // this is our broker

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() { //setting up wifi connection
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); // select "wifi station" mode (client mode)
  WiFi.begin(ssid, password); //initiate connection

  while (WiFi.status() != WL_CONNECTED) { //wait for connection (retry)
    delay(500);
    Serial.print(".");
  }

  Serial.println(""); //when connected 
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) { //callback function (trigger)
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) { //print received character array in one line
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if a '1' was received as first character
  if ((char)payload[0] == 't') {
    digitalWrite(D2, LOW); 
    digitalWrite(D3,LOW) ; // Turn the LED on (LOW is the active state)
  } else if((char)payload[0] == 'f') {
    digitalWrite(D2, HIGH);
    digitalWrite(D3,HIGH) ;  // Turn the LED off (HIGH is the inactive state)
  }
}

void reconnect() { //connecting to MQTT broker
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, resubscribe
      client.subscribe("HM2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(D3, OUTPUT);
  pinMode(D2,OUTPUT); // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200); // begin serial monitor 
  setup_wifi(); //call function
  client.setServer(mqtt_server, 1883); // connect to MQTT
  client.setCallback(callback); //Listen to incoming messages and trigger callback
}

void loop() {
  if (!client.connected()) { //retry until connected 
    reconnect();
  }
  client.loop(); //MQTT loop, we keep listening 
} 