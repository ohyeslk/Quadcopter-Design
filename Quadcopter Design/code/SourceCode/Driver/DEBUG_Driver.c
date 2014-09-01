#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stdio.h"
//#include "misc.h"

void DEBUG_USART1_Configuration(void);


void DEBUG_USART1_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
//Enable RCC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 |RCC_APB2Periph_GPIOA, ENABLE);//启用外设
//GPIO Init	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         			//USART1 TX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    				//复用推挽输出
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		    				//A端口 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         			//USART1 RX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   		//复用开漏输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		         			//A端口 
//USART1 Init	
  	USART_InitStructure.USART_BaudRate = 115200;						//速率115200bps
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位8位
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位1位
  	USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
  	USART_Init(USART1, &USART_InitStructure);										//配置串口参数函数
	USART_Cmd(USART1, ENABLE);
//  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                  //使能接收中断
//  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);					//使能发送缓冲空中断
//	USART_ITConfig(USART1, USART_IT_TC, ENABLE);					//使能发送完成中断
  		
}



#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif 
/*** Printf Function Repositioning **/
PUTCHAR_PROTOTYPE
{
  	USART_SendData(USART1, (uint8_t) ch);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	 	;
  	return ch;
}
  	  

void USART_Send_Buffer(uint8_t Buffer)
{
	USART_SendData(USART1, Buffer);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	 	;
}

/*
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		   

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
*/




