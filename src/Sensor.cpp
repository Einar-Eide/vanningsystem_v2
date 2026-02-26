#include "Sensor.h"

Sensor::Sensor(String name, Sensor_Type type, uint8_t pin_in, uint16_t min, uint16_t max) : 
    name(name), type(type), INPUT_PIN(pin_in), min_value(min), max_value(max)
{
    const esp_timer_create_args_t timer_args = {
        .callback = &Sensor::static_timer_callback,     // Static callback function
            .arg = this,                                    // Optional argument passed to the callback
        .name = "MyPeriodicTimer",                          // Name for debugging
        .skip_unhandled_events = true                       // Optional setting for light sleep behavior
    };

    // Create the timer instance
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer_handle));

    ESP_LOGI(TAG, "Timer started, waiting for callbacks...");


}

void Sensor::start_timer(int interval) {
    read_interval = interval * 1000000;
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle, interval));
}
void Sensor::stop_timer() {
    ESP_ERROR_CHECK(esp_timer_stop(timer_handle));
}
void Sensor::delete_timer() {
    ESP_ERROR_CHECK(esp_timer_delete(timer_handle));
}