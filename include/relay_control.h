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
// CẤU HÌNH BARRIER
// ==========================================

// Lệnh điều khiển Barrier
typedef enum {
    ACTION_OPEN  = 1,
    ACTION_CLOSE = 2,
    ACTION_STOP  = 3
} BarrierAction;

// Trạng thái Barrier (Dựa trên đọc tín hiệu DI)
typedef enum {
    BARRIER_UNKNOWN  = 0,
    BARRIER_IDLE     = 1,   // Trạng thái rảnh ban đầu khi chưa đọc được tín hiệu
    BARRIER_OPENING  = 2,   // Đang nâng
    BARRIER_OPEN     = 3,   // Đã mở hoàn toàn
    BARRIER_CLOSING  = 4,   // Đang hạ
    BARRIER_CLOSED   = 5,   // Đã đóng hoàn toàn
    BARRIER_STOPPED  = 6,   // Bị dừng ở vị trí lửng
} BarrierState;

// Kết quả trả về khi gọi lệnh Barrier
typedef enum {
    BARRIER_CMD_OK        = 0,  // Lệnh được chấp nhận và thực thi
    BARRIER_CMD_BUSY      = 1,  // Đang bận, yêu cầu bị từ chối
    BARRIER_CMD_PREEMPTED = 2,  // Lệnh dừng đã chen ngang thành công
    BARRIER_CMD_ERR_ID    = 3,  // Sai ID barrier (chỉ hỗ trợ 1 hoặc 2)
    BARRIER_CMD_IGNORED   = 4,  // Bỏ qua do phần mềm khóa (Software Interlock)
} BarrierResult;

// ==========================================
// HÀM ĐIỀU KHIỂN RELAY NGUYÊN THUỶ (8 kênh)
// ==========================================
void Relay_Init();
void Relay_Loop();                                    // Gọi trong loop() để xử lý pulse timer và DI State Machine
void Relay_Pulse(uint8_t ch, uint16_t duration_ms);  // Xung kênh ch trong duration_ms ms
void Relay_On(uint8_t ch);                            // Bật kênh ch giữ nguyên
void Relay_Off(uint8_t ch);                           // Tắt kênh ch
uint8_t Relay_GetState();                             // Trả về byte trạng thái 8 kênh

// ==========================================
// HÀM ĐIỀU KHIỂN BARRIER (Hỗ trợ 2 Barrier)
// ==========================================
// barrier_id: 1 hoặc 2
BarrierResult  Relay_BarrierCmd(uint8_t barrier_id, BarrierAction action, uint16_t duration_ms = 400);
BarrierState   Relay_GetBarrierState(uint8_t barrier_id);
const char*    Relay_BarrierStateName(BarrierState state);

// ==========================================
// HÀM CHẨN ĐOÁN
// ==========================================
String Relay_I2CScan();
void   Relay_WriteRaw(uint8_t data);

#endif // RELAY_CONTROL_H
