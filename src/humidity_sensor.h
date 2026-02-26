#pragma once

#include "sensor.h"

class Humidity_Sensor : public Sensor{
public:
    using Sensor::Sensor;
    
    float read() { 
        return read_raw();
    }
};