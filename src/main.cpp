#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include ".secrets.h"

// ====== WIFI SETTINGS ======
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// ====== MQTT SETTINGS ======
const char* mqtt_server = MQTT_SERVER_IP;  // Ubuntu server IP
const int mqtt_port = MOSQUITO_PORT;
const char* mqtt_topic = MQTT_BASE_TOPIC;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
int counter = 0;

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

// ====== MQTT Message Callback ======
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

// ====== MQTT Reconnect ======
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      // Subscribe
      client.subscribe(mqtt_topic);

      // Publish initial message
      client.publish(mqtt_topic, "ESP32 connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publish every 5 seconds
  unsigned long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;

    String message = "Hello from ESP32, count=";
    message += counter++;

    client.publish(mqtt_topic, message.c_str());
    Serial.println("Message published: " + message);
  }
}

