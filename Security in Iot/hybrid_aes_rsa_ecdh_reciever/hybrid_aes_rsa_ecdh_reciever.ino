#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "mbedtls/ecdh.h"
#include "mbedtls/aes.h"
#include "mbedtls/rsa.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

// Wi-Fi credentials
const char* ssid = "PratikOne";
const char* password = "pratik1234";
const char* mqtt_server = "192.168.52.84"; // Use HiveMQ or a local MQTT broker

// DHT sensor setup (optional, if needed on receiver side)
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// MQTT client setup
WiFiClient espClient;
PubSubClient client(espClient);

// Cryptographic keys
mbedtls_ecdh_context ecdh;
mbedtls_mpi shared_secret;  // Store shared secret as mbedtls_mpi
unsigned char aes_key[32];  // AES key from shared secret
unsigned char iv[12] = {0}; // Initialization Vector (fixed for simplicity)

// RSA key pair for signing/verification
mbedtls_rsa_context rsa;
unsigned char public_key[512];  // Buffer for public key
unsigned char signature[512];    // Buffer for signature

// For random number generation
mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;

void setup() {
  Serial.begin(115200);
  
  // Initialize Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Wi-Fi connected.");

  // Initialize MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);

  // Initialize ECDH for key exchange
  mbedtls_ecdh_init(&ecdh);
  mbedtls_ecp_group_load(&ecdh.grp, MBEDTLS_ECP_DP_SECP256R1);

  // Initialize the entropy source and CTR_DRBG context for random numbers
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&ctr_drbg);
  
  // Seed the random number generator
  const char *pers = "esp32";
  if (mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers, strlen(pers)) != 0) {
    Serial.println("Failed to initialize random number generator");
    while (1);
  }

  // Initialize RSA for public key verification (receiver's RSA key)
  mbedtls_rsa_init(&rsa, MBEDTLS_RSA_PKCS_V15, 0);
  int ret = mbedtls_rsa_gen_key(&rsa, mbedtls_ctr_drbg_random, &ctr_drbg, 2048, 65537);
  if (ret != 0) {
    Serial.println("RSA Key generation failed!");
    while (1);
  }

  // Subscribe to the MQTT topic to receive the public key and signature
  client.subscribe("esp32/keys");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

// Callback function to handle incoming MQTT messages
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, "esp32/keys") == 0) {
    // Receive the public key and signature
    unsigned char received_public_key[512];
    unsigned char received_signature[512];

    // Parse the received payload (public key and signature)
    parseReceivedData(payload, received_public_key, received_signature);

    // Validate the received public key using the signature and RSA
    if (validatePublicKeySignature(received_public_key, received_signature) == 0) {
      Serial.println("Public key signature validated successfully.");
      
      // Convert the received public key to an mbedtls_ecp_point structure
      mbedtls_ecp_point received_public_point;
      mbedtls_ecp_point_init(&received_public_point);
      int ret = mbedtls_ecp_point_read_binary(&ecdh.grp, &received_public_point, received_public_key, sizeof(received_public_key));
      if (ret != 0) {
        Serial.println("Failed to parse received public key.");
        return;
      }

      // Compute the shared secret using the received public key and the local private key
      mbedtls_mpi shared_secret_mpi;
      mbedtls_mpi_init(&shared_secret_mpi);
      ret = mbedtls_ecdh_compute_shared(&ecdh.grp, &shared_secret_mpi, &received_public_point, &ecdh.d, mbedtls_ctr_drbg_random, &ctr_drbg);
      if (ret != 0) {
        Serial.println("ECDH failed to compute shared secret.");
        return;
      }

      // Convert the shared secret to a byte array for further use
      size_t secret_len = mbedtls_mpi_size(&shared_secret_mpi);
      ret = mbedtls_mpi_write_binary(&shared_secret_mpi, aes_key, sizeof(aes_key));
      if (ret != 0) {
        Serial.println("Failed to convert shared secret to AES key.");
      }

      // Now you can derive the AES key from the shared secret and use it to decrypt data
    } else {
      Serial.println("Public key signature validation failed.");
    }
  }
}

// Validate the received public key using the RSA signature
int validatePublicKeySignature(unsigned char* public_key, unsigned char* signature) {
  int ret = mbedtls_rsa_pkcs1_verify(&rsa, mbedtls_ctr_drbg_random, &ctr_drbg, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, 0, public_key, signature);
  if (ret != 0) {
    Serial.println("RSA verification failed.");
    return -1;
  }
  return 0;  // Success
}

// Parse the received MQTT payload containing public key and signature
void parseReceivedData(byte* payload, unsigned char* public_key, unsigned char* signature) {
  // A simple example of parsing received data (assuming it's in a simple format)
  // A JSON library could be used to properly parse a structured message.
  
  // This is a simple placeholder for the actual parsing logic
  memcpy(public_key, payload, 512);
  memcpy(signature, payload + 512, 512);
}

// Reconnect to the MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Receiver")) {
      Serial.println("Connected to MQTT broker.");
    } else {
      delay(5000);
    }
  }
}
