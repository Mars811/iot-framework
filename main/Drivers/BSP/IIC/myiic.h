/**
 ****************************************************************************************************
 * @file        myiic.h
 * @author      Mr.Mr.
 * @version     V1.0
 * @date        2023-04-23
 * @brief       IIC��������
 * @license     Copyright (c) 2020-2032,  
 ****************************************************************************************************
 * @attention
 * 
 * ʵ��ƽ̨:ST-1 STM32F407���İ�
 *  
 *  
 * ��˾��ַ:www.genbotter.com
 * �����ַ:makerbase.taobao.com
 * 
 ****************************************************************************************************
 */

#ifndef __MYIIC_H
#define __MYIIC_H

#include "./SYSTEM/sys/sys.h"

/* ���Ŷ��� */
#define IIC_SCL_GPIO_PORT           GPIOB
#define IIC_SCL_GPIO_PIN            GPIO_PIN_8
#define IIC_SCL_GPIO_CLK_ENABLE()   do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)
#define IIC_SDA_GPIO_PORT           GPIOB
#define IIC_SDA_GPIO_PIN            GPIO_PIN_9
#define IIC_SDA_GPIO_CLK_ENABLE()   do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)

/* IO���� */
#define IIC_SCL(x)                  do { (x) ?                                                                  \
                                        HAL_GPIO_WritePin(IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN, GPIO_PIN_SET):   \
                                        HAL_GPIO_WritePin(IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN, GPIO_PIN_RESET); \
                                    } while (0)
#define IIC_SDA(x)                  do { (x) ?                                                                  \
                                        HAL_GPIO_WritePin(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN, GPIO_PIN_SET):   \
                                        HAL_GPIO_WritePin(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN, GPIO_PIN_RESET); \
                                    } while (0)
#define IIC_SDA_READ                ((HAL_GPIO_ReadPin(IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)

/* �������� */
void iic_init(void);                /* ��ʼ��IIC */
void iic_start(void);               /* ����IIC��ʼ�ź� */
void iic_stop(void);                /* ����IICֹͣ�ź� */
uint8_t iic_wait_ack(void);         /* �ȴ�IICӦ���ź� */
void iic_ack(void);                 /* ����IIC ACK�ź� */
void iic_nack(void);                /* ����IIC NACK�ź� */
void iic_send_byte(uint8_t data);   /* IIC����һ���ֽ� */
uint8_t iic_read_byte(uint8_t ack); /* IIC��ȡһ���ֽ� */

#endif
