#include "GlobalConfig.h"
#include "stm32f10x.h"

#include "ITG3200.h"


BOOLEAN ITG3200_WriteReg(INT8U Register,INT8U Parameter)
{
	if (!I2C_Start()) 
		return FALSE; 
	I2C_SendByte(ITG_WRITE_ADDR);//Send WriteAddress
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	}
	I2C_SendByte(Register);
	I2C_WaitAck(); 
	I2C_SendByte(Parameter);
	I2C_WaitAck(); 
	I2C_Stop();
	return TRUE; 
}

INT8U ITG3200_ReadReg(INT8U Register)
{
	INT8U tmp;
	
	if (!I2C_Start()) 
		return FALSE;	
	I2C_SendByte(ITG_WRITE_ADDR);//Send WriteAddress For Set Register Pointer	
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	}
	I2C_SendByte(Register);
	I2C_WaitAck();
	
	if (!I2C_Start()) 
		return FALSE;	
	I2C_SendByte(ITG_READ_ADDR);
	I2C_WaitAck();
	tmp = I2C_ReceiveByte();
	I2C_NoAck();
	I2C_Stop(); 	
	return (tmp);
}

BOOLEAN ITG3200_ReadData(INT8U Reg_Pnt,INT8U Num_Byte,INT8U *Buffer)
{
	INT8U count;
	if (!I2C_Start()) 
		return FALSE;	
	I2C_SendByte(ITG_WRITE_ADDR);//Send WriteAddress For Set Register Pointer	
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	}
	I2C_SendByte(Reg_Pnt);
	I2C_WaitAck();
	
	if (!I2C_Start()) 
		return FALSE;	
	I2C_SendByte(ITG_READ_ADDR);
	I2C_WaitAck();
	for (count=0; count<Num_Byte; count++)
	{	
		Buffer[count] = I2C_ReceiveByte();
		if ( count < Num_Byte-1 )
			I2C_Ack();  
		else
			I2C_NoAck();
	}
	I2C_Stop(); 	

	return 0;
}



ERROR_SENSOR ITG3200_Init(void)
{
	INT8U buffer[8];
	INT32U i;
	ITG3200_WriteReg(SAMPLE_RATE_REG,SAMPLE_RATE);
	ITG3200_WriteReg(SENSOR_SETING_REG,FULL_SCALER_2000|LOW_PASS_256Hz);
	ITG3200_WriteReg(INT_CONFIG_REG,RAW_RDY_EN);
	ITG3200_WriteReg(POWER_MAMAGE_REG,CLK_SEL_PLL_X);

//	while(1)
//	{
//		for (i=0;i<0xFFFFF;i++);
//		ITG3200_ReadData(TEMPERATURE_H_REG,8,buffer);

//		#ifdef ITG_DEBUG
//			ITG_Debug(ITG3200_DAT,buffer);
//		#endif
//	}
	return 0;
}



void ITG3200_DataRdy_EXTI_Init(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
/* Configure PB2 pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
/* Connect EXTI2 Line to PB2 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource2);
/* Configure EXTI2 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
/* Enable and set EXTI2 Interrupt to the lowest priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //4 pre-emption priority & 4 subpriority
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}




 































