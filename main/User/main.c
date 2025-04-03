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
    HAL_Init();                         /* ��ʼ��HAL�� */
    xTaskCreate(rtos_task_empty, "task_empty", 128, NULL, 1, &rtos_task_empty_handle);
    uart_init();
    sys_stm32_clock_init(336, 8, 2, 7); /* ����ʱ�ӣ�168MHz */
    delay_init(168);                    /* ��ʼ����ʱ */
    usart_init(9600);                 /* ��ʼ������ */
    led_init();                         /* ��ʼ��LED */
    key_init();                         /* ��ʼ������ */
    sram_init();
    tim2_init();
    
    btim_timx_int_init(1000 - 1, 84);   // 1ms�жϳ�ʼ������װ��ֵ��Ԥ��Ƶֵ
    lv_init();                          // lvgl��ʼ��
    lv_port_disp_init();                // ��ʾ�豸��ʼ��
    lv_port_indev_init();               // �����豸��ʼ��
    
    setup_ui(&guider_ui);
    events_init(&guider_ui);
    
    printf("��ʼ���ɹ���\n");

    rtos_init();

    
    while(1){

    }
}
