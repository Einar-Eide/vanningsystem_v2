#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include ".secrets.h"
#include "MQTT_Broadcaster.h"
#include "humidity_sensor.h"
#include "DS18B20_Thermometer.h"
#include "pump.h"

// ====== WIFI SETTINGS ======
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

WiFiClient espClient;
Mqtt_Broadcaster mqtt_client(espClient);
Mqtt_Broadcaster* p_mqtt_client = &mqtt_client;


// ====== Sensors ======
Humidity_Sensor h1("test", Sensor_Type::HUMIDITY_SENSOR, 32);

DS18B20_Thermometer t1("thermometer", GPIO_NUM_4, "bed_room");

// ======  Pumps  ======
Cfg_Pump cfg1 = {
  "pump1",
  0,
  1000,
  0,
  GPIO_NUM_27,
  "wateringsystem/container1/fill"
};
Pump p1(cfg1);

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

  Serial.begin(115200);
  setup_wifi();
  p_mqtt_client->reconnect();

  p1.init(p_mqtt_client);

  h1.init(p_mqtt_client);
  h1.start_timer(10);

  t1.init(p_mqtt_client);
  t1.start_timer(5);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }

  mqtt_client.update();

  h1.update();
  t1.update();
  delay(10);
}

