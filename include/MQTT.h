#ifndef MQTT_H
#define MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "secrets.h"


// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


/**
 * Sets up the MQTT client with the specified server and port.
 * 
 * @param mqttServer The MQTT server address.
 * @param mqttPort The MQTT server port.
 * @param callback The callback function to be called when a message is received.
 */
void setupMQTT(const char* mqttServer, int mqttPort, void callback(char* topic, byte* payload, unsigned int length))
{
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);
}


/**
 * @brief Reconnects to the MQTT broker.
 * 
 * This function attempts to reconnect to the MQTT broker until a successful connection is established.
 * It generates a unique client ID and uses it to connect to the broker with the provided username and password.
 * If the connection is successful, it subscribes to specific MQTT topics.
 * If the connection fails, it prints an error message and retries after a delay of 5 seconds.
 */
void reconnect(std::vector<String> subTopics)
{
  Serial.println("Connecting to MQTT broker...");

  while (!mqttClient.connected())
  {
    Serial.println("Reconnecting to MQTT broker...");

    String clientId = MQTT_CLIENT_ID;
    clientId += String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str(), MQTT_USER ,MQTT_PASSWORD))
    {
      Serial.println("Connected to MQTT broker.");

      for (int i = 0; i < subTopics.size(); i++)
      {
        mqttClient.subscribe(subTopics[i].c_str());
      }

      // mqttClient.subscribe("HA/thermostat");
      // mqttClient.subscribe("HA/masterBed/fanControl");
      // mqttClient.subscribe("device/masterBed/tempHumid");
    }
    else
    {
      Serial.print("Failed to connect to MQTT broker. Error code: ");
      Serial.print(mqttClient.state());
      Serial.println(". Retrying in 5 seconds...");

      delay(5000);
    }
  }
}

/**
 * Publishes an MQTT message to the specified topic.
 *
 * @param topic The topic to publish the message to.
 * @param message The JSON document containing the message to publish.
 */
void mqttPublish(const char* topic, JsonDocument message)
{
  char buffer[1024];
  size_t data = serializeJson(message, buffer);
  mqttClient.publish(topic, buffer, data);
}

/**
 * @brief Performs the MQTT loop, checking the connection status and reconnecting if necessary.
 *        This function should be called periodically to ensure proper MQTT communication.
 */
void mqttLoop(std::vector<String> subTopics)
{
  if (!mqttClient.connected())
  {
    reconnect(subTopics);
  }
  mqttClient.loop();
}

#endif // MQTT_H