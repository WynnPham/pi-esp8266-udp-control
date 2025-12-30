#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* WIFI_SSID = "TMOBILE";
const char* WIFI_PASS = "Uyen2812";

const unsigned int UDP_PORT = 5005;

WiFiUDP udp;
char packetBuffer[256];

enum State { IDLE, RUNNING };
State state = IDLE;

IPAddress piIP;
uint16_t piPort = 0;

unsigned long nextBlinkMs = 0;
unsigned long nextSampleMs = 0;
unsigned long nextSendMs = 0;

bool ledOn = false;

int samples[5];
int sampleIndex = 0;
int sampleCount = 0;

void onboardLed(bool on) {
  digitalWrite(LED_BUILTIN, on ? LOW : HIGH);
  ledOn = on;
}

void resetState() {
  state = IDLE;
  piPort = 0;
  sampleIndex = 0;
  sampleCount = 0;
  onboardLed(false);
}

void startState(IPAddress ip, uint16_t port) {
  resetState();
  state = RUNNING;
  piIP = ip;
  piPort = port;

  unsigned long now = millis();
  nextBlinkMs = now + 500;
  nextSampleMs = now + 1000;
  nextSendMs  = now + 2000;
}

int average5() {
  long sum = 0;
  for (int i = 0; i < 5; i++) sum += samples[i];
  return sum / 5;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  onboardLed(false);

  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("BOOT (115200)");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("ESP IP: ");
  Serial.println(WiFi.localIP());

  udp.begin(UDP_PORT);
  Serial.print("UDP listening on port: ");
  Serial.println(UDP_PORT);

  resetState();
}

void loop() {
  /* ====== RECEIVE UDP ====== */
  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
    int len = udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
      String msg = String(packetBuffer);
      msg.trim();

      if (msg == "START") {
        Serial.print("RX START from ");
        Serial.print(udp.remoteIP());
        Serial.print(":");
        Serial.println(udp.remotePort());
        startState(udp.remoteIP(), udp.remotePort());
      }

      if (msg == "STOP") {
        Serial.println("RX STOP");
        resetState();
      }
    }
  }

  if (state != RUNNING) return;

  unsigned long now = millis();

  /* ====== BLINK ONBOARD LED 0.5s ====== */
  if ((long)(now - nextBlinkMs) >= 0) {
    onboardLed(!ledOn);
    nextBlinkMs += 500;
  }

  /* ====== READ A0 EVERY 1s ====== */
  if ((long)(now - nextSampleMs) >= 0) {
    int v = analogRead(A0);
    samples[sampleIndex] = v;
    sampleIndex = (sampleIndex + 1) % 5;
    if (sampleCount < 5) sampleCount++;
    nextSampleMs += 1000;
  }

  /* ====== DEBUG A0 EVERY 1s ====== */
  static unsigned long t = 0;
  if (millis() - t > 1000) {
    t = millis();
    Serial.print("A0 = ");
    Serial.println(analogRead(A0));
  }

  /* ====== SEND AVG EVERY 2s AFTER 5 SAMPLES ====== */
  if (sampleCount >= 5 && piPort != 0 && (long)(now - nextSendMs) >= 0) {
    int avg = average5();
    char out[16];
    sprintf(out, "%d", avg);

    udp.beginPacket(piIP, piPort);
    udp.write((uint8_t*)out, strlen(out));
    udp.endPacket();

    Serial.print("TX AVG -> ");
    Serial.println(out);

    nextSendMs += 2000;
  }
}
