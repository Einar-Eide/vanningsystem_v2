#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include ".secrets.h"
#include "MQTT_Broadcaster.h"
#include "humidity_sensor.h"
#include "DS18B20_Thermometer.h"
#include "pump.h"
#include "emergancy.h"

// ====== WIFI SETTINGS ======
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

WiFiClient espClient;
Mqtt_Broadcaster mqtt_client(espClient);
Mqtt_Broadcaster* p_mqtt_client = &mqtt_client;

Humidity_Sensor leak_sensor("Leak_sensor", Sensor_Type::HUMIDITY_SENSOR, GPIO_NUM_34);
Emergancy_handler emergancy_handler(&leak_sensor, 2200, false);
Emergancy_handler* p_emergancy_handler = &emergancy_handler;

// ====== Sensors ======
//Humidity_Sensor h1("test", Sensor_Type::HUMIDITY_SENSOR, 32);

//DS18B20_Thermometer t1("thermometer", GPIO_NUM_4, "bed_room");

// ======  Pumps  ======
Cfg_Pump cfg1 = {
  "pump1",
  0,
  1000,
  0,
  GPIO_NUM_32,
  "wateringsystem/container1/fill"
};
Pump p1(cfg1);

Cfg_Pump cfg2 = {
  "pump1",
  0,
  1000,
  0,
  GPIO_NUM_27,
  "wateringsystem/container1/empty"
};
Pump p2(cfg2);

Cfg_Pump cfg3 = {
  "pump1",
  0,
  1000,
  0,
  GPIO_NUM_26,
  "wateringsystem/container2/fill"
};
Pump p3(cfg3);

Cfg_Pump cfg4 = {
  "pump1",
  0,
  1000,
  0,
  GPIO_NUM_25,
  "wateringsystem/container2/empty"
};
Pump p4(cfg4);

Cfg_Pump cfg5 = {
  "pump1",
  0,
  1000,
  0,
  GPIO_NUM_33,
  "wateringsystem/eddie/water"
};
Pump p5(cfg5);


// ====== WiFi Connect ======
void setup_wifi() {
  delay(10);
  g_p_Mqtt_Broadcaster->log("Connecting to WiFi: " + String(ssid));

  WiFi.begin(ssid, password);

  int waittime = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    waittime += 500;
    if (waittime > 2000){
      g_p_Mqtt_Broadcaster->log("Failed to connect. Restarting...");
      ESP.restart();
    }
  }
  
  g_p_Mqtt_Broadcaster->log("WiFi connected, IP adress:" + String(WiFi.localIP()));
}



void setup() {
  pinMode(GPIO_NUM_2, OUTPUT);
  digitalWrite(GPIO_NUM_2, LOW);

  Serial.begin(115200);
  setup_wifi();
  p_mqtt_client->reconnect();

  p1.init(p_mqtt_client, p_emergancy_handler);
  p2.init(p_mqtt_client, p_emergancy_handler);
  p3.init(p_mqtt_client, p_emergancy_handler);
  p4.init(p_mqtt_client, p_emergancy_handler);
  p5.init(p_mqtt_client, p_emergancy_handler);

  emergancy_handler.init(p_mqtt_client);
  leak_sensor.init(p_mqtt_client);
  leak_sensor.start_timer(1);

  //h1.init(p_mqtt_client);
  //h1.start_timer(10);

  //t1.init(p_mqtt_client);
  //t1.start_timer(5);
}


void blink_led(int ms) {
  static bool LED_state = false;
  static int timestamp = millis();

  if (millis() - timestamp > ms){
    timestamp = millis();
    LED_state = !LED_state;
    digitalWrite(GPIO_NUM_2, LED_state);
  }
}


void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }

  mqtt_client.update();

  // emergancy check
  leak_sensor.update();
  p_emergancy_handler->update();

  if (g_emergancy_stoped) {
    blink_led(50);
  }

  //h1.update();
  //t1.update();
  delay(10);
}
