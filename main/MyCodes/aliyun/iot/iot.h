#ifndef IOT_IOT_H
#define IOT_IOT_H

#include "aliyun_config.h"


extern uint16_t buflen; //临时缓存数量
extern unsigned char buf[200]; //临时缓存数组

uint8_t IOT_connect(void); //IOT物联网平台连接
void IOT_ping(void); //IOT物联网平台PING（心跳包）
uint8_t IOT_subscribe(void);//subscribe主题订阅（订阅成功后才能接收订阅消息）
uint8_t IOT_publish(char* payload);//publish主题发布（参数是发布信息内容，用双引号包含）

#endif

/*********************************************************************************************
 * 洋桃电子 www.DoYoung.net
 * 本程序应用于洋桃IoT开发板，可到洋桃电子官网观看教学视频、购买、下载资料
*********************************************************************************************/
