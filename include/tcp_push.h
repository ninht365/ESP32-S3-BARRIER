#ifndef TCP_PUSH_H
#define TCP_PUSH_H

#include <Arduino.h>
#include <Ethernet.h>

#define TCP_PUSH_PORT        8080
#define TCP_PUSH_MAX_CLIENTS 4

// Khởi tạo TCP Push Server trên port 8080
void TcpPush_Init();

// Gọi trong loop() để chấp nhận client mới
void TcpPush_Loop();

// Broadcast một bản tin JSON đến tất cả client đang kết nối
void TcpPush_Broadcast(const String& json);

// Trả về số client đang kết nối
uint8_t TcpPush_ClientCount();

#endif // TCP_PUSH_H
