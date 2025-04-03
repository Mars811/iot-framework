#ifndef ESP8266_ESP8266_H
#define ESP8266_ESP8266_H

#include "aliyun_config.h"

uint8_t esp8266_send_cmd(char *cmd, char *ack, uint16_t waittime);
uint8_t* esp8266_check_cmd(uint8_t *str);
uint8_t esp8266_Connect_IOTServer(void); //�����������Ʒ�����IP
uint8_t esp8266_Connect_AP(void); //����AP·����
uint8_t esp8266_Connect_Server(void); //���ӷ�����
uint8_t esp8266_quit_trans(void); //�ж�ָ���˳�

#endif
