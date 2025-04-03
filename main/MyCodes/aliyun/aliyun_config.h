#ifndef _ALIYUN_CONFIG_H_
#define _ALIYUN_CONFIG_H_

// 移植相关的头文件
#include "stm32f4xx_hal.h"
#include "../usart/usart.h"
#include "../tim/tim.h"

// 逻辑相关的头文件
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "esp8266/esp8266.h"
#include "mqtt/MQTTPacket.h"
#include "mqtt/transport.h"
#include "iot/iot.h"
/**
 * ----------------------------------------------------------------------
 * @brief  宏配置
 * ----------------------------------------------------------------------
 * 外部串口、物联网相关TIM定时的API在此处调整。
 * ----------------------------------------------------------------------
**/
#define USART1_RX_BUF       USART1_RX_BUF
#define USART1_RX_STA       USART1_RX_STA
#define USART1_NewData      USART1_NewData

#define USART3_RX_BUF       USART3_RX_BUF
#define USART3_RX_STA       USART3_RX_STA
#define USART3_NewData      USART3_NewData

#define WIFI_printf         WIFI_printf
#define WIFI_TCP_SEND       WIFI_TCP_SEND

#define htim2               htim2

#define stm32f1xx_hal       stm32f4xx_hal

/**
 * ----------------------------------------------------------------------
 * @attention 软件配置
 * ----------------------------------------------------------------------
**/
// 网络连接配置
#define SSID "Mars811" 
#define PASS "2263226401eee"   

#define IOT_DOMAIN_NAME "iot-06z00iiu01mgty5.mqtt.iothub.aliyuncs.com" 
#define IOT_PORTNUM 	"1883"            

// MQTT配置
#define  PRODUCTKEY           "k1y9j9tETVD" //产品ID（ProductKey）【必须按您的实际情况修改】
#define  PRODUCTKEY_LEN       strlen(PRODUCTKEY) //产品ID长度
#define  DEVICENAME			 "esp8266" //设备名（DeviceName）【必须按您的实际情况修改】
#define  DEVICENAME_LEN       strlen(DEVICENAME) //设备名长度
#define  DEVICESECRE          "4958b3477168ec37d467364e9585b3cf" //设备秘钥（DeviceSecret）【必须按您的实际情况修改】
#define  DEVICESECRE_LEN      strlen(DEVICESECRE) //设备秘钥长度

//MQTT参数
#define MQTT_CLIENTID			"k1y9j9tETVD.esp8266|securemode=2,signmethod=hmacsha256,timestamp=1730873955822|"
#define MQTT_USERNAME			"esp8266&k1y9j9tETVD"
#define MQTT_PASSWORD			"e84791b019cc33073faf9c9f6aa2f2284461b36a26b19517b4b7f2ac9afe24b9"

#define  TOPIC_PUBLISH			"/k1y9j9tETVD/esp8266/user/iot_msg_pub"
#define  TOPIC_SUBSCRIBE		"/k1y9j9tETVD/esp8266/user/iot_msg_get"
#define  TOPIC_QOS				0  //QoS服务质量数值（0/1）
#define  MSGID					1  //信息识别ID

#define  MQTTVERSION			4 //MQTT协议版本号（3表示V3.1，4表示V3.1.1）
#define  KEEPALIVEINTERVAL		120 //保活计时器，服务器收到客户端消息（含心跳包）的最大间隔（单位是秒）

#endif
