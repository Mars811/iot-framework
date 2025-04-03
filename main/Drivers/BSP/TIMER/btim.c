/**
 ****************************************************************************************************
 * @file        btim.c
 * @author      小丁
 * @version     V1.0
 * @date        2023-04-23
 * @brief       基本定时器驱动代码
 ****************************************************************************************************
 * @attention
 * 
 * 适配开发板:DRG ST-1 STM32F407VET6开发板
 * 购买网址：https://makerbase.taobao.com/
 * 
 ****************************************************************************************************
 */

#include "./BSP/TIMER/btim.h"
#include "./BSP/LED/led.h"
#include "lvgl.h"
#include "tim.h"

/* 基本定时器句柄 */
TIM_HandleTypeDef g_timx_handle = {0};

/**
 * @brief   初始化基本定时器
 * @param   arr: 自动重装载值
 * @param   psc: 预分频系数
 * @retval  无
 */
void btim_timx_int_init(uint16_t arr, uint16_t psc)
{
    g_timx_handle.Instance = BTIM_TIMX_INT;
    g_timx_handle.Init.Prescaler = psc;
    g_timx_handle.Init.Period = arr;
    HAL_TIM_Base_Init(&g_timx_handle);
    HAL_TIM_Base_Start_IT(&g_timx_handle);
}

/**
 * @brief   HAL库TIM初始化MSP函数
 * @param   无
 * @retval  无
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIMX_INT)
    {
        /* 使能基本定时器时钟 */
        BTIM_TIMX_INT_CLK_ENABLE();
        
        /* 配置中断优先级并使能中断 */
        HAL_NVIC_SetPriority(BTIM_TIMX_INT_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(BTIM_TIMX_INT_IRQn);
    }
}

/**
 * @brief   基本定时器中断服务函数
 * @param   无
 * @retval  无
 */
void BTIM_TIMX_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_handle);
}

/**
 * @brief   HAL库基本定时器超时中断回调函数
 * @param   无
 * @retval  无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIMX_INT)
    {
        lv_tick_inc(1);  //进入中断的ms数要和本函数的输入参数相同
    }

    if(htim ==&htim2)//判断是否是定时器2中断（定时器到时表示一组字符串接收结束）
    {
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		USART3_RX_STA|=0x8000;//接收标志位最高位置1表示接收完成
		__HAL_TIM_CLEAR_FLAG(&htim2,TIM_EVENTSOURCE_UPDATE );//清除TIM2更新中断标志
		__HAL_TIM_DISABLE(&htim2);//关闭定时器2
    }
}
