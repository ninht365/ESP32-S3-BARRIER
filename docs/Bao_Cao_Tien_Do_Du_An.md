# BÁO CÁO TIẾN ĐỘ & NÂNG CẤP HỆ THỐNG ESP32-S3 BARRIER
**Người báo cáo:** [Tên của bạn]
**Giai đoạn:** Nâng cấp tính năng Dual-Barrier & Tích hợp DI Feedback

---

## I. TỔNG QUAN YÊU CẦU NÂNG CẤP
Hệ thống cũ chỉ đáp ứng điều khiển cơ bản cho 1 Barrier. Để đáp ứng nhu cầu thực tế của dự án, hệ thống đã được tái cấu trúc và nâng cấp với 3 mục tiêu cốt lõi:
1. Quản lý đồng thời **2 Barrier độc lập** trên cùng một bo mạch ESP32-S3 PoE ETH.
2. Tích hợp đọc tín hiệu phản hồi từ phần cứng (Digital Input - DI) của board CAME ZL38 để xác định **chính xác trạng thái thực tế** của cổng.
3. Nâng cấp phương thức giao tiếp mạng (Web UI, API, TCP Socket) để điều khiển và phản hồi theo **thời gian thực (Real-time)**.

---

## II. CÁC HẠNG MỤC ĐÃ HOÀN THÀNH

### 1. Cấu trúc lại Firmware (Hardware Layer)
- **Thiết lập Hardware Mapping cho 2 Barrier:**
  - **Barrier 1:** Sử dụng Kênh Rơ-le 1 (Mở), 2 (Đóng), 3 (Dừng). Phản hồi DI qua DI2 (Đã mở) và DI1 (Đang chạy/Đã đóng).
  - **Barrier 2:** Sử dụng Kênh Rơ-le 4 (Mở), 5 (Đóng), 6 (Dừng). Phản hồi DI qua DI4 (Đã mở) và DI3 (Đang chạy/Đã đóng).
- **Xử lý chống nhiễu phần cứng:** Cấu hình toàn bộ chân DI sang chế độ `INPUT_PULLDOWN` để loại bỏ hoàn toàn nhiễu tín hiệu (tín hiệu ảo) khi bo mạch thả nổi hoặc khi rớt kết nối vật lý với Barrier.

### 2. Phát triển State Machine (Logic Layer)
Xây dựng một Cỗ máy trạng thái (State Machine) thông minh dựa trên tín hiệu của chân DI:
- **Xử lý tín hiệu nhấp nháy:** Phát hiện xung đảo mức (0/1) liên tục từ CAME ZL38 để tự động suy luận trạng thái `OPENING` (Đang nâng) hoặc `CLOSING` (Đang hạ).
- **Xử lý tín hiệu đứng yên:** Sử dụng timer để theo dõi. Nếu tín hiệu giữ nguyên trong 2 giây, hệ thống sẽ chốt trạng thái `CLOSED` (Đã đóng hoàn toàn), `OPEN` (Đã mở hoàn toàn) hoặc `STOPPED` (Bị kẹt/Dừng lửng).
- **Chống lỗi khởi động (Boot False-Positive):** Cài đặt biến lưu trữ lùi thời gian (offset 3s) khi mạch vừa cấp nguồn để ngăn chặn việc State Machine hiểu nhầm trạng thái Đóng thành Đang Nâng/Hạ trong 2 giây đầu tiên.

### 3. Nâng cấp Giao thức Mạng & API (Network Layer)
- **RESTful API mới:** Sửa đổi Endpoint `/api/barrier` để nhận diện tham số `id=1` hoặc `id=2`. Trả về cục diện (JSON) chi tiết của Barrier đang được gọi.
- **TCP Push Server (Port 8080):** Thay vì bắt phần mềm quản lý bãi xe phải liên tục gọi API (Polling) gây lãng phí băng thông và độ trễ cao, ESP32 giờ đây đóng vai trò là một Push Server. Ngay khi Barrier có thay đổi trạng thái (do app gọi, hoặc do người dùng thao tác tay), ESP32 lập tức chủ động đẩy một bản tin JSON (`barrier_state`, `barrier_cmd`) qua giao thức TCP Raw Socket đến mọi Client đang theo dõi.

### 4. Thiết kế lại Giao diện người dùng (Web UI)
- Đập đi xây lại toàn bộ giao diện điều khiển nội bộ (HTML/CSS/JS nhúng thẳng trong C++).
- Giao diện Dark Mode hiện đại, chia thành 2 cột điều khiển độc lập cho 2 Barrier.
- **Tính năng Interlock an toàn:** Nút bấm MỞ sẽ tự động bị khóa (vô hiệu hóa) nếu trạng thái của Barrier đang là OPEN. Tương tự với nút ĐÓNG.
- **Xử lý mất kết nối (Fail-safe):** Web UI liên tục ping ngầm. Nếu cáp mạng bị đứt hoặc mất kết nối, màn hình sẽ hiện cảnh báo đỏ và khóa toàn bộ nút bấm để đảm bảo an toàn.

---

## III. KẾT QUẢ ĐẠT ĐƯỢC
- Code đang hoạt động ổn định trên nhánh `feature/dual-barrier`, không làm ảnh hưởng đến tiến độ của nhánh `main`.
- Đã giải quyết triệt để lỗi biên dịch thư viện `EthernetServer::begin()` do xung đột với ESP32 Core phiên bản mới.
- Đã bổ sung tài liệu `API_Integration_Guide.md` hoàn chỉnh để bàn giao cho đội ngũ phần mềm bên trên (Camera AI / Web bãi xe) có thể tiến hành ghép nối.

---
*Tài liệu này được biên soạn để báo cáo nghiệm thu kỹ thuật nội bộ dự án.*
