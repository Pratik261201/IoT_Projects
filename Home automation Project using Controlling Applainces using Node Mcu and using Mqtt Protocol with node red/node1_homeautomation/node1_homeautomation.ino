#include <WiFi.h>          // Import WiFi library
#include <PubSubClient.h>  // Import mqtt library

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
  if ((char)payload[0]== 't') {
    digitalWrite(4, LOW); 
   // Turn the LEDs on (LOW is the active state)
  } else if((char)payload[0] == 'f') {
    digitalWrite(4, HIGH);
    // Turn the LEDs off (HIGH is the inactive state)
  }
   else if((int)payload[0]== '0'){
     digitalWrite(2, LOW); 

  }
    else if((int)payload[0]=='1'){
  digitalWrite(2, HIGH);  
  }
   else if((int)payload[0]=='2'){
     digitalWrite(5, LOW); 

  }
   else if((int)payload[0]=='3'){
  digitalWrite(5, HIGH);  
  }
     else if((int)payload[0]=='4'){
     digitalWrite(18, LOW); 

  }
   else if((int)payload[0]=='5'){
  digitalWrite(18, HIGH);  
  }
     else if((int)payload[0]=='6'){
     digitalWrite(19, LOW); 

  }
   else if((int)payload[0]=='7'){
  digitalWrite(19, HIGH);  
  }
     else if((int)payload[0]=='8'){
     digitalWrite(21, LOW); 

  }
   else if((int)payload[0]=='9'){
  digitalWrite(21, HIGH);  
  }

}

void reconnect() { //connecting to MQTT broker
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
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
  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(21, OUTPUT); // Initialize the pins as outputs
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
