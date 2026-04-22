#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================
// OLED
// =====================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// =====================
// Data structure
// =====================
typedef struct struct_message {
  float temperature;
  float humidity;
  unsigned long packetId;
} struct_message;

struct_message incomingData;

// =====================
// State
// =====================
float latestTemp = 0.0;
float latestHumidity = 0.0;
unsigned long latestPacketId = 0;
bool hasNewData = false;
bool dataReceived = false;

unsigned long lastReceiveMillis = 0;

// =====================
// Non-blocking display refresh
// =====================
unsigned long previousDisplayMillis = 0;
const unsigned long displayInterval = 300;

// =====================
// Utility
// =====================
void printMacAddress() {
  Serial.print("Receiver MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void drawScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ESP-NOW Receiver");

  if (dataReceived) {
    display.setCursor(0, 16);
    display.print("Temp: ");
    display.print(latestTemp, 1);
    display.println(" C");

    display.setCursor(0, 30);
    display.print("Humi: ");
    display.print(latestHumidity, 1);
    display.println(" %");

    display.setCursor(0, 44);
    display.print("Packet: ");
    display.println(latestPacketId);

    display.setCursor(0, 56);
    display.print("Last: ");
    display.print((millis() - lastReceiveMillis) / 1000);
    display.print("s ago");
  } else {
    display.setCursor(0, 24);
    display.println("Waiting data...");
  }

  display.display();
}

void onDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingDataBytes, int len) {
  if (len == sizeof(struct_message)) {
    memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));

    latestTemp = incomingData.temperature;
    latestHumidity = incomingData.humidity;
    latestPacketId = incomingData.packetId;
    lastReceiveMillis = millis();
    hasNewData = true;
    dataReceived = true;

    Serial.println("=== Data Received ===");
    Serial.print("Temp: ");
    Serial.println(latestTemp);
    Serial.print("Humidity: ");
    Serial.println(latestHumidity);
    Serial.print("Packet ID: ");
    Serial.println(latestPacketId);
  } else {
    Serial.println("Received data size mismatch");
  }
}

bool initEspNow() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return false;
  }

  esp_now_register_recv_cb(onDataRecv);
  return true;
}

bool initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    return false;
  }

  display.clearDisplay();
  display.display();
  return true;
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  printMacAddress();

  Wire.begin(21, 22);

  if (!initDisplay()) {
    Serial.println("Display setup failed");
    while (true) {
      delay(1000);
    }
  }

  if (!initEspNow()) {
    Serial.println("ESP-NOW setup failed");
    while (true) {
      delay(1000);
    }
  }

  drawScreen();
  Serial.println("Receiver ready");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousDisplayMillis >= displayInterval) {
    previousDisplayMillis = currentMillis;
    drawScreen();
  }
}
