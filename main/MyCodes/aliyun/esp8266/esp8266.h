#ifndef ESP8266_ESP8266_H
#define ESP8266_ESP8266_H

#include "aliyun_config.h"

uint8_t esp8266_send_cmd(char *cmd, char *ack, uint16_t waittime);
uint8_t* esp8266_check_cmd(uint8_t *str);
uint8_t esp8266_Connect_IOTServer(void); //连接物联网云服务器IP
uint8_t esp8266_Connect_AP(void); //连接AP路由器
uint8_t esp8266_Connect_Server(void); //连接服务器
uint8_t esp8266_quit_trans(void); //判断指令退出

#endif
