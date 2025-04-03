/**
 ****************************************************************************************************
 * @file        atim.c
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

#include "./BSP/TIMER/atim.h"

/* �߼���ʱ������ */
TIM_HandleTypeDef g_timx_npwm_handle = {0};
TIM_HandleTypeDef g_timx_comp_handle = {0};
TIM_HandleTypeDef g_timx_cplm_handle = {0};
TIM_HandleTypeDef g_timx_pwmin_handle = {0};

/* ���ָ������PWM��ʱ����ر��� */
uint32_t g_timx_chy_npwm_remain = 0;                                    /* ���ָ������PWM��ʱ��ʣ����������� */

/* ����������������ƶ�ʱ����ر��� */
TIM_BreakDeadTimeConfigTypeDef g_break_dead_time_config_struct = {0};   /* �������Ʋ������þ�� */

/* PWM���붨ʱ����ر��� */
uint8_t g_timx_chy_pwmin_sta = 0;                                       /* ����״̬ */
uint32_t g_timx_chy_pwmin_hval;                                         /* PWM�ߵ�ƽ���� */
uint32_t g_timx_chy_pwmin_cval;                                         /* PWM���� */

/**
 * @brief   ��ʼ���߼���ʱ�����ָ������PWM
 * @param   arr: �Զ���װ��ֵ
 * @param   psc: Ԥ��Ƶϵ��
 * @retval  ��
 */
void atim_timx_chy_npwm_init(uint16_t arr, uint16_t psc)
{
    GPIO_InitTypeDef gpio_init_struct;
    TIM_OC_InitTypeDef timx_oc_init_struct = {0};
    
    /* ʹ�����ʱ�� */
    ATIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE();
    ATIM_TIMX_NPWM_CLK_ENABLE();
    
    /* ����PWM������� */
    gpio_init_struct.Pin = ATIM_TIMX_NPWM_CHY_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = ATIM_TIMX_NPWM_CHY_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_NPWM_CHY_GPIO_PORT, &gpio_init_struct);
    
    /* �����ж����ȼ���ʹ���ж� */
    HAL_NVIC_SetPriority(ATIM_TIMX_NPWM_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ATIM_TIMX_NPWM_IRQn);
    
    g_timx_npwm_handle.Instance = ATIM_TIMX_NPWM;                                               /* ��ʱ�� */
    g_timx_npwm_handle.Init.Prescaler = psc;                                                    /* Ԥ��Ƶ��ϵ�� */
    g_timx_npwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                                   /* ����ģʽ */
    g_timx_npwm_handle.Init.Period = arr;                                                       /* �Զ���װ��ֵ */
    g_timx_npwm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;                  /* �Զ���װ��Ԥ���� */
    g_timx_npwm_handle.Init.RepetitionCounter = 0;                                              /* �ظ�������ֵ */
    HAL_TIM_PWM_Init(&g_timx_npwm_handle);                                                      /* ��ʼ����ʱ��PWM��� */
    
    timx_oc_init_struct.OCMode = TIM_OCMODE_PWM1;                                               /* ����Ƚ�ģʽ */
    timx_oc_init_struct.Pulse = (arr + 1) >> 1;                                                 /* ռ�ձ� */
    timx_oc_init_struct.OCPolarity = TIM_OCPOLARITY_HIGH;                                       /* ��Ч��ƽ */
    HAL_TIM_PWM_ConfigChannel(&g_timx_npwm_handle, &timx_oc_init_struct, ATIM_TIMX_NPWM_CHY);   /* ���ö�ʱ��PWMͨ�� */
    
    __HAL_TIM_ENABLE_IT(&g_timx_npwm_handle, TIM_IT_UPDATE);                                    /* ʹ�ܶ�ʱ�������ж� */
    HAL_TIM_PWM_Start(&g_timx_npwm_handle, ATIM_TIMX_NPWM_CHY);                                 /* ������ʱ��PWM��� */
}

/**
 * @brief   ���ø߼���ʱ�����ָ������PWM
 * @param   npwm: ָ��PWM����
 * @retval  ��
 */
void atim_timx_chy_npwm_set(uint32_t npwm)
{
    if (npwm == 0)
    {
        return;
    }
    
    /* ���������¼������ָ������PWM */
    g_timx_chy_npwm_remain = npwm;
    HAL_TIM_GenerateEvent(&g_timx_npwm_handle, TIM_EVENTSOURCE_UPDATE);
    __HAL_TIM_ENABLE(&g_timx_npwm_handle);
}

/**
 * @brief   ���ָ������PWM�߼���ʱ���жϻص�����
 * @param   ��
 * @retval  ��
 */
void ATIM_TIMX_NPWM_IRQHandler(void)
{
    uint16_t npwm = 0;
    
    /* ��ʱ�������¼��ж� */
    if (__HAL_TIM_GET_FLAG(&g_timx_npwm_handle, TIM_FLAG_UPDATE) != RESET)
    {
        /* ÿ��������256��PWM��TIMx_RCRֻ��[7:0]��Ч�� */
        if (g_timx_chy_npwm_remain > 256)
        {
            g_timx_chy_npwm_remain -= 256;
            npwm = 256;
        }
        else if ((g_timx_chy_npwm_remain % 256) != 0)
        {
            npwm = g_timx_chy_npwm_remain % 256;
            g_timx_chy_npwm_remain = 0;
        }
        
        if (npwm != 0)
        {
            /* ���ø߼���ʱ�����ظ������������PWM */
            ATIM_TIMX_NPWM->RCR = npwm - 1;
            HAL_TIM_GenerateEvent(&g_timx_npwm_handle, TIM_EVENTSOURCE_UPDATE);
            __HAL_TIM_ENABLE(&g_timx_npwm_handle);
        }
        else
        {
            ATIM_TIMX_NPWM->CR1 &= ~(TIM_CR1_CEN);
        }
        
        /* ��������¼���־ */
        __HAL_TIM_CLEAR_IT(&g_timx_npwm_handle, TIM_IT_UPDATE);
    }
}

/**
 * @brief   ��ʼ���߼���ʱ������Ƚ�
 * @param   arr: �Զ���װ��ֵ
 * @param   psc: Ԥ��Ƶϵ��
 * @retval  ��
 */
void atim_timx_comp_init(uint16_t arr, uint16_t psc)
{
    GPIO_InitTypeDef gpio_init_struct;
    TIM_OC_InitTypeDef timx_oc_init_struct = {0};
    
    /* ʹ�����ʱ�� */
    ATIM_TIMX_COMP_CH1_GPIO_CLK_ENABLE();
    ATIM_TIMX_COMP_CH2_GPIO_CLK_ENABLE();
    ATIM_TIMX_COMP_CLK_ENABLE();
    
    /* ��������Ƚ�ͨ��1������� */
    gpio_init_struct.Pin = ATIM_TIMX_COMP_CH1_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = ATIM_TIMX_COMP_CH1_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_COMP_CH1_GPIO_PORT, &gpio_init_struct);
    
    /* ��������Ƚ�ͨ��2������� */
    gpio_init_struct.Pin = ATIM_TIMX_COMP_CH2_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = ATIM_TIMX_COMP_CH2_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_COMP_CH2_GPIO_PORT, &gpio_init_struct);
    
    g_timx_comp_handle.Instance = ATIM_TIMX_COMP;                                       /* ��ʱ�� */
    g_timx_comp_handle.Init.Prescaler = psc;                                            /* Ԥ��Ƶ��ϵ�� */
    g_timx_comp_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                           /* ����ģʽ */
    g_timx_comp_handle.Init.Period = arr;                                               /* �Զ���װ��ֵ */
    g_timx_comp_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;          /* �Զ���װ��Ԥ���� */
    g_timx_comp_handle.Init.RepetitionCounter = 0;                                      /* �ظ�������ֵ */
    HAL_TIM_OC_Init(&g_timx_comp_handle);                                               /* ��ʼ����ʱ������Ƚ� */
    
    timx_oc_init_struct.OCMode = TIM_OCMODE_TOGGLE;                                     /* ����Ƚ�ģʽ */
    timx_oc_init_struct.Pulse = ((arr + 1) >> 2) * 1;                                   /* �Ƚ�ֵ */
    timx_oc_init_struct.OCPolarity = TIM_OCPOLARITY_HIGH;                               /* ����Ƚϼ��� */
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_handle, &timx_oc_init_struct, TIM_CHANNEL_1); /* ��������Ƚ�ͨ�� */
    
    timx_oc_init_struct.OCMode = TIM_OCMODE_TOGGLE;                                     /* ����Ƚ�ģʽ */
    timx_oc_init_struct.Pulse = ((arr + 1) >> 2) * 2;                                   /* �Ƚ�ֵ */
    timx_oc_init_struct.OCPolarity = TIM_OCPOLARITY_HIGH;                               /* ����Ƚϼ��� */
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_handle, &timx_oc_init_struct, TIM_CHANNEL_2); /* ��������Ƚ�ͨ�� */
    
    /* ��������Ƚ�ͨ����� */
    HAL_TIM_OC_Start(&g_timx_comp_handle, TIM_CHANNEL_1);
    HAL_TIM_OC_Start(&g_timx_comp_handle, TIM_CHANNEL_2);
}

/**
 * @brief   ��ʼ���߼���ʱ�������������������
 * @param   arr: �Զ���װ��ֵ
 * @param   psc: Ԥ��Ƶϵ��
 * @retval  ��
 */
void atim_timx_cplm_init(uint16_t arr, uint16_t psc)
{
    GPIO_InitTypeDef gpio_init_struct;
    TIM_OC_InitTypeDef timx_oc_struct = {0};
    
    /* ʹ�����ʱ�� */
    ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE();
    ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE();
    ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE();
    ATIM_TIMX_CPLM_CLK_ENABLE();
    
    /* ��������Ƚ�ͨ��1������� */
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHY_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = ATIM_TIMX_CPLM_CHY_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_CPLM_CHY_GPIO_PORT, &gpio_init_struct);
    
    /* ��������Ƚ�ͨ��1����������� */
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHYN_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = ATIM_TIMX_CPLM_CHYN_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_CPLM_CHYN_GPIO_PORT, &gpio_init_struct);
    
    /* ��������Ƚ�ͨ��ɲ���������� */
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_BKIN_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = ATIM_TIMX_CPLM_BKIN_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_CPLM_BKIN_GPIO_PORT, &gpio_init_struct);
    
    g_timx_cplm_handle.Instance = ATIM_TIMX_CPLM;                                           /* ��ʱ�� */
    g_timx_cplm_handle.Init.Prescaler = psc;                                                /* Ԥ��Ƶ��ϵ�� */
    g_timx_cplm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                               /* ����ģʽ */
    g_timx_cplm_handle.Init.Period = arr;                                                   /* �Զ���װ��ֵ */
    g_timx_cplm_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;                         /* ����ʱ�ӷ�Ƶ��ϵ�� */
    g_timx_cplm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;              /* �Զ���װ��Ԥ���� */
    g_timx_cplm_handle.Init.RepetitionCounter = 0;                                          /* �ظ�������ֵ */
    HAL_TIM_PWM_Init(&g_timx_cplm_handle);                                                  /* ��ʼ����ʱ��PWM��� */
        
    timx_oc_struct.OCMode = TIM_OCMODE_PWM1;                                                /* ����Ƚ�ģʽ */
    timx_oc_struct.Pulse = (arr + 1) >> 1;                                                  /* �Ƚ�ֵ */
    timx_oc_struct.OCPolarity = TIM_OCPOLARITY_LOW;                                         /* ����Ƚϼ��� */
    timx_oc_struct.OCNPolarity = TIM_OCNPOLARITY_LOW;                                       /* ����ͨ������Ƚϼ��� */
    timx_oc_struct.OCIdleState = TIM_OCIDLESTATE_SET;                                       /* ����������� */
    timx_oc_struct.OCNIdleState = TIM_OCNIDLESTATE_SET;                                     /* ����ͨ������������� */
    HAL_TIM_PWM_ConfigChannel(&g_timx_cplm_handle, &timx_oc_struct, ATIM_TIMX_CPLM_CHY);    /* ����PWM���ͨ�� */
    
    g_break_dead_time_config_struct.OffStateRunMode = TIM_OSSR_DISABLE;                     /* ����ģʽ�Ĺر����״̬ */
    g_break_dead_time_config_struct.OffStateIDLEMode = TIM_OSSI_DISABLE;                    /* ����ģʽ�Ĺر����״̬ */
    g_break_dead_time_config_struct.LockLevel = TIM_LOCKLEVEL_OFF;                          /* �Ĵ����� */
    g_break_dead_time_config_struct.BreakState = TIM_BREAK_ENABLE;                          /* ɲ������ʹ�� */
    g_break_dead_time_config_struct.BreakPolarity = TIM_BREAKPOLARITY_HIGH;                 /* ɲ��������Ч���� */
    g_break_dead_time_config_struct.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;           /* �Զ�ʹ����� */
    HAL_TIMEx_ConfigBreakDeadTime(&g_timx_cplm_handle, &g_break_dead_time_config_struct);   /* ����������ɲ���Ȳ��� */
    
    /* ����PWM���ͨ����� */
    HAL_TIM_PWM_Start(&g_timx_cplm_handle, ATIM_TIMX_CPLM_CHY);
    HAL_TIMEx_PWMN_Start(&g_timx_cplm_handle, ATIM_TIMX_CPLM_CHY);
}

/**
 * @brief   ���ø߼���ʱ�������������������
 * @param   ccr: ����Ƚ�ֵ
 * @param   dtg: ����ʱ��
 * @retval  ��
 */
void atim_timx_cplm_set(uint16_t ccr, uint8_t dtg)
{
    g_break_dead_time_config_struct.DeadTime = dtg;                                         /* ����ʱ�� */
    HAL_TIMEx_ConfigBreakDeadTime(&g_timx_cplm_handle, &g_break_dead_time_config_struct);   /* ��������ʱ����� */
    __HAL_TIM_MOE_ENABLE(&g_timx_cplm_handle);                                              /* ʹ������� */
    ATIM_TIMX_CPLM->CCR1 = ccr;                                                             /* ���ñȽϼĴ��� */
}

/**
 * @brief   ��ʼ���߼���ʱ��PWM����
 * @param   psc: Ԥ��Ƶϵ��
 * @retval  ��
 */
void atim_timx_pwmin_chy_init(uint16_t psc)
{
    GPIO_InitTypeDef gpio_init_struct;
    TIM_SlaveConfigTypeDef tim_slave_config_struct = {0};
    TIM_IC_InitTypeDef tim_ic_init_struct = {0};
    
    /* ʹ�����ʱ�� */
    ATIM_TIMX_PWMIN_CLK_ENABLE();
    ATIM_TIMX_PWMIN_CHY_GPIO_CLK_ENABLE();
    
    /* ����PWM�������� */
    gpio_init_struct.Pin = ATIM_TIMX_PWMIN_CHY_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP; 
    gpio_init_struct.Pull = GPIO_PULLDOWN;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = ATIM_TIMX_PWMIN_CHY_GPIO_AF;
    HAL_GPIO_Init(ATIM_TIMX_PWMIN_CHY_GPIO_PORT, &gpio_init_struct);
    
    g_timx_pwmin_handle.Instance = ATIM_TIMX_PWMIN;                                     /* ��ʱ�� */
    g_timx_pwmin_handle.Init.Prescaler = psc;                                           /* Ԥ��Ƶ��ϵ�� */
    g_timx_pwmin_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                          /* ����ģʽ */
    g_timx_pwmin_handle.Init.Period = 0xFFFF;                                           /* �Զ���װ��ֵ */
    HAL_TIM_IC_Init(&g_timx_pwmin_handle);
    
    tim_slave_config_struct.SlaveMode = TIM_SLAVEMODE_RESET;                            /* ��ģʽ */
    tim_slave_config_struct.InputTrigger = TIM_TS_TI1FP1;                               /* ���봥��Դ */
    tim_slave_config_struct.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;               /* �������� */
    tim_slave_config_struct.TriggerFilter = 0;                                          /* �����˲� */
    HAL_TIM_SlaveConfigSynchro(&g_timx_pwmin_handle, &tim_slave_config_struct);         /* ���ô�ģʽ */

    tim_ic_init_struct.ICPolarity = TIM_ICPOLARITY_RISING;                              /* ���벶���� */
    tim_ic_init_struct.ICSelection = TIM_ICSELECTION_DIRECTTI;                          /* ���벶��ѡ�� */
    tim_ic_init_struct.ICPrescaler = TIM_ICPSC_DIV1;                                    /* ���벶���Ƶϵ�� */
    tim_ic_init_struct.ICFilter = 0;                                                    /* ���벶���˲� */
    HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_handle, &tim_ic_init_struct, TIM_CHANNEL_1); /* �������벶��ͨ��1 */
    
    tim_ic_init_struct.ICPolarity = TIM_ICPOLARITY_FALLING;                             /* ���벶���� */
    tim_ic_init_struct.ICSelection = TIM_ICSELECTION_INDIRECTTI;                        /* ���벶��ѡ�� */
    tim_ic_init_struct.ICPrescaler = TIM_ICPSC_DIV1;                                    /* ���벶���Ƶϵ�� */
    tim_ic_init_struct.ICFilter = 0;                                                    /* ���벶���˲� */
    HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_handle, &tim_ic_init_struct, TIM_CHANNEL_2); /* �������벶��ͨ��2 */
    
    /* �����ж����ȼ���ʹ���ж� */
    HAL_NVIC_SetPriority(ATIM_TIMX_PWMIN_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ( ATIM_TIMX_PWMIN_IRQn );
    
    /* �������벶��ͨ�� */
    HAL_TIM_IC_Start_IT(&g_timx_pwmin_handle, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&g_timx_pwmin_handle, TIM_CHANNEL_2);
}

/**
 * @brief   PWM����߼���ʱ���жϻص�����
 * @param   ��
 * @retval  ��
 */
void ATIM_TIMX_PWMIN_IRQHandler(void)
{
    /* ���벶��ͨ��1���������� */
    if (__HAL_TIM_GET_FLAG(&g_timx_pwmin_handle, TIM_FLAG_CC1) != RESET)
    {
        /* ��ȡ����PWM������ */
        g_timx_chy_pwmin_cval = HAL_TIM_ReadCapturedValue(&g_timx_pwmin_handle, TIM_CHANNEL_1) + 1;
        g_timx_chy_pwmin_sta = 1;
        
        __HAL_TIM_CLEAR_FLAG(&g_timx_pwmin_handle, TIM_FLAG_CC1);
    }
    
    /* ���벶��ͨ��2�����½��� */
    if (__HAL_TIM_GET_FLAG(&g_timx_pwmin_handle, TIM_FLAG_CC2) != RESET)
    {
        /* ��ȡ����PWM�ĸߵ�ƽ���� */
        g_timx_chy_pwmin_hval = HAL_TIM_ReadCapturedValue(&g_timx_pwmin_handle, TIM_CHANNEL_2) + 1;
        
        __HAL_TIM_CLEAR_FLAG(&g_timx_pwmin_handle, TIM_FLAG_CC2);
    }
}
