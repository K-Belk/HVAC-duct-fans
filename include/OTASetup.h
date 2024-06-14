#ifndef OTASetup_h
#define OTASetup_h


#include <WiFi.h>
#include <ElegantOTA.h>
#include <WebServer.h>

WebServer server(80);

/**
 * @brief Sets up the WiFi connection with the provided credentials and optional static IP configuration.
 * 
 * @param ssid The SSID of the WiFi network.
 * @param password The password of the WiFi network.
 * @param staticIP The static IP address to be assigned to the device (optional).
 * @param gateway The gateway IP address (default: 192.168.1.1).
 * @param subnet The subnet mask (default: 255.255.255.0).
 */
void setupWiFi(const char* ssid, const char* password, IPAddress staticIP , IPAddress gateway = IPAddress(192, 168, 1, 1), IPAddress subnet = IPAddress(255, 255, 255, 0)) {
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    if (!WiFi.config(staticIP, gateway, subnet)) {
        Serial.println("Failed to configure static IP");
    }

    // WIFI Connection, Reboot after 30 attempts
    uint8_t not_connected_counter = 0;
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
        not_connected_counter++;
        if (not_connected_counter > 30)
        {
        Serial.println("Resetting due to Wifi not connecting...");
        ESP.restart();
        }
    }
    Serial.println("Connected to WiFi");
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC address: ");
    Serial.println(WiFi.macAddress());

  server.on("/", []() {
    server.send(200, "text/plain", "Hi! This is ElegantOTA Demo.");
  });

}

/**
 * @brief Function called when OTA update starts.
 * 
 * This function is called when the OTA (Over-The-Air) update process starts.
 * It logs a message indicating that the OTA update has started.
 * 
 * @note You can add your own code inside this function to perform any additional tasks during the OTA update process.
 */
void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

unsigned long ota_progress_millis = 0; // Variable to store the last time OTA progress was logged
/**
 * Callback function for OTA (Over-The-Air) progress.
 * 
 * This function is called to report the progress of an OTA update.
 * It logs the current and final size of the update every 1 second.
 * 
 * @param current The current size of the update in bytes.
 * @param final The final size of the update in bytes.
 */
void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

/**
 * Callback function called when OTA (Over-The-Air) update has ended.
 * 
 * @param success A boolean indicating whether the OTA update was successful or not.
 */
void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}

/**
 * @brief Sets up the OTA (Over-The-Air) feature.
 * 
 * This function enables the OTA feature and starts the HTTP server for OTA updates.
 * It also sets up the callbacks for ElegantOTA events such as start, progress, and end.
 * 
 * @param None
 * @return None
 */
void setupOTA() {
    Serial.println("\nEnabling OTA Feature");
    ElegantOTA.begin(&server);    // Start ElegantOTA
    // ElegantOTA callbacks
    ElegantOTA.onStart(onOTAStart);
    ElegantOTA.onProgress(onOTAProgress);
    ElegantOTA.onEnd(onOTAEnd);

    server.begin();
    Serial.println("HTTP server started");
}
#endif // OTASetup_h