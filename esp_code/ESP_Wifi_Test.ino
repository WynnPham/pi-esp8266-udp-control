#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// ===== CHANGE THESE =====
const char* WIFI_SSID = "TMOBILE";
const char* WIFI_PASS = "Uyen2812";

// Put your Raspberry Pi IP here (from: hostname -I)
const char* PI_IP = "192.168.12.77";
// ========================

const unsigned int UDP_PORT = 5005;

WiFiUDP udp;

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

  // Start UDP (bind to any local port)
  udp.begin(UDP_PORT);
  Serial.print("UDP started. Sending to Pi at ");
  Serial.print(PI_IP);
  Serial.print(":");
  Serial.println(UDP_PORT);
}

void loop() {
  const char* msg = "PING_FROM_ESP8266";

  udp.beginPacket(PI_IP, UDP_PORT);
  udp.write((const uint8_t*)msg, strlen(msg));
  udp.endPacket();

  Serial.println("Sent UDP: PING_FROM_ESP8266");
  delay(2000);
}
