#include "Sensor.h"

Sensor::Sensor(String name, Sensor_Type type, uint8_t pin_in, uint16_t min, uint16_t max) : 
    name(name), type(type), INPUT_PIN(pin_in), min_value(min), max_value(max) {}

void Sensor::update(){

    if ( new_value ) {
        String topic = name + "/value";
        String message = "{\"value\":" + String(read()) + "}";
        p_mqtt_client->publish(topic, message);
        new_value = false;
    }
}


void Sensor::init(MQTT_Handler* p_mqtt_handler) {
    p_mqtt_client = p_mqtt_handler;

    Serial.println("Creating timer for sensor: " + name);

    const esp_timer_create_args_t timer_args = {
        .callback = &Sensor::static_timer_callback,         // Static callback function
            .arg = this,                                    // Optional argument passed to the callback
        .name = name.c_str(),                  // Name for debugging
        .skip_unhandled_events = true                       // Optional setting for light sleep behavior
    };

    // Create the timer instance
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer_handle));

    Serial.println(name + " timer started!");
}


void Sensor::start_timer(int interval) {
    uint64_t interval_us = interval * 1000000;
    read_interval = interval_us;
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle, interval_us));
}
void Sensor::stop_timer() {
    ESP_ERROR_CHECK(esp_timer_stop(timer_handle));
}
void Sensor::delete_timer() {
    ESP_ERROR_CHECK(esp_timer_delete(timer_handle));
}