#if !defined(CONFIG_H)
#define CONFIG_H

#include <BGWiFiConfig.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <WiFiUdp.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 14, 2, U8X8_PIN_NONE);

#define LED_PIN 4
#define BTN0_PIN 0
#define U8LOG_WIDTH 20
#define U8LOG_HEIGHT 6
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];
U8G2LOG u8g2log;

BGWiFiConfig wifipw;

void showStartLog(String msg)
{
    u8g2log.print(msg);
    u8g2log.print("\n");
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawLog(0, 8, u8g2log);
    u8g2.sendBuffer();
}

void initDisplay()
{
    Serial.println("Initializing display...");
    u8g2.begin();
    u8g2log.begin(U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
    u8g2.enableUTF8Print(); // enable UTF8
    Serial.println("Display initialized");
}

void initPin()
{
    showStartLog("Initializing pin...");
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BTN0_PIN, INPUT_PULLUP);
    showStartLog("Pin initialized");
}

void initWiFi()
{
    showStartLog("Initializing WiFi...");
    // wifipw.outWiFiSET(true);
    // wifipw.autoStart(true);
    wifipw.begin();
    showStartLog("WiFi initialized");
}

void showMsg(String msg)
{
    u8g2.setFont(u8g2_font_wqy16_t_gb2312); //设置中文字符集
    u8g2.clearBuffer();
    u8g2.setCursor(0, 32);
    u8g2.print(msg);
    u8g2.sendBuffer();
}

// 清除配网
void clearWiFi()
{
    showMsg("清除配网...");
    wifipw.clearWiFi();
}

#endif // CONFIG_H