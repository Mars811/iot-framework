/**
 ****************************************************************************************************
 * @file        atim.h
 * @author      С��
 * @version     V1.0
 * @date        2023-04-23
 * @brief       �߼���ʱ����������
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

#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"

/* �߼���ʱ������ */
#define ATIM_TIMX_NPWM                          TIM8
#define ATIM_TIMX_NPWM_IRQn                     TIM8_UP_TIM13_IRQn
#define ATIM_TIMX_NPWM_IRQHandler               TIM8_UP_TIM13_IRQHandler
#define ATIM_TIMX_NPWM_CLK_ENABLE()             do { __HAL_RCC_TIM8_CLK_ENABLE(); } while (0)
#define ATIM_TIMX_NPWM_CHY                      TIM_CHANNEL_1
#define ATIM_TIMX_NPWM_CHY_GPIO_PORT            GPIOC
#define ATIM_TIMX_NPWM_CHY_GPIO_PIN             GPIO_PIN_6
#define ATIM_TIMX_NPWM_CHY_GPIO_AF              GPIO_AF3_TIM8
#define ATIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)

#define ATIM_TIMX_COMP                          TIM8
#define ATIM_TIMX_COMP_CLK_ENABLE()             do { __HAL_RCC_TIM8_CLK_ENABLE(); } while (0)
#define ATIM_TIMX_COMP_CH1_GPIO_PORT            GPIOC
#define ATIM_TIMX_COMP_CH1_GPIO_PIN             GPIO_PIN_6
#define ATIM_TIMX_COMP_CH1_GPIO_AF              GPIO_AF3_TIM8
#define ATIM_TIMX_COMP_CH1_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)
#define ATIM_TIMX_COMP_CH2_GPIO_PORT            GPIOC
#define ATIM_TIMX_COMP_CH2_GPIO_PIN             GPIO_PIN_7
#define ATIM_TIMX_COMP_CH2_GPIO_AF              GPIO_AF3_TIM8
#define ATIM_TIMX_COMP_CH2_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)

#define ATIM_TIMX_CPLM                          TIM1
#define ATIM_TIMX_CPLM_CLK_ENABLE()             do { __HAL_RCC_TIM1_CLK_ENABLE(); } while (0)
#define ATIM_TIMX_CPLM_CHY                      TIM_CHANNEL_1
#define ATIM_TIMX_CPLM_CHY_GPIO_PORT            GPIOA
#define ATIM_TIMX_CPLM_CHY_GPIO_PIN             GPIO_PIN_8
#define ATIM_TIMX_CPLM_CHY_GPIO_AF              GPIO_AF1_TIM1
#define ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)
#define ATIM_TIMX_CPLM_CHYN_GPIO_PORT           GPIOA
#define ATIM_TIMX_CPLM_CHYN_GPIO_PIN            GPIO_PIN_7
#define ATIM_TIMX_CPLM_CHYN_GPIO_AF             GPIO_AF1_TIM1
#define ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE()   do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)
#define ATIM_TIMX_CPLM_BKIN_GPIO_PORT           GPIOA
#define ATIM_TIMX_CPLM_BKIN_GPIO_PIN            GPIO_PIN_6
#define ATIM_TIMX_CPLM_BKIN_GPIO_AF             GPIO_AF1_TIM1
#define ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE()   do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)

#define ATIM_TIMX_PWMIN                         TIM8
#define ATIM_TIMX_PWMIN_IRQn                    TIM8_CC_IRQn
#define ATIM_TIMX_PWMIN_IRQHandler              TIM8_CC_IRQHandler
#define ATIM_TIMX_PWMIN_CLK_ENABLE()            do { __HAL_RCC_TIM8_CLK_ENABLE(); } while (0)
#define ATIM_TIMX_PWMIN_CHY                     TIM_CHANNEL_1
#define ATIM_TIMX_PWMIN_CHY_GPIO_PORT           GPIOC
#define ATIM_TIMX_PWMIN_CHY_GPIO_PIN            GPIO_PIN_6
#define ATIM_TIMX_PWMIN_CHY_GPIO_AF             GPIO_AF3_TIM8
#define ATIM_TIMX_PWMIN_CHY_GPIO_CLK_ENABLE()   do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)

/* �������� */
void atim_timx_chy_npwm_init(uint16_t arr, uint16_t psc);   /* ��ʼ���߼���ʱ�����ָ������PWM */
void atim_timx_chy_npwm_set(uint32_t npwm);                 /* ���ø߼���ʱ�����ָ������PWM */
void atim_timx_comp_init(uint16_t arr, uint16_t psc);       /* ��ʼ���߼���ʱ������Ƚ� */
void atim_timx_cplm_init(uint16_t arr, uint16_t psc);       /* ��ʼ���߼���ʱ������������������� */
void atim_timx_cplm_set(uint16_t ccr, uint8_t dtg);         /* ���ø߼���ʱ������������������� */
void atim_timx_pwmin_chy_init(uint16_t psc);                /* ��ʼ���߼���ʱ��PWM���� */

#endif
