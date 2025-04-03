#ifndef __RTOS_APP_H__
#define __RTOS_APP_H__
#include "FreeRTOS.h"
#include "task.h"
#include "lvgl.h"
#include "lvgl_app.h"
#include "./BSP/KEY/key.h"
#include "semphr.h"
#include "./BSP/LED/led.h"
#include "info_processor.h"

// ϵͳ״̬����/��⣩
#define IDLE        0
#define INPUT       1
#define OUTPUT      2

// ������
void rtos_init(void);
void rtos_task_empty(void);

#endif
