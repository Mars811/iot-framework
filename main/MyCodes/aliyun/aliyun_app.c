#include "aliyun_app.h"

void aliyun_main(void){
	uint16_t a=0,b=0;
	int t,qos,payloadinlen; //为下面即将解析的消息定义所需变量
	unsigned char dup,retained;
	unsigned short msgid;
	unsigned char* payloadin;
	MQTTString receiveTopic;

    //等待服务器连接
    while(esp8266_Connect_IOTServer());//AT指令连接TCP连接云服务器（IP和端口参数在esp8266.h文件内修改设置）
    while(IOT_connect());//用MQTT协议+三元组信息连接阿里云物联网平台（三元组参数在iot.h文件内修改设置）
    printf("订阅云服务器\r\n");
    HAL_Delay(100);//等待
    IOT_subscribe();//主题订阅（订阅成功后才能接收订阅消息）
    a=0xFFF0; //强制发送心跳包的计数溢出，立即重发心跳包

    while(1){
        //数据上传
        /*
        char temp[30];
        snprintf(temp, sizeof(temp), "temperature:%d", para_temperature);
        IOT_publish(temp);
        HAL_Delay(100);

        snprintf(temp, sizeof(temp), "humidity:%d", para_humidity);
        IOT_publish(temp);
        HAL_Delay(100);

        snprintf(temp, sizeof(temp), "lightness:%d", para_lightness);
        IOT_publish(temp);
        HAL_Delay(100);

        snprintf(temp, sizeof(temp), "temperature_th:%d", para_temperature_th);
        IOT_publish(temp);
        */

        // 心跳包
        HAL_Delay(10);		//主循环的间隔延时（防止刷新过快）
        a++;					//计算加1
        if(a>1000){ 			//每1000*10毫秒延时发送一次Ping心跳包（保保持与云服务器的连接）
              a=0;			//计算标志清0
              IOT_ping();		//发送Ping心跳包
              if(MQTTPacket_read(buf, buflen, transport_getdata)==PINGRESP){		//判断心跳包是不回复确认
                  printf("心跳成功\r\n"); 											//回复0xD0，0x00时表示心跳成功的回复
              }else {
                  printf("心跳失败\r\n");//无回复表示失败
                  a=0xFFF0; //强制发送心跳包的计数溢出，立即重发心跳包
                  b++;//重启计数加1
                  if(b>20) //如果快速发送心跳包20次后无回复，则复位WIFI模块重新连接
                  {
                      while(esp8266_Connect_IOTServer());//AT指令连接TCP连接云服务器（IP和端口参数在esp8266.h文件内修改设置）
                      while(IOT_connect());//用MQTT协议+三元组信息连接阿里云物联网平台（三元组参数在iot.h文件内修改设置）
                      a=0;b=0;//计算标志清0
                  }
              }
              USART3_RX_STA = 0;//串口3接收标志位清0
        }
  
        //接收云端的订阅消息
        if(USART3_RX_STA&0x8000) //判断云服务器发布的消息
        {
            switch (USART3_RX_BUF[0]/16){//判断接收到的报文类型
                  case PUBLISH:
                      t = MQTTDeserialize_publish( 				//对接收的MQTT原始数据进行解码（返回1表示成功，其他值表示错误）
                        &dup, 								//【得出】重发标志位（0首发，1早前报文的重发）
                        &qos, 								//【得出】服务质量等级（0最多分发一次，1至少分发一次，2只分发一次）
                        &retained, 							//【得出】保留位参数
                        &msgid, 								//【得出】消息ID
                        &receiveTopic, 						//【得出】订阅主题名
                        &payloadin, 							//【得出】消息内容
                        &payloadinlen, 						//【得出】消息长度
                        USART3_RX_BUF, USART3_RX_STA&0x7FFF	//【输入】原始数据缓存（数组+数量）
                    );
  
                      //正确接收数据
                      if(t){
                        printf("接收到主题:%.*s  ", receiveTopic.lenstring.len, receiveTopic.lenstring.data);//显示接收主题
                        printf("消息内容:%.*s  ", payloadinlen, payloadin);
                        printf("QoS:%d\r\n", qos);
                        USART3_RX_STA = 0;
  
                        //变量调节
                        if(strncmp(payloadin, "para_lightness:", 15) == 0) {
                            IOT_publish("para_lightness has been changed.");
                            printf("para_lightness has been changed.");
  
                        }
                        if(strncmp(payloadin, "para_temperature_th:", 20) == 0) {
                            IOT_publish("para_temperature_th has been changed.");
                            printf("para_temperature_th has been changed.");
                        }
                      }else{
                          printf("接收订阅消息时出错\r\n");//接收错误时的显示
                      }
                      break;
                  case CONNACK: //连接报文确认
                  //插入您的处理程序（也可空置）
                      break;
                  case SUBACK: //订阅请求报文确认
                  //插入您的处理程序（也可空置）
                      break;
                  case UNSUBACK: //取消订阅报文确认
                  //插入您的处理程序（也可空置）
                      break;
                  default:
                  //冗余语句
                      break;
            }
            USART3_RX_STA = 0;//串口3接收标志位清0
        }	
    }
}