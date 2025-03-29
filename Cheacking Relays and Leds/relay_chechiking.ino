// Define the GPIO pins for the relays
#define RELAY1  2
#define RELAY2  5
#define RELAY3  19
#define RELAY4  23

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Set relay pins as OUTPUT
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  // Initialize all relays to OFF (assuming LOW is off)
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);
  digitalWrite(RELAY4, LOW);
}

void loop() {
  // Turn ON all relays
  digitalWrite(RELAY1, HIGH);
  digitalWrite(RELAY2, HIGH);
  digitalWrite(RELAY3, HIGH);
  digitalWrite(RELAY4, HIGH);
  Serial.println("Relays ON");
  delay(2000); // 2-second delay

  // Turn OFF all relays
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);
  digitalWrite(RELAY4, LOW);
  Serial.println("Relays OFF");
  delay(2000); // 2-second delay
}
