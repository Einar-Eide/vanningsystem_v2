#include "Sensor.h"

Sensor::Sensor(Cfg_Sensor cfg) : 
    name(cfg.name), type(cfg.type), INPUT_PIN(cfg.pin_in), // basics
    uses_mux_adress(cfg.uses_mux_adress), mux_adress(cfg.mux_adress), mux_adress_pins(cfg.mux_adress_pins), // mux adressing
    min_value(cfg.min), max_value(cfg.max) {} // min max values

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
        .arg = this,                                        // Optional argument passed to the callback
        .name = name.c_str(),                               // Name for debugging
        .skip_unhandled_events = true                       // Optional setting for light sleep behavior
    };

    // Create the timer instance
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer_handle));

    Serial.println(name + " timer created!");
}

void Sensor::set_mux_adress_pins() {
    for (int i=0; i<mux_adress_pins.size(); i++) {
        digitalWrite(mux_adress_pins.at(i), mux_adress & (1 << i));
    }
}


void Sensor::start_timer(uint64_t interval) {
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