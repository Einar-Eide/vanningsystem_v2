#include "emergancy.h"
#include "pump.h"

bool g_emergancy_stoped = false;

Emergancy_handler::Emergancy_handler(Sensor* sensor, uint16_t threshold, bool check_value_is_greater_than_threshold) : 
    p_sensor{sensor}, threshold{threshold}, check_value_is_greater_than_threshold{check_value_is_greater_than_threshold}
{

}

void Emergancy_handler::init(Mqtt_Broadcaster* p_mqtt_broadcaster) {
    p_mqtt_broadcaster->add_subscribtion(mqtt_topic_stop);
    p_mqtt_broadcaster->add_subscribtion(mqtt_topic_restart);
    p_mqtt_broadcaster->add_subscriber(this);

    // read a dummy value to prevent first value from beeing 0
    analogRead(p_sensor->get_pin());
}

void Emergancy_handler::onMqttMessage(const String& topic, const String& msg) {
    if (topic == mqtt_topic_stop) {
        emergancy_stop();
    } else if (topic == mqtt_topic_restart){
        ESP.restart();
    }
}

void Emergancy_handler::addPump(Pump* pump) {
    pumps.push_back(pump);
}

void Emergancy_handler::update() {
    if (p_sensor->is_online())
    {
        if (check_value_is_greater_than_threshold)
        {
            if(p_sensor->read() > threshold) {
                emergancy_stop();
            }
        }
        else 
        {
            if(p_sensor->read() < threshold) {
                emergancy_stop();
            }
        }
    }
}


void Emergancy_handler::emergancy_stop(){
    g_emergancy_stoped = true;

    Serial.print("EMERGANCY!");
    Serial.print("STOPING!");

    for (int i=0; i<pumps.size(); i++) {
        pumps.at(i)->emergency_stop();
    }
}