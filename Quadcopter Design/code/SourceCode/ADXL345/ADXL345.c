#include "stm32f10x.h"
#include "GlobalConfig.h"
#include "ADXL345.h"
#include "ADXL_Driver.h"




BOOLEAN ADXL35_ReadData(INT8U Reg_Pnt,INT8U Num_Byte,INT8U *Buffer)
{
	return ADXL_Read_Multi_Driver(Reg_Pnt,Num_Byte,Buffer);
}

BOOLEAN ADXL345_WriteReg(INT8U Register,INT8U Parameter)
{
	return ADXL_WriteReg_Driver(Register,Parameter);	
}


BOOLEAN ADXL345_ReadReg(INT8U Register,INT8U *Buffer)
{
	return ADXL_Read_Driver(Register,Buffer);
}

INT8U ADXL_Wait_Data_Ready() //Get Int On Pins For self Test
{
	return ((INT8U)( (GPIOB->IDR) & GPIO_Pin_0 )  );
}	
	
	
	
	
	
	
ERROR_SENSOR ADXL345_SelfTest(void)
{
	INT8U	 buffer[4][2];
	INT8U  	 device_ID;
	INT8U    i,j,state;
	INT16S   sum_st_on[3]  = {0};
	INT16S   sum_st_off[3] = {0};
	INT32U	 timeout;
	
	ADXL345_ReadReg(DEVICEID_REG,&device_ID);
	if (device_ID != ADXL345_ID)
	{
	#ifdef ADXL_DEBUG
		ADXL_Debug(ADXL345_ID_ERROR,(void*)(&device_ID));
	#endif	
		return (ADXL345_ID_ERROR);
	}
/* Device ID is OK */
	ADXL345_WriteReg(OFSX_REG,0x00);
	ADXL345_WriteReg(OFSY_REG,0x00);
	ADXL345_WriteReg(OFSZ_REG,0x00);   //Set ALL Offset Register To 0
	ADXL345_WriteReg(BW_RATE_REG,NORMAL_OPERATION|BAND_0050_00); //OutPut Rate 100Hz

	ADXL345_WriteReg(INT_MAP_REG,DATA_READY_INT1);  //DataRdy map On INT1

	ADXL345_WriteReg(INT_ENABLE_REG,DATA_READY_EN); //Enable INT On Data Ready
	ADXL345_WriteReg(DATA_FORMAT_REG,INT_ACTIVE_HIGH|FULL_RES_MODE|
						                DATA_RIGTH_JUST|G_RANGE_16G);

	ADXL345_WriteReg(POWER_CTL_REG,GO_MEASURE|LEAVE_SLEEP);  //Wake Up & Go Measure Mode
	
	for (i=0; i<10; i++)                                     //Get Data With Out SelfTest
	{
		timeout = 0;
		do{
			state = ADXL_Wait_Data_Ready();
			if ( (timeout++) > 0xFFFFF )
			{
			#ifdef ADXL_DEBUG
				ADXL_Debug(ADXL345_TIMEOUT,NULL);
			#endif		
				return (ADXL345_TIMEOUT);
			}
		}while(state == 0x00);
		
		ADXL35_ReadData(DATAX0_REG,6,(INT8U *)buffer);
		#ifdef ADXL_DEBUG
			ADXL_Debug(ADXL345_AXIS_DAT,(void*)buffer);
		#endif	
		for(j=0;j<3;j++)
			sum_st_off[j] += (INT16S)( (buffer[j][1]<<8)|buffer[j][0] );
		for (timeout=0;timeout<0xFFFF;timeout++)
			;
	} //Get Data With Out SelfTest
	
	ADXL345_WriteReg( DATA_FORMAT_REG,FORCE_SELF_TEST|INT_ACTIVE_HIGH|
							FULL_RES_MODE|DATA_RIGTH_JUST|G_RANGE_16G);
		#ifdef ADXL_DEBUG
			ADXL_Debug(PRINT_ENTER,NULL);                    //For Better View Of Data
		#endif   
		
	for (i=0; i<10; i++)                                     //Get Data With SelfTest
	{
		timeout = 0;
		do{
			state = ADXL_Wait_Data_Ready();
			if ( (timeout++) > 0xFFFFF )
			{
			#ifdef ADXL_DEBUG
				ADXL_Debug(ADXL345_TIMEOUT,NULL);
			#endif		
				return (ADXL345_TIMEOUT);
			}
		}while(state == 0x00);
		
		ADXL35_ReadData(DATAX0_REG,6,(INT8U *)buffer);
		#ifdef ADXL_DEBUG
			ADXL_Debug(ADXL345_AXIS_DAT,(void*)buffer);
		#endif	
		for(j=0;j<3;j++)
			sum_st_on[j] += (INT16S)( (buffer[j][1]<<8)|buffer[j][0] );
		for (timeout=0;timeout<0xFFFF;timeout++)
			;
	}//Get Data With SelfTest	
	for (j=0; j<3; j++)
		sum_st_on[j] = (sum_st_on[j]-sum_st_off[j])/10;
		
	if ( (sum_st_on[0]<89)||(sum_st_on[0]>956)||
		 (sum_st_on[1]<-956)||(sum_st_on[1]>-89)||
		 (sum_st_on[2]<133)||(sum_st_on[2]>1549)  )
	{
		#ifdef ADXL_DEBUG
			ADXL_Debug(ADXL345_BRRAK,NULL);
		#endif			
			return (ADXL345_BRRAK);
	}
/*89~956 	-956~-89  133~1549 */
	ADXL345_WriteReg(DATA_FORMAT_REG,INT_ACTIVE_HIGH|FULL_RES_MODE|
						                DATA_RIGTH_JUST|G_RANGE_16G);	
	ADXL345_WriteReg(POWER_CTL_REG,GO_SLEEP|WAKEUP_FREG_8Hz);  //Go Sleep Mode

	return (ADXL345_NICE);
}


void ADSX345_Init(void)
{
	ADXL345_WriteReg(OFSX_REG,-1);
	ADXL345_WriteReg(OFSY_REG,0x00);
	ADXL345_WriteReg(OFSZ_REG,0x00); //Set ALL Offset Register To 0
	ADXL345_WriteReg(BW_RATE_REG,NORMAL_OPERATION|BAND_0100_00); //OutPut Rate 200Hz

	ADXL345_WriteReg(INT_MAP_REG,DATA_READY_INT1);  //DataRdy map On INT1

	ADXL345_WriteReg(INT_ENABLE_REG,DATA_READY_EN); //Enable INT On Data Ready
	ADXL345_WriteReg(DATA_FORMAT_REG,INT_ACTIVE_HIGH|FULL_RES_MODE|
						                DATA_RIGTH_JUST|G_RANGE_4G);

	ADXL345_WriteReg(POWER_CTL_REG,GO_MEASURE|LEAVE_SLEEP);  //Wake Up & Go Measure Mode	
}


void ADXL_DataRdy_EXTI_Init(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  
/* Configure PB2 pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
/* Connect EXTI0 Line to PB0 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
/* Configure EXTI2 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
/* Enable and set EXTI2 Interrupt to the lowest priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //4 pre-emption priority & 4 subpriority
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //优先级比陀螺仪稍低
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
