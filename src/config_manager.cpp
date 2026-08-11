#include "config_manager.h"

static Preferences prefs;
static IPAddress storedIP(192, 168, 1, 200);
static IPAddress storedGW(192, 168, 1, 1);
static IPAddress storedSN(255, 255, 255, 0);

void Config_Init() {
    prefs.begin("netcfg", false); // read-write

    String ip = prefs.getString("ip", "192.168.1.200");
    String gw = prefs.getString("gw", "192.168.1.1");
    String sn = prefs.getString("sn", "255.255.255.0");
    prefs.end();

    if (!storedIP.fromString(ip)) storedIP = IPAddress(192, 168, 1, 200);
    if (!storedGW.fromString(gw)) storedGW = IPAddress(192, 168, 1,   1);
    if (!storedSN.fromString(sn)) storedSN = IPAddress(255, 255, 255, 0);

    Serial.printf("[CONFIG] Doc NVS: IP=%s GW=%s SN=%s\n",
                  storedIP.toString().c_str(),
                  storedGW.toString().c_str(),
                  storedSN.toString().c_str());
}

IPAddress Config_GetIP()      { return storedIP; }
IPAddress Config_GetGateway() { return storedGW; }
IPAddress Config_GetSubnet()  { return storedSN; }

bool Config_SaveNetwork(const String& ip, const String& gateway, const String& subnet) {
    IPAddress testIP, testGW, testSN;
    if (!testIP.fromString(ip))      { Serial.println("[CONFIG] IP khong hop le: " + ip);      return false; }
    if (!testGW.fromString(gateway)) { Serial.println("[CONFIG] GW khong hop le: " + gateway); return false; }
    if (!testSN.fromString(subnet))  { Serial.println("[CONFIG] SN khong hop le: " + subnet);  return false; }

    prefs.begin("netcfg", false); // read-write
    prefs.putString("ip", ip);
    prefs.putString("gw", gateway);
    prefs.putString("sn", subnet);
    prefs.end();

    storedIP = testIP;
    storedGW = testGW;
    storedSN = testSN;
    Serial.printf("[CONFIG] Da luu: IP=%s GW=%s SN=%s\n", ip.c_str(), gateway.c_str(), subnet.c_str());
    return true;
}

String Config_GetJson() {
    return "{\"ip\":\"" + storedIP.toString()
         + "\",\"gateway\":\"" + storedGW.toString()
         + "\",\"subnet\":\"" + storedSN.toString() + "\"}";
}
