#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// CẤU HÌNH TỔNG QUAN HỆ THỐNG ESP32-S3 BARRIER
// (Bo mạch Waveshare ESP32-S3 POE ETH 8DI 8RO)
// ==========================================

// 1. SPI BUS cho W5500 Ethernet
#define ETH_SPI_SCK     15
#define ETH_SPI_MISO    14
#define ETH_SPI_MOSI    13
#define ETH_SPI_CS      16
#define ETH_RST_PIN     39

// 2. I2C BUS cho Chip TCA9554PWR (Điều khiển 8 Kênh Relay)
#define TCA9554_I2C_ADDR 0x20
#define I2C_SDA_PIN      42
#define I2C_SCL_PIN      41

// 3. DIGITAL INPUT PINS (DI1 -> DI8)
#define DI1_PIN 4
#define DI2_PIN 5
#define DI3_PIN 6
#define DI4_PIN 7
#define DI5_PIN 8
#define DI6_PIN 9
#define DI7_PIN 10
#define DI8_PIN 11

// 4. CẤU HÌNH BARRIER 1 -> 4 (RELAY CHANNELS)
// Barrier 1 (Ô tô 1)
#define B1_RELAY_OPEN   1
#define B1_RELAY_CLOSE  2

// Barrier 2 (Xe máy 1)
#define B2_RELAY_OPEN   3
#define B2_RELAY_CLOSE  4

// Barrier 3 (Xe máy 2)
#define B3_RELAY_OPEN   5
#define B3_RELAY_CLOSE  6

// Barrier 4 (Dự phòng)
#define B4_RELAY_OPEN   7
#define B4_RELAY_CLOSE  8

// 5. Thời gian xung mặc định (milliseconds)
#define DEFAULT_PULSE_MS 400

// 6. Cấu hình Cổng Web Server
#define HTTP_PORT        80

#endif // CONFIG_H
