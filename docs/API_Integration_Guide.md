# TÀI LIỆU TÍCH HỢP HỆ THỐNG ĐIỀU KHIỂN BARRIER (ESP32-S3)

**Phiên bản:** 1.0.0
**Mô tả:** Tài liệu hướng dẫn giao thức giao tiếp và tích hợp API cho bộ điều khiển Barrier trung tâm (ESP32-S3). Tài liệu này dành cho các nhà phát triển phần mềm, kỹ thuật viên tích hợp hệ thống (Camera AI, phần mềm bãi xe).

---

## 1. TỔNG QUAN HỆ THỐNG

Bộ điều khiển cung cấp 2 phương thức giao tiếp chính qua chuẩn mạng Ethernet TCP/IP:
1.  **HTTP RESTful API (Port 80):** Dùng để phần mềm trung tâm (Client) chủ động gọi lệnh điều khiển (Open/Close/Stop) và hỏi trạng thái.
2.  **TCP Socket Push Server (Port 8080):** Kênh duy trì kết nối theo thời gian thực. Thiết bị sẽ chủ động "đẩy" (Push) các sự kiện về phần mềm trung tâm (như có người tác động bằng tay, sự cố).

**Định dạng dữ liệu:** Tất cả dữ liệu trả về đều dưới chuẩn `JSON`.

---

## 2. GIAO THỨC HTTP API (ĐIỀU KHIỂN & TRUY VẤN)

Giao thức sử dụng là `HTTP GET`.
*Giả sử IP của thiết bị ESP32 được cấu hình tĩnh là: `192.168.1.100`*

### 2.1. Điều khiển Barrier (Quan trọng nhất)
Sử dụng API này để ra lệnh Đóng, Mở hoặc Dừng Barrier. Thiết bị tự động tính toán xuất xung điều khiển (mặc định 400ms) đến các rơ-le tương ứng.

*   **Endpoint:** `GET http://192.168.1.100/api/barrier`
*   **Tham số (Parameters):**
    *   `action` (Bắt buộc): `open` | `close` | `stop`
    *   `duration` (Tùy chọn): Thời gian giữ xung tính bằng mili-giây (Mặc định 400ms, phạm vi 100 - 5000).

**Ví dụ:** Mở cổng
`GET http://192.168.1.100/api/barrier?action=open`

**Phản hồi (JSON):**
```json
{
  "result": "ok", 
  "action": "open",
  "duration_ms": 400,
  "current_state": "OPENING"
}
```
*(Ghi chú: Nếu trạng thái đang bận, `result` có thể trả về "busy" hoặc "preempted")*

### 2.2. Kiểm tra trạng thái hệ thống
Sử dụng để kiểm tra thiết bị có đang online không, trạng thái các rơ-le và tình trạng barrier.

*   **Endpoint:** `GET http://192.168.1.100/api/status`

**Phản hồi (JSON):**
```json
{
  "status": "online",
  "ip": "192.168.1.100",
  "gateway": "192.168.1.1",
  "uptime_s": 3600,
  "eth_link": true,
  "tcp_clients": 1,
  "relays_byte": 1,
  "relays": {
    "CH1": true,
    "CH2": false,
    "CH3": false,
    "CH4": false,
    "CH5": false,
    "CH6": false,
    "CH7": false,
    "CH8": false
  },
  "barrier_state": "IDLE"
}
```

### 2.3. Điều khiển Relay mức thấp (Raw Control)
Dành cho trường hợp muốn điều khiển từng kênh Relay cụ thể thay vì dùng logic Barrier.

*   **Endpoint:** `GET http://192.168.1.100/api/relay`
*   **Tham số:**
    *   `ch` (Kênh Relay): Từ `1` đến `8`, hoặc `all`.
    *   `action` (Hành động): `on` | `off`. (Nếu không truyền action, mặc định sẽ xuất xung pulse).
    *   `duration` (Tùy chọn): Thời gian xung (nếu không có action).

**Ví dụ:** Bật Relay 1
`GET http://192.168.1.100/api/relay?ch=1&action=on`

**Phản hồi (JSON):**
```json
{
  "status": "success",
  "channel": 1,
  "duration_ms": 400,
  "command": "on"
}
```

---

## 3. GIAO THỨC TCP PUSH (REAL-TIME EVENT)

Giao thức TCP raw socket giúp phần mềm bãi xe không cần tốn tài nguyên gọi API (polling) liên tục.

*   **IP / Port:** `192.168.1.100:8080`
*   **Luồng hoạt động:**
    1. Client (Phần mềm PC) mở kết nối Socket TCP đến Port 8080.
    2. Ngay khi kết nối thành công, ESP32 sẽ gửi bản tin chào đón (Welcome message).
    3. Client giữ nguyên kết nối (Keep-Alive). Bất cứ khi nào Barrier hoạt động, ESP32 sẽ đẩy bản tin về.

**Bản tin chào đón (ngay khi connect):**
```json
{
  "event": "connected",
  "ip": "192.168.1.100",
  "port": 8080,
  "version": "1.0"
}
```

*(Ghi chú: Nếu hệ thống đã đạt giới hạn Client, kết nối sẽ bị từ chối với thông báo: `{"event":"rejected","reason":"max_clients"}`)*

---

## 4. HƯỚNG DẪN CẤU HÌNH MẠNG BAN ĐẦU (CHO KỸ THUẬT VIÊN)

Để thiết bị kết nối vào LAN, kỹ thuật viên cần thiết lập IP tĩnh.
1. Nguồn, cắm cáp mạng.
2. Thiết bị mặc định có IP, ví dụ `192.168.1.200`. Truy cập trình duyệt `http://192.168.1.200`.
3. Giao diện Web hiển thị, nhập IP Tĩnh, Gateway, Subnet mong muốn rồi nhấn **Lưu**.
4. Hoặc sử dụng API đổi IP:
   `GET http://192.168.1.200/api/config/setip?ip=192.168.1.100&gw=192.168.1.1&sn=255.255.255.0`
5. Thiết bị tự động khởi động lại và nhận IP mới.

---
*Tài liệu lưu hành nội bộ dự án.*
