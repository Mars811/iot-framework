#include "rtos_app.h"

uint8_t system_status = IDLE;
SemaphoreHandle_t system_status_mutex = NULL;

/* ------------------------------------- �����б� ------------------------------------------ */
/**
 * ----------------------------------------------------------------------
 * @brief  ���������ڳ�ʼ��ϵͳ
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
 * @brief  ������¼��Ĵ���
 * 
 * az���ƺ��ñ���һ�±�������Ȼ�������
 * ----------------------------------------------------------------------
**/
void rtos_task_system_deal(void){
    for(;;){
        if(xSemaphoreTake(system_status_mutex, portMAX_DELAY) == pdTRUE) {
            // ����¼�
            if(system_status == INPUT){
            
                system_status = IDLE;
            }else if(system_status == OUTPUT){
                // �����¼�

                system_status = IDLE; 
            }
            xSemaphoreGive(system_status_mutex);
    }}
    vTaskDelay (100);
}

/**
 * ----------------------------------------------------------------------
 * @brief             KEYO ��������
 * @param  parameter  
 * ----------------------------------------------------------------------
**/
static void rtos_task_key0 (void* parameter)
{
  typedef enum
  {
    IDLE_STATE,   //����
    PRESS_DETECTED_STATE,  //��������
    RELEASE_DETECTED_STATE, //�����ͷ�
    SHORT_PRESS_STATE,     //�̰�
    LONG_PRESS_STATE,     //����
    DOUBLE_PRESS_STATE,    //˫��
		LONG_PRESS_STATE_END //��������״̬
  } ButtonState;

  uint16_t SHORT_CLICK_THRESHOLD = 200;  // ����ǵ�һ���ɿ�ʱ��͵ڶ��ΰ���ʱ����ж�ʱ��
  uint16_t LONG_PRESS_THRESHOLD = 1000;   // ���峤����ʱ����ֵ����FreeRTOSʱ��Ϊ��λ��
  uint16_t PRESS_Time = 100;             // �ж�ë��ʱ��
  uint16_t BUTTON_ERROR_Time = 1500;   // ��������״̬������ֵ

  ButtonState buttonState = IDLE_STATE;  //����״̬
  TickType_t pressStartTime = 0;        //��¼����ʱ��
  TickType_t lastReleaseTime = 0;       //��¼��һ���ͷ�ʱ��
  TickType_t ReleaseTime = 0;

  while (1)
  {
    int keyStatus = HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN);  //��ⰴ��״̬
    TickType_t currentTime = xTaskGetTickCount();
    switch (buttonState)
    {
    case IDLE_STATE:
        if (keyStatus == 0)   //�����������
        {
            buttonState = PRESS_DETECTED_STATE; //�л�����������״̬
            pressStartTime = currentTime;       //��¼����ʱ��
        }
        break;

    case PRESS_DETECTED_STATE:    //���� ����״̬
        if (keyStatus == 1)       //��ⰴ���ɿ�ʱ��
        {
            ReleaseTime = currentTime;  //��¼��������ʱ��
            buttonState = RELEASE_DETECTED_STATE;    //������º��ͷ����������
        }
        else if (currentTime - pressStartTime > LONG_PRESS_THRESHOLD) //�жϰ���������ʱ�������������ת�����
        {
            buttonState = LONG_PRESS_STATE;
        }
                else if(currentTime - pressStartTime>BUTTON_ERROR_Time) //��ֹ��������
                {
                buttonState = IDLE_STATE;
                }
        break;
    case LONG_PRESS_STATE_END:    //�����������״̬��ת����ȴ������ɿ� ��ֹһֱ���ڳ���״̬   /
        if (keyStatus == 1)
        {
            buttonState = IDLE_STATE;
        }
        break;

    case RELEASE_DETECTED_STATE:    //�����ͷŹ����״̬
        if ( (keyStatus == 0) && (currentTime - ReleaseTime < SHORT_CLICK_THRESHOLD))        //��������ٴΰ��²��ҵڶ���ʱ������500ms���ڰ��µ�
        {
            buttonState = DOUBLE_PRESS_STATE;
        }
        else if ( (ReleaseTime - pressStartTime > PRESS_Time) && (currentTime - ReleaseTime > SHORT_CLICK_THRESHOLD)) //�жϰ�������ʱ����ֹë�� ���жϵ�ǰ�Ƿ���˫����Χ��
        {
            buttonState = SHORT_PRESS_STATE;
        }
        else if (currentTime - ReleaseTime > BUTTON_ERROR_Time) //���������޷��ж�����ص���̬
        {
            buttonState = IDLE_STATE;
        }
        break;

    case SHORT_PRESS_STATE:
        if(xSemaphoreTake(system_status_mutex, portMAX_DELAY) == pdTRUE) {
            printf ("k0������תΪ����״̬\r\n");
            system_status = OUTPUT;
            xSemaphoreGive(system_status_mutex);
        }else{
            printf ("k0������תΪ����״̬ʧ��\r\n");
        }
        buttonState = LONG_PRESS_STATE_END;
        break;

    case LONG_PRESS_STATE:
        printf ("k0����\r\n");
        buttonState = LONG_PRESS_STATE_END;
        break;

    case DOUBLE_PRESS_STATE:
        buttonState = LONG_PRESS_STATE_END;
        printf ("k0˫��\r\n");
        break;
    }
    vTaskDelay (10);
  }
}

/**
 * ----------------------------------------------------------------------
 * @brief             KEY1 ��������
 * @param  parameter  
 * ----------------------------------------------------------------------
**/
static void rtos_task_key1 (void* parameter)
{
  typedef enum
  {
    IDLE_STATE,   //����
    PRESS_DETECTED_STATE,  //��������
    RELEASE_DETECTED_STATE, //�����ͷ�
    SHORT_PRESS_STATE,     //�̰�
    LONG_PRESS_STATE,     //����
    DOUBLE_PRESS_STATE,    //˫��
		LONG_PRESS_STATE_END //��������״̬
  } ButtonState;

  uint16_t SHORT_CLICK_THRESHOLD = 200;  // ����ǵ�һ���ɿ�ʱ��͵ڶ��ΰ���ʱ����ж�ʱ��
  uint16_t LONG_PRESS_THRESHOLD = 1000;   // ���峤����ʱ����ֵ����FreeRTOSʱ��Ϊ��λ��
  uint16_t PRESS_Time = 100;             // �ж�ë��ʱ��
  uint16_t BUTTON_ERROR_Time = 1500;   // ��������״̬������ֵ

  ButtonState buttonState = IDLE_STATE;  //����״̬
  TickType_t pressStartTime = 0;        //��¼����ʱ��
  TickType_t lastReleaseTime = 0;       //��¼��һ���ͷ�ʱ��
  TickType_t ReleaseTime = 0;

  while (1)
  {
    int keyStatus = HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN);  //��ⰴ��״̬
    TickType_t currentTime = xTaskGetTickCount();
    switch (buttonState)
    {
    case IDLE_STATE:
        if (keyStatus == 0)   //�����������
        {
            buttonState = PRESS_DETECTED_STATE; //�л�����������״̬
            pressStartTime = currentTime;       //��¼����ʱ��
        }
        break;

    case PRESS_DETECTED_STATE:    //���� ����״̬
        if (keyStatus == 1)       //��ⰴ���ɿ�ʱ��
        {
            ReleaseTime = currentTime;  //��¼��������ʱ��
            buttonState = RELEASE_DETECTED_STATE;    //������º��ͷ����������
        }
        else if (currentTime - pressStartTime > LONG_PRESS_THRESHOLD) //�жϰ���������ʱ�������������ת�����
        {
            buttonState = LONG_PRESS_STATE;
        }
        else if(currentTime - pressStartTime>BUTTON_ERROR_Time) //��ֹ��������
        {
            buttonState = IDLE_STATE;
        }
        break;
    case LONG_PRESS_STATE_END:    //�����������״̬��ת����ȴ������ɿ� ��ֹһֱ���ڳ���״̬   /
        if (keyStatus == 1)
        {
            buttonState = IDLE_STATE;
        }
        break;

    case RELEASE_DETECTED_STATE:    //�����ͷŹ����״̬
        if ( (keyStatus == 0) && (currentTime - ReleaseTime < SHORT_CLICK_THRESHOLD))        //��������ٴΰ��²��ҵڶ���ʱ������500ms���ڰ��µ�
        {
            buttonState = DOUBLE_PRESS_STATE;
        }
        else if ( (ReleaseTime - pressStartTime > PRESS_Time) && (currentTime - ReleaseTime > SHORT_CLICK_THRESHOLD)) //�жϰ�������ʱ����ֹë�� ���жϵ�ǰ�Ƿ���˫����Χ��
        {
            buttonState = SHORT_PRESS_STATE;
        }
        else if (currentTime - ReleaseTime > BUTTON_ERROR_Time) //���������޷��ж�����ص���̬
        {
            buttonState = IDLE_STATE;
        }
        break;

    case SHORT_PRESS_STATE:
        if(xSemaphoreTake(system_status_mutex, portMAX_DELAY) == pdTRUE) {
            printf ("k1������תΪ���״̬\r\n");
            system_status = INPUT;
            xSemaphoreGive(system_status_mutex);
        }else{
            printf ("k1������״̬��ȡʧ��\r\n");
        }
        buttonState = LONG_PRESS_STATE_END;
        break;

    case LONG_PRESS_STATE:
        printf ("k1����\r\n");
        buttonState = LONG_PRESS_STATE_END;
        break;

    case DOUBLE_PRESS_STATE:
        buttonState = LONG_PRESS_STATE_END;
        printf ("k1˫��\r\n");
        break;
    }
    vTaskDelay (10);
  }
}



/* ------------------------------------- ϵͳ���� ------------------------------------------ */
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
