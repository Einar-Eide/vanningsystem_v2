#pragma once

#include <Arduino.h>
#include <esp_timer.h>
#include <String>

#include "MQTT_Broadcaster.h"
class Emergancy_handler;

/**
 * String name
    uint64_t interval
    uint32_t default_duration
    uint32_t startup_delay
    uint8_t CONTROLL_PIN
    String MQTT_start_watering_topic
 */
struct Cfg_Pump {
    String name;
    uint64_t interval;
    uint32_t default_duration;
    uint32_t startup_delay;
    uint8_t CONTROLL_PIN;
    String MQTT_start_watering_topic;
};

class Pump : public Mqtt_Listener {
private:
    Cfg_Pump cfg;

    esp_timer_handle_t timer_handle;

    Mqtt_Broadcaster* p_mqtt_client;

    bool stoped = false;

    void onMqttMessage(const String& topic, const String& msg) override;

    static void static_water_duration_timer_callback(void* arg) {
        Pump* instance = static_cast<Pump*>(arg);

        // pause interupts while reading data
        portDISABLE_INTERRUPTS();

        // turn off pump
        digitalWrite(instance->cfg.CONTROLL_PIN, 0);
        digitalWrite(GPIO_NUM_2, LOW);

        // activate interupts again after data is read
        portENABLE_INTERRUPTS();

        // Serial.println(instance->cfg.name + " pump stoped watering.");
        g_p_Mqtt_Broadcaster->log(instance->cfg.name + " pump stoped watering.");
    };

public:
    Pump(Cfg_Pump cfg);

    void init(Mqtt_Broadcaster* p_mqtt_broadcaster, Emergancy_handler* p_emergancy_handler);

    void start_watering(uint64_t duration_ms);
    void start_timer(uint64_t duration_ms);
    void stop_timer();
    void delete_timer();

    void emergency_stop();
    
    // void update_config();
};