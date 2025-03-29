// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPL3UN_0TjJ9"
#define BLYNK_TEMPLATE_NAME "Home automation"
#define BLYNK_AUTH_TOKEN "UTDuE5KMgVfR3eTz0oZdDuCKMBgHxhEI"



// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "PratikOne";
char pass[] = "pratik1234";

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();
  digitalWrite(2, value);
  Serial.print(value);
  // Update state
 // Blynk.virtualWrite(V1, value);
}
BLYNK_WRITE(V1)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();
  digitalWrite(22, value);
  Serial.print(value);
  // Update state
 // Blynk.virtualWrite(V1, value);
}
BLYNK_WRITE(V2)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();
  digitalWrite(23, value);
  Serial.print(value);
  // Update state
 // Blynk.virtualWrite(V1, value);
}
BLYNK_WRITE(V3)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();
  digitalWrite(35, value);
  Serial.print(value);
  // Update state
 // Blynk.virtualWrite(V1, value);
}


void setup()
{
  pinMode(19,OUTPUT);
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

}

void loop()
{
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
