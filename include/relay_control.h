#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include <Arduino.h>

// ==========================================
// CẤU HÌNH I2C & TCA9554
// ==========================================
#define TCA9554_I2C_ADDR 0x20
#define I2C_SDA_PIN      42    // Waveshare ESP32-S3: SDA = GPIO42
#define I2C_SCL_PIN      41    // Waveshare ESP32-S3: SCL = GPIO41

// ==========================================
// CẤU HÌNH BARRIER (1 Barrier = 3 kênh)
// ==========================================
#define BARRIER_CH_OPEN   1   // CH1: Lệnh MỞ    - Ưu tiên thường
#define BARRIER_CH_STOP   2   // CH2: Lệnh DỪNG  - Ưu tiên CAO NHẤT (có thể chen ngang)
#define BARRIER_CH_CLOSE  3   // CH3: Lệnh ĐÓNG  - Ưu tiên thường

// Trạng thái Barrier (State Machine)
typedef enum {
    BARRIER_IDLE     = 0,   // Rảnh, sẵn sàng nhận lệnh
    BARRIER_OPENING  = 1,   // CH1 đang xung (đang mở)
    BARRIER_STOPPING = 2,   // CH2 đang xung (đang dừng) - ưu tiên cao
    BARRIER_CLOSING  = 3,   // CH3 đang xung (đang đóng)
} BarrierState;

// Kết quả trả về khi gọi lệnh Barrier
typedef enum {
    BARRIER_CMD_OK        = 0,  // Lệnh được chấp nhận và thực thi
    BARRIER_CMD_BUSY      = 1,  // Đang bận (OPEN/CLOSE đang chạy), yêu cầu bị từ chối
    BARRIER_CMD_PREEMPTED = 2,  // STOP đã chen ngang thành công (kênh cũ bị ngắt)
} BarrierResult;

// ==========================================
// HÀM ĐIỀU KHIỂN RELAY NGUYÊN THUỶ (8 kênh)
// ==========================================
void Relay_Init();
void Relay_Loop();                                    // Gọi trong loop() để xử lý pulse timer
void Relay_Pulse(uint8_t ch, uint16_t duration_ms);  // Xung kênh ch trong duration_ms ms
void Relay_On(uint8_t ch);                            // Bật kênh ch giữ nguyên
void Relay_Off(uint8_t ch);                           // Tắt kênh ch
uint8_t Relay_GetState();                             // Trả về byte trạng thái 8 kênh

// ==========================================
// HÀM ĐIỀU KHIỂN BARRIER (Có Interlock)
// ==========================================
// action_ch: BARRIER_CH_OPEN / BARRIER_CH_STOP / BARRIER_CH_CLOSE
BarrierResult  Relay_BarrierCmd(uint8_t action_ch, uint16_t duration_ms = 400);
BarrierState   Relay_GetBarrierState();
const char*    Relay_BarrierStateName();  // "IDLE" / "OPENING" / "STOPPING" / "CLOSING"

// ==========================================
// HÀM CHẨN ĐOÁN
// ==========================================
String Relay_I2CScan();
void   Relay_WriteRaw(uint8_t data);

#endif // RELAY_CONTROL_H
