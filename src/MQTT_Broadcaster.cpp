#include "MQTT_Broadcaster.h"
Mqtt_Broadcaster* g_p_Mqtt_Broadcaster = nullptr;

Mqtt_Broadcaster::Mqtt_Broadcaster(WiFiClient& espClient) {

  client.setClient(espClient);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqtt_callback);

  add_subscribtion("wateringsystem/keep_alive");
  add_subscribtion("home/connection");

  g_p_Mqtt_Broadcaster = this;
}

void Mqtt_Broadcaster::update() {
  if (!client.connected()) {
      reconnect();
  }
  
  // brodcast all messages recieved
  while (!internal_messages_brodcast_queue.empty()) {
    String topic = internal_messages_brodcast_queue.front().first;
    String message = internal_messages_brodcast_queue.front().second;
    broadcast(topic, message);
    internal_messages_brodcast_queue.pop();

    log("Received message: \"" + message + "\", on topic: " + topic);
  }

  while (!external_messages_publish_queue.empty()) {
    String topic = external_messages_publish_queue.front().first;
    String message = external_messages_publish_queue.front().second;
    external_messages_publish_queue.pop();

    publish(topic, message);
  }

  client.loop();
}

// ====== MQTT Reconnect ======
void Mqtt_Broadcaster::reconnect() {
  uint8_t retry_counter = 0;
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      // Subscribe
      for (int i=0; i<subscribed_topics.size(); i++){
        String topic = subscribed_topics[i];
        client.subscribe( topic.c_str() );
      }

      // Publish initial message
      client.publish("home/connection", "Wateringsystem ESP32 connected!");

    } else if (retry_counter < 5) {
      retry_counter += 1;
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    } else {
      ESP.restart();
    }
  }
}


void Mqtt_Broadcaster::add_to_publish_queue(String& topic, String& message) {
  external_messages_publish_queue.push( {topic, message} );
}

void Mqtt_Broadcaster::publish(String& sufix_topic, String& message) {
  String topic = base_topic + sufix_topic;
  client.publish(topic.c_str(), message.c_str());
  Serial.println("published: " + message + ", to topic: " + topic);

  Serial.println();
  Serial.println("-----------------------");
}

/**
 * Adds message to mqtt publish queue with topic: <base-topic>/log
 */
void Mqtt_Broadcaster::log(String message){
  String log_topic = "log"; // TODO: add to .secerets.h or some sort of config
  add_to_publish_queue(log_topic, "[" + String(millis()) + "] " + message);

  if (true) { // TODO: swap 'true' with config defined variable, something like USING_SERIAL 
    Serial.println(message);
  }
}

void Mqtt_Broadcaster::add_to_brodcast_queue(String& topic, String& message) {
  internal_messages_brodcast_queue.push( {topic, message} );
}

void Mqtt_Broadcaster::broadcast(String& topic, String& message) {
  //topic.remove(0, std::strlen(base_topic) ); // TODO: remove basetopic before broadcasting

  Serial.print("Broadcasting topic: ");
  Serial.print(topic);
  Serial.print(", and message: ");
  Serial.println(message.c_str());

  for (int i=0; i<listeners.size(); i++) {
    listeners.at(i)->onMqttMessage(topic, message);
  }
}

void Mqtt_Broadcaster::add_subscribtion(String topic) {
  //######################
  // TODO: add base topic
  //######################
  subscribed_topics.push_back(topic);
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

  g_p_Mqtt_Broadcaster->add_to_brodcast_queue(topic_str, message);

  
  Serial.println();
  Serial.println("-----------------------");
}