#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include <Ethernet.h>

// Đọc cấu hình mạng từ NVS khi khởi động fgfg
void Config_Init();

// Lấy giá trị IP hiện tại (đã đọc từ NVS hoặc mặc định)
IPAddress Config_GetIP();
IPAddress Config_GetGateway();
IPAddress Config_GetSubnet();

// Lưu cấu hình mạng mới vào NVS, trả về true nếu IP hợp lệ
bool Config_SaveNetwork(const String& ip, const String& gateway, const String& subnet);

// Lấy JSON thông tin cấu hình hiện tại
String Config_GetJson();

#endif // CONFIG_MANAGER_H
