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

// 3. Thời gian xung mặc định (milliseconds)
#define DEFAULT_PULSE_MS 400

// 4. Cấu hình Cổng Web Server
#define HTTP_PORT        80

#endif // CONFIG_H
