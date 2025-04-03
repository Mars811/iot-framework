#ifndef _ALIYUN_CONFIG_H_
#define _ALIYUN_CONFIG_H_

// ��ֲ��ص�ͷ�ļ�
#include "stm32f4xx_hal.h"
#include "../usart/usart.h"
#include "../tim/tim.h"

// �߼���ص�ͷ�ļ�
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "esp8266/esp8266.h"
#include "mqtt/MQTTPacket.h"
#include "mqtt/transport.h"
#include "iot/iot.h"
/**
 * ----------------------------------------------------------------------
 * @brief  ������
 * ----------------------------------------------------------------------
 * �ⲿ���ڡ����������TIM��ʱ��API�ڴ˴�������
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
 * @attention �������
 * ----------------------------------------------------------------------
**/
// ������������
#define SSID "Mars811" 
#define PASS "2263226401eee"   

#define IOT_DOMAIN_NAME "iot-06z00iiu01mgty5.mqtt.iothub.aliyuncs.com" 
#define IOT_PORTNUM 	"1883"            

// MQTT����
#define  PRODUCTKEY           "k1y9j9tETVD" //��ƷID��ProductKey�������밴����ʵ������޸ġ�
#define  PRODUCTKEY_LEN       strlen(PRODUCTKEY) //��ƷID����
#define  DEVICENAME			 "esp8266" //�豸����DeviceName�������밴����ʵ������޸ġ�
#define  DEVICENAME_LEN       strlen(DEVICENAME) //�豸������
#define  DEVICESECRE          "4958b3477168ec37d467364e9585b3cf" //�豸��Կ��DeviceSecret�������밴����ʵ������޸ġ�
#define  DEVICESECRE_LEN      strlen(DEVICESECRE) //�豸��Կ����

//MQTT����
#define MQTT_CLIENTID			"k1y9j9tETVD.esp8266|securemode=2,signmethod=hmacsha256,timestamp=1730873955822|"
#define MQTT_USERNAME			"esp8266&k1y9j9tETVD"
#define MQTT_PASSWORD			"e84791b019cc33073faf9c9f6aa2f2284461b36a26b19517b4b7f2ac9afe24b9"

#define  TOPIC_PUBLISH			"/k1y9j9tETVD/esp8266/user/iot_msg_pub"
#define  TOPIC_SUBSCRIBE		"/k1y9j9tETVD/esp8266/user/iot_msg_get"
#define  TOPIC_QOS				0  //QoS����������ֵ��0/1��
#define  MSGID					1  //��Ϣʶ��ID

#define  MQTTVERSION			4 //MQTTЭ��汾�ţ�3��ʾV3.1��4��ʾV3.1.1��
#define  KEEPALIVEINTERVAL		120 //�����ʱ�����������յ��ͻ�����Ϣ�����������������������λ���룩

#endif
