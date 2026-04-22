# ESP32 ESP-NOW: Sender DHT22 → Receiver OLED

โปรเจกต์ตัวอย่างสำหรับมือใหม่ ใช้ **ESP32 DevKit V1** จำนวน 2 ฝั่งขึ้นไป โดยมีโครงสร้างดังนี้:

- **Sender**: อ่านค่า DHT22 ที่ `GPIO4`
- **Receiver**: รับข้อมูลผ่าน ESP-NOW แล้วแสดงผลบนจอ OLED 0.96" (SSD1306 I2C)
- ใช้แนวคิด **non-blocking** ด้วย `millis()`
- มีไฟล์สำหรับดู **MAC Address** ของบอร์ดก่อนนำไปใช้งานจริง

## บทความที่เกี่ยวข้อง

- [ESP32 คืออะไร](https://devadiy.com/esp32-%E0%B8%84%E0%B8%B7%E0%B8%AD%E0%B8%AD%E0%B8%B0%E0%B9%84%E0%B8%A3/)
- [ESP32 ESP-NOW คืออะไร](https://devadiy.com/esp32-esp-now-what-is/)

## โครงสร้างโปรเจกต์

```text
src/
├── mac_address/
│   └── mac_address.ino
├── sender/
│   └── sender.ino
└── receiver/
    └── receiver.ino
```

## อุปกรณ์

### Sender
- ESP32 DevKit V1
- DHT22

### Receiver
- ESP32 DevKit V1
- OLED 0.96" I2C (SSD1306)

## ไลบรารีที่ต้องติดตั้งใน Arduino IDE

- `DHT sensor library` by Adafruit
- `Adafruit Unified Sensor`
- `Adafruit GFX Library`
- `Adafruit SSD1306`

## การต่อวงจร

### Sender: DHT22
- VCC → 3.3V
- GND → GND
- DATA → GPIO4

> ถ้าเป็น DHT22 แบบตัวเปล่า ให้ใส่ pull-up resistor 10k ระหว่าง DATA กับ VCC

### Receiver: OLED SSD1306 I2C
- VCC → 3.3V
- GND → GND
- SDA → GPIO21
- SCL → GPIO22

## ขั้นตอนใช้งาน

### 1) หา MAC Address ของ Receiver
อัปโหลดไฟล์ `src/mac_address/mac_address.ino` ไปที่บอร์ด Receiver แล้วเปิด Serial Monitor ที่ `115200`

จะได้ MAC ประมาณนี้:

```text
Receiver MAC Address: 24:6F:28:AA:BB:CC
```

### 2) นำ MAC ไปใส่ในไฟล์ Sender
เปิดไฟล์ `src/sender/sender.ino` แล้วแก้ตัวแปรนี้:

```cpp
uint8_t receiverMac[] = {0x24, 0x6F, 0x28, 0xAA, 0xBB, 0xCC};
```

### 3) อัปโหลด Receiver ก่อน
อัปโหลด `src/receiver/receiver.ino`

### 4) อัปโหลด Sender
อัปโหลด `src/sender/sender.ino`

## แนวคิดที่ใช้ในโปรเจกต์

- ใช้ `WiFi.mode(WIFI_STA)` เพื่อเปิดโหมดที่เหมาะกับ ESP-NOW
- ใช้ `esp_now_init()` เพื่อเริ่มระบบสื่อสาร
- ใช้ `struct` เพื่อส่งข้อมูลหลายค่าไปพร้อมกัน
- ใช้ `millis()` แทน `delay()` เพื่อให้เป็น **non-blocking**

## ข้อมูลที่ส่ง

Sender ส่งข้อมูลชุดเดียวกันนี้ไปยัง Receiver:

- `temperature`
- `humidity`
- `packetId`

## หมายเหตุ

- โค้ดชุดนี้เป็นตัวอย่างแบบ **Unicast** คือ Sender ส่งไปหา Receiver ที่ระบุ MAC ไว้เพียงเครื่องเดียว
- ถ้าต้องการทำ **1 ส่ง 4 รับ** สามารถต่อยอดเป็นแบบ Broadcast หรือเพิ่ม Peer หลายตัวได้

## License

ใช้งานเพื่อการเรียนรู้และทดลองต่อยอดได้ตามเหมาะสม
