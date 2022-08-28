#if !defined(CONFIG_H)
#define CONFIG_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include <strings_en.h>
#include <WiFiManager.h>
#include <string.h>

// 建立WiFiManager对象
WiFiManager wifiManager;
// 建立WiFiClient对象 用于TCP连接
WiFiClient client;
// 建立UDP对象
WiFiUDP Udp;

const unsigned int tcpPort = 1024;
const unsigned int udpLocalPort = 8266;

// void waitTCP()
// {
//     const char *host = "192.168.8.247";
//     while (!client.connected())
//     {
//         if (client.connect(host, tcpPort))
//         {
//             Serial.println("connected");
//             break;
//         }
//         else
//         {
//             Serial.println("connection failed");
//             delay(1000);
//         }
//     }
// }

String readTCP()
{
    if (client.available())
    {
        String line = client.readStringUntil(';');
        return line;
    }
    return "";
}

String readUdp()
{
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
        String line = Udp.readStringUntil(';');
        return line;
    }
    return "";
}

void showStartLog(String msg)
{
    // u8g2log.print(msg);
    // u8g2log.print("\n");
    // u8g2.setFont(u8g2_font_5x7_tr);
    // u8g2.drawLog(0, 8, u8g2log);
    // u8g2.sendBuffer();
    Serial.println(msg);
}

void initDisplay()
{
    // Serial.println("Initializing display...");
    // u8g2.begin();
    // u8g2log.begin(U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
    // u8g2.enableUTF8Print(); // enable UTF8
    // Serial.println("Display initialized");
}

// void initPin()
// {
//     showStartLog("Initializing pin...");
//     pinMode(LED_BUILTIN, OUTPUT);
//     pinMode(LED_PIN, OUTPUT);
//     pinMode(BTN0_PIN, INPUT_PULLUP);
//     showStartLog("Pin initialized");
// }

void initWiFi()
{
    showStartLog("Initializing WiFi...");
    // 自动连接WiFi。以下语句的参数是连接ESP8266时的WiFi名称
    wifiManager.autoConnect("AutoConnectAP");

    // 如果您希望该WiFi添加密码，可以使用以下语句：
    // wifiManager.autoConnect("AutoConnectAP", "12345678");

    showStartLog("Connected to ");
    showStartLog(WiFi.SSID());
    showStartLog("IP address: ");
    showStartLog(WiFi.localIP().toString());
    showStartLog("WiFi initialized");
}

// void showMsg(String msg)
// {
//     u8g2.setFont(u8g2_font_wqy16_t_gb2312); //设置中文字符集
//     u8g2.clearBuffer();
//     u8g2.setCursor(0, 32);
//     u8g2.print(msg);
//     u8g2.sendBuffer();
// }

// 清除配网
void clearWiFi()
{
    // showMsg("清除配网...");
    // wifipw.clearWiFi();
}

#endif // CONFIG_H