#pragma once

#include <Arduino.h>
#include <esp_timer.h>
#include <vector>
#include "MQTT_Handler.h"



enum class Sensor_Type {
    HUMIDITY_SENSOR, THERMOMETER, LOAD_CELL, PHOTO_DIODE, AIR_QUALITY
};

struct Cfg_Sensor {
    String name;
    Sensor_Type type;
    uint8_t pin_in;
    bool uses_mux_adress = false;
    uint8_t mux_adress;
    std::vector<uint8_t> mux_adress_pins; // shared pointer??
    uint16_t min=0; 
    uint16_t max=4095;
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
    bool uses_mux_adress = false;
    uint8_t mux_adress;
    std::vector<uint8_t> mux_adress_pins; // shared pointer??

    uint16_t min_value;
    uint16_t max_value;
    uint64_t read_interval;

    void set_mux_adress_pins();

    static void static_timer_callback(void* arg) {
        Sensor* instance = static_cast<Sensor*>(arg);

        // pause interupts while reading data
        portDISABLE_INTERRUPTS();

        // test mux adressing
        if (instance->uses_mux_adress) {
            instance->set_mux_adress_pins();
        }

        instance->newest_raw_value = analogRead(instance->INPUT_PIN);
        instance->new_value = true;

        // activate interupts again after data is read
        portENABLE_INTERRUPTS();

        Serial.print(instance->name + " sensor read new value: ");
        Serial.println(instance->newest_raw_value);
    };

public:
    Sensor(Cfg_Sensor cfg);
    Sensor(String name, Sensor_Type type, uint8_t pin_in, uint16_t min=0, uint16_t max=4095);

    void init(MQTT_Handler* p_mqtt_handler);
    void start_timer(uint64_t interval);
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