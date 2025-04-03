/*
 * tim.c
 *
 *  Created on: Apr 23, 2022
 *      Author: Administrator
 */

#include "tim.h"
// #include "main.h"
TIM_HandleTypeDef htim2;

// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //定时器中断回调函数
// {
//    if(htim ==&htim2)//判断是否是定时器2中断（定时器到时表示一组字符串接收结束）
//     {
// 		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
// 		USART3_RX_STA|=0x8000;//接收标志位最高位置1表示接收完成
// 		__HAL_TIM_CLEAR_FLAG(&htim2,TIM_EVENTSOURCE_UPDATE );//清除TIM2更新中断标志
// 		__HAL_TIM_DISABLE(&htim2);//关闭定时器2
//     }
// }


void tim2_init(){
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 7199;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 999;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
}