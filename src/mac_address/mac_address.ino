#include <WiFi.h>

void printMacAddress() {
  Serial.print("STA MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  delay(500);
  printMacAddress();
}

void loop() {
}
