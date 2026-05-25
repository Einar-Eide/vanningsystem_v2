#pragma once

#include <vector>

#include "MQTT_Broadcaster.h"
#include "sensor.h"
class Pump;

extern bool g_emergancy_stoped;

class Emergancy_handler : public Mqtt_Listener {
private:
    String mqtt_topic_stop = "wateringsystem/STOP";
    String mqtt_topic_restart = "wateringsystem/RESTART";
    std::vector<Pump*> pumps;
    
    Sensor* p_sensor;
    uint16_t threshold;
    bool check_value_is_greater_than_threshold;

public: 

    Emergancy_handler(Sensor* sensor, uint16_t threshold, bool check_value_is_greater_than_threshold=true);
    void init(Mqtt_Broadcaster* p_mqtt_broadcaster);
    void onMqttMessage(const String& topic, const String& msg);
    void addPump(Pump* pump);

    void update();

    void emergancy_stop();

};