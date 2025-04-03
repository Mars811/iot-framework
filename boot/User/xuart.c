#include "xuart.h"

// // 在文件顶部添加缓冲区定义
// #define RX_BUFFER_SIZE 256
// static uint8_t rx_buffer[RX_BUFFER_SIZE];
// static volatile uint16_t rx_head = 0;
// static volatile uint16_t rx_tail = 0;

void xuart_init(void){
    // 启用时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 配置GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    // TX (PA9) 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // RX (PA10) 浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    // GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置USART参数
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = XUART_BAUDRATE; // 921600
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // 配置NVIC
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 使能USART
    USART_Cmd(USART1, ENABLE);

    USART_ClearFlag(USART1, USART_FLAG_RXNE);
}

static void xuart_send_char(char ch) {
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, (uint16_t)ch);
}

void xuart_printf(const char* fmt, ...) {
    char buffer[256];
    va_list args;
    
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    for(char* p = buffer; *p; p++) {
        xuart_send_char(*p);
    }
}

void xuart_send_byte(uint8_t byte) {
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, byte);
}

// // 取消注释并完善中断服务函数（文件末尾）
// void USART1_IRQHandler(void) {
//     if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
//         /* 数据读取部分 */
//         // 写入环形缓冲区
//         rx_buffer[rx_head] = USART_ReceiveData(USART1);
//         rx_head = (rx_head + 1) % RX_BUFFER_SIZE;
        
//         // 检测缓冲区溢出
//         if(rx_head == rx_tail){
//             rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
//         }

//         /* 数据处理部分 */
        
//         USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//     }
// }

/**
 * ----------------------------------------------------------------------
 * @brief         串口读取一个字节
 * @param   byte  字节数据
 * @return        是否有内容
 * ----------------------------------------------------------------------
 * 使用例如下,想要获取数据时先使用if进行判断,如果有数据就可以正常读取。
 * uint8_t data;
 * if(xuart_read_byte(&data)){
 *     xuart_printf("Received: 0x%02X\r\n", data);
 * }
 * ----------------------------------------------------------------------
**/
// xuart_state xuart_read_byte(uint8_t* byte) {
//     if(rx_head == rx_tail) return XUART_NO_DATA; // 缓冲区空
    
//     __disable_irq();
//     *byte = rx_buffer[rx_tail];
//     rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
//     __enable_irq();
//     // xm_debug_uart_send_byte(*byte);
    
//     return XUART_NEW_DATA;
// }
