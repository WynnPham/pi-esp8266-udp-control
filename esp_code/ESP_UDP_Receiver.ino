#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* WIFI_SSID = "TMOBILE";
const char* WIFI_PASS = "Uyen2812";

const unsigned int UDP_PORT = 5005;
const int LED_PIN = D1;

WiFiUDP udp;
char packetBuffer[255];
String lastMsg = "";

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);

  Serial.begin(115200);
  Serial.setDebugOutput(false);
  delay(2000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.flush();
  }

  Serial.println();
  Serial.print("ESP IP: ");
  Serial.println(WiFi.localIP());
  Serial.flush();

  udp.begin(UDP_PORT);
  Serial.print("Listening UDP port: ");
  Serial.println(UDP_PORT);
  Serial.flush();
}

void loop() {
  int packetSize = udp.parsePacket();
  if (!packetSize) return;

  int len = udp.read(packetBuffer, 254);
  if (len <= 0) return;
  packetBuffer[len] = 0;

  String msg = String(packetBuffer);
  msg.trim();

  if (msg == lastMsg) return;
  lastMsg = msg;

  Serial.print("UDP from ");
  Serial.print(udp.remoteIP());
  Serial.print(": ");
  Serial.println(msg);
  Serial.flush();

  if (msg == "START") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
    Serial.flush();
  } else if (msg == "STOP") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
    Serial.flush();
  }
}
