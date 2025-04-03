#ifndef INC_USART_H_
#define INC_USART_H_


#include "../aliyun/aliyun_config.h"
#include "stm32f4xx_hal.h" //HAL库文件声明
#include <string.h>//用于字符串处理的库
// #include "../inc/retarget.h"//用于printf函数串口重映射
#include "../tim/tim.h"
#include "../usart/usart.h"
// #include "main.h"
#include <string.h>//用于字符串处理的库
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include "stdio.h"

// 串口配置
#define USART1_REC_LEN          200
#define USART2_REC_LEN          200
#define USART3_REC_LEN          200

extern TIM_HandleTypeDef htim2;

extern UART_HandleTypeDef huart1;//声明USART1的HAL库结构体
extern UART_HandleTypeDef huart2;//声明USART2的HAL库结构体
extern UART_HandleTypeDef huart3;//声明USART2的HAL库结构体

extern uint8_t  USART1_RX_BUF[USART1_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern uint16_t USART1_RX_STA;          //接收状态标记
extern uint8_t  USART1_NewData;         //当前串口中断接收的1个字节数据的缓存

extern uint8_t  USART2_RX_BUF[USART2_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern uint16_t USART2_RX_STA;          //接收状态标记
extern uint8_t  USART2_NewData;         //当前串口中断接收的1个字节数据的缓存
extern uint8_t  RS485orBT;              //当RS485orBT标志位为1时是RS485模式，为0时是蓝牙模式

extern uint8_t  USART3_RX_BUF[USART3_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern uint16_t USART3_RX_STA;          //接收状态标记
extern uint8_t  USART3_NewData;         //当前串口中断接收的1个字节数据的缓存

void HAL_UART_RxCpltCallback(UART_HandleTypeDef  *huart);//串口中断回调函数声明
void WIFI_printf (char *fmt, ...); //WIFI模块发送
void WIFI_TCP_SEND (char *fmt, ...);//在TCP模式下的发送数据（不处理返回状态的盲发）

#endif /* INC_USART_H_ */
