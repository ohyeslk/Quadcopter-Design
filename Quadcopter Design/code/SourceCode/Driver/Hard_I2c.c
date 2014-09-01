#include "GlobalConfig.h"
#include "stm32f10x.h"


extern INT8U Motor1,Motor2,Motor3,Motor4;



 void InitI2c(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef   I2C_InitStructure;
    
//clock set
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 
//nvic set
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //4 pre-emption priority & 4 subpriority
	NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//PIN FUNC SET
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

//I2C1 configuration
	I2C_Cmd(I2C1, ENABLE);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x33;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 50000;
	I2C_Init(I2C1, &I2C_InitStructure);
  
	I2C_ITConfig(I2C1, I2C_IT_EVT /*| I2C_IT_BUF*/, ENABLE); 
  
}



INT32U ii;

static INT32U i2cEvent;

static INT8U Moto_Num = 0;



void I2C1_EV_IRQHandler(void)
{    
	i2cEvent = I2C_GetLastEvent(I2C1);
	

	
	switch (i2cEvent)
	{		   								//0x00030001
		case I2C_EVENT_MASTER_MODE_SELECT:  //BUSY, MSL and SB flag SB = 1 EV5 Start发送完毕，产生中断
			switch(Moto_Num)
			{
				case 0:
					I2C_Send7bitAddress(I2C1, 0x54, I2C_Direction_Transmitter);	//Moto 1
			   		break;
				case 1:
					I2C_Send7bitAddress(I2C1, 0x52, I2C_Direction_Transmitter); //Moto 2
					break;
				case 2:
					I2C_Send7bitAddress(I2C1, 0x56, I2C_Direction_Transmitter); //Moto 3
			   		break;
				case 3:
					I2C_Send7bitAddress(I2C1, 0x58, I2C_Direction_Transmitter);	//Moto 4
			   		break;
			}
			break;
        	   											 	//0x00070082  BUSY, MSL, ADDR, TXE and TRA flags 
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:  	//ADDR = 1收到ACK; 紧接着TxE置位,数据和移位寄存器
			switch(Moto_Num)
			{
				case 0:
					I2C_SendData(I2C1, Motor1);	//Moto 1
			   		break;
				case 1:
					I2C_SendData(I2C1, Motor2); //Moto 2
					break;
				case 2:
					I2C_SendData(I2C1, Motor3); //Moto 3
			   		break;
				case 3:
					I2C_SendData(I2C1, Motor4);	//Moto 4
			   		break;
			} 
			break;
			 												//0x00070084   TRA, BUSY, MSL, TXE and BTF flags 
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED: 			//With BTF EV8-2
			I2C_GenerateSTOP(I2C1, ENABLE);
			while(I2C_GetLastEvent(I2C1) == I2C_EVENT_MASTER_BYTE_TRANSMITTED)
				;	
			 Moto_Num++;
			 if (Moto_Num < 4)
			 	I2C_GenerateSTART( I2C1,ENABLE);
			 else
			 	Moto_Num = 0;		
			break;
		default:	
			break;
	}
}








	
	


















/*
void I2C1_EV_IRQHandler(void)  //0xa2,0xa0 
{ 
	INT16U SR1,SR2; 
	
	SR1=I2C1->SR1; 
	if(SR1&0x0001)//SB 
	{ 
		I2C1->DR=I2C_SLA;   //地址
		I2C1_CR2_ITERREN=1;  
	} 

	if(SR1&0x0002)//ADDR 
	{ 
		SR2=I2C1->SR2; 
   
		I2C1->DR=I2C_SUB; 
		I2C1_CR2_ITBUFEN=I2C_ByteNum>0?1:0;//根据要发送到数据长度决定  
	} 
  
	if(SR1&0x0004)//BTF 
	{ 

		I2C1_CR2_ITEVTEN=0; 
		I2C1_CR2_ITERREN=0;  
		I2C1_CR1_STOP=1;  
		while(I2C1->SR1&0x0004);   
	} 

	if(I2C1_CR2_ITBUFEN>0) 
	{ 
		if(SR1&0x0080)//TXE 
		{       
			I2C1->DR=*I2C_DataPoint++; 
			I2C_ByteNum--; 
			if(I2C_ByteNum<=0) 
			{ 
				I2C1_CR2_ITBUFEN=0; 
			} 
		} 
		if(SR1&0x0040)//RXNE 
		{       
			*I2C_DataPoint++=I2C1->DR; 
			I2C_ByteNum--; 
			if(I2C_ByteNum<=1) 
			{ 
				if(I2C_ByteNum>0) 
				{ 
					I2C1_CR1_ACK =0; 
					I2C1_CR1_STOP=1;  
				} 
				else 
				{ 
					I2C1_CR2_ITBUFEN=0; 
					I2C1_CR2_ITEVTEN=0; 
					I2C1_CR2_ITERREN=0;   
				} 
			} 
		} 
	} 
} 
*/

/*
void I2C1_ER_IRQHandler(void) 
{ 
  u16 SR1,SR2; 
  SR1=I2C1->SR1; 
  if(SR1&0x0400)//AF 
   { 
 I2C1->SR1&=~0x0400; 
 I2C1_CR2_ITEVTEN=0; 
 I2C1_CR2_ITERREN=0;  
 I2C1_CR2_ITBUFEN=0; 
 I2C1_CR1_STOP=1; 
   } 
} 

 */

