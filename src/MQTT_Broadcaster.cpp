#include "MQTT_Broadcaster.h"
Mqtt_Broadcaster* g_p_Mqtt_Broadcaster = nullptr;

Mqtt_Broadcaster::Mqtt_Broadcaster(WiFiClient& espClient) {

  client.setClient(espClient);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqtt_callback);

  g_p_Mqtt_Broadcaster = this;
}

void Mqtt_Broadcaster::update() {
  if (!client.connected()) {
      reconnect();
  }
  client.loop();
}

// ====== MQTT Reconnect ======
void Mqtt_Broadcaster::reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      // Subscribe
      String topic = String(base_topic) + "connection";
      client.subscribe( topic.c_str() );

      // Publish initial message
      client.publish(topic.c_str(), "ESP32 connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void Mqtt_Broadcaster::publish(String& sufix_topic, String& message) {
  String topic = base_topic + sufix_topic;
  client.publish(topic.c_str(), message.c_str());
  Serial.println("published: " + message + ", to topic: " + topic);

  Serial.println();
  Serial.println("-----------------------");
}

void Mqtt_Broadcaster::broadcast(String topic, String& message) {
  //topic.remove(0, std::strlen(base_topic) ); // remove basetopic before broadcasting
  for (int i=0; i<listeners.size(); i++) {
    Serial.print("Broadcasting topic: ");
    Serial.print(topic);
    Serial.print(", and message: ");
    Serial.println(message.c_str());
    listeners.at(i)->onMqttMessage(topic, message);
  }
}

void Mqtt_Broadcaster::add_subscribtion(String topic) {
  //######################
  // TODO: add base topic
  //######################
  client.subscribe(topic.c_str()); 
  Serial.print("Subscribed to new topic: ");
  Serial.printf(topic.c_str());
}

void Mqtt_Broadcaster::add_subscriber(Mqtt_Listener* new_listener){
  listeners.push_back(new_listener);
}

// ====== MQTT Message Callback ======
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.print(topic);

  String message = "";

  
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print(", message: ");
  Serial.println(message.c_str());

  String topic_str = topic;

  g_p_Mqtt_Broadcaster->broadcast(topic_str, message);

  
  Serial.println();
  Serial.println("-----------------------");
}