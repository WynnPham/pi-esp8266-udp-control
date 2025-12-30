#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// ===== CHANGE THESE =====
const char* WIFI_SSID = "TMOBILE";
const char* WIFI_PASS = "Uyen2812";
// ========================

const unsigned int UDP_PORT = 5005;

WiFiUDP udp;
char packetBuffer[255];

void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println();
  Serial.println("Booting...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("ESP IP: ");
  Serial.println(WiFi.localIP());

  // Listen for UDP on UDP_PORT
  udp.begin(UDP_PORT);
  Serial.print("UDP Receiver listening on port ");
  Serial.println(UDP_PORT);
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(packetBuffer, sizeof(packetBuffer) - 1);
    if (len > 0) packetBuffer[len] = '\0';

    Serial.print("Received UDP from ");
    Serial.print(udp.remoteIP());
    Serial.print(":");
    Serial.print(udp.remotePort());
    Serial.print(" -> ");
    Serial.println(packetBuffer);
  }
}
