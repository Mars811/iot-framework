/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/

#include "aliyun_config.h"
#include "transport.h"

#if !defined(SOCKET_ERROR)
	/** error in socket operation */
	#define SOCKET_ERROR -1
#endif

/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	
	USART3_RX_STA = 0;
	memset(USART3_RX_BUF,0,USART3_REC_LEN);
	HAL_UART_Transmit(&huart3, buf, buflen,1000);//调用串口3发送HAL库函数
	return buflen;
}

int transport_getdata(unsigned char* buf, int count)
{
	memcpy(buf, (const char*)USART3_RX_BUF, count);
	USART3_RX_STA = 0; //接收标志位清0
	memset(USART3_RX_BUF,0,USART3_REC_LEN);//缓存清0
	return count;
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	return 0;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(char* addr, int port)
{
	return 0;
}

int transport_close(int sock)
{
	return 0;
}

/*********************************************************************************************
 * 洋桃电子 www.DoYoung.net
 * 本程序应用于洋桃IoT开发板，可到洋桃电子官网观看教学视频、购买、并下载资料
*********************************************************************************************/
