#ifndef OTASetup_h
#define OTASetup_h


#include <WiFi.h>
#include <ElegantOTA.h>
#include <WebServer.h>

WebServer server(80);

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

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

unsigned long ota_progress_millis = 0;

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}


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