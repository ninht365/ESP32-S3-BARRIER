#include "ethernet_driver.h"
#include "config_manager.h"

static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

void Ethernet_Init() {
    Serial.println("[ETH] Reset phan cung W5500 (GPIO 39)...");
    pinMode(ETH_RST_PIN, OUTPUT);
    digitalWrite(ETH_RST_PIN, LOW);
    delay(100);
    digitalWrite(ETH_RST_PIN, HIGH);
    delay(300);

    Serial.println("[ETH] Khoi tao SPI (SCK:15 MISO:14 MOSI:13 CS:16)...");
    SPI.begin(ETH_SPI_SCK, ETH_SPI_MISO, ETH_SPI_MOSI, ETH_SPI_CS);
    Ethernet.init(ETH_SPI_CS);

    // Đọc IP từ NVS (Config_Manager đã đọc trong Config_Init trước đó)
    IPAddress ip  = Config_GetIP();
    IPAddress gw  = Config_GetGateway();
    IPAddress sn  = Config_GetSubnet();

    Ethernet.begin(mac, ip, gw, gw, sn);
    delay(500);

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("[ETH] LOI: CHIP W5500 KHONG PHAT HIEN! Kiem tra nguon hoac CS:16");
    } else if (Ethernet.hardwareStatus() == EthernetW5500) {
        Serial.println("[ETH] CHIP W5500 PHAT HIEN OK!");
    }

    // Chờ tối đa 2.5s để chip W5500 hoàn tất thương lượng tín hiệu lớp vật lý (Auto-negotiation)
    unsigned long startCheck = millis();
    bool linkOk = false;
    while (millis() - startCheck < 2500) {
        if (Ethernet.linkStatus() == LinkON) {
            linkOk = true;
            break;
        }
        delay(100);
    }

    if (linkOk) {
        Serial.println("[ETH] Cap mang da ket noi (Link ON)!");
    } else {
        Serial.println("[ETH] CANH BAO: Chua cam cap mang (Link OFF)!");
    }

    Serial.print("[ETH] IP Tinh: ");
    Serial.println(Ethernet.localIP());
}
