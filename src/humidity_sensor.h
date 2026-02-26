#pragma once

#include <Arduino.h>
#include "sensor.h"

class Humidity_Sensor : public Sensor{
public:
    using Sensor::Sensor;

    float read() { 
        float value = read_raw();
        return value;
    }
};