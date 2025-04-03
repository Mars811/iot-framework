#include "rtos_app.h"

uint8_t system_status = IDLE;
SemaphoreHandle_t system_status_mutex = NULL;

/* ------------------------------------- 任务列表 ------------------------------------------ */
/**
 * ----------------------------------------------------------------------
 * @brief  空任务，用于初始化系统
 * ----------------------------------------------------------------------
**/
TaskHandle_t rtos_task_empty_handle = NULL;
void rtos_task_empty(void){
    while(1){
        vTaskDelay(1000);
    }
}

void rtos_task_lvgl(void){
    while(1){
        lv_task_handler();
        vTaskDelay(5);
    } 
}

void rtos_task_led_flash(void){
    while(1){
        vTaskDelay(500);
        LED2_TOGGLE();
    } 
}


/**
 * ----------------------------------------------------------------------
 * @brief  出入库事件的处理
 * 
 * az，似乎得保护一下变量，不然会出问题
 * ----------------------------------------------------------------------
**/
void rtos_task_system_deal(void){
    for(;;){
        if(xSemaphoreTake(system_status_mutex, portMAX_DELAY) == pdTRUE) {
            // 入库事件
            if(system_status == INPUT){
            
                system_status = IDLE;
            }else if(system_status == OUTPUT){
                // 出库事件

                system_status = IDLE; 
            }
            xSemaphoreGive(system_status_mutex);
    }}
    vTaskDelay (100);
}

/**
 * ----------------------------------------------------------------------
 * @brief             KEYO 按键处理
 * @param  parameter  
 * ----------------------------------------------------------------------
**/
static void rtos_task_key0 (void* parameter)
{
  typedef enum
  {
    IDLE_STATE,   //空闲
    PRESS_DETECTED_STATE,  //按键按下
    RELEASE_DETECTED_STATE, //按键释放
    SHORT_PRESS_STATE,     //短按
    LONG_PRESS_STATE,     //长按
    DOUBLE_PRESS_STATE,    //双击
		LONG_PRESS_STATE_END //长按结束状态
  } ButtonState;

  uint16_t SHORT_CLICK_THRESHOLD = 200;  // 这个是第一次松开时间和第二次按下时间的判断时长
  uint16_t LONG_PRESS_THRESHOLD = 1000;   // 定义长按的时间阈值（以FreeRTOS时基为单位）
  uint16_t PRESS_Time = 100;             // 判断毛刺时长
  uint16_t BUTTON_ERROR_Time = 1500;   // 按键长久状态卡死阈值

  ButtonState buttonState = IDLE_STATE;  //按键状态
  TickType_t pressStartTime = 0;        //记录按下时间
  TickType_t lastReleaseTime = 0;       //记录上一次释放时间
  TickType_t ReleaseTime = 0;

  while (1)
  {
    int keyStatus = HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN);  //检测按键状态
    TickType_t currentTime = xTaskGetTickCount();
    switch (buttonState)
    {
    case IDLE_STATE:
        if (keyStatus == 0)   //如果按键按下
        {
            buttonState = PRESS_DETECTED_STATE; //切换到按键按下状态
            pressStartTime = currentTime;       //记录按下时间
        }
        break;

    case PRESS_DETECTED_STATE:    //按键 按下状态
        if (keyStatus == 1)       //检测按键松开时间
        {
            ReleaseTime = currentTime;  //记录按键放松时间
            buttonState = RELEASE_DETECTED_STATE;    //如果按下后释放则进入这里
        }
        else if (currentTime - pressStartTime > LONG_PRESS_THRESHOLD) //判断按键长按的时长如果符合则跳转到这里。
        {
            buttonState = LONG_PRESS_STATE;
        }
                else if(currentTime - pressStartTime>BUTTON_ERROR_Time) //防止按键卡死
                {
                buttonState = IDLE_STATE;
                }
        break;
    case LONG_PRESS_STATE_END:    //长按结束后的状态跳转这里等待按键松开 防止一直处于长按状态   /
        if (keyStatus == 1)
        {
            buttonState = IDLE_STATE;
        }
        break;

    case RELEASE_DETECTED_STATE:    //按键释放过后的状态
        if ( (keyStatus == 0) && (currentTime - ReleaseTime < SHORT_CLICK_THRESHOLD))        //如果按键再次按下并且第二次时长是在500ms以内按下的
        {
            buttonState = DOUBLE_PRESS_STATE;
        }
        else if ( (ReleaseTime - pressStartTime > PRESS_Time) && (currentTime - ReleaseTime > SHORT_CLICK_THRESHOLD)) //判断按键按下时长防止毛刺 在判断当前是否在双击范围内
        {
            buttonState = SHORT_PRESS_STATE;
        }
        else if (currentTime - ReleaseTime > BUTTON_ERROR_Time) //按键出现无法判断情况回到初态
        {
            buttonState = IDLE_STATE;
        }
        break;

    case SHORT_PRESS_STATE:
        if(xSemaphoreTake(system_status_mutex, portMAX_DELAY) == pdTRUE) {
            printf ("k0单击，转为出库状态\r\n");
            system_status = OUTPUT;
            xSemaphoreGive(system_status_mutex);
        }else{
            printf ("k0单击，转为出库状态失败\r\n");
        }
        buttonState = LONG_PRESS_STATE_END;
        break;

    case LONG_PRESS_STATE:
        printf ("k0长按\r\n");
        buttonState = LONG_PRESS_STATE_END;
        break;

    case DOUBLE_PRESS_STATE:
        buttonState = LONG_PRESS_STATE_END;
        printf ("k0双击\r\n");
        break;
    }
    vTaskDelay (10);
  }
}

/**
 * ----------------------------------------------------------------------
 * @brief             KEY1 按键处理
 * @param  parameter  
 * ----------------------------------------------------------------------
**/
static void rtos_task_key1 (void* parameter)
{
  typedef enum
  {
    IDLE_STATE,   //空闲
    PRESS_DETECTED_STATE,  //按键按下
    RELEASE_DETECTED_STATE, //按键释放
    SHORT_PRESS_STATE,     //短按
    LONG_PRESS_STATE,     //长按
    DOUBLE_PRESS_STATE,    //双击
		LONG_PRESS_STATE_END //长按结束状态
  } ButtonState;

  uint16_t SHORT_CLICK_THRESHOLD = 200;  // 这个是第一次松开时间和第二次按下时间的判断时长
  uint16_t LONG_PRESS_THRESHOLD = 1000;   // 定义长按的时间阈值（以FreeRTOS时基为单位）
  uint16_t PRESS_Time = 100;             // 判断毛刺时长
  uint16_t BUTTON_ERROR_Time = 1500;   // 按键长久状态卡死阈值

  ButtonState buttonState = IDLE_STATE;  //按键状态
  TickType_t pressStartTime = 0;        //记录按下时间
  TickType_t lastReleaseTime = 0;       //记录上一次释放时间
  TickType_t ReleaseTime = 0;

  while (1)
  {
    int keyStatus = HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN);  //检测按键状态
    TickType_t currentTime = xTaskGetTickCount();
    switch (buttonState)
    {
    case IDLE_STATE:
        if (keyStatus == 0)   //如果按键按下
        {
            buttonState = PRESS_DETECTED_STATE; //切换到按键按下状态
            pressStartTime = currentTime;       //记录按下时间
        }
        break;

    case PRESS_DETECTED_STATE:    //按键 按下状态
        if (keyStatus == 1)       //检测按键松开时间
        {
            ReleaseTime = currentTime;  //记录按键放松时间
            buttonState = RELEASE_DETECTED_STATE;    //如果按下后释放则进入这里
        }
        else if (currentTime - pressStartTime > LONG_PRESS_THRESHOLD) //判断按键长按的时长如果符合则跳转到这里。
        {
            buttonState = LONG_PRESS_STATE;
        }
        else if(currentTime - pressStartTime>BUTTON_ERROR_Time) //防止按键卡死
        {
            buttonState = IDLE_STATE;
        }
        break;
    case LONG_PRESS_STATE_END:    //长按结束后的状态跳转这里等待按键松开 防止一直处于长按状态   /
        if (keyStatus == 1)
        {
            buttonState = IDLE_STATE;
        }
        break;

    case RELEASE_DETECTED_STATE:    //按键释放过后的状态
        if ( (keyStatus == 0) && (currentTime - ReleaseTime < SHORT_CLICK_THRESHOLD))        //如果按键再次按下并且第二次时长是在500ms以内按下的
        {
            buttonState = DOUBLE_PRESS_STATE;
        }
        else if ( (ReleaseTime - pressStartTime > PRESS_Time) && (currentTime - ReleaseTime > SHORT_CLICK_THRESHOLD)) //判断按键按下时长防止毛刺 在判断当前是否在双击范围内
        {
            buttonState = SHORT_PRESS_STATE;
        }
        else if (currentTime - ReleaseTime > BUTTON_ERROR_Time) //按键出现无法判断情况回到初态
        {
            buttonState = IDLE_STATE;
        }
        break;

    case SHORT_PRESS_STATE:
        if(xSemaphoreTake(system_status_mutex, portMAX_DELAY) == pdTRUE) {
            printf ("k1单击，转为入库状态\r\n");
            system_status = INPUT;
            xSemaphoreGive(system_status_mutex);
        }else{
            printf ("k1单击，状态获取失败\r\n");
        }
        buttonState = LONG_PRESS_STATE_END;
        break;

    case LONG_PRESS_STATE:
        printf ("k1长按\r\n");
        buttonState = LONG_PRESS_STATE_END;
        break;

    case DOUBLE_PRESS_STATE:
        buttonState = LONG_PRESS_STATE_END;
        printf ("k1双击\r\n");
        break;
    }
    vTaskDelay (10);
  }
}



/* ------------------------------------- 系统加载 ------------------------------------------ */
void rtos_init(void){
    xTaskCreate(rtos_task_lvgl, "task_lvgl", 1024, NULL, 1, NULL);
    xTaskCreate(rtos_task_key0, "task_key0", 512, NULL, 1, NULL);
    xTaskCreate(rtos_task_key1, "task_key1", 512, NULL, 1, NULL);
    xTaskCreate(rtos_task_system_deal, "task_system_deal", 128, NULL, 1, NULL);
    xTaskCreate(rtos_task_led_flash, "task_led_flash", 128, NULL, 1, NULL);
    system_status_mutex = xSemaphoreCreateMutex();

    if (rtos_task_empty_handle != NULL) {
        vTaskDelete(rtos_task_empty_handle);
    }
    vTaskStartScheduler();
}
