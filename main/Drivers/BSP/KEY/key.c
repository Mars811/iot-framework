#include "./BSP/KEY/key.h"
#include "./SYSTEM/delay/delay.h"

void key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    /* 使能GPIO端口时钟 */
    WKUP_GPIO_CLK_ENABLE();
    KEY0_GPIO_CLK_ENABLE();
    
    /* 配置WKUP控制引脚 */
    gpio_init_struct.Pin = WKUP_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLDOWN;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(WKUP_GPIO_PORT, &gpio_init_struct);
    
    /* 配置KEY0控制引脚 */
    gpio_init_struct.Pin = KEY0_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(KEY0_GPIO_PORT, &gpio_init_struct);

    /* 配置KEY1控制引脚 */
    gpio_init_struct.Pin = KEY1_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(KEY1_GPIO_PORT, &gpio_init_struct);
}

uint8_t key_get_num(void){
    uint8_t key_num = NONE_PRES;

    if(HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN) == GPIO_PIN_RESET){
        delay_ms(20);
        if(HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN) == GPIO_PIN_RESET){
            while(!HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN));
            key_num = KEY0_PRES;
        }
    }else if(HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == GPIO_PIN_RESET){
        delay_ms(20);
        if(HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == GPIO_PIN_RESET){
            while(!HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN));
            key_num = KEY1_PRES;
        }
    }else{
        key_num = NONE_PRES;
    }
    return key_num;
}
