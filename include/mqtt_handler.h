#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <WiFi.h>
#include <PubSubClient.h>

// Global Client Instances
extern WiFiClient espClient;
extern PubSubClient client;

// Function declarations
void setup_wifi();
void callback(char *topic, byte *payload, unsigned int length);
void reconnect();

#endif // MQTT_HANDLER_H
