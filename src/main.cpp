
#include <WiFiClient.h>
#include <OTASetup.h>
#include <secrets.h>


const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

IPAddress staticIP(192, 168, 1, 150);


void setup(void) {
  Serial.begin(115200);
  
  setupWiFi(ssid, password, staticIP);
  setupOTA();
}

void loop(void) {
  server.handleClient();
  ElegantOTA.loop();
}