#include "MQTT_Handler.h"

MQTT_Handler::MQTT_Handler(WiFiClient& espClient) {
    client.setClient(espClient);

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqtt_callback);
}

void MQTT_Handler::update() {
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

// ====== MQTT Reconnect ======
void MQTT_Handler::reconnect() {
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


// ====== MQTT Message Callback ======
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  String message = "";

  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.printf(message.c_str());
  Serial.println();
  Serial.println("-----------------------");
}