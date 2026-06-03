#include "mqtt_handler.h"
#include "config.h"

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHT_PIN, DHT11);

void setup_wifi() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char message[64];
  size_t msgLen = (length < sizeof(message) - 1) ? length : sizeof(message) - 1;
  memcpy(message, payload, msgLen);
  message[msgLen] = '\0';
  Serial.println(message);

  int pin = -1;
  if (strcmp(topic, mqtt_topic_relay) == 0)   pin = RELAY_PIN;
  if (strcmp(topic, mqtt_topic_relay_2) == 0) pin = RELAY_PIN_2;

  if (pin != -1) {
    if (strcmp(message, "ON") == 0 || strcmp(message, "on") == 0 || strcmp(message, "1") == 0) {
      Serial.println("-> Action: Turn Relay ON");
      digitalWrite(pin, HIGH);
    } else if (strcmp(message, "OFF") == 0 || strcmp(message, "off") == 0 || strcmp(message, "0") == 0) {
      Serial.println("-> Action: Turn Relay OFF");
      digitalWrite(pin, LOW);
    }
  }
}

void dht_setup() {
  dht.begin();
}

void dht_publish() {
  static unsigned long lastPublish = 0;
  const unsigned long interval = 10000;

  unsigned long now = millis();
  if (now - lastPublish < interval) return;
  lastPublish = now;

  if (!client.connected()) return;

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT11 read failed");
    return;
  }

  char payload[48];
  snprintf(payload, sizeof(payload), "{\"temperature\":%.1f,\"humidity\":%.1f}", temp, hum);
  client.publish(mqtt_topic_dht, payload);

  Serial.print("DHT11 published: ");
  Serial.println(payload);
}

void reconnect() {
  static unsigned long lastAttempt = 0;
  const unsigned long retryInterval = 5000;

  if (client.connected()) return;

  unsigned long now = millis();
  if (now - lastAttempt < retryInterval) return;
  lastAttempt = now;

  Serial.print("Attempting MQTT connection...");

  char clientId[32];
  snprintf(clientId, sizeof(clientId), "ESP32-%s", WiFi.macAddress().c_str());

  if (client.connect(clientId)) {
    Serial.println("connected to MQTT");
    client.subscribe(mqtt_topic_relay);
    client.subscribe(mqtt_topic_relay_2);
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
  }
}
