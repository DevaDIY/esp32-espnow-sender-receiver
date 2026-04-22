#include <WiFi.h>
#include <esp_now.h>
#include <DHT.h>

// =====================
// DHT22
// =====================
#define DHTPIN   4
#define DHTTYPE  DHT22

DHT dht(DHTPIN, DHTTYPE);

// =====================
// ESP-NOW
// =====================
// ใส่ MAC Address ของบอร์ด Receiver ตรงนี้
uint8_t receiverMac[] = {0x24, 0x6F, 0x28, 0xAA, 0xBB, 0xCC};

typedef struct struct_message {
  float temperature;
  float humidity;
  unsigned long packetId;
} struct_message;

struct_message sensorData;

// =====================
// Non-blocking timer
// =====================
unsigned long previousSendMillis = 0;
const unsigned long sendInterval = 2000;

// =====================
// Utility
// =====================
void printMacAddress() {
  Serial.print("Sender MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Success");
  } else {
    Serial.println("Fail");
  }
}

bool initEspNow() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return false;
  }

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return false;
  }

  return true;
}

void readAndSendDHT() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT22");
    return;
  }

  sensorData.temperature = t;
  sensorData.humidity = h;
  sensorData.packetId++;

  esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&sensorData, sizeof(sensorData));

  Serial.print("Temp: ");
  Serial.print(sensorData.temperature);
  Serial.print(" C, Humidity: ");
  Serial.print(sensorData.humidity);
  Serial.print(" %, Packet ID: ");
  Serial.println(sensorData.packetId);

  if (result != ESP_OK) {
    Serial.print("esp_now_send error: ");
    Serial.println(result);
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  printMacAddress();

  dht.begin();

  if (!initEspNow()) {
    Serial.println("ESP-NOW setup failed, restart in 3 sec...");
    delay(3000);
    ESP.restart();
  }

  sensorData.packetId = 0;

  Serial.println("Sender ready");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousSendMillis >= sendInterval) {
    previousSendMillis = currentMillis;
    readAndSendDHT();
  }
}
