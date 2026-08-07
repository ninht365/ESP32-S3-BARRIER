#ifndef ETHERNET_DRIVER_H
#define ETHERNET_DRIVER_H

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

// Sơ đồ chân SPI Ethernet W5500 chuẩn cho bo mạch Waveshare ESP32-S3
#define ETH_SPI_SCK  15
#define ETH_SPI_MISO 14
#define ETH_SPI_MOSI 13
#define ETH_SPI_CS   16
#define ETH_RST_PIN  39

void Ethernet_Init();

#endif // ETHERNET_DRIVER_H
