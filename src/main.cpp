#include <Arduino.h>
#include "config.h"
#include "mqtt_handler.h"

void setup() {
  Serial.begin(115200);

  // ตั้งค่าขา Relay เป็น Output และสั่งปิดไว้ก่อนตั้งแต่เริ่มต้น
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(RELAY_PIN_2, OUTPUT);
  digitalWrite(RELAY_PIN_2, LOW);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  dht_setup();
  ldr_setup();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  dht_publish();
  ldr_publish();
}