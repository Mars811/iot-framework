/**
 * ----------------------------------------------------------------------
 * @file xmodem.c 
 * @author Mars811
 * ----------------------------------------------------------------------
 * ����˼·��
 * 1. ͨ����ʱ���жϷ��Ϳ�ʼ���룬�Ա�MCU�����������ݰ���
 * 2. ͨ���߼��Ͻ���״̬�����ʱ����ơ�
 * 3. ͨ���޸Ĵ��ڽ����жϣ�ʶ�������ֽڡ�      
 *     
 * @date 2025-03-26
 * ----------------------------------------------------------------------
**/
#include "xmodem.h"

// ȫ�ֱ�������
volatile xm_state xm_event = XM_IDLE;
volatile uint8_t xm_u1_new_byte = 0; // ����Ƿ��½��յ��ֽ�
volatile uint8_t rx_byte = 0;
volatile uint8_t rx_data_counter = 0; // ��ǰ�����ݼ�����
uint8_t xm_rx_buffer[XMODEM_DATA_SIZE + 5];  // 133�ֽڻ���������֤�����
uint8_t packet_num = 1;                   // ��������

/**
 * ----------------------------------------------------------------------
 * @brief         ���� CRC
 * @param   data  
 * @param   len   
 * @return        
 * ----------------------------------------------------------------------
 * ���� XMODEM-CRC �㷨����ֵ 0��ʹ�ö���ʽ 0x1021�����ֽڴ�������
 * ----------------------------------------------------------------------
**/
static uint16_t calc_crc(uint8_t *data, uint16_t len)
{
    uint16_t crc = 0;
    for (uint16_t i = 0; i < len; i++) {
        crc ^= ((uint16_t)data[i] << 8);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

void xm_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // ʹ��GPIOA��USART1��USART2������ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // ����USART2 Tx (PA.2) Ϊ�����������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ����USART2 Rx (PA.3) Ϊ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ����USART2
    USART_InitStructure.USART_BaudRate = 921600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    // ʹ��USART2�����ж�
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);

    // ����NVIC����USART2�ж�
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    // ��ʱ��TIM3���ã��˴���������ʱ����Ҫ���ڷ��� C_START��
    // ����ϵͳʱ��Ƶ��Ϊ72MHz
    uint16_t PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1; // ��Ƶ��1MHz����������1us

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_Period = 20000 - 1;   // 20000������
    TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;   // ��Ƶϵ����Ϊ7200
    TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

    // ʹ��TIM3�����ж�
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    // ����TIM3��NVIC
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // ʹ��TIM3
    TIM_Cmd(TIM3, ENABLE);

    // ����ȫ���ж�
    __enable_irq();

    xm_u1_new_byte = 0;
    xm_event = XM_IDLE;
    
    // xm_u1_printf("Xmodem u1 Init OK!\r\n");
    // xm_u2_printf("Xmodem u2 Init OK!\r\n");
}

void xm_u2_send_byte(uint8_t byte)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, byte);
}

void xm_u2_printf(const char* fmt, ...)
{
    char buffer[256];
    va_list args;
    
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    for (char* p = buffer; *p; p++) {
        xm_u2_send_byte(*p);
    }
}


/**
 * ----------------------------------------------------------------------
 * @brief         ����1�жϷ�����
 * ----------------------------------------------------------------------
 * �����ֽں��䱣�浽ȫ�ֱ��� rx_byte������� xm_u1_new_byte = 1
 * ----------------------------------------------------------------------
**/
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
    {
        rx_byte = USART_ReceiveData(USART1); 
        xm_u1_new_byte = 1;

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/**
 * ----------------------------------------------------------------------
 * @brief         TIM3 �жϷ�����
 * ----------------------------------------------------------------------
 * ��ʱ���� C_START ������ڴ��� xmodem ������״̬��
 * ----------------------------------------------------------------------
**/
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        if (xm_event == XM_IDLE || xm_event == XM_START)
        {
            xm_u1_send_byte(C_START);
        }
    }
}

/**
 * ----------------------------------------------------------------------
 * @brief         ���ݰ����Ĵ�����
 * ----------------------------------------------------------------------
**/
void xm_date_dealer(void)
{
    xm_u2_printf("д���%d������\r\n", packet_num);
    // ��������ʼ��ַ��xm_rx_buffer[3]
    

    // 128�ֽ����ݣ���Ҫת��Ϊ uint32_t ����
    // ע�⣺Ҫ�� data_ptr ��������32λ����ģ���������Ҫ��4���ֽ���ϳ�һ��32λ���ݷ��͸�FLASH_ProgramWord
    // ������轫4���ֽںϳ�һ��uint32_t

    if(packet_num == 23){
        for (int i = 84 + 3; i < 128 + 3; i++) { 
            xm_rx_buffer[i] = 0xFF;
        }
    }

    uint8_t *data_ptr = &xm_rx_buffer[3];
    uint32_t temp_buffer[ XMODEM_DATA_SIZE / 4 ];
    uint32_t reversed_buffer[ XMODEM_DATA_SIZE / 4 ];

    // ����ת��
    for (int i = 0; i < (XMODEM_DATA_SIZE / 4); i++) {
        temp_buffer[i] = (data_ptr[i * 4] << 24) | (data_ptr[i * 4 + 1] << 16) |
                         (data_ptr[i * 4 + 2] << 8) | (data_ptr[i * 4 + 3]);
        
        reversed_buffer[i] = ((temp_buffer[i] & 0x000000FF) << 24) | \
        ((temp_buffer[i] & 0x0000FF00) << 8) | \
        ((temp_buffer[i] & 0x00FF0000) >> 8) | \
        ((temp_buffer[i] & 0xFF000000) >> 24);
    }

    // ��¼������ʾ
    for(int i = 0; i < (XMODEM_DATA_SIZE / 4); i++){
        xm_u2_printf("%08X ", reversed_buffer[i]);
        if (((i + 1) % 4) == 0)
            xm_u2_printf("\r\n");
    }

    // ����һ��д�룬д�� 128 �ֽ�����
    xfmc_write_flash(APP_SADDRESS + (packet_num - 1) * 128, reversed_buffer, 128);
}


/**
 * ----------------------------------------------------------------------
 * @brief         xmodem����״̬��
 * ----------------------------------------------------------------------
 * 1. XM_START ״̬���ȴ����ֽ�Ϊ SOH �� EOT
 * 2. XM_RECEIVE ״̬������ʣ�� 132 ���ֽ�(��ͷ�����Ų��롢128 �ֽ����ݡ�2 ��
 *    ��CRC) 
 * 3. ������ɺ�԰��š�У��ͽ��м�⣬��ȷ��������ݴ����������ظ� ACK��
 *    ������ظ� NAK�����������ֽڼ�������������ȷ��
 * 4. ���յ� EOT ���� ACK�������� XM_END ״̬�˳�
 * ----------------------------------------------------------------------
**/
void xm_start(void)
{
    uint16_t crc_calculated;
    uint16_t crc_received;
    
    xm_event = XM_START;  // ��������״̬

    while (1)
    {
        if (xm_u1_new_byte)
        {
            xm_u1_new_byte = 0;
            
            if (xm_event == XM_START)
            {
                if (rx_byte == SOH)
                {
                    // ��⵽���ݰ���ʼ������ SOH �ֽڣ����ü�������0��ʼ
                    rx_data_counter = 0;
                    xm_rx_buffer[rx_data_counter++] = rx_byte;
                    xm_event = XM_RECEIVE;
                }
                else if (rx_byte == EOT)
                {
                    // ����������ظ� ACK ���˳�
                    xm_u1_send_byte(ACK);
                    xm_u2_printf("�������\r\n");
                    xm_event = XM_END;
                    continue;
                }
                // �����ֽ�״̬�º��Դ���
            }
            else if (xm_event == XM_RECEIVE)
            {
                // ÿ�����յ����ֽ����α��浽������
                xm_rx_buffer[rx_data_counter++] = rx_byte;
                
                if (rx_data_counter == (XMODEM_DATA_SIZE + 5)) // 133�ֽ��ѽ���
                {
                    // ��֤���Ų�����ȷ��
                    if ((xm_rx_buffer[1] + xm_rx_buffer[2]) != 0xFF)
                    {
                        xm_u2_printf("����У������յ� %02X, %02X\r\n", xm_rx_buffer[1], xm_rx_buffer[2]);
                        xm_u1_send_byte(NAK);
                        xm_event = XM_START;
                        continue;
                    }
                    
                    // У������Ƿ����Ԥ��
                    if (xm_rx_buffer[1] != packet_num)
                    {
                        xm_u2_printf("���Ŵ����յ� %02X, ���� %02X\r\n", xm_rx_buffer[1], packet_num);
                        xm_u1_send_byte(NAK);
                        xm_event = XM_START;
                        continue;
                    }
                    
                    // ����128�ֽ����ݵ�CRC
                    crc_calculated = calc_crc(&xm_rx_buffer[3], XMODEM_DATA_SIZE);
                    crc_received = ((uint16_t)xm_rx_buffer[131] << 8) | xm_rx_buffer[132];
                    
                    if (crc_calculated != crc_received)
                    {
                        xm_u2_printf("CRC���󣺼���ֵ %04X, �յ� %04X\r\n", crc_calculated, crc_received);
                        xm_u1_send_byte(NAK);
                        xm_event = XM_START;
                        continue;
                    }
                    
                    // ���ݰ����ճɹ����������ݴ�����
                    xm_date_dealer();
                    
                    // �ظ� ACK���ȴ���һ�����ݰ�
                    xm_u1_send_byte(ACK);
                    packet_num++;  // ������һ������
                    xm_event = XM_START;
                }
            }
        }

        if (xm_event == XM_END){
            packet_num = 1;
            break;
        }
    }
}