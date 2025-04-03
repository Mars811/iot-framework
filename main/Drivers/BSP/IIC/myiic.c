/**
 ****************************************************************************************************
 * @file        myiic.c
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

#include "./BSP/IIC/myiic.h"
#include "./SYSTEM/delay/delay.h"

/**
 * @brief   IIC��ʱ����
 * @note    ���ڿ���IICͨ������
 * @param   ��
 * @retval  ��
 */
static void iic_delay(void)
{
    delay_us(2);
}

/**
 * @brief   ��ʼ��IIC
 * @param   ��
 * @retval  ��
 */
void iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* ʱ��ʹ�� */
    IIC_SCL_GPIO_CLK_ENABLE();
    IIC_SDA_GPIO_CLK_ENABLE();
    
    /* ����SCL���� */
    gpio_init_struct.Pin = IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_SCL_GPIO_PORT, &gpio_init_struct);
    
    /* ����SDA���� */
    gpio_init_struct.Pin = IIC_SDA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_SDA_GPIO_PORT, &gpio_init_struct);
    
    iic_stop();
}

/**
 * @brief   ����IIC��ʼ�ź�
 * @param   ��
 * @retval  ��
 */
void iic_start(void)
{
    IIC_SDA(1);
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}

/**
 * @brief   ����IICֹͣ�ź�
 * @param   ��
 * @retval  ��
 */
void iic_stop(void)
{
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(1);
    iic_delay();
}

/**
 * @brief   �ȴ�IICӦ���ź�
 * @param   ��
 * @retval  �ȴ����
 * @arg     0: �ȴ�IICӦ���źųɹ�
 * @arg     1: �ȴ�IICӦ���ź�ʧ��
 */
uint8_t iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    
    IIC_SDA(1);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    
    while (IIC_SDA_READ != 0)
    {
        waittime++;
        if (waittime > 250)
        {
            iic_stop();
            rack = 1;
            break;
        }
    }
    
    IIC_SCL(0);
    iic_delay();
    
    return rack;
}

/**
 * @brief   ����IIC ACK�ź�
 * @param   ��
 * @retval  ��
 */
void iic_ack(void)
{
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
    IIC_SDA(1);
    iic_delay();
}

/**
 * @brief   ����IIC NACK�ź�
 * @param   ��
 * @retval  ��
 */
void iic_nack(void)
{
    IIC_SDA(1);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}

/**
 * @brief   IIC����һ���ֽ�
 * @param   data: �����͵�һ�ֽ�����
 * @retval  ��
 */
void iic_send_byte(uint8_t data)
{
    uint8_t t;
    
    for (t=0; t<8; t++)
    {
        IIC_SDA((data & 0x80) >> 7);
        iic_delay();
        IIC_SCL(1);
        iic_delay();
        IIC_SCL(0);
        data <<= 1;
    }
    
    IIC_SDA(1);
}

/**
 * @brief   IIC��ȡһ���ֽ�
 * @param   ack: ��Ӧ
 * @arg     0: ����ACK��Ӧ
 * @arg     1: ����NACK��Ӧ
 * @retval  ��ȡ����һ�ֽ�����
 */
uint8_t iic_read_byte(uint8_t ack)
{
    uint8_t i;
    uint8_t receive = 0;
    
    for (i=0; i<8; i++)
    {
        receive <<= 1;
        IIC_SCL(1);
        iic_delay();
        
        if (IIC_SDA_READ)
        {
            receive++;
        }
        
        IIC_SCL(0);
        iic_delay();
    }
    
    if (ack == 0)
    {
        iic_nack();
    }
    else
    {
        iic_ack();
    }
    
    return receive;
}
