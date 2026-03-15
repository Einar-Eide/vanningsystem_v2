#pragma once

#include <Arduino.h>
#include <esp_timer.h>
#include <String>

#include "MQTT_Handler.h"

struct Cfg_Pump {
    String name;
    uint64_t nterval;
    uint32_t default_duration;
    uint32_t startup_delay;
    uint8_t CONTROLL_PIN;
    String MQTT_topic;
};

class Pump {
private:
    Cfg_Pump cfg;

    esp_timer_handle_t timer_handle;

    MQTT_Handler* p_mqtt_client;

    static void static_water_duration_timer_callback(void* arg) {
        Pump* instance = static_cast<Pump*>(arg);

        // pause interupts while reading data
        portDISABLE_INTERRUPTS();

        // turn off pump
        digitalWrite(instance->cfg.CONTROLL_PIN, 0);

        // activate interupts again after data is read
        portENABLE_INTERRUPTS();

        Serial.print(instance->cfg.name + " pump stoped watering.");
    };
    
public:
    Pump(Cfg_Pump &cfg);

    void start_watering(uint64_t duration_ms);
    void start_timer(uint64_t duration_ms);
    void stop_timer();
    void delete_timer();
    
    void update_config();
};