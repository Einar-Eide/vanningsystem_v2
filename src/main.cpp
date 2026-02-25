#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include ".secrets.h"
#include "MQTT_Handler.h"
#include "Sensor.h"

// ====== WIFI SETTINGS ======
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

WiFiClient espClient;
MQTT_Handler mqtt_client(espClient);

// ====== WiFi Connect ======
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int waittime = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    waittime += 500;
    if (waittime > 5000){
      ESP.restart();
    }
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}



void setup() {
  Serial.begin(115200);
  setup_wifi();
}

void loop() {
  mqtt_client.update();
}

