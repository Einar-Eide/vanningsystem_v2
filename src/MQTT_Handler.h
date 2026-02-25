#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include ".secrets.h"

class MQTT_Handler {
    // ====== MQTT SETTINGS ======
    const char* mqtt_server = MQTT_SERVER_IP;
    const int mqtt_port = MOSQUITO_PORT;
    const char* mqtt_topic = MQTT_BASE_TOPIC;

    PubSubClient client;

    unsigned long lastMsg = 0;
    int counter = 0;

public:
    MQTT_Handler(WiFiClient& espClient);

    void update();
    void reconnect();

    friend void mqtt_callback(char* topic, byte* payload, unsigned int length);
};

void mqtt_callback(char* topic, byte* payload, unsigned int length);