#include <Arduino.h>
#include "config_manager.h"
#include "relay_control.h"
#include "ethernet_driver.h"
#include "tcp_push.h"
#include "web_server.h"

void setup() {
    Serial.begin(115200);
    unsigned long start = millis();
    while (!Serial && (millis() - start < 3000)) { delay(10); }
    delay(300);

    Serial.println("\n==========================================");
    Serial.println("  ESP32-S3 BARRIER SYSTEM                    ");
    Serial.println("==========================================");

    // 1. Đọc cấu hình IP từ NVS (phải chạy trước Ethernet_Init)
    Config_Init();

    // 2. Khởi tạo Relay (I2C TCA9554)
    Relay_Init();

    // 3. Khởi tạo Ethernet W5500 (dùng IP từ Config_Manager)
    Ethernet_Init();

    // 4. Khởi tạo TCP Push Server (port 8080)
    TcpPush_Init();

    // 5. Khởi tạo HTTP Web Server (port 80)
    WebServer_Init();

    Serial.println("\n[READY] He thong san sang!");
    Serial.printf("[READY] Web UI  : http://%s\n", Ethernet.localIP().toString().c_str());
    Serial.printf("[READY] TCP Push: %s:8080\n",   Ethernet.localIP().toString().c_str());
}

static unsigned long lastHeartbeat = 0;
unsigned long lastHeartbeat_export = 0; // export cho web_server nếu cần

void loop() {
    Relay_Loop();       // Xử lý pulse timer non-blocking
    TcpPush_Loop();     // Chấp nhận client TCP mới
    WebServer_Loop();   // Xử lý HTTP request

    // Heartbeat mỗi 3 phút
    if (millis() - lastHeartbeat > 180000) {
        lastHeartbeat = millis();
        Serial.printf("[HEARTBEAT] IP:%s | Link:%s | B1:%s B2:%s | TCP:%d clients\n",
            Ethernet.localIP().toString().c_str(),
            (Ethernet.linkStatus() == LinkON) ? "OK" : "OFF",
            Relay_BarrierStateName(Relay_GetBarrierState(1)),
            Relay_BarrierStateName(Relay_GetBarrierState(2)),
            TcpPush_ClientCount());
    }
}
