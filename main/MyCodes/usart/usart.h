#ifndef INC_USART_H_
#define INC_USART_H_


#include "../aliyun/aliyun_config.h"
#include "stm32f4xx_hal.h" //HAL���ļ�����
#include <string.h>//�����ַ�������Ŀ�
// #include "../inc/retarget.h"//����printf����������ӳ��
#include "../tim/tim.h"
#include "../usart/usart.h"
// #include "main.h"
#include <string.h>//�����ַ�������Ŀ�
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include "stdio.h"

// ��������
#define USART1_REC_LEN          200
#define USART2_REC_LEN          200
#define USART3_REC_LEN          200

extern TIM_HandleTypeDef htim2;

extern UART_HandleTypeDef huart1;//����USART1��HAL��ṹ��
extern UART_HandleTypeDef huart2;//����USART2��HAL��ṹ��
extern UART_HandleTypeDef huart3;//����USART2��HAL��ṹ��

extern uint8_t  USART1_RX_BUF[USART1_REC_LEN];//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�
extern uint16_t USART1_RX_STA;          //����״̬���
extern uint8_t  USART1_NewData;         //��ǰ�����жϽ��յ�1���ֽ����ݵĻ���

extern uint8_t  USART2_RX_BUF[USART2_REC_LEN];//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�
extern uint16_t USART2_RX_STA;          //����״̬���
extern uint8_t  USART2_NewData;         //��ǰ�����жϽ��յ�1���ֽ����ݵĻ���
extern uint8_t  RS485orBT;              //��RS485orBT��־λΪ1ʱ��RS485ģʽ��Ϊ0ʱ������ģʽ

extern uint8_t  USART3_RX_BUF[USART3_REC_LEN];//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�
extern uint16_t USART3_RX_STA;          //����״̬���
extern uint8_t  USART3_NewData;         //��ǰ�����жϽ��յ�1���ֽ����ݵĻ���

void HAL_UART_RxCpltCallback(UART_HandleTypeDef  *huart);//�����жϻص���������
void WIFI_printf (char *fmt, ...); //WIFIģ�鷢��
void WIFI_TCP_SEND (char *fmt, ...);//��TCPģʽ�µķ������ݣ���������״̬��ä����

#endif /* INC_USART_H_ */
