#include "pump.h"

Pump::Pump(Cfg_Pump &cfg) : cfg(cfg) {

    p_mqtt_client->add_subscribtion(cfg.MQTT_topic);

    Serial.println("Creating timer for pump: " + cfg.name);

    const esp_timer_create_args_t timer_args = {
        .callback = &Pump::static_water_duration_timer_callback,        // Static callback function
        .arg = this,                                                    // Optional argument passed to the callback
        .name = cfg.name.c_str(),                                       // Name for debugging
        .skip_unhandled_events = true                                   // Optional setting for light sleep behavior
    };

    // Create the timer instance
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer_handle));

    Serial.println(cfg.name + " timer created!");
}

void Pump::start_watering(uint64_t duration_ms) {
    start_timer(duration_ms);
    digitalWrite(cfg.CONTROLL_PIN, 1);
}

void Pump::start_timer(uint64_t duration_ms) {
    uint64_t interval_us = duration_ms * 1000;
    ESP_ERROR_CHECK(esp_timer_start_once(timer_handle, interval_us));
}
void Pump::stop_timer() {
    ESP_ERROR_CHECK(esp_timer_stop(timer_handle));
}
void Pump::delete_timer() {
    ESP_ERROR_CHECK(esp_timer_delete(timer_handle));
}