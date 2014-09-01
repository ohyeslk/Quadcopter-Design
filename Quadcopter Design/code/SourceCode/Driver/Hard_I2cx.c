#include "stm32f10x.h"
#include "GlobalConfig.h"


 /*
void Hard_I2C_Init(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	DMA_InitTypeDef   sEEDMA_InitStructure;
     
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE); 


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel =  DMA1_Channel6_IRQn;  //DMA 发送通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;   //DMA 接收通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);  

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel6);
	DMA_DeInit(sEE_I2C_DMA_CHANNEL_TX);
	sEEDMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)0x40005410);
	sEEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;   // This parameter will be configured durig communication 
	sEEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    // This parameter will be configured durig communication
	sEEDMA_InitStructure.DMA_BufferSize = 0xFFFF;            // This parameter will be configured durig communication
	sEEDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	sEEDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	sEEDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	sEEDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	sEEDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	sEEDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	sEEDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel6 , &sEEDMA_InitStructure);

//I2C RX DMA Channel configuration 
	DMA_DeInit(DMA1_Channel7);
	DMA_Init(DMA1_Channel7, &sEEDMA_InitStructure);   
//Enable the DMA Channels Interrupts
	DMA_ITConfig(sEE_I2C_DMA_CHANNEL_TX, DMA_IT_TC, ENABLE);
	DMA_ITConfig(sEE_I2C_DMA_CHANNEL_RX, DMA_IT_TC, ENABLE); 



	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0xBB;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 20000;

	I2C_Cmd(I2C1, ENABLE);

	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_DMACmd(sEE_I2C, ENABLE);
}
*/

/*
void Write()
{


*/
