/*
 * tim.c
 *
 *  Created on: Apr 23, 2022
 *      Author: Administrator
 */

#include "tim.h"
// #include "main.h"
TIM_HandleTypeDef htim2;

// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //��ʱ���жϻص�����
// {
//    if(htim ==&htim2)//�ж��Ƿ��Ƕ�ʱ��2�жϣ���ʱ����ʱ��ʾһ���ַ������ս�����
//     {
// 		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
// 		USART3_RX_STA|=0x8000;//���ձ�־λ���λ��1��ʾ�������
// 		__HAL_TIM_CLEAR_FLAG(&htim2,TIM_EVENTSOURCE_UPDATE );//���TIM2�����жϱ�־
// 		__HAL_TIM_DISABLE(&htim2);//�رն�ʱ��2
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