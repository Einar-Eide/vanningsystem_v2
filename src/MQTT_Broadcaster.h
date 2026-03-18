#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <vector>
#include <cstring> // Required for strlen()

#include ".secrets.h"

class Mqtt_Listener {
public: 
    virtual void onMqttMessage(const String& topic, const String& msg) = 0;
};

class Mqtt_Broadcaster {
    // ====== MQTT SETTINGS ======
    const char* mqtt_server = MQTT_SERVER_IP;
    const int mqtt_port = MOSQUITO_PORT;
    const char* base_topic = MQTT_BASE_TOPIC;

    PubSubClient client;

    std::vector<Mqtt_Listener*> listeners;

public:
    Mqtt_Broadcaster(WiFiClient& espClient);

    void update();
    void reconnect();

    void publish(String& topic, String& message);
    
    void broadcast(String topic, String& message);
    void add_subscribtion(String topic);
    void add_subscriber(Mqtt_Listener* new_listener);

    friend void mqtt_callback(char* topic, byte* payload, unsigned int length);
};

void mqtt_callback(char* topic, byte* payload, unsigned int length);


extern Mqtt_Broadcaster* g_p_Mqtt_Broadcaster;