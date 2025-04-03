/**
 * ----------------------------------------------------------------------
 * @file usart.c  
 * @author Mars811 + 洋桃电子
 * ----------------------------------------------------------------------
 * 工程外设适配程序（不适合移植）。内容包括三个基本串口以及WIFI模块的串口应用重
 * 定向。          
 * ----------------------------------------------------------------------
 * @date 2025-04-02
 * ----------------------------------------------------------------------
**/
#include "usart.h"

uint8_t USART1_RX_BUF[USART1_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节.
uint16_t USART1_RX_STA=0;//接收状态标记//bit15：接收完成标志，bit14：接收到0x0d，bit13~0：接收到的有效字节数目
uint8_t USART1_NewData;//当前串口中断接收的1个字节数据的缓存

uint8_t USART2_RX_BUF[USART2_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节.
uint16_t USART2_RX_STA=0;//接收状态标记//bit15：接收完成标志，bit14：接收到0x0d，bit13~0：接收到的有效字节数目
uint8_t USART2_NewData;//当前串口中断接收的1个字节数据的缓存
uint8_t RS485orBT;//当RS485orBT标志位为1时是RS485模式，为0时是蓝牙模式

uint8_t USART3_RX_BUF[USART3_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节.
uint16_t USART3_RX_STA=0;//接收状态标记//bit15：接收完成标志，bit14：接收到0x0d，bit13~0：接收到的有效字节数目
uint8_t USART3_NewData;//当前串口中断接收的1个字节数据的缓存

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

void  HAL_UART_RxCpltCallback(UART_HandleTypeDef  *huart)//串口中断回调函数
{

	if(huart ==&huart1)//判断中断来源（串口1：USB转串口）
    {
       printf("%c",USART1_NewData); //把收到的数据以 a符号变量 发送回电脑
       if((USART1_RX_STA&0x8000)==0){//接收未完成
           if(USART1_RX_STA&0x4000){//接收到了0x0d
               if(USART1_NewData!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
               else USART1_RX_STA|=0x8000;   //接收完成了
           }else{ //还没收到0X0D
               if(USART1_NewData==0x0d)USART1_RX_STA|=0x4000;
               else{
                  USART1_RX_BUF[USART1_RX_STA&0X3FFF]=USART1_NewData; //将收到的数据放入数组
                  USART1_RX_STA++;  //数据长度计数加1
                  if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收
               }
           }
       }
       HAL_UART_Receive_IT(&huart1,(uint8_t *)&USART1_NewData,1); //再开启接收中断
    }
    if(huart ==&huart2)//判断中断来源（RS485/蓝牙）
    {
    	if(RS485orBT){//判断当RS485orBT标志位为1时是RS485模式，为0时是蓝牙模式
		   USART2_RX_BUF[0]=USART2_NewData;//收到数据放入缓存数组（只用到1个数据存放在数组[0]）
		   USART2_RX_STA++;//数据接收标志位加1
		   HAL_UART_Receive_IT(&huart2,(uint8_t *)&USART2_NewData, 1); //再开启接收中断
    	}else{
		   if((USART2_RX_STA&0x8000)==0){//接收未完成（将USART2_RX_STA最高位1位规定是接收完成标志位）
			   if(USART2_NewData==0x5A)//如收到0x5A表示接收到结束符（手机APP“蓝牙调试器”回复数据以0x5A为结束符）
			   {
				   USART2_RX_STA|=0x8000;//收到0x0A，接受完成
			   }
			   else{ //如没有收到0x0A则继续接收数据内容并把数量加1
				  USART2_RX_BUF[USART2_RX_STA&0X7FFF]=USART2_NewData; //将收到的数据放入数组
				  USART2_RX_STA++;  //数据长度计数加1
				  if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//接收数据错误,重新开始接收
			   }
		   }
		   HAL_UART_Receive_IT(&huart2,(uint8_t *)&USART2_NewData,1); //再开启接收中断
    	}
    }
	if(huart ==&huart3)//判断中断来源（串口3：WIFI模块）//接收完的一批数据,还没有被处理,则不再接收其他数据
	{
		if(USART3_RX_STA<USART3_REC_LEN)//还可以接收数据
		{
			__HAL_TIM_SET_COUNTER(&htim2,0); //计数器清空
			if(USART3_RX_STA==0) //使能定时器2的中断
			{
				__HAL_TIM_ENABLE(&htim2); //使能定时器2
			}
			USART3_RX_BUF[USART3_RX_STA++] = USART3_NewData;//最新接收数据放入数组
		}else
		{
			USART3_RX_STA|=0x8000;//强制标记接收完成
		}

		HAL_UART_Receive_IT(&huart3,(uint8_t *)&USART3_NewData,1); //再开启串口3接收中断
	}
}

void WIFI_printf (char *fmt, ...)
{
	char buff[USART3_REC_LEN+1];  //用于存放转换后的数据 [长度]
	uint16_t i=0;
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsnprintf(buff, USART3_REC_LEN+1, fmt, arg_ptr);//数据转换
	i=strlen(buff);//得出数据长度
	if(strlen(buff)>USART3_REC_LEN)i=USART3_REC_LEN;//如果长度大于最大值，则长度等于最大值（多出部分忽略）
    HAL_UART_Transmit(&huart3,(uint8_t *)buff,i,0xffff);//串口发送函数（串口号，内容，数量，溢出时间）
    va_end(arg_ptr);
}
//WIFI模块在TCP模式下的数据发送：TCP发送的规定是先发AT+CIPSEND=数量，等待返回“>“后再发送数据内容。
//调用方法：WIFI_TCP_SEND("123\r\n"); //TCP方式发送字符123和回车换行
void WIFI_TCP_SEND (char *fmt, ...)
{
	char buff[USART3_REC_LEN+1];  //用于存放转换后的数据 [长度]
	uint16_t i=0;
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsnprintf(buff, USART3_REC_LEN+1, fmt, arg_ptr);//数据转换
	i=strlen(buff);//得出数据长度
	if(strlen(buff)>USART3_REC_LEN)i=USART3_REC_LEN;//如果长度大于最大值，则长度等于最大值（多出部分忽略）
	WIFI_printf("AT+CIPSEND=%d\r\n",i);//先发送AT指令和数据数量
	HAL_Delay(100);//等待WIFI模块返回">"，此处没做返回是不是">"的判断。稳定性要求高的项目要另加判断。
    HAL_UART_Transmit(&huart3,(uint8_t *)buff,i,0xffff);//发送数据内容（串口号，内容，数量，溢出时间）
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