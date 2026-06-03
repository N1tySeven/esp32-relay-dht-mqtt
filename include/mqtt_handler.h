#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Global Client Instances
extern WiFiClient espClient;
extern PubSubClient client;
extern DHT dht;

// Function declarations
void setup_wifi();
void callback(char *topic, byte *payload, unsigned int length);
void reconnect();
void dht_setup();
void dht_publish();

#endif // MQTT_HANDLER_H
