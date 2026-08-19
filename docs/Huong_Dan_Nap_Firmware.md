# HƯỚNG DẪN ĐÓNG GÓI VÀ NẠP FIRMWARE (KHÔNG CẦN PLATFORMIO)

Tài liệu này hướng dẫn quy trình trích xuất bản dịch (file `.bin`) từ mã nguồn và cách sử dụng các công cụ nạp (Flasher) chuyên dụng để nạp thẳng vào mạch ESP32-S3. Phương pháp này rất hữu ích khi cần bàn giao cho kỹ thuật viên lắp đặt hoặc khách hàng tự nạp phần mềm mà không yêu cầu họ phải biết về lập trình hay cài đặt VS Code / PlatformIO.

---

## PHẦN 1: DÀNH CHO NHÀ PHÁT TRIỂN (QUY TRÌNH ĐÓNG GÓI FIRMWARE)

Khi bạn đã hoàn thiện code trên PlatformIO và muốn tạo một "Gói Cài Đặt", bạn cần lấy các file nhị phân (`.bin`) đã được biên dịch.

### 1. Dịch mã nguồn (Build)
Trên VS Code / PlatformIO, nhấn nút **Build (dấu tick 	✓)** để biên dịch toàn bộ dự án.

### 2. Trích xuất các file `.bin`
Sau khi Build thành công, hãy vào thư mục ẩn `.pio/build/esp32-s3-devkitc-1/` trong thư mục dự án của bạn. 
Hãy copy 4 file sau ra một thư mục riêng (ví dụ: tạo thư mục `Release_Firmware_v1.0`):

1. `bootloader.bin` (Nằm ở `.pio/build/esp32-s3-devkitc-1/bootloader.bin`)
2. `partitions.bin` (Nằm ở `.pio/build/esp32-s3-devkitc-1/partitions.bin`)
3. `firmware.bin` (Nằm ở `.pio/build/esp32-s3-devkitc-1/firmware.bin`)
4. *(Tùy chọn nếu có)* `boot_app0.bin` (Có thể nằm trong thư mục cài đặt package của framework, hoặc đôi khi không bắt buộc tùy vào partition scheme, nhưng thường nạp ở địa chỉ `0xe000`).

### 3. Đóng gói phân phối
Bạn nén thư mục `Release_Firmware_v1.0` thành file ZIP và gửi cho kỹ thuật viên cùng với phần hướng dẫn nạp dưới đây.

---

## PHẦN 2: DÀNH CHO KỸ THUẬT VIÊN (CÁCH NẠP FIRMWARE)

Để nạp các file `.bin` vào mạch ESP32-S3, kỹ thuật viên có thể sử dụng 1 trong 2 cách sau đây.

### CÁCH 1: Nạp qua giao diện Web (Cực kỳ dễ, không cần cài đặt phần mềm)
Đây là cách đơn giản nhất, chỉ yêu cầu máy tính có kết nối Internet và sử dụng trình duyệt **Google Chrome** hoặc **Microsoft Edge**.

1. Cắm cáp USB kết nối máy tính với mạch ESP32-S3.
2. Mở trình duyệt Chrome/Edge và truy cập vào trang: **[Adafruit ESPTool Web](https://adafruit.github.io/Adafruit_WebSerial_ESPTool/)** hoặc **[ESP Web Tools](https://espressif.github.io/esptool-js/)**.
3. Chọn nút **Connect** (hoặc nhấn chọn Baudrate là `115200` rồi Connect). Trình duyệt sẽ hiện cửa sổ yêu cầu quyền truy cập cổng Serial ➔ Chọn cổng COM của mạch ESP32-S3 ➔ Nhấn **Kết nối (Connect)**.
4. Sau khi kết nối thành công, tải lên 4 file `.bin` tương ứng vào đúng các ô địa chỉ (Offsets) như sau:
   - File `bootloader.bin` ➔ Offset: `0x0`
   - File `partitions.bin` ➔ Offset: `0x8000`
   - File `boot_app0.bin` ➔ Offset: `0xe000` *(Nếu có)*
   - File `firmware.bin` ➔ Offset: `0x10000`
5. Nhấn nút **Program** (Nạp) và chờ thanh tiến trình chạy đến 100%.
6. Khởi động lại mạch (nhấn nút Reset hoặc rút cáp cắm lại) để firmware bắt đầu chạy.

---

### CÁCH 2: Dùng phần mềm Flash Download Tools của Espressif (Windows GUI)
Cách này ổn định và chuyên nghiệp, do chính nhà sản xuất chip Espressif cung cấp.

**Bước 1: Tải công cụ**
- Lên Google gõ hoặc truy cập link tải: **[Flash Download Tools](https://www.espressif.com/en/support/download/other-tools)**.
- Tải về và giải nén, chạy file `flash_download_tool_x.x.x.exe`.

**Bước 2: Mở cấu hình phần mềm**
- Cửa sổ nhỏ hiện ra, cấu hình như sau:
  - **Chip Type:** Chọn `ESP32-S3`
  - **WorkMode:** Chọn `Develop`
  - **LoadMode:** Chọn `UART`
- Nhấn **OK** để mở giao diện chính.

**Bước 3: Chọn File và Địa chỉ (Quan trọng nhất)**
Trên giao diện chính, tích vào 4 ô trống đầu tiên (cột bên trái) và lần lượt chọn đường dẫn tới 4 file `.bin` bạn đã giải nén, đồng thời nhập chính xác địa chỉ (`@`) vào cột bên phải:
1. `[v]` Chọn file `bootloader.bin` ➔ Cột `@`: gõ `0x0`
2. `[v]` Chọn file `partitions.bin` ➔ Cột `@`: gõ `0x8000`
3. `[v]` Chọn file `boot_app0.bin` ➔ Cột `@`: gõ `0xe000` *(Nếu không có file này thì bỏ qua không tick)*
4. `[v]` Chọn file `firmware.bin` ➔ Cột `@`: gõ `0x10000`

**Bước 4: Cấu hình thông số nạp**
Ở phía dưới cửa sổ, cấu hình:
- **SPI SPEED:** `80MHz`
- **SPI MODE:** `QIO` (Rất quan trọng với bo mạch dùng PSRAM/OPI như Waveshare)
- **COM:** Chọn cổng COM của mạch (Ví dụ: `COM5`).
- **BAUD:** `115200` (Hoặc có thể chọn `921600` để nạp nhanh hơn).

**Bước 5: Bắt đầu nạp**
- Cắm mạch ESP32-S3 vào máy tính.
- Nhấn nút **START** ở góc dưới bên trái.
- Nếu mạch không tự động vào chế độ nạp (chữ `SYNC` hiện mãi), bạn có thể cần nhấn giữ nút **BOOT** trên mạch ESP32-S3.
- Chờ thanh màu xanh lá chạy xong và hiện chữ **FINISH**. 
- Rút mạch ra, cắm lại hoặc nhấn nút Reset trên mạch để khởi động firmware mới!
