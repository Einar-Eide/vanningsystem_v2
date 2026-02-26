#pragma once

#include <Arduino.h>
#include <esp_timer.h>
#include "MQTT_Handler.h"


enum class Sensor_Type {
    HUMIDITY_SENSOR, THERMOMETER, LOAD_CELL, PHOTO_DIODE, AIR_QUALITY
};


class Sensor {

protected:
    String name;
    Sensor_Type type;

    esp_timer_handle_t timer_handle;

    volatile int newest_raw_value;
    bool new_value;

    MQTT_Handler* p_mqtt_client;
    
    uint8_t INPUT_PIN;
    uint16_t min_value;
    uint16_t max_value;
    uint64_t read_interval;

    static void static_timer_callback(void* arg) {
        Sensor* instance = static_cast<Sensor*>(arg);

        instance->newest_raw_value = analogRead(instance->INPUT_PIN);
        instance->new_value = true;

        Serial.print(instance->name + " sensor read new value: ");
        Serial.println(instance->newest_raw_value);
    };

public:
    Sensor(String name, Sensor_Type type, uint8_t pin_in, uint16_t min=0, uint16_t max=4095);

    void init(MQTT_Handler* p_mqtt_handler);
    void start_timer(int interval);
    void stop_timer();
    void delete_timer();

    virtual float read() = 0;
    int read_raw() { return newest_raw_value; }

    void update();

    String get_name()       { return name; }
    Sensor_Type get_type()  { return type; }
    int get_pin()           { return INPUT_PIN; }
    int get_read_interval() { return read_interval; }

};