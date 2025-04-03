#ifndef BOOT_KEY_H
#define BOOT_KEY_H

#include "stm32f10x.h"
#include "Delay.h"
#include "xuart.h"

#define KEY_PORT GPIOC
#define KEY_PIN GPIO_Pin_15
#define KEY_RCC_APB2Periph RCC_APB2Periph_GPIOC
#define KEY_INPUT_MODE GPIO_Mode_IPD    // 需要根据电平调整
#define KEY_ENABLE 1                    // 按键按下的电平

void boot_key_init(void);

#endif