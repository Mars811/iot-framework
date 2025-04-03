/**
 ****************************************************************************************************
 * @file        led.h
 * @author      genbotter
 * @version     V1.0
 * @date        2023-04-23
 * @brief       LED��������
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention
 * 
 * ʵ��ƽ̨:ST-1 STM32F407���İ�
 * 
 * 
 * 
 * �������ӣ�https://item.taobao.com/item.htm?spm=a21n57.1.0.0.4d53523chbY4Wg&id=730217214783&ns=1&abbucket=12#detail
 * 
 ****************************************************************************************************
 */

#ifndef __LED_H
#define __LED_H

#include "./SYSTEM/sys/sys.h"

/* ���Ŷ��� */
#define LED2_GPIO_PORT          GPIOA
#define LED2_GPIO_PIN           GPIO_PIN_6
#define LED2_GPIO_CLK_ENABLE()  do { __HAL_RCC_GPIOF_CLK_ENABLE(); } while (0)
#define LED3_GPIO_PORT          GPIOA
#define LED3_GPIO_PIN           GPIO_PIN_7
#define LED3_GPIO_CLK_ENABLE()  do { __HAL_RCC_GPIOF_CLK_ENABLE(); } while (0)

/* IO���� */
#define LED2(x)                 do { (x) ?                                                              \
                                    HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_SET):     \
                                    HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_RESET);   \
                                } while (0)
#define LED3(x)                 do { (x) ?                                                              \
                                    HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_PIN_SET):     \
                                    HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_GPIO_PIN, GPIO_PIN_RESET);   \
                                } while (0)
#define LED2_TOGGLE()           do { HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_GPIO_PIN); } while (0)
#define LED3_TOGGLE()           do { HAL_GPIO_TogglePin(LED3_GPIO_PORT, LED3_GPIO_PIN); } while (0)

/* �������� */
void led_init(void);    /* ��ʼ��LED */

#endif
