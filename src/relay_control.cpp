#include "relay_control.h"
#include "config.h"
#include "tcp_push.h"
#include <Wire.h>

// ==========================================
// TRẠNG THÁI NỘI BỘ
// ==========================================
static unsigned long relayOffTime[8]  = {0};
static bool          relayPulsing[8]  = {false};
static uint8_t       currentRelayState = 0x00;

struct BarrierContext {
    BarrierState state = BARRIER_IDLE;
    unsigned long lastDIToggleTime = 0;
    bool lastDIMovingState = false;
    uint8_t relayOpenCh;
    uint8_t relayCloseCh;
    uint8_t relayStopCh;
    uint8_t diFullyOpenPin;
    uint8_t diMovingClosedPin;
};

static BarrierContext barriers[2];

// ==========================================
// THAO TÁC I2C THẤP
// ==========================================
static void writeTCA9554(uint8_t data) {
    Wire.beginTransmission(TCA9554_I2C_ADDR);
    Wire.write(0x01); // Output Port Register
    Wire.write(data);
    Wire.endTransmission();
}

// ==========================================
// KHỞI TẠO
// ==========================================
void Relay_Init() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    delay(10);

    Serial.printf("[RELAY] Quet I2C tai 0x%02X (SDA:%d SCL:%d)...\n",
                  TCA9554_I2C_ADDR, I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.beginTransmission(TCA9554_I2C_ADDR);
    if (Wire.endTransmission() == 0) {
        Serial.println("[RELAY] TCA9554 PHAT HIEN OK!");
    } else {
        Serial.println("[RELAY] CANH BAO: TCA9554 KHONG PHAT HIEN! Kiem tra SDA/SCL.");
    }

    // Cấu hình tất cả 8 chân TCA9554 là OUTPUT
    Wire.beginTransmission(TCA9554_I2C_ADDR);
    Wire.write(0x03); // Configuration Register
    Wire.write(0x00); // 0x00 = tất cả OUTPUT
    Wire.endTransmission();

    writeTCA9554(0x00); // Tắt tất cả relay

    // Cấu hình DI Pins (sử dụng PULLDOWN để ngầm định = 0 khi thả nổi)
    pinMode(DI1_PIN, INPUT_PULLDOWN);
    pinMode(DI2_PIN, INPUT_PULLDOWN);
    pinMode(DI3_PIN, INPUT_PULLDOWN);
    pinMode(DI4_PIN, INPUT_PULLDOWN);
    pinMode(DI5_PIN, INPUT_PULLDOWN);
    pinMode(DI6_PIN, INPUT_PULLDOWN);
    pinMode(DI7_PIN, INPUT_PULLDOWN);
    pinMode(DI8_PIN, INPUT_PULLDOWN);

    // Cấu hình Barrier 1
    barriers[0].relayOpenCh = B1_RELAY_OPEN;
    barriers[0].relayCloseCh = B1_RELAY_CLOSE;
    barriers[0].relayStopCh = B1_RELAY_STOP;
    barriers[0].diFullyOpenPin = DI2_PIN;
    barriers[0].diMovingClosedPin = DI1_PIN;
    barriers[0].lastDIMovingState = digitalRead(DI1_PIN);
    barriers[0].lastDIToggleTime = millis() - 3000; // Giả lập đã đứng yên 3s

    // Cấu hình Barrier 2
    barriers[1].relayOpenCh = B2_RELAY_OPEN;
    barriers[1].relayCloseCh = B2_RELAY_CLOSE;
    barriers[1].relayStopCh = B2_RELAY_STOP;
    barriers[1].diFullyOpenPin = DI4_PIN;
    barriers[1].diMovingClosedPin = DI3_PIN;
    barriers[1].lastDIMovingState = digitalRead(DI3_PIN);
    barriers[1].lastDIToggleTime = millis() - 3000; // Giả lập đã đứng yên 3s

    Serial.printf("[RELAY] Khoi tao xong 2 Barrier.\n");
}

// ==========================================
// ĐIỀU KHIỂN KÊNH NGUYÊN THUỶ
// ==========================================
void Relay_On(uint8_t ch) {
    if (ch < 1 || ch > 8) return;
    currentRelayState |= (1 << (ch - 1));
    writeTCA9554(currentRelayState);
}

void Relay_Off(uint8_t ch) {
    if (ch < 1 || ch > 8) return;
    currentRelayState &= ~(1 << (ch - 1));
    writeTCA9554(currentRelayState);
}

void Relay_Pulse(uint8_t ch, uint16_t duration_ms) {
    if (ch < 1 || ch > 8) return;
    Relay_On(ch);
    relayOffTime[ch - 1]  = millis() + duration_ms;
    relayPulsing[ch - 1]  = true;
}

uint8_t Relay_GetState() {
    return currentRelayState;
}

// ==========================================
// LOOP & STATE MACHINE
// ==========================================
void Relay_Loop() {
    unsigned long now = millis();
    
    // 1. Tắt các relay đã hết thời gian pulse
    for (int i = 0; i < 8; i++) {
        if (relayPulsing[i] && now >= relayOffTime[i]) {
            relayPulsing[i] = false;
            Relay_Off(i + 1);
            uint8_t ch = i + 1;
            String evt = "{\"event\":\"relay_off\",\"channel\":"
                       + String(ch) + ",\"timestamp_ms\":" + String(now) + "}";
            TcpPush_Broadcast(evt);
            Serial.printf("[RELAY] CH%d -> OFF (ket thuc xung)\n", ch);
        }
    }

    // 2. Đọc trạng thái DI cho 2 Barrier
    for (int i = 0; i < 2; i++) {
        bool fullyOpen = digitalRead(barriers[i].diFullyOpenPin);
        bool movingClosed = digitalRead(barriers[i].diMovingClosedPin);

        if (movingClosed != barriers[i].lastDIMovingState) {
            barriers[i].lastDIMovingState = movingClosed;
            barriers[i].lastDIToggleTime = now;
        }

        unsigned long timeSinceToggle = now - barriers[i].lastDIToggleTime;
        BarrierState newState = barriers[i].state;

        if (fullyOpen) {
            newState = BARRIER_OPEN;
        } else if (timeSinceToggle < 2000) {
            // Đang nhấp nháy -> Đang nâng hoặc đang hạ
            if (currentRelayState & (1 << (barriers[i].relayCloseCh - 1))) {
                newState = BARRIER_CLOSING;
            } else {
                newState = BARRIER_OPENING; 
            }
        } else {
            // Đã đứng yên quá 2 giây
            if (movingClosed == 1) {
                newState = BARRIER_CLOSED;
            } else {
                newState = BARRIER_STOPPED;
            }
        }

        if (newState != barriers[i].state && newState != BARRIER_UNKNOWN) {
            barriers[i].state = newState;
            String evt = "{\"event\":\"barrier_state\",\"barrier\":" + String(i + 1) + ",\"state\":\"" + Relay_BarrierStateName(newState) + "\",\"timestamp_ms\":" + String(now) + "}";
            TcpPush_Broadcast(evt);
            Serial.printf("[BARRIER %d] State changed to %s\n", i + 1, Relay_BarrierStateName(newState));
        }
    }
}

// ==========================================
// BARRIER INTERLOCK STATE MACHINE
// ==========================================
BarrierState Relay_GetBarrierState(uint8_t barrier_id) {
    if (barrier_id < 1 || barrier_id > 2) return BARRIER_UNKNOWN;
    return barriers[barrier_id - 1].state;
}

const char* Relay_BarrierStateName(BarrierState state) {
    switch (state) {
        case BARRIER_IDLE:     return "IDLE";
        case BARRIER_OPENING:  return "OPENING";
        case BARRIER_OPEN:     return "OPEN";
        case BARRIER_CLOSING:  return "CLOSING";
        case BARRIER_CLOSED:   return "CLOSED";
        case BARRIER_STOPPED:  return "STOPPED";
        default:               return "UNKNOWN";
    }
}

BarrierResult Relay_BarrierCmd(uint8_t barrier_id, BarrierAction action, uint16_t duration_ms) {
    if (barrier_id < 1 || barrier_id > 2) return BARRIER_CMD_ERR_ID;
    int idx = barrier_id - 1;
    
    uint8_t ch = 0;
    const char* actionName = "";

    if (action == ACTION_OPEN) {
        ch = barriers[idx].relayOpenCh;
        actionName = "open";
    } else if (action == ACTION_CLOSE) {
        ch = barriers[idx].relayCloseCh;
        actionName = "close";
    } else if (action == ACTION_STOP) {
        ch = barriers[idx].relayStopCh;
        actionName = "stop";
    }

    if (ch == 0) return BARRIER_CMD_ERR_ID;

    // Pulse lệnh điều khiển nút bấm
    Relay_Pulse(ch, duration_ms);

    String evt = "{\"event\":\"barrier_cmd\",\"barrier\":" + String(barrier_id) 
               + ",\"channel\":" + String(ch) 
               + ",\"action\":\"" + String(actionName) 
               + "\",\"duration_ms\":" + String(duration_ms) 
               + ",\"timestamp_ms\":" + String(millis()) + "}";
    TcpPush_Broadcast(evt);
    Serial.printf("[BARRIER %d] Nhan lenh %s (CH%d xung %dms)\n", barrier_id, actionName, ch, duration_ms);

    return BARRIER_CMD_OK;
}

// ==========================================
// CHẨN ĐOÁN
// ==========================================
static const uint8_t PIN_CANDIDATES[][2] = {
    {42, 41},  // Waveshare ESP32-S3 (chuẩn)
    {8,  9},
    {6,  7},
    {1,  2},
    {10, 11},
    {21, 47},
    {3,  46},
};
static const int NUM_PIN_CANDIDATES = sizeof(PIN_CANDIDATES) / sizeof(PIN_CANDIDATES[0]);

String Relay_I2CScan() {
    String result = "{\"results\":[";
    bool firstPair = true;
    for (int p = 0; p < NUM_PIN_CANDIDATES; p++) {
        uint8_t sda = PIN_CANDIDATES[p][0];
        uint8_t scl = PIN_CANDIDATES[p][1];
        Wire.end();
        Wire.begin(sda, scl);
        delay(5);

        String devicesFound = "";
        bool firstDev = true;
        for (uint8_t addr = 1; addr < 127; addr++) {
            Wire.beginTransmission(addr);
            if (Wire.endTransmission() == 0) {
                if (!firstDev) devicesFound += ",";
                char buf[8]; sprintf(buf, "\"0x%02X\"", addr);
                devicesFound += buf;
                firstDev = false;
            }
        }
        if (devicesFound.length() > 0) {
            if (!firstPair) result += ",";
            result += "{\"sda\":" + String(sda) + ",\"scl\":" + String(scl)
                    + ",\"devices\":[" + devicesFound + "]}";
            firstPair = false;
        }
    }
    Wire.end();
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); // Khôi phục về chân chuẩn
    result += "],\"current_sda\":" + String(I2C_SDA_PIN)
            + ",\"current_scl\":" + String(I2C_SCL_PIN) + "}";
    return result;
}

void Relay_WriteRaw(uint8_t data) {
    currentRelayState = data;
    writeTCA9554(data);
    Serial.printf("[RELAY RAW] Ghi 0x%02X vao TCA9554\n", data);
}
