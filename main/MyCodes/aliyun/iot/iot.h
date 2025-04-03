#ifndef IOT_IOT_H
#define IOT_IOT_H

#include "aliyun_config.h"


extern uint16_t buflen; //��ʱ��������
extern unsigned char buf[200]; //��ʱ��������

uint8_t IOT_connect(void); //IOT������ƽ̨����
void IOT_ping(void); //IOT������ƽ̨PING����������
uint8_t IOT_subscribe(void);//subscribe���ⶩ�ģ����ĳɹ�����ܽ��ն�����Ϣ��
uint8_t IOT_publish(char* payload);//publish���ⷢ���������Ƿ�����Ϣ���ݣ���˫���Ű�����

#endif

/*********************************************************************************************
 * ���ҵ��� www.DoYoung.net
 * ������Ӧ��������IoT�����壬�ɵ����ҵ��ӹ����ۿ���ѧ��Ƶ��������������
*********************************************************************************************/
