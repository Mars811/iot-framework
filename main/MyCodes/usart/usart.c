/**
 * ----------------------------------------------------------------------
 * @file usart.c  
 * @author Mars811 + ���ҵ���
 * ----------------------------------------------------------------------
 * ��������������򣨲��ʺ���ֲ�������ݰ����������������Լ�WIFIģ��Ĵ���Ӧ����
 * ����          
 * ----------------------------------------------------------------------
 * @date 2025-04-02
 * ----------------------------------------------------------------------
**/
#include "usart.h"

uint8_t USART1_RX_BUF[USART1_REC_LEN];//���ջ���,���USART_REC_LEN���ֽ�.
uint16_t USART1_RX_STA=0;//����״̬���//bit15��������ɱ�־��bit14�����յ�0x0d��bit13~0�����յ�����Ч�ֽ���Ŀ
uint8_t USART1_NewData;//��ǰ�����жϽ��յ�1���ֽ����ݵĻ���

uint8_t USART2_RX_BUF[USART2_REC_LEN];//���ջ���,���USART_REC_LEN���ֽ�.
uint16_t USART2_RX_STA=0;//����״̬���//bit15��������ɱ�־��bit14�����յ�0x0d��bit13~0�����յ�����Ч�ֽ���Ŀ
uint8_t USART2_NewData;//��ǰ�����жϽ��յ�1���ֽ����ݵĻ���
uint8_t RS485orBT;//��RS485orBT��־λΪ1ʱ��RS485ģʽ��Ϊ0ʱ������ģʽ

uint8_t USART3_RX_BUF[USART3_REC_LEN];//���ջ���,���USART_REC_LEN���ֽ�.
uint16_t USART3_RX_STA=0;//����״̬���//bit15��������ɱ�־��bit14�����յ�0x0d��bit13~0�����յ�����Ч�ֽ���Ŀ
uint8_t USART3_NewData;//��ǰ�����жϽ��յ�1���ֽ����ݵĻ���

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

void  HAL_UART_RxCpltCallback(UART_HandleTypeDef  *huart)//�����жϻص�����
{

	if(huart ==&huart1)//�ж��ж���Դ������1��USBת���ڣ�
    {
       printf("%c",USART1_NewData); //���յ��������� a���ű��� ���ͻص���
       if((USART1_RX_STA&0x8000)==0){//����δ���
           if(USART1_RX_STA&0x4000){//���յ���0x0d
               if(USART1_NewData!=0x0a)USART1_RX_STA=0;//���մ���,���¿�ʼ
               else USART1_RX_STA|=0x8000;   //���������
           }else{ //��û�յ�0X0D
               if(USART1_NewData==0x0d)USART1_RX_STA|=0x4000;
               else{
                  USART1_RX_BUF[USART1_RX_STA&0X3FFF]=USART1_NewData; //���յ������ݷ�������
                  USART1_RX_STA++;  //���ݳ��ȼ�����1
                  if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//�������ݴ���,���¿�ʼ����
               }
           }
       }
       HAL_UART_Receive_IT(&huart1,(uint8_t *)&USART1_NewData,1); //�ٿ��������ж�
    }
    if(huart ==&huart2)//�ж��ж���Դ��RS485/������
    {
    	if(RS485orBT){//�жϵ�RS485orBT��־λΪ1ʱ��RS485ģʽ��Ϊ0ʱ������ģʽ
		   USART2_RX_BUF[0]=USART2_NewData;//�յ����ݷ��뻺�����飨ֻ�õ�1�����ݴ��������[0]��
		   USART2_RX_STA++;//���ݽ��ձ�־λ��1
		   HAL_UART_Receive_IT(&huart2,(uint8_t *)&USART2_NewData, 1); //�ٿ��������ж�
    	}else{
		   if((USART2_RX_STA&0x8000)==0){//����δ��ɣ���USART2_RX_STA���λ1λ�涨�ǽ�����ɱ�־λ��
			   if(USART2_NewData==0x5A)//���յ�0x5A��ʾ���յ����������ֻ�APP���������������ظ�������0x5AΪ��������
			   {
				   USART2_RX_STA|=0x8000;//�յ�0x0A���������
			   }
			   else{ //��û���յ�0x0A����������������ݲ���������1
				  USART2_RX_BUF[USART2_RX_STA&0X7FFF]=USART2_NewData; //���յ������ݷ�������
				  USART2_RX_STA++;  //���ݳ��ȼ�����1
				  if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����
			   }
		   }
		   HAL_UART_Receive_IT(&huart2,(uint8_t *)&USART2_NewData,1); //�ٿ��������ж�
    	}
    }
	if(huart ==&huart3)//�ж��ж���Դ������3��WIFIģ�飩//�������һ������,��û�б�����,���ٽ�����������
	{
		if(USART3_RX_STA<USART3_REC_LEN)//�����Խ�������
		{
			__HAL_TIM_SET_COUNTER(&htim2,0); //���������
			if(USART3_RX_STA==0) //ʹ�ܶ�ʱ��2���ж�
			{
				__HAL_TIM_ENABLE(&htim2); //ʹ�ܶ�ʱ��2
			}
			USART3_RX_BUF[USART3_RX_STA++] = USART3_NewData;//���½������ݷ�������
		}else
		{
			USART3_RX_STA|=0x8000;//ǿ�Ʊ�ǽ������
		}

		HAL_UART_Receive_IT(&huart3,(uint8_t *)&USART3_NewData,1); //�ٿ�������3�����ж�
	}
}

void WIFI_printf (char *fmt, ...)
{
	char buff[USART3_REC_LEN+1];  //���ڴ��ת��������� [����]
	uint16_t i=0;
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsnprintf(buff, USART3_REC_LEN+1, fmt, arg_ptr);//����ת��
	i=strlen(buff);//�ó����ݳ���
	if(strlen(buff)>USART3_REC_LEN)i=USART3_REC_LEN;//������ȴ������ֵ���򳤶ȵ������ֵ��������ֺ��ԣ�
    HAL_UART_Transmit(&huart3,(uint8_t *)buff,i,0xffff);//���ڷ��ͺ��������ںţ����ݣ����������ʱ�䣩
    va_end(arg_ptr);
}
//WIFIģ����TCPģʽ�µ����ݷ��ͣ�TCP���͵Ĺ涨���ȷ�AT+CIPSEND=�������ȴ����ء�>�����ٷ����������ݡ�
//���÷�����WIFI_TCP_SEND("123\r\n"); //TCP��ʽ�����ַ�123�ͻس�����
void WIFI_TCP_SEND (char *fmt, ...)
{
	char buff[USART3_REC_LEN+1];  //���ڴ��ת��������� [����]
	uint16_t i=0;
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsnprintf(buff, USART3_REC_LEN+1, fmt, arg_ptr);//����ת��
	i=strlen(buff);//�ó����ݳ���
	if(strlen(buff)>USART3_REC_LEN)i=USART3_REC_LEN;//������ȴ������ֵ���򳤶ȵ������ֵ��������ֺ��ԣ�
	WIFI_printf("AT+CIPSEND=%d\r\n",i);//�ȷ���ATָ�����������
	HAL_Delay(100);//�ȴ�WIFIģ�鷵��">"���˴�û�������ǲ���">"���жϡ��ȶ���Ҫ��ߵ���ĿҪ����жϡ�
    HAL_UART_Transmit(&huart3,(uint8_t *)buff,i,0xffff);//�����������ݣ����ںţ����ݣ����������ʱ�䣩
    va_end(arg_ptr);
}

void uart_init(){
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
}

// int fputc(int ch, FILE *f)
// {
//   HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
//   return ch;
// }