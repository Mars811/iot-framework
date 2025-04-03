#ifndef XMODEM_H
#define XMODEM_H

#include "stm32f10x.h"
#include "xuart.h"
#include "Delay.h"
#include "xfmc.h"

#define XMODEM_RETRIES     10       // 最大重试次数
#define XMODEM_TIMEOUT_MS  1000    // 超时时间(毫秒)
#define XMODEM_DATA_SIZE   128     // 正文数据包大小
#define SOH 0x01                  // 数据包头标识
#define EOT 0x04                  // 传输结束标识
#define ACK 0x06                  // 确认响应
#define NAK 0x15                  // 否定响应
#define C_START 0x43              // 开始传输命令

// 函数宏定义
#define xm_u1_printf xuart_printf
#define xm_u1_send_byte xuart_send_byte

void xm_init(void);
void xm_u2_send_byte(uint8_t byte);
void xm_u2_printf(const char* fmt, ...);
void xm_date_dealer(void);
void xm_start(void);

typedef enum {
    XM_START,
    XM_RECEIVE,
    XM_END,
    XM_ERROR,
    XM_IDLE,
    XM_TIMEOUT
}xm_state;

#endif
