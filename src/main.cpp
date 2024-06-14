#include <Arduino.h>
#include <WiFiClient.h>
#include <OTASetup.h>
#include <secrets.h>
#include <MQTT.h>
#include <Fan.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

IPAddress staticIP(STATIC_IP_ADDRESS);

Fan masterBedroomFanNorth(18, 19, 5000, 0, 8);
Fan masterBedroomFanSouth(17, 5, 5000, 1, 8);

long lastMsg = 0; // Last time a message was sent to the MQTT broker

// Variables to store data from MQTT topic "thermostat"
int tempSetpoint = 0; // Desired temperature setpoint
int targetTempHigh = 0; // Upper limit for temperature
int targetTempLow = 0; // Lower limit for temperature
float currentTemp = 0; // Current temperature
char* thermoFan = new char[32]; // State of the thermostat fan
char* state = new char[32]; // State of the thermostat

/**
 * @brief Callback function for handling MQTT messages.
 * 
 * This function is called when an MQTT message is received. It parses the message payload
 * and performs different actions based on the topic of the message.
 * 
 * @param topic The topic of the MQTT message.
 * @param payload The payload of the MQTT message.
 * @param length The length of the payload.
 */
void callback(char* topic, byte* payload, unsigned int length)
{
  JsonDocument doc;
  deserializeJson(doc, payload, length);

  // If the topic is "thermostat", parse the data accordingly
  if (strcmp(topic, "HA/thermostat") == 0)
  {
    strlcpy(state, doc["state"], 32);
    currentTemp = atoi(doc["currentTemp"]);
    strlcpy(thermoFan, doc["fan"], 32);
    targetTempHigh = atoi(doc["targetTempHigh"]);
    targetTempLow = atoi(doc["targetTempLow"]);
  }
  // If the topic is "HA/masterBed/fanControl", parse the data accordingly
  else if (strcmp(topic, "HA/masterBed/fanControl") == 0)
  {
    doc["message"] = "fanControl";
    masterBedroomFanNorth.parseMessage(doc);
    masterBedroomFanSouth.parseMessage(doc);
    Serial.println("Fan control message received");
  }
  else if (strcmp(topic, "device/masterBed/tempHumid") == 0)
  {
    doc["message"] = "roomTemp";
    masterBedroomFanNorth.parseMessage(doc);
    masterBedroomFanSouth.parseMessage(doc);
    // Serial.println("Room temperature message received");
  }
  else
  {
    Serial.println("Invalid topic");
  }
}

// MQTT broker settings
const char* mqttServer = MQTT_SERVER; // MQTT broker IP address
int mqttPort = MQTT_PORT; // MQTT broker port


/**
 * @brief Initializes the necessary components and configurations for the program.
 * 
 * This function sets up the serial communication, WiFi connection, OTA (Over-The-Air) updates,
 * and MQTT (Message Queuing Telemetry Transport) client.
 */
void setup(void) {
  Serial.begin(115200);
  
  setupWiFi(ssid, password, staticIP); // Setup WiFi connection
  setupOTA(); // Setup OTA updates

  setupMQTT(mqttServer, mqttPort, callback); // Setup MQTT client
}

/**
 * The main loop function that runs repeatedly in the program.
 * It handles client requests, performs OTA updates, and publishes temperature data to the MQTT broker.
 * It also calls the loop function of the master bedroom fans to update their state based on the current temperature.
 */
void loop(void) {

  long now = millis(); // Get the current time in milliseconds

  server.handleClient(); // Handle client requests
  ElegantOTA.loop(); // Perform OTA updates
  mqttLoop(); // Handle MQTT messages

  if (now - lastMsg > 1000) {
    lastMsg = now;
    // Publish the current temperature to the MQTT broker
    mqttPublish("device/masterBed/fan/north", masterBedroomFanNorth.pubMessage); // Publish the north fan message
    mqttPublish("device/masterBed/fan/south", masterBedroomFanSouth.pubMessage); // Publish the south fan message
  }
  masterBedroomFanNorth.loop(currentTemp); // Update the state of the north fan
  masterBedroomFanSouth.loop(currentTemp); // Update the state of the south fan
  
}