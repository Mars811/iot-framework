/**
 * ----------------------------------------------------------------------
 * @file xmodem.c 
 * @author Mars811
 * ----------------------------------------------------------------------
 * 核心思路：
 * 1. 通过定时器中断发送开始代码，以便MCU立即捕获数据包。
 * 2. 通过逻辑严谨的状态机完成时序控制。
 * 3. 通过修改串口接收中断，识别最新字节。      
 *     
 * @date 2025-03-26
 * ----------------------------------------------------------------------
**/
#include "xmodem.h"

// 全局变量定义
volatile xm_state xm_event = XM_IDLE;
volatile uint8_t xm_u1_new_byte = 0; // 标记是否新接收到字节
volatile uint8_t rx_byte = 0;
volatile uint8_t rx_data_counter = 0; // 当前包数据计数器
uint8_t xm_rx_buffer[XMODEM_DATA_SIZE + 5];  // 133字节缓冲区，保证不溢出
uint8_t packet_num = 1;                   // 期望包号

/**
 * ----------------------------------------------------------------------
 * @brief         计算 CRC
 * @param   data  
 * @param   len   
 * @return        
 * ----------------------------------------------------------------------
 * 采用 XMODEM-CRC 算法：初值 0，使用多项式 0x1021，逐字节处理数据
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

    // 使能GPIOA及USART1、USART2的外设时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 配置USART2 Tx (PA.2) 为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART2 Rx (PA.3) 为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART2
    USART_InitStructure.USART_BaudRate = 921600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    // 使能USART2接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);

    // 配置NVIC用于USART2中断
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    // 定时器TIM3配置（此处保留，定时器主要用于发送 C_START）
    // 假设系统时钟频率为72MHz
    uint16_t PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1; // 分频到1MHz，计数周期1us

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_Period = 20000 - 1;   // 20000个周期
    TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;   // 分频系数设为7200
    TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

    // 使能TIM3更新中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    // 配置TIM3的NVIC
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 使能TIM3
    TIM_Cmd(TIM3, ENABLE);

    // 开启全局中断
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
 * @brief         串口1中断服务函数
 * ----------------------------------------------------------------------
 * 接收字节后将其保存到全局变量 rx_byte，并标记 xm_u1_new_byte = 1
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
 * @brief         TIM3 中断服务函数
 * ----------------------------------------------------------------------
 * 定时发送 C_START 命令，用于触发 xmodem 主接收状态机
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
 * @brief         数据包正文处理函数
 * ----------------------------------------------------------------------
**/
void xm_date_dealer(void)
{
    xm_u2_printf("写入第%d包数据\r\n", packet_num);
    // 数据区起始地址：xm_rx_buffer[3]
    

    // 128字节数据，需要转换为 uint32_t 数组
    // 注意：要求 data_ptr 的数据是32位对齐的，或者你需要将4个字节组合成一个32位数据发送给FLASH_ProgramWord
    // 下面假设将4个字节合成一个uint32_t

    if(packet_num == 23){
        for (int i = 84 + 3; i < 128 + 3; i++) { 
            xm_rx_buffer[i] = 0xFF;
        }
    }

    uint8_t *data_ptr = &xm_rx_buffer[3];
    uint32_t temp_buffer[ XMODEM_DATA_SIZE / 4 ];
    uint32_t reversed_buffer[ XMODEM_DATA_SIZE / 4 ];

    // 数据转换
    for (int i = 0; i < (XMODEM_DATA_SIZE / 4); i++) {
        temp_buffer[i] = (data_ptr[i * 4] << 24) | (data_ptr[i * 4 + 1] << 16) |
                         (data_ptr[i * 4 + 2] << 8) | (data_ptr[i * 4 + 3]);
        
        reversed_buffer[i] = ((temp_buffer[i] & 0x000000FF) << 24) | \
        ((temp_buffer[i] & 0x0000FF00) << 8) | \
        ((temp_buffer[i] & 0x00FF0000) >> 8) | \
        ((temp_buffer[i] & 0xFF000000) >> 24);
    }

    // 烧录内容提示
    for(int i = 0; i < (XMODEM_DATA_SIZE / 4); i++){
        xm_u2_printf("%08X ", reversed_buffer[i]);
        if (((i + 1) % 4) == 0)
            xm_u2_printf("\r\n");
    }

    // 调用一次写入，写入 128 字节数据
    xfmc_write_flash(APP_SADDRESS + (packet_num - 1) * 128, reversed_buffer, 128);
}


/**
 * ----------------------------------------------------------------------
 * @brief         xmodem接收状态机
 * ----------------------------------------------------------------------
 * 1. XM_START 状态：等待首字节为 SOH 或 EOT
 * 2. XM_RECEIVE 状态：接收剩余 132 个字节(包头、包号补码、128 字节数据、2 字
 *    节CRC) 
 * 3. 接收完成后对包号、校验和进行检测，正确则调用数据处理函数，并回复 ACK，
 *    错误则回复 NAK。（包数、字节计数器均处理正确）
 * 4. 当收到 EOT 后发送 ACK，并进入 XM_END 状态退出
 * ----------------------------------------------------------------------
**/
void xm_start(void)
{
    uint16_t crc_calculated;
    uint16_t crc_received;
    
    xm_event = XM_START;  // 进入启动状态

    while (1)
    {
        if (xm_u1_new_byte)
        {
            xm_u1_new_byte = 0;
            
            if (xm_event == XM_START)
            {
                if (rx_byte == SOH)
                {
                    // 检测到数据包开始：保存 SOH 字节，并置计数器从0开始
                    rx_data_counter = 0;
                    xm_rx_buffer[rx_data_counter++] = rx_byte;
                    xm_event = XM_RECEIVE;
                }
                else if (rx_byte == EOT)
                {
                    // 传输结束：回复 ACK 并退出
                    xm_u1_send_byte(ACK);
                    xm_u2_printf("传输结束\r\n");
                    xm_event = XM_END;
                    continue;
                }
                // 其他字节状态下忽略处理
            }
            else if (xm_event == XM_RECEIVE)
            {
                // 每个接收到的字节依次保存到缓冲区
                xm_rx_buffer[rx_data_counter++] = rx_byte;
                
                if (rx_data_counter == (XMODEM_DATA_SIZE + 5)) // 133字节已接齐
                {
                    // 验证包号补码正确性
                    if ((xm_rx_buffer[1] + xm_rx_buffer[2]) != 0xFF)
                    {
                        xm_u2_printf("包号校验错误：收到 %02X, %02X\r\n", xm_rx_buffer[1], xm_rx_buffer[2]);
                        xm_u1_send_byte(NAK);
                        xm_event = XM_START;
                        continue;
                    }
                    
                    // 校验包号是否符合预期
                    if (xm_rx_buffer[1] != packet_num)
                    {
                        xm_u2_printf("包号错误：收到 %02X, 期望 %02X\r\n", xm_rx_buffer[1], packet_num);
                        xm_u1_send_byte(NAK);
                        xm_event = XM_START;
                        continue;
                    }
                    
                    // 计算128字节数据的CRC
                    crc_calculated = calc_crc(&xm_rx_buffer[3], XMODEM_DATA_SIZE);
                    crc_received = ((uint16_t)xm_rx_buffer[131] << 8) | xm_rx_buffer[132];
                    
                    if (crc_calculated != crc_received)
                    {
                        xm_u2_printf("CRC错误：计算值 %04X, 收到 %04X\r\n", crc_calculated, crc_received);
                        xm_u1_send_byte(NAK);
                        xm_event = XM_START;
                        continue;
                    }
                    
                    // 数据包接收成功，调用数据处理函数
                    xm_date_dealer();
                    
                    // 回复 ACK，等待下一个数据包
                    xm_u1_send_byte(ACK);
                    packet_num++;  // 期望下一个包号
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