#include "stm32f10x.h"
#include "GlobalConfig.h"
#include "MotoDriver.h"

#define MOTO_SCL_H         (GPIOB->BSRR) = GPIO_Pin_6 
#define MOTO_SCL_L         (GPIOB->BRR)  = GPIO_Pin_6  
    
#define MOTO_SDA_H         (GPIOB->BSRR) = GPIO_Pin_7 
#define MOTO_SDA_L         (GPIOB->BRR)  = GPIO_Pin_7

#define MOTO_SCL_READ      (GPIOB->IDR)  & GPIO_Pin_6 
#define MOTO_SDA_READ      (GPIOB->IDR)  & GPIO_Pin_7 


void MOTO_I2C_GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  	 //Enable GPIOB CLOCK
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;//6-I2C_SCL 7-I2C_SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;         //OutPut OpenDrain
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
}


void MOTO_I2C_delay(void) 
{ 
   volatile INT8U i = 50;
   
   while(i)  i--;  
} 

BOOLEAN MOTO_I2C_Start(void) 
{ 
	MOTO_SDA_H;
	MOTO_I2C_delay();
	MOTO_SCL_H; 
	MOTO_I2C_delay(); 
	if(!MOTO_SDA_READ)
		return FALSE; //SDA线为低电平则总线忙,退出 
	MOTO_SDA_L; 
	MOTO_I2C_delay(); 
	MOTO_SCL_L; 
	MOTO_I2C_delay(); 
	if(MOTO_SDA_READ) 
		return FALSE; //SDA线为高电平则总线出错,退出 	
	return TRUE; 
} 

void MOTO_I2C_Stop(void) 
{ 
	MOTO_SCL_L; 
	MOTO_I2C_delay(); 
	MOTO_SDA_L; 
	MOTO_I2C_delay(); 
	MOTO_SCL_H; 
	MOTO_I2C_delay(); 
	MOTO_SDA_H; 
	MOTO_I2C_delay(); 
} 

void MOTO_I2C_Ack(void) 
{ 
	MOTO_SCL_L; 
	MOTO_I2C_delay(); 
	MOTO_SDA_L; 
	MOTO_I2C_delay(); 
	MOTO_SCL_H; 
	MOTO_I2C_delay(); 
	MOTO_SCL_L; 
	MOTO_I2C_delay(); 
} 

void MOTO_I2C_NoAck(void) 
{ 
	MOTO_SCL_L; 
	MOTO_I2C_delay(); 
	MOTO_SDA_H; 
	MOTO_I2C_delay(); 
	MOTO_SCL_H; 
	MOTO_I2C_delay(); 
	MOTO_SCL_L; 
	MOTO_I2C_delay(); 
} 

BOOLEAN MOTO_I2C_WaitAck(void)   //返回为:=1有ACK,=0无ACK 
{ 
	MOTO_SCL_L; 
	MOTO_SDA_H; 
	MOTO_I2C_delay(); 
	MOTO_SCL_H; 
	MOTO_I2C_delay(); 
	if(MOTO_SDA_READ) 
	{	 
		MOTO_SCL_L; 
		return FALSE; 
	} 
	MOTO_SCL_L; 
	return TRUE; 
} 

void MOTO_I2C_SendByte(INT8U SendByte) //数据从高位到低位
{ 
    INT8U i=8; 
	
    while(i--) 
    { 
        MOTO_SCL_L; 
        MOTO_I2C_delay(); 
		if(SendByte&0x80) 
			MOTO_SDA_H;   
		else  
			MOTO_SDA_L;    
        SendByte<<=1; 
        MOTO_I2C_delay(); 
		MOTO_SCL_H; 
        MOTO_I2C_delay(); 
    } 
    MOTO_SCL_L; 
} 

INT8U MOTO_I2C_ReceiveByte(void)  //数据从高位到低位// 
{  
    INT8U i=8; 
    INT8U receivebyte=0; 

    MOTO_SDA_H; 
    while(i--) 
    { 
		receivebyte <<= 1;       
		MOTO_SCL_L; 
		MOTO_I2C_delay(); 
		MOTO_SCL_H; 
		MOTO_I2C_delay(); 
		if(MOTO_SDA_READ)
			receivebyte |= 0x01; 
    } 
    MOTO_SCL_L; 
    return receivebyte; 
} 



BOOLEAN MOTO_Write(INT8U Addr,INT8U Parameter)
{
	if (!MOTO_I2C_Start()) 
		return FALSE; 
	MOTO_I2C_SendByte(Addr);//Send WriteAddress
	if (!MOTO_I2C_WaitAck()) 
	{ 
		MOTO_I2C_Stop();  
		return FALSE; 
	}
	MOTO_I2C_SendByte(Parameter);
	MOTO_I2C_WaitAck(); 
	MOTO_I2C_Stop();
	return TRUE; 
}


