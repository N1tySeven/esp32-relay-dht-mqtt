#ifndef CONFIG_H
#define CONFIG_H

#include "secrets.h"

// --- ตั้งค่า HiveMQ Cloud ---
extern const char* const mqtt_server;
extern const char* const mqtt_topic_relay;
extern const char* const mqtt_topic_relay_2;
extern const char* const mqtt_topic_dht;
extern const char* const mqtt_topic_ldr;
extern const int mqtt_port;

// --- ตั้งค่าขาสัญญาณ ESP32 ---
#define RELAY_PIN   18
#define RELAY_PIN_2 19
#define LED_PIN     2
#define DHT_PIN     4
#define LDR_PIN     34

#endif // CONFIG_H

