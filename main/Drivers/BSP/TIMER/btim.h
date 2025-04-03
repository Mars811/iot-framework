/**
 ****************************************************************************************************
 * @file        btim.h
 * @author      С��
 * @version     V1.0
 * @date        2023-04-23
 * @brief       ������ʱ����������
 ****************************************************************************************************
 * @attention
 * 
 * ���俪����:DRG STM32F407VET6������
 * ������ַ��https://makerbase.taobao.com/
 * 
 ****************************************************************************************************
 */

#ifndef __BTIM_H
#define __BTIM_H

#include "./SYSTEM/sys/sys.h"

/* ������ʱ������ */
#define BTIM_TIMX_INT               TIM6
#define BTIM_TIMX_INT_IRQn          TIM6_DAC_IRQn
#define BTIM_TIMX_INT_IRQHandler    TIM6_DAC_IRQHandler
#define BTIM_TIMX_INT_CLK_ENABLE()  do { __HAL_RCC_TIM6_CLK_ENABLE(); } while (0)

/* �������� */
void btim_timx_int_init(uint16_t arr, uint16_t psc);    /* ��ʼ��������ʱ�� */

#endif
