#include "boot_key.h"

void boot_key_init(void){
    GPIO_InitTypeDef GPIO_InitStructure;

    // ʹ��GPIO�˿�ʱ��
    RCC_APB2PeriphClockCmd(KEY_RCC_APB2Periph, ENABLE);

    // ����GPIO����Ϊ����ģʽ
    GPIO_InitStructure.GPIO_Pin = KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = KEY_INPUT_MODE; // �������룬����ʵ���������
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);
}
