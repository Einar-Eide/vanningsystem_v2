#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include ".secrets.h"
#include "MQTT_Handler.h"
#include "humidity_sensor.h"

// ====== WIFI SETTINGS ======
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

WiFiClient espClient;
MQTT_Handler mqtt_client(espClient);
MQTT_Handler* p_mqtt_client = &mqtt_client;


// ====== Sensors ======
Humidity_Sensor h1("test", Sensor_Type::HUMIDITY_SENSOR, 32);


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
  p_mqtt_client->reconnect();

  h1.init(p_mqtt_client);
  h1.start_timer(5);
}

void loop() {
  mqtt_client.update();

  h1.update();
  delay(100);
}

