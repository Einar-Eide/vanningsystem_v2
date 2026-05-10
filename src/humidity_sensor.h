#pragma once

#include <Arduino.h>
#include "sensor.h"

class Humidity_Sensor : public Sensor{
public:
    using Sensor::Sensor;

    float read_raw() override {
        return analogRead(INPUT_PIN);
    }

    float read() { 
        float value = get_raw();
        return value;
    }
};