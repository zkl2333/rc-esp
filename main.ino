#include "config.h"
#include <string.h>

WiFiClient client;
WiFiUDP Udp;
const char *host = "192.168.8.247";
const unsigned int tcpPort = 1024;
const unsigned int udpLocalPort = 8266;

//读取键值
#define keydown digitalRead(BTN0_PIN)

uint8_t func_index = 0;
uint8_t menu_index = 0;

void showMenu()
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_wqy14_t_gb2312a);
    u8g2.setCursor(20, 16 * 1 - 2);
    u8g2.print(" 1.协议选择");
    u8g2.setCursor(20, 16 * 2 - 2);
    u8g2.print(" 2.手柄配对");
    u8g2.setCursor(20, 16 * 3 - 2);
    u8g2.print(" 3.网络信息");
    u8g2.setCursor(20, 16 * 4 - 2);
    u8g2.print(" 4.取消配网");
    //显示光标
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
    u8g2.drawGlyph(5, 16 * ((menu_index % 4) + 1), 118);
    u8g2.sendBuffer();
}

void waitTCP()
{
    while (!client.connected())
    {
        if (client.connect(host, tcpPort))
        {
            Serial.println("connected");
            break;
        }
        else
        {
            Serial.println("connection failed");
            delay(1000);
        }
    }
}

void waitWiFi()
{
    if (!wifipw.OK())
    {
        showMsg("等待配网");
        return;
    }
    if (!wifipw.OK(true))
    {
        showMsg("连接失败");
        return;
    }
}

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Starting...");
    initDisplay();
    initPin();
    initWiFi();
    // 等待网络连接
    waitWiFi();
    // 等待TCP连接
    waitTCP();
    // 等待UDP连接
    Udp.begin(udpLocalPort);
    // 看门狗初始化
    ESP.wdtEnable(5000);
}

// 命令解析
void commandParsing(String command)
{
    // 命令格式： [key]:[value],[key]:[value],...
    // 如果命令中包含了逗号,那么就是多个命令
    if (command.indexOf(',') != -1)
    {
        // 多个命令
        char delims[] = ",";
        char *result = NULL;
        result = strtok((char *)command.c_str(), delims);
        while (result != NULL)
        {
            commandParsing(result);
            result = strtok(NULL, delims);
        }
    }
    else
    {
        // 单个命令
        String key = command.substring(0, command.indexOf(':'));
        String value = command.substring(command.indexOf(':') + 1);
        if (key == "12" && value == "1000")
        {
            menu_index--;
        }
        if (key == "13" && value == "1000")
        {
            menu_index++;
        }
        if (key == "6")
        {
            analogWrite(LED_PIN, 1000 - value.toInt());
        }
    }

    // if (command == "down")
    // {
    //   menu_index++;
    // }
    // else if (command == "up")
    // {
    //   menu_index--;
    // }
}

String readUdp()
{
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
        String line = Udp.readStringUntil(',');
        return line;
    }
    return "";
}

String readTCP()
{
    if (client.available())
    {
        String line = client.readStringUntil(',');
        return line;
    }
    return "";
}

void loop(void)
{
    // 网络连接
    wifipw.Loop();

    // 读取UDP数据
    // String line = readUdp();
    String line = readTCP();

    if (line != "")
    {
        // Serial.printf("Received from server: %s\n", line.c_str());
        commandParsing(line);
    }

    //切换功能
    switch (func_index)
    {
    case 0:
        showMenu();
        break;
    default:
        // showTempAndHumidity();
        break;
    }

    if (keydown == LOW)
    {
        delay(3); //消抖
        if (keydown == LOW)
        {
            u32 last_key_time = millis();
            while (keydown == LOW)
            {
                delay(5);
                // 喂 狗
                ESP.wdtFeed();
            }
            // 长按
            if (millis() - last_key_time > 500)
            {
                Serial.println("长按");

                if (func_index == 0)
                {
                    func_index = menu_index;
                }
                else
                {
                    func_index = 0;
                }
            }
            else
            // 短按
            {
                Serial.println("短按");
                if (func_index == 0)
                {
                    if (menu_index > 4)
                    {
                        menu_index = 0;
                    }
                    menu_index++;
                }
            }
        }
    }
    ESP.wdtFeed(); // 喂 狗
}
