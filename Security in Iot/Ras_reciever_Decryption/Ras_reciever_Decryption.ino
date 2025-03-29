#include <WiFi.h>
#include <PubSubClient.h>
#include <Crypto.h>
#include <RSA.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT Broker details
const char* mqtt_server = "192.168.52.84";
const char* mqtt_topic = "esp32/rsa/data";

// MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

// RSA Private Key for decryption
const char* privateKey =
    "-----BEGIN PRIVATE KEY-----\n"
    "MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDhAylMzJ0nr2Dv
dDpGC39jnbgPHgjS2eRVV6wVExUS0XS+F37UkM5eqGmWtpAqaiRwtgEBY3FUz8Dh
lrzCC1CH+8+v4s+3sE5Eo9we/XWYt776FThcWQzb84CYbqhviJ3ko6fbApgn71vS
dvQbj5/5K3a8s5w7ksYUsI7XoLiYxFKubKdMibiIIS4f2mRHBbNukuDcMRfjo4HH
3eaLz+jTR4dP/KmDHJo3HSrsRqrhx0Rx36PFL3BVuyOjB7QeK1HyEsLBOroARkb9
kEUgpDS32KrJ2JMxjTQJ4godSASAXYS/mkABpFCzqDNi2/Yxyz9rQF4WvyRjc2ml
LgrrzBepAgMBAAECggEAALrHNbYPDN9lRtOx9fAtASNE+b6stkBQYUrpTkx+B8lx
Hf9x1OXXlHpzh5pXMerLsNZRqcTrqGenKmMqmr6olQiz3aR5Bt9LwbtYEdCvG8TY
nL5vpWrWUG1/W4NZamrKbP/xJ2WtZc6t+Rh5jXf6Tr76rVr0kFvqnFtTDsi5HHOE
/GBeBD7rabqLelcKfyydHGRityJS1xQ1NZ49KVflAJ+/8FedCV9GDRclxjZLg9Qu
iuMZrIv5f3jYKMI8wv4PfhvcuV6VXzWd1Rq6h18K5wrCsNSaYe3mS7w1B+INpl2T
1wD0UxQwS45PlRZdC0S4/l8t4YJzYD1O6XC7CftaXQKBgQD7LZQEYJMQeEE+3F+a
bXAMWhYnbSfGZ2icLrj+0UMmQ1j8XRW6hQVcgzblEQ4TYSVZ7OX43RhExaAE3v7/
92WemYxkax3vynE5e8lkjBbXJLl10ki6KbhzUVNvnfEXOLh35FT1/+oDz8ErlVxn
PWh7h69QhJ58Pu7dEuSfL8VxDQKBgQDlVP23u3Suo9269q4VdsWj+5KfvreJForM
+xrBmn0Dp2unZ9eNzSia3gm6zAW3ll4sFUGfNxATbKKuYzdjCL4kLDWYDWrKYZTb
cDoEdNs30my2LGSg35RWIs5J3e6/WK5/O/1HxE7Nk1X4Tb/BOgYL7gIsHp19B4TM
g1p2ziBCDQKBgB9/FaJ+4bIdAkF5rNtWgchqu4ByJZyxduKu0S/S2dLAezYaDhF3
gLabFZUf0F4Jvn9KP4Q3ZUWqamhx9hPu1+VNZ9dnvzdEcXI93YEoN+z+g+sMVWZS
uPhs6EOE0KiSyj7ueODoGFYmBRkL/6xKfQGQgCFen+kAkz9RkS2M9fs5AoGARUEQ
SX7Lcr27hO1fjs3KS4X6NlFyvWxYSCSAZPzRSdwyL2C1gHX3ECWDTKy2AUXgdHGK
2pjXOvZwfjcqD7chLG1JVfyfRjSHrmjKrHPZi5b5C16GmOpLsmNaZoYlxhTyxSuJ
hjWrEnuu14grIx/xmYKrl0U7i/lMSjJUjFIc/bUCgYAtVMeQv8moUV+vQtDkuz4H
y0GajZWY4MtDhNgXh74bx3O8K/xfW6zbOZiJVdnZjA0u2nYrMVoL7Dp8JMjVAh97
KUWxgQJIZMwm5+GQoYprILTBFW5lbZFTPxE/x+Cn+CZkrU6ihxevCh9Zs1X0J9n9
dPMWhyLAiomEcfxLbx0OXQ=="
    "-----END PRIVATE KEY-----";

RSA rsa;

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set up MQTT client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Connect to MQTT
  reconnect();

  // Set up RSA
  rsa.setPrivateKey(privateKey);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Message received");

  // Decrypt the payload
  byte decrypted[256];
  size_t decryptedLen;
  if (rsa.privateDecrypt(payload, length, decrypted, &decryptedLen)) {
    String decryptedMessage = String((char*)decrypted);
    Serial.print("Decrypted Binary Number: ");
    Serial.println(decryptedMessage);
  } else {
    Serial.println("Decryption failed");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32_Node2")) {
      Serial.println("Connected to MQTT Broker");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
