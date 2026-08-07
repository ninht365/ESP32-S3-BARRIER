#include "relay_control.h"
#include "tcp_push.h"
#include <Wire.h>

// ==========================================
// TRẠNG THÁI NỘI BỘ
// ==========================================
static unsigned long relayOffTime[8]  = {0};
static bool          relayPulsing[8]  = {false};
static uint8_t       currentRelayState = 0x00;
static BarrierState  barrierState      = BARRIER_IDLE;

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
    barrierState = BARRIER_IDLE;
    Serial.printf("[RELAY] Khoi tao xong. Barrier: IDLE\n");
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

void Relay_Loop() {
    unsigned long now = millis();
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
}

// ==========================================
// BARRIER INTERLOCK STATE MACHINE
// ==========================================
BarrierState Relay_GetBarrierState() {
    return barrierState;
}

const char* Relay_BarrierStateName() {
    switch (barrierState) {
        case BARRIER_IDLE:     return "IDLE";
        case BARRIER_OPENING:  return "OPENING";
        case BARRIER_STOPPING: return "STOPPING";
        case BARRIER_CLOSING:  return "CLOSING";
        default:               return "UNKNOWN";
    }
}

BarrierResult Relay_BarrierCmd(uint8_t action_ch, uint16_t duration_ms) {
    (void)duration_ms; // Tat ca 3 kenh deu o che do GIU MAI (hold mode), khong tu dong nha
    unsigned long now = millis();
    bool isStop  = (action_ch == BARRIER_CH_STOP);  // CH2
    bool isOpen  = (action_ch == BARRIER_CH_OPEN);  // CH1
    bool isClose = (action_ch == BARRIER_CH_CLOSE); // CH3

    if (!isStop && !isOpen && !isClose) return BARRIER_CMD_BUSY;

    // --- 1. LENH MO (CH1) ---
    if (isOpen) {
        if (barrierState == BARRIER_OPENING) {
            // Da dang MO -> Giu nguyen, khong tat
            return BARRIER_CMD_OK;
        }
        // Tat ngay lap tuc CH2 (DUNG) va CH3 (DONG) neu dang bat
        Relay_Off(BARRIER_CH_STOP);
        Relay_Off(BARRIER_CH_CLOSE);

        // Bat CH1 (MO) va GIU NGUYEN
        barrierState = BARRIER_OPENING;
        Relay_On(BARRIER_CH_OPEN);
        TcpPush_Broadcast("{\"event\":\"barrier_cmd\",\"barrier\":1,\"channel\":1,\"action\":\"open\",\"mode\":\"hold\",\"timestamp_ms\":" + String(now) + "}");
        TcpPush_Broadcast("{\"event\":\"barrier_state\",\"barrier\":1,\"state\":\"OPENING\",\"timestamp_ms\":" + String(now) + "}");
        Serial.println("[BARRIER] CH1 (MO) -> ON (giu mai)");
        return BARRIER_CMD_OK;
    }

    // --- 2. LENH DUNG (CH2) ---
    if (isStop) {
        if (barrierState == BARRIER_STOPPING) {
            // Da dang DUNG -> Giu nguyen, khong tat
            return BARRIER_CMD_OK;
        }
        // Tat ngay lap tuc CH1 (MO) va CH3 (DONG) neu dang bat
        Relay_Off(BARRIER_CH_OPEN);
        Relay_Off(BARRIER_CH_CLOSE);

        // Bat CH2 (DUNG) va GIU NGUYEN
        barrierState = BARRIER_STOPPING;
        Relay_On(BARRIER_CH_STOP);
        TcpPush_Broadcast("{\"event\":\"barrier_cmd\",\"barrier\":1,\"channel\":2,\"action\":\"stop\",\"mode\":\"hold\",\"timestamp_ms\":" + String(now) + "}");
        TcpPush_Broadcast("{\"event\":\"barrier_state\",\"barrier\":1,\"state\":\"STOPPING\",\"timestamp_ms\":" + String(now) + "}");
        Serial.println("[BARRIER] CH2 (DUNG) -> ON (giu mai)");
        return BARRIER_CMD_OK;
    }

    // --- 3. LENH DONG (CH3) ---
    if (isClose) {
        if (barrierState == BARRIER_CLOSING) {
            // Da dang DONG -> Giu nguyen, khong tat
            return BARRIER_CMD_OK;
        }
        // Tat ngay lap tuc CH1 (MO) va CH2 (DUNG) neu dang bat
        Relay_Off(BARRIER_CH_OPEN);
        Relay_Off(BARRIER_CH_STOP);

        // Bat CH3 (DONG) va GIU NGUYEN
        barrierState = BARRIER_CLOSING;
        Relay_On(BARRIER_CH_CLOSE);
        TcpPush_Broadcast("{\"event\":\"barrier_cmd\",\"barrier\":1,\"channel\":3,\"action\":\"close\",\"mode\":\"hold\",\"timestamp_ms\":" + String(now) + "}");
        TcpPush_Broadcast("{\"event\":\"barrier_state\",\"barrier\":1,\"state\":\"CLOSING\",\"timestamp_ms\":" + String(now) + "}");
        Serial.println("[BARRIER] CH3 (DONG) -> ON (giu mai)");
        return BARRIER_CMD_OK;
    }

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
