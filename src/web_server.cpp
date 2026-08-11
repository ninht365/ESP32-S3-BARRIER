#include "web_server.h"
#include "relay_control.h"
#include "config_manager.h"
#include "tcp_push.h"
#include "web_index.h"
#include <Ethernet.h>

// Override begin() để tránh ambiguous call
class FixedEthernetServer : public EthernetServer {
public:
    FixedEthernetServer(uint16_t port) : EthernetServer(port) {}
    void begin(uint16_t port = 0) {
        (void)port;
        EthernetServer::begin();
    }
};

static FixedEthernetServer server(80);

void WebServer_Init() {
    server.begin();
    Serial.println("[WEB] HTTP Server Port 80 san sang.");
}

// Gửi HTML theo chunk nhỏ để tránh tràn buffer W5500
static void sendChunked(EthernetClient& client, const char* data, size_t len) {
    const size_t CHUNK = 512;
    size_t sent = 0;
    while (sent < len) {
        size_t toSend = min(CHUNK, len - sent);
        client.write((const uint8_t*)(data + sent), toSend);
        sent += toSend;
        delay(1);
    }
}

// Helper: gửi JSON response
static void sendJSON(EthernetClient& client, const String& json, int code = 200) {
    String status = (code == 200) ? "200 OK" : (code == 400 ? "400 Bad Request" : "500 Internal Server Error");
    client.println("HTTP/1.1 " + status);
    client.println("Content-Type: application/json");
    client.println("Access-Control-Allow-Origin: *");
    client.println("Connection: close");
    client.println("Content-Length: " + String(json.length()));
    client.println();
    client.print(json);
}

// Helper: đọc tham số URL, VD: getParam("ip=192.168.1.1", "ip") → "192.168.1.1"
static String getParam(const String& req, const String& key) {
    String search = key + "=";
    int idx = req.indexOf(search);
    if (idx == -1) return "";
    idx += search.length();
    int end = req.indexOf("&", idx);
    if (end == -1) end = req.indexOf(" ", idx);
    if (end == -1) end = req.length();
    String val = req.substring(idx, end);
    // URL decode: + → space, %XX → char
    val.replace("+", " ");
    // Xử lý %XX cơ bản
    String decoded = "";
    for (int i = 0; i < (int)val.length(); i++) {
        if (val[i] == '%' && i + 2 < (int)val.length()) {
            char hex[3] = {val[i+1], val[i+2], 0};
            decoded += (char)strtol(hex, nullptr, 16);
            i += 2;
        } else {
            decoded += val[i];
        }
    }
    return decoded;
}

void WebServer_Loop() {
    EthernetClient client = server.accept();
    if (!client) client = server.available();
    if (!client) return;

    String reqLine = "";
    bool gotReqLine = false;
    bool currentLineIsBlank = true;

    unsigned long timeout = millis() + 3000;
    unsigned long lastRelayTick = millis();

    while (client.connected() && millis() < timeout) {
        // Gọi Relay_Loop để nhả xung đúng hẹn ngay cả khi đang đợi HTTP headers
        if (millis() - lastRelayTick >= 10) {
            Relay_Loop();
            lastRelayTick = millis();
        }

        if (!client.available()) continue;
        char c = client.read();

        if (!gotReqLine) {
            if (c == '\n' || c == '\r') {
                if (reqLine.length() > 0) gotReqLine = true;
            } else {
                reqLine += c;
            }
        }

        if (c == '\n' && currentLineIsBlank) {
            // Không in log Serial cho request /api/status polling liên tục
            if (reqLine.indexOf("/api/status") == -1) {
                Serial.println("[HTTP] " + reqLine);
            }

            // ==================================================
            // ROUTE: GET /api/barrier?id=1|2&action=open|stop|close
            // ==================================================
            if (reqLine.indexOf("/api/barrier") != -1) {
                if (Ethernet.linkStatus() != LinkON) {
                    sendJSON(client, "{\"result\":\"error\",\"error\":\"eth_link_off\",\"message\":\"Day mang bi ngat\"}", 503);
                } else {
                    String action = getParam(reqLine, "action");
                    int duration = 400;
                    String durStr = getParam(reqLine, "duration");
                    if (durStr.length() > 0) duration = durStr.toInt();
                    if (duration < 100 || duration > 5000) duration = 400;

                    String idStr = getParam(reqLine, "id");
                    int barrier_id = (idStr.length() > 0) ? idStr.toInt() : 1;

                    BarrierAction act = (BarrierAction)0;
                    if      (action == "open")  act = ACTION_OPEN;
                    else if (action == "stop")  act = ACTION_STOP;
                    else if (action == "close") act = ACTION_CLOSE;

                    if (act == 0) {
                        sendJSON(client, "{\"result\":\"error\",\"error\":\"action phai la open/stop/close\"}", 400);
                    } else if (barrier_id < 1 || barrier_id > 2) {
                        sendJSON(client, "{\"result\":\"error\",\"error\":\"id phai la 1 hoac 2\"}", 400);
                    } else {
                        BarrierResult res = Relay_BarrierCmd(barrier_id, act, (uint16_t)duration);
                        String resStr = (res == BARRIER_CMD_OK)        ? "ok"
                                      : (res == BARRIER_CMD_PREEMPTED) ? "preempted"
                                      : (res == BARRIER_CMD_IGNORED)   ? "ignored"
                                      :                                  "busy";
                        String jsonResp = "{\"result\":\"" + resStr
                                        + "\",\"barrier\":" + String(barrier_id)
                                        + ",\"action\":\"" + action
                                        + "\",\"duration_ms\":" + String(duration)
                                        + ",\"current_state\":\"" + String(Relay_BarrierStateName(Relay_GetBarrierState(barrier_id))) + "\"}";
                        sendJSON(client, jsonResp);
                    }
                }

            // ==================================================
            // ROUTE: GET /api/status
            // ==================================================
            } else if (reqLine.indexOf("/api/status") != -1) {
                uint8_t st = Relay_GetState();
                bool ethLink = (Ethernet.linkStatus() == LinkON);
                unsigned long uptimeSec = millis() / 1000;
                String chJson = "{";
                for (int i = 1; i <= 8; i++) {
                    if (i > 1) chJson += ",";
                    chJson += "\"CH" + String(i) + "\":" + (((st >> (i-1)) & 1) ? "true" : "false");
                }
                chJson += "}";
                String jsonResp = "{\"status\":\"online\""
                    + String(",\"ip\":\"") + Ethernet.localIP().toString() + "\""
                    + ",\"gateway\":\"" + Config_GetGateway().toString() + "\""
                    + ",\"uptime_s\":"  + String(uptimeSec)
                    + ",\"eth_link\":"  + (ethLink ? "true" : "false")
                    + ",\"tcp_clients\":" + String(TcpPush_ClientCount())
                    + ",\"relays_byte\":"+ String(st)
                    + ",\"relays\":"    + chJson
                    + ",\"barrier_1_state\":\"" + String(Relay_BarrierStateName(Relay_GetBarrierState(1))) + "\""
                    + ",\"barrier_2_state\":\"" + String(Relay_BarrierStateName(Relay_GetBarrierState(2))) + "\""
                    + "}";
                sendJSON(client, jsonResp);

            // ==================================================
            // ROUTE: GET /api/config/get
            // ==================================================
            } else if (reqLine.indexOf("/api/config/get") != -1) {
                sendJSON(client, Config_GetJson());

            // ==================================================
            // ROUTE: GET /api/config/setip?ip=...&gw=...&sn=...
            // ==================================================
            } else if (reqLine.indexOf("/api/config/setip") != -1) {
                String ip = getParam(reqLine, "ip");
                String gw = getParam(reqLine, "gw");
                String sn = getParam(reqLine, "sn");
                if (sn.length() == 0) sn = "255.255.255.0"; // mặc định

                if (ip.length() == 0 || gw.length() == 0) {
                    sendJSON(client, "{\"status\":\"error\",\"error\":\"Thieu tham so ip hoac gw\"}", 400);
                } else if (!Config_SaveNetwork(ip, gw, sn)) {
                    sendJSON(client, "{\"status\":\"error\",\"error\":\"IP hoac GW khong hop le\"}", 400);
                } else {
                    sendJSON(client, "{\"status\":\"ok\",\"new_ip\":\"" + ip + "\",\"message\":\"Dang khoi dong lai...\"}");
                    client.flush();
                    delay(500);
                    client.stop();
                    delay(200);
                    Serial.println("[CONFIG] Reboot de ap dung IP moi: " + ip);
                    ESP.restart();
                }

            // ==================================================
            // ROUTE: GET /api/relay  (raw channel access, tương thích ngược)
            // ==================================================
            } else if (reqLine.indexOf("/api/relay") != -1) {
                int duration = 400;
                String durStr = getParam(reqLine, "duration");
                if (durStr.length() > 0) duration = durStr.toInt();

                bool isOn  = (reqLine.indexOf("action=on")  != -1);
                bool isOff = (reqLine.indexOf("action=off") != -1);
                bool isAll = (reqLine.indexOf("ch=all")     != -1);

                String jsonResp;
                if (isAll) {
                    for (int i = 1; i <= 8; i++) { if (isOn) Relay_On(i); else Relay_Off(i); }
                    jsonResp = "{\"status\":\"ok\",\"all_action\":\"" + String(isOn ? "on" : "off") + "\"}";
                } else {
                    String chStr = getParam(reqLine, "ch");
                    int ch = chStr.toInt();
                    if (ch < 1 || ch > 8) ch = 1;
                    if (isOn)       Relay_On(ch);
                    else if (isOff) Relay_Off(ch);
                    else            Relay_Pulse(ch, (uint16_t)duration);
                    const char* cmd = isOn ? "on" : (isOff ? "off" : "pulse");
                    jsonResp = "{\"status\":\"success\",\"channel\":" + String(ch)
                             + ",\"duration_ms\":" + String(duration)
                             + ",\"command\":\"" + cmd + "\"}";
                }
                sendJSON(client, jsonResp);

            // ==================================================
            // ROUTE: GET /api/i2cscan
            // ==================================================
            } else if (reqLine.indexOf("/api/i2cscan") != -1) {
                sendJSON(client, Relay_I2CScan());

            // ==================================================
            // ROUTE: GET /api/rawwrite?val=N
            // ==================================================
            } else if (reqLine.indexOf("/api/rawwrite") != -1) {
                String valStr = getParam(reqLine, "val");
                uint8_t rawVal = (valStr.length() > 0) ? (uint8_t)valStr.toInt() : 0xFF;
                Relay_WriteRaw(rawVal);
                sendJSON(client, "{\"status\":\"ok\",\"raw_byte\":" + String(rawVal) + "}");

            // ==================================================
            // ROUTE: GET /  (Web UI HTML)
            // ==================================================
            } else {
                size_t htmlLen = strlen(INDEX_HTML);
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/html; charset=utf-8");
                client.println("Access-Control-Allow-Origin: *");
                client.println("Connection: close");
                client.println("Content-Length: " + String(htmlLen));
                client.println();
                sendChunked(client, INDEX_HTML, htmlLen);
                Serial.println("[WEB] Gui HTML " + String(htmlLen) + " bytes.");
            }
            break;
        }

        if (c == '\n') {
            currentLineIsBlank = true;
        } else if (c != '\r') {
            currentLineIsBlank = false;
        }
    }

    delay(10);
    client.stop();
}
