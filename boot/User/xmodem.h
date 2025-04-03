#ifndef XMODEM_H
#define XMODEM_H

#include "stm32f10x.h"
#include "xuart.h"
#include "Delay.h"
#include "xfmc.h"

#define XMODEM_RETRIES     10       // ������Դ���
#define XMODEM_TIMEOUT_MS  1000    // ��ʱʱ��(����)
#define XMODEM_DATA_SIZE   128     // �������ݰ���С
#define SOH 0x01                  // ���ݰ�ͷ��ʶ
#define EOT 0x04                  // ���������ʶ
#define ACK 0x06                  // ȷ����Ӧ
#define NAK 0x15                  // ����Ӧ
#define C_START 0x43              // ��ʼ��������

// �����궨��
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
