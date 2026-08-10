#include "tcp_push.h"

// EthernetServer la abstract class → phai override begin()
class FixedEthernetServer : public EthernetServer {
public:
    FixedEthernetServer(uint16_t port) : EthernetServer(port) {}
    void begin(uint16_t port = 0) {
        (void)port;
        EthernetServer::begin();
    }
};

static FixedEthernetServer pushServer(TCP_PUSH_PORT);
static EthernetClient pushClients[TCP_PUSH_MAX_CLIENTS];

void TcpPush_Init() {
    pushServer.begin();
    Serial.printf("[TCP PUSH] Server lang nghe tren Port %d (toi da %d client)\n",
                  TCP_PUSH_PORT, TCP_PUSH_MAX_CLIENTS);
}

void TcpPush_Loop() {
    EthernetClient newClient = pushServer.accept();
    if (newClient) {
        bool accepted = false;
        for (int i = 0; i < TCP_PUSH_MAX_CLIENTS; i++) {
            if (!pushClients[i].connected()) {
                pushClients[i].stop();
                pushClients[i] = newClient;
                Serial.printf("[TCP PUSH] Client moi ket noi (slot %d)\n", i);
                // Gửi bản tin chào
                String welcome = "{\"event\":\"connected\",\"ip\":\""
                               + Ethernet.localIP().toString()
                               + "\",\"port\":" + String(TCP_PUSH_PORT)
                               + ",\"version\":\"1.0\"}\r\n";
                pushClients[i].print(welcome);
                accepted = true;
                break;
            }
        }
        if (!accepted) {
            // Đã đầy slot - từ chối
            newClient.println("{\"event\":\"rejected\",\"reason\":\"max_clients\"}");
            newClient.stop();
            Serial.println("[TCP PUSH] Tu choi client moi: da du slot!");
        }
    }
}

void TcpPush_Broadcast(const String& json) {
    for (int i = 0; i < TCP_PUSH_MAX_CLIENTS; i++) {
        if (pushClients[i].connected()) {
            pushClients[i].println(json);
        } else {
            pushClients[i].stop();
        }
    }
}

uint8_t TcpPush_ClientCount() {
    uint8_t cnt = 0;
    for (int i = 0; i < TCP_PUSH_MAX_CLIENTS; i++) {
        if (pushClients[i].connected()) cnt++;
    }
    return cnt;
}
