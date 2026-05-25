#include "pump.h"
#include "emergancy.h"

Pump::Pump(Cfg_Pump cfg) : cfg(cfg) {

}

void Pump::init(Mqtt_Broadcaster* p_mqtt_broadcaster, Emergancy_handler* p_emergancy_handler) {

    p_mqtt_client = p_mqtt_broadcaster;

    p_mqtt_client->add_subscribtion(cfg.MQTT_start_watering_topic);
    p_mqtt_client->add_subscriber(this);

    p_emergancy_handler->addPump(this);

    Serial.println("Creating timer for pump: " + cfg.name);

    const esp_timer_create_args_t timer_args = {
        .callback = &Pump::static_water_duration_timer_callback,        // Static callback function
        .arg = this,                                                    // Optional argument passed to the callback
        .name = cfg.name.c_str(),                                       // Name for debugging
        .skip_unhandled_events = true                                   // Optional setting for light sleep behavior
    };

    // Create the timer instance
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer_handle));

    // Activate Controllpin
    pinMode(cfg.CONTROLL_PIN, OUTPUT);

    Serial.println(cfg.name + " timer created!");
}

void Pump::start_watering(uint64_t duration_ms = 0) {
    if (duration_ms == 0) {
        duration_ms = cfg.default_duration;
    }
    
    if (!g_emergancy_stoped) {
        Serial.print("Started watering timer duration (ms): ");
        Serial.println(duration_ms);
        digitalWrite(cfg.CONTROLL_PIN, HIGH);
        digitalWrite(GPIO_NUM_2, HIGH);
        start_timer(duration_ms);
    }
}

void Pump::start_timer(uint64_t duration_ms) {
    uint64_t interval_us = duration_ms * 1000;
    ESP_ERROR_CHECK(esp_timer_start_once(timer_handle, interval_us));
}
void Pump::stop_timer() {
    ESP_ERROR_CHECK(esp_timer_stop(timer_handle));
}
void Pump::delete_timer() {
    ESP_ERROR_CHECK(esp_timer_delete(timer_handle));
}

void Pump::onMqttMessage(const String& topic, const String& msg) {

    if (topic == cfg.MQTT_start_watering_topic) {

        int duration_ms = msg.toInt();
        
        // clean the message
        if (duration_ms == 0 && msg != 0){
            String cleanMsg = msg;
            cleanMsg.remove(0, 1);
            cleanMsg.remove(cleanMsg.length() - 1, 1);
            cleanMsg.trim();  // removes whitespace like \n, \r, spaces

            duration_ms = cleanMsg.toInt();
        }
        
        start_watering( duration_ms );
    } 
}

void Pump::emergency_stop() {
    digitalWrite(cfg.CONTROLL_PIN, LOW);
}