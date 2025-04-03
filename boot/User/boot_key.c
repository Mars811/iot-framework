#include "boot_key.h"

void boot_key_init(void){
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIO端口时钟
    RCC_APB2PeriphClockCmd(KEY_RCC_APB2Periph, ENABLE);

    // 配置GPIO引脚为输入模式
    GPIO_InitStructure.GPIO_Pin = KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = KEY_INPUT_MODE; // 浮空输入，根据实际情况调整
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);
}
