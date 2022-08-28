#include "config.h"
#include <math.h>

//读取键值
#define keydown digitalRead(BTN0_PIN)

// 左电机
#define PWM1 15
#define AIN1 4
#define AIN2 5
// 右电机
#define PWM2 12
#define BIN1 16
#define BIN2 14

#define LeftMotor 1
#define RightMotor 0

// 正反转
#define Backward 1
#define Forward 0
#define Stop 2

// 电机，正反转，速度
void Motor(int motor, int dir, int speed)
{
    int IN1 = motor == LeftMotor ? AIN1 : BIN1;
    int IN2 = motor == LeftMotor ? AIN2 : BIN2;
    int PWM = motor == LeftMotor ? PWM1 : PWM2;
    if (dir == Forward)
    {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
    }
    else if (dir == Backward)
    {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
    }
    else
    {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
    }
    analogWrite(PWM, speed);
}

// 发送UDP消息
void sendUdp(String msg)
{
    Udp.beginPacket(udpRemoteIP.c_str(), udpRemotePort);
    Udp.write(msg.c_str());
    Udp.endPacket();
}

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Starting...");
    initDisplay();
    // initPin();
    initWiFi();
    // 等待TCP连接
    // waitTCP();
    // 等待UDP连接
    Udp.begin(udpLocalPort);
    // 看门狗初始化
    ESP.wdtEnable(5000);
    pinMode(PWM1, OUTPUT);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWM2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    analogWrite(PWM1, 0); // Speed control of Motor A
    analogWrite(PWM2, 0); // Speed control of Motor B
}

int a1 = 0;
int a0 = 0;

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
        if (key == "6")
        {
            // 左电机 前进
            Motor(LeftMotor, Forward, value.toInt());
        }
        if (key == "7")
        {
            // 右电机 前进
            Motor(RightMotor, Forward, value.toInt());
        }
        if (key == "a1" || key == "a0")
        {
            if (key == "a1")
                a1 = value.toInt();
            if (key == "a0")
                a0 = value.toInt();
            int LeftSpeed = (a1 + a0) / 2;
            int RightSpeed = (a1 - a0) / 2;
            // 停止
            if (LeftSpeed == 0 && RightSpeed == 0)
            {
                Motor(LeftMotor, Stop, 0);
                Motor(RightMotor, Stop, 0);
            }
            else
            {
                Motor(LeftMotor, LeftSpeed > 0 ? Forward : Backward, abs(LeftSpeed));
                Motor(RightMotor, RightSpeed > 0 ? Forward : Backward, abs(RightSpeed));
            }
        }
    }
}

bool isConnected = false;
int lastHeartbeat = 0;

void loop(void)
{
    // 每10s发送一次心跳包
    if (lastHeartbeat == 0 || millis() - lastHeartbeat > 10000)
    {
        lastHeartbeat = millis();
        sendUdp("heartbeat");
    }

    // 读取UDP数据
    String line = readUdp();
    // String line = readTCP();

    if (line != "")
    {
        // 读取到数据
        if (line == "heartbeat")
        {
            // 收到心跳
            udpRemoteIP = Udp.remoteIP().toString();
            isConnected = true;
        }
        else
        {
            // 收到命令
            commandParsing(line);
        }
    }

    ESP.wdtFeed(); // 喂 狗
}
