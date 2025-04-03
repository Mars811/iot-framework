/**
 ****************************************************************************************************
 * @file        gtim.h
 * @author      С��
 * @version     V1.0
 * @date        2023-04-23
 * @brief       ͨ�ö�ʱ����������
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention
 * 
 * ʵ�鿪����:DRG ST-1 STM32F407VET6���İ�
 * 
  *
 * ��˾��ַ:www.genbotter.com
 * �����ַ:makerbase.taobao.com
 * 
 ****************************************************************************************************
 */

#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"

/* ͨ�ö�ʱ������ */
#define GTIM_TIMX_INT                       TIM3
#define GTIM_TIMX_INT_IRQn                  TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define GTIM_TIMX_INT_CLK_ENABLE()          do { __HAL_RCC_TIM3_CLK_ENABLE(); } while (0)

#define GTIM_TIMX_PWM                       TIM3
#define GTIM_TIMX_PWM_CLK_ENABLE()          do { __HAL_RCC_TIM3_CLK_ENABLE(); } while (0)
#define GTIM_TIMX_PWM_CHY                   TIM_CHANNEL_1
#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOB
#define GTIM_TIMX_PWM_CHY_GPIO_PIN          GPIO_PIN_4
#define GTIM_TIMX_PWM_CHY_GPIO_AF           GPIO_AF2_TIM3
#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)

#define GTIM_TIMX_CAP                       TIM5
#define GTIM_TIMX_CAP_IRQn                  TIM5_IRQn
#define GTIM_TIMX_CAP_IRQHandler            TIM5_IRQHandler
#define GTIM_TIMX_CAP_CLK_ENABLE()          do { __HAL_RCC_TIM5_CLK_ENABLE(); } while (0)
#define GTIM_TIMX_CAP_CHY                   TIM_CHANNEL_1
#define GTIM_TIMX_CAP_CHY_GPIO_PORT         GPIOA
#define GTIM_TIMX_CAP_CHY_GPIO_PIN          GPIO_PIN_0
#define GTIM_TIMX_CAP_CHY_GPIO_AF           GPIO_AF2_TIM5
#define GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE() do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)

#define GTIM_TIMX_CNT                       TIM2
#define GTIM_TIMX_CNT_IRQn                  TIM2_IRQn
#define GTIM_TIMX_CNT_IRQHandler            TIM2_IRQHandler
#define GTIM_TIMX_CNT_CLK_ENABLE()          do { __HAL_RCC_TIM2_CLK_ENABLE(); } while (0)
#define GTIM_TIMX_CNT_CHY                   TIM_CHANNEL_1
#define GTIM_TIMX_CNT_CHY_GPIO_PORT         GPIOA
#define GTIM_TIMX_CNT_CHY_GPIO_PIN          GPIO_PIN_0
#define GTIM_TIMX_CNT_CHY_GPIO_AF           GPIO_AF1_TIM2
#define GTIM_TIMX_CNT_CHY_GPIO_CLK_ENABLE() do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)

/* �������� */
void gtim_timx_int_init(uint16_t arr, uint16_t psc);        /* ��ʼ��ͨ�ö�ʱ���ж� */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc);    /* ��ʼ��ͨ�ö�ʱ��PWM */
void gtim_timx_cap_chy_init(uint16_t arr, uint16_t psc);    /* ��ʼ��ͨ�ö�ʱ�����벶�� */
void gtim_timx_cnt_chy_init(uint16_t psc);                  /* ��ʼ��ͨ�ö�ʱ��������� */
uint32_t gtim_timx_cnt_chy_get_count(void);                 /* ��ȡͨ�ö�ʱ���������ֵ */
void gtim_timx_cnt_chy_restart(void);                       /* ����ͨ�ö�ʱ��������� */

#endif
