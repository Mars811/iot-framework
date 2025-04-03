#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TOUCH/touch.h"
#include "./BSP/SRAM/sram.h"
#include "./BSP/TIMER/btim.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "gui_guider.h"
#include "events_init.h"
#include "rtos_app.h"
#include "FreeRTOS.h"
#include "task.h"
#include "tim.h"
#include "usart.h"

volatile lv_ui guider_ui;
extern TaskHandle_t rtos_task_empty_handle;

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    xTaskCreate(rtos_task_empty, "task_empty", 128, NULL, 1, &rtos_task_empty_handle);
    uart_init();
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置时钟，168MHz */
    delay_init(168);                    /* 初始化延时 */
    usart_init(9600);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    sram_init();
    tim2_init();
    
    btim_timx_int_init(1000 - 1, 84);   // 1ms中断初始化，重装载值、预分频值
    lv_init();                          // lvgl初始化
    lv_port_disp_init();                // 显示设备初始化
    lv_port_indev_init();               // 输入设备初始化
    
    setup_ui(&guider_ui);
    events_init(&guider_ui);
    
    printf("初始化成功！\n");

    rtos_init();

    
    while(1){

    }
}
