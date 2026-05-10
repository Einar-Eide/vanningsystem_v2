#pragma once

#include <Arduino.h>
#include "sensor.h"

#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20_Thermometer : public Sensor{
public:

    OneWire oneWire;
    DallasTemperature sensors;

    DS18B20_Thermometer(String name, uint8_t pin_in, String location="Undefined") : Sensor(name, Sensor_Type::THERMOMETER, pin_in) {
        oneWire = OneWire(pin_in);
        sensors = DallasTemperature(&oneWire);
        sensors.begin();
        data_doc["location"] = location;
    }

    DS18B20_Thermometer(const Cfg_Sensor &cfg) : Sensor(cfg) {
        oneWire = OneWire(cfg.pin_in);
        sensors = DallasTemperature(&oneWire);
        sensors.begin();
    }

    float read_raw() override {
        sensors.requestTemperatures(); 
        return sensors.getTempCByIndex(0);
    }

    float read() { 
        float value = get_raw();
        return value;
    }
};