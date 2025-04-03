#ifndef XUART_H
#define XUART_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "xmodem.h"

#define XUART_BAUDRATE 115200

void xuart_init(void);
void xuart_printf(const char* fmt,...);
void xuart_send_byte(uint8_t byte);
// void xuart_receive_byte(uint8_t* byte);
uint8_t xuart_read_byte(uint8_t* byte);

typedef enum {
    XUART_NEW_DATA,
    XUART_USED_DATA,
    XUART_NO_DATA
}xuart_state;

#endif
