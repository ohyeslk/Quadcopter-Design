#include "stm32f10x.h"
#include "GlobalConfig.h"
#include "I2C_Driver.h"

#define SCL_H         (GPIOB->BSRR) = GPIO_Pin_10 
#define SCL_L         (GPIOB->BRR)  = GPIO_Pin_10  
    
#define SDA_H         (GPIOB->BSRR) = GPIO_Pin_11 
#define SDA_L         (GPIOB->BRR)  = GPIO_Pin_11

#define SCL_READ      (GPIOB->IDR)  & GPIO_Pin_10 
#define SDA_READ      (GPIOB->IDR)  & GPIO_Pin_11 


void I2C_GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  	 //Enable GPIOB CLOCK
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;//10-I2C_SCL 11-I2C_SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;         //OutPut OpenDrain
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
}


void I2C_delay(void) 
{ 
   volatile INT8U i = 8;//230Kbps 10 - 200kbps 
   
   while(i)  i--;  
} 

BOOLEAN I2C_Start(void) 
{ 
	SDA_H;
	I2C_delay();
	SCL_H; 
	I2C_delay(); 
	if(!SDA_READ)
		return FALSE; //SDA线为低电平则总线忙,退出 
	SDA_L; 
	I2C_delay(); 
	SCL_L; 
	I2C_delay(); 
	if(SDA_READ) 
		return FALSE; //SDA线为高电平则总线出错,退出 	
	return TRUE; 
} 

void I2C_Stop(void) 
{ 
	SCL_L; 
	I2C_delay(); 
	SDA_L; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	SDA_H; 
	I2C_delay(); 
} 

void I2C_Ack(void) 
{ 
	SCL_L; 
	I2C_delay(); 
	SDA_L; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	SCL_L; 
	I2C_delay(); 
} 

void I2C_NoAck(void) 
{ 
	SCL_L; 
	I2C_delay(); 
	SDA_H; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	SCL_L; 
	I2C_delay(); 
} 

BOOLEAN I2C_WaitAck(void)   //返回为:=1有ACK,=0无ACK 
{ 
	SCL_L; 
	SDA_H; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	if(SDA_READ) 
	{	 
		SCL_L; 
		return FALSE; 
	} 
	SCL_L; 
	return TRUE; 
} 

void I2C_SendByte(INT8U SendByte) //数据从高位到低位
{ 
    INT8U i = 8; 
	
    while(i--) 
    { 
        SCL_L; 
        I2C_delay(); 
		if(SendByte&0x80) 
			SDA_H;   
		else  
			SDA_L;    
        SendByte<<=1; 
        I2C_delay(); 
		SCL_H; 
        I2C_delay(); 
    } 
    SCL_L; 
} 

INT8U I2C_ReceiveByte(void)  //数据从高位到低位// 
{  
    INT8U i=8; 
    INT8U receivebyte=0; 

    SDA_H; 
    while(i--) 
    { 
		receivebyte <<= 1;       
		SCL_L; 
		I2C_delay(); 
		SCL_H; 
		I2C_delay(); 
		if(SDA_READ)
			receivebyte |= 0x01; 
    } 
    SCL_L; 
    return receivebyte; 
} 







