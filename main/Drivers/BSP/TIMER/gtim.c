/**
 ****************************************************************************************************
 * @file        gtim.c
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

#include "./BSP/TIMER/gtim.h"
#include "./BSP/LED/led.h"

/* ͨ�ö�ʱ����� */
TIM_HandleTypeDef g_timx_int_handle = {0};
TIM_HandleTypeDef g_timx_pwm_handle = {0};
TIM_HandleTypeDef g_timx_cap_handle = {0};
TIM_HandleTypeDef g_timx_cnt_handle = {0};

/* ���벶��ʱ����ر��� */
uint8_t g_timx_chy_cap_sta = 0;     /* [7]: ������� [6]: ���񵽸ߵ�ƽ [5:0]: �������ֵ��������� */
uint16_t g_timx_chy_cap_val = 0;    /* ����ļ���ֵ */

/* ���������ʱ����ر��� */
uint32_t g_timx_chy_cnt_ofcnt = 0;  /* ���������ʱ����������� */

/**
 * @brief   ��ʼ��ͨ�ö�ʱ���ж�
 * @param   arr: �Զ���װ��ֵ
 * @param   psc: Ԥ��Ƶϵ��
 * @retval  ��
 */
void gtim_timx_int_init(uint16_t arr, uint16_t psc)
{
    g_timx_int_handle.Instance = GTIM_TIMX_INT;                 /* ��ʱ�� */
    g_timx_int_handle.Init.Prescaler = psc;                     /* Ԥ��Ƶϵ�� */
    g_timx_int_handle.Init.CounterMode = TIM_COUNTERMODE_UP;    /* ����ģʽ */
    g_timx_int_handle.Init.Period = arr;                        /* ��װ��ֵ */
    HAL_TIM_Base_Init(&g_timx_int_handle);                      /* ��ʱ��������ʼ�� */
    HAL_TIM_Base_Start_IT(&g_timx_int_handle);                  /* ������ʱ���������ж� */
}

/**
 * @brief   ��ʼ��ͨ�ö�ʱ��PWM
 * @param   arr: �Զ���װ��ֵ
 * @param   psc: Ԥ��Ƶϵ��
 * @retval  ��
 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc)
{
    TIM_OC_InitTypeDef timx_oc_pwm_struct = {0};
    
    g_timx_pwm_handle.Instance = GTIM_TIMX_PWM;                                             /* ��ʱ�� */
    g_timx_pwm_handle.Init.Prescaler = psc;                                                 /* Ԥ��Ƶ��ϵ�� */
    g_timx_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                                /* ����ģʽ */
    g_timx_pwm_handle.Init.Period = arr;                                                    /* ��װ��ֵ */
    HAL_TIM_PWM_Init(&g_timx_pwm_handle);                                                   /* ��ʱ��PWM��ʼ�� */
    
    timx_oc_pwm_struct.OCMode = TIM_OCMODE_PWM1;                                            /* ����Ƚ�ģʽ */
    timx_oc_pwm_struct.Pulse = (arr + 1) >> 1;                                              /* ռ�ձ� */
    timx_oc_pwm_struct.OCPolarity = TIM_OCPOLARITY_HIGH;                                    /* ��Ч��ƽ */
    HAL_TIM_PWM_ConfigChannel(&g_timx_pwm_handle, &timx_oc_pwm_struct, GTIM_TIMX_PWM_CHY);  /* ���ö�ʱ��PWMͨ�� */
    HAL_TIM_PWM_Start(&g_timx_pwm_handle, GTIM_TIMX_PWM_CHY);                               /* ������ʱ��PWM��� */
}

/**
 * @brief   ��ʼ��ͨ�ö�ʱ�����벶��
 * @param   arr: �Զ���װ��ֵ
 * @param   psc: Ԥ��Ƶϵ��
 * @retval  ��
 */
void gtim_timx_cap_chy_init(uint16_t arr, uint16_t psc)
{
    TIM_IC_InitTypeDef timx_ic_cap_struct = {0};
    
    g_timx_cap_handle.Instance = GTIM_TIMX_CAP;                                             /* ��ʱ�� */
    g_timx_cap_handle.Init.Prescaler = psc;                                                 /* Ԥ��Ƶ��ϵ�� */
    g_timx_cap_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                                /* ����ģʽ */
    g_timx_cap_handle.Init.Period = arr;                                                    /* ��װ��ֵ */
    HAL_TIM_IC_Init(&g_timx_cap_handle);                                                    /* ��ʱ�����벶���ʼ�� */
    
    timx_ic_cap_struct.ICPolarity = TIM_ICPOLARITY_RISING;                                  /* ������ */
    timx_ic_cap_struct.ICSelection = TIM_ICSELECTION_DIRECTTI;                              /* ӳ��ѡ�� */
    timx_ic_cap_struct.ICPrescaler = TIM_ICPSC_DIV1;                                        /* �����Ƶ */
    timx_ic_cap_struct.ICFilter = 0;                                                        /* �����˲� */
    HAL_TIM_IC_ConfigChannel(&g_timx_cap_handle, &timx_ic_cap_struct, GTIM_TIMX_CAP_CHY);   /* ���ö�ʱ�����벶��ͨ�� */
    
    __HAL_TIM_ENABLE_IT(&g_timx_cap_handle, TIM_IT_UPDATE);                                 /* ʹ�ܶ�ʱ�������ж� */
    HAL_TIM_IC_Start_IT(&g_timx_cap_handle, GTIM_TIMX_CAP_CHY);                             /* ������ʱ�����벶�� */
}

/**
 * @brief   ��ʼ��ͨ�ö�ʱ���������
 * @param   psc: Ԥ��Ƶϵ��
 * @retval  ��
 */
void gtim_timx_cnt_chy_init(uint16_t psc)
{
    TIM_SlaveConfigTypeDef tim_slave_config_struct = {0};
    
    g_timx_cnt_handle.Instance = GTIM_TIMX_CNT;                                 /* ��ʱ�� */
    g_timx_cnt_handle.Init.Prescaler = psc;                                     /* Ԥ��Ƶ��ϵ�� */
    g_timx_cnt_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                    /* ����ģʽ */
    g_timx_cnt_handle.Init.Period = 0xFFFF;                                     /* ��װ��ֵ */
    HAL_TIM_IC_Init(&g_timx_cnt_handle);                                        /* ��ʱ�����벶���ʼ�� */
    
    tim_slave_config_struct.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;                /* ��ģʽ */
    tim_slave_config_struct.InputTrigger = TIM_TS_TI1FP1;                       /* ���봥�� */
    tim_slave_config_struct.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;       /* �������� */
    tim_slave_config_struct.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;       /* ������Ƶ */
    tim_slave_config_struct.TriggerFilter = 0;                                  /* �����˲��� */
    HAL_TIM_SlaveConfigSynchro(&g_timx_cnt_handle, &tim_slave_config_struct);   /* ���ö�ʱ����ģʽ */
    
    __HAL_TIM_ENABLE_IT(&g_timx_cnt_handle, TIM_IT_UPDATE);                     /* ʹ�ܶ�ʱ�������ж� */
    HAL_TIM_IC_Start(&g_timx_cnt_handle, GTIM_TIMX_CNT_CHY);                    /* ������ʱ�����벶�� */
}

/**
 * @brief   ��ȡͨ�ö�ʱ���������ֵ
 * @param   ��
 * @retval  �������ֵ
 */
uint32_t gtim_timx_cnt_chy_get_count(void)
{
    uint32_t total;
    
    /* �������������ֵ */
    total = g_timx_chy_cnt_ofcnt * 0xFFFF;
    total += __HAL_TIM_GET_COUNTER(&g_timx_cnt_handle);
    
    return total;
}

/**
 * @brief   ����ͨ�ö�ʱ���������
 * @param   ��
 * @retval  ��
 */
void gtim_timx_cnt_chy_restart(void)
{
    __HAL_TIM_DISABLE(&g_timx_cnt_handle);
    g_timx_chy_cnt_ofcnt = 0;
    __HAL_TIM_SET_COUNTER(&g_timx_cnt_handle, 0);
    __HAL_TIM_ENABLE(&g_timx_cnt_handle);
}

/**
 * @brief   HAL�������ʱ����ʼ��MSP����
 * @param   ��
 * @retval  ��
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == GTIM_TIMX_INT)
    {
        /* ʹ��ͨ�ö�ʱ��ʱ�� */
        GTIM_TIMX_INT_CLK_ENABLE();
        
        /* �����ж����ȼ���ʹ���ж� */
        HAL_NVIC_SetPriority(GTIM_TIMX_INT_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(GTIM_TIMX_INT_IRQn);
    }
}

/**
 * @brief   HAL�ⶨʱ��PWM��ʼ��MSP����
 * @param   ��
 * @retval  ��
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    /* PWM�����ʱ�� */
    if (htim->Instance == GTIM_TIMX_PWM)
    {
        /* ʹ�����ʱ�� */
        GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE();
        GTIM_TIMX_PWM_CLK_ENABLE();
        
        /* ��ʼ��PWM������� */
        gpio_init_struct.Pin = GTIM_TIMX_PWM_CHY_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = GTIM_TIMX_PWM_CHY_GPIO_AF;
        HAL_GPIO_Init(GTIM_TIMX_PWM_CHY_GPIO_PORT, &gpio_init_struct);
    }
}

/**
 * @brief   HAL�ⶨʱ����������ʼ��MSP����
 * @param   ��
 * @retval  ��
 */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    /* ���벶��ʱ�� */
    if (htim->Instance == GTIM_TIMX_CAP)
    {
        /* ʹ�����ʱ�� */
        GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE();
        GTIM_TIMX_CAP_CLK_ENABLE();
        
        /* ��ʼ�����벶������ */
        gpio_init_struct.Pin = GTIM_TIMX_CAP_CHY_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLDOWN;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = GTIM_TIMX_CAP_CHY_GPIO_AF;
        HAL_GPIO_Init(GTIM_TIMX_CAP_CHY_GPIO_PORT, &gpio_init_struct);
        
        /* �����ж����ȼ���ʹ���ж� */
        HAL_NVIC_SetPriority(GTIM_TIMX_CAP_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(GTIM_TIMX_CAP_IRQn);
    }
    /* ���������ʱ�� */
    if (htim->Instance == GTIM_TIMX_CNT)
    {
        /* ʹ�����ʱ�� */
        GTIM_TIMX_CNT_CHY_GPIO_CLK_ENABLE();
        GTIM_TIMX_CNT_CLK_ENABLE();
        
        /* ��ʼ�������������� */
        gpio_init_struct.Pin = GTIM_TIMX_CNT_CHY_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLDOWN;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = GTIM_TIMX_CNT_CHY_GPIO_AF;
        HAL_GPIO_Init(GTIM_TIMX_CNT_CHY_GPIO_PORT, &gpio_init_struct);
        
        /* �����ж����ȼ���ʹ���ж� */
        HAL_NVIC_SetPriority(GTIM_TIMX_CNT_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(GTIM_TIMX_CNT_IRQn);
    }
}

/**
 * @brief   ����ͨ�ö�ʱ���жϷ�����
 * @param   ��
 * @retval  ��
 */
void GTIM_TIMX_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_int_handle);
}

/**
 * @brief   ���벶��ͨ�ö�ʱ���жϻص�����
 * @param   ��
 * @retval  ��
 */
void GTIM_TIMX_CAP_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_cap_handle);
}

/**
 * @brief   �������ͨ�ö�ʱ���жϻص�����
 * @param   ��
 * @retval  ��
 */
void GTIM_TIMX_CNT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_cnt_handle);
}

/**
 * @brief   HAL�ⶨʱ����ʱ�жϻص�����
 * @param   ��
 * @retval  ��
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* ������ʱ�� */
    if (htim->Instance == GTIM_TIMX_INT)
    {
        LED1_TOGGLE();
    }
    /* ���벶��ʱ�� */
    else if (htim->Instance == GTIM_TIMX_CAP)
    {
        if ((g_timx_chy_cap_sta & 0x80) == 0)               /* ��δ������� */
        {
            if ((g_timx_chy_cap_sta & 0x40) != 0)           /* �Ѿ����񵽸ߵ�ƽ */
            {
                if ((g_timx_chy_cap_sta & 0x3F) == 0x3F)    /* �������ֵ������������� */
                {
                    /* ����ʹ�ܲ���ǿ�Ʊ��Ϊ������� */
                    TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_handle, GTIM_TIMX_CAP_CHY);
                    TIM_SET_CAPTUREPOLARITY(&g_timx_cap_handle, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING);
                    g_timx_chy_cap_sta |= 0x80;
                    g_timx_chy_cap_val = 0xFFFF;
                }
                else
                {
                    /* ���²������ֵ��������� */
                    g_timx_chy_cap_sta++;
                }
            }
        }
    }
    /* ���������ʱ�� */
    else if (htim->Instance == GTIM_TIMX_CNT)
    {
        /* �������������ʱ��������� */
        g_timx_chy_cnt_ofcnt++;
    }

    if(htim ==&htim2)//�ж��Ƿ��Ƕ�ʱ��2�жϣ���ʱ����ʱ��ʾһ���ַ������ս�����
    {
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		USART3_RX_STA|=0x8000;//���ձ�־λ���λ��1��ʾ�������
		__HAL_TIM_CLEAR_FLAG(&htim2,TIM_EVENTSOURCE_UPDATE );//���TIM2�����жϱ�־
		__HAL_TIM_DISABLE(&htim2);//�رն�ʱ��2
    }
}

/**
 * @brief   HAL�ⶨʱ�����벶���жϻص�����
 * @param   ��
 * @retval  ��
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == GTIM_TIMX_CAP)
    {
        if ((g_timx_chy_cap_sta & 0x80) == 0)       /* ����δ��� */
        {
            if ((g_timx_chy_cap_sta & 0x40) != 0)   /* �Ѳ��������� */
            {
                /* ���Ϊ������ɲ����¿������벶�� */
                g_timx_chy_cap_sta |= 0x80; 
                g_timx_chy_cap_val = HAL_TIM_ReadCapturedValue(&g_timx_cap_handle, GTIM_TIMX_CAP_CHY);
                TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_handle, GTIM_TIMX_CAP_CHY);
                TIM_SET_CAPTUREPOLARITY(&g_timx_cap_handle, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING);
            }
            else                                    /* δ���������� */
            {
                /* �Ӳ��񵽵�һ�������ؿ�ʼ���� */
                g_timx_chy_cap_sta = 0;
                g_timx_chy_cap_val = 0;
                g_timx_chy_cap_sta |= 0x40;
                __HAL_TIM_DISABLE(&g_timx_cap_handle);
                __HAL_TIM_SET_COUNTER(&g_timx_cap_handle, 0);
                TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_handle, GTIM_TIMX_CAP_CHY);
                TIM_SET_CAPTUREPOLARITY(&g_timx_cap_handle, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_FALLING);
                __HAL_TIM_ENABLE(&g_timx_cap_handle);
            }
        }
    }
}
