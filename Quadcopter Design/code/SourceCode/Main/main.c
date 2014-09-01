#include "GlobalConfig.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "ADXL345.h"
#include "PPM_Signal.h"
#include "ITG3200.h"




#define LONG_FIX_TERM 128

#define MOTO_I2C_START ( I2C1->CR1 |= 0x0100 )	//启动发送


 extern INT8U Motor1;
  extern INT8U Motor2;
   extern INT8U Motor3;
    extern INT8U Motor4;
 extern INT32S Raw_Integral_Roll;
  extern INT32S Raw_Integral_Pitch;
 extern INT32S Fix_Integral_Roll;
  extern INT32S Fix_Integral_Pitch;
 
 extern INT8U Gyro_Data_Buf[];
 extern INT8U Acc_Data_Buf[]  ;

 extern INT8U Gyro_Rdy_Signal;

extern INT16S Acc_Roll_Tmp;
extern INT16S Acc_Pitch_Tmp;

extern INT16S Acc_Average_Roll;
extern INT16S Acc_Average_Pitch;

extern INT8U PPM_UpDate;

extern INT8U PPM_Signal_Missing;
extern INT8U Motor1,Motor2,Motor3,Motor4;
volatile INT8U Long_Term_Cnt; 

extern INT32U Error_ADXL;


void delay_5s(void)
{
	INT32U I;
	INT8U P;
	for (P=0;P<20;P++)
		for(I=0;I<0XFFFFF;I++);
}



int main(void)
{
   INT32U ii,i;
   INT8U buffer[6];
   INT8U roll_h,roll_l,pitch_h,pitch_l;
   INT8U acc_x_h ,acc_x_l ,acc_y_h,acc_y_l;
   INT32U softbreak=0;
   INT8U software_delay = 0;


	SystemInit();



	RCC_HSEConfig(RCC_HSE_Bypass); 
	
	DEBUG_USART1_Configuration();

	printf(" Nice \r\n");

   	LED_PORT_Init();
	PPM_Capture_Init();
	I2C_GPIO_Configuration();	   //For Gyro & Hmc

	ITG3200_Init();
	Gyro_Set_Neutral();
	ITG3200_DataRdy_EXTI_Init();   //Data Rdy Interrupt
  	
	PPM_Set_Netural();	  //8888888888888888888888888888888888888888888888888888888888888

//	HMC5883L_SelfTest();
//	HMC5883L_Init();
	ADXL345_Driver_Init();
	ADXL345_SelfTest();
	ADSX345_Init();
   	ADXL_DataRdy_EXTI_Init();
	ADXL35_ReadData(DATAX0_REG,6,(INT8U *)buffer);


   InitI2c();

	delay_5s();

	while(1)
	{  	

		software_delay++;

		if ( (Error_ADXL ++) > 5 )
		{
		 	GPIOC->BSRR = GPIO_Pin_6;	   //ADXL Works WRONG
		 	ADXL35_ReadData(DATAX0_REG,6,(INT8U *)buffer);
		}
		

		if  (software_delay>127)
		{
	 		GPIOC->BRR = GPIO_Pin_7;	   //ITG Works Well
		}
		else	 
			GPIOC->BSRR = GPIO_Pin_7;	   //ITG Works Well


/***********************************************************************/		  
		while( Gyro_Rdy_Signal < 5 )	 //Wait Gyro Rdy
		{
			if ( (softbreak++) > 0x1FFFF )	    //Gyro Break
			{
				Motor1 = Motor2 = Motor3 = Motor4 = 50; 
				GPIOC->BSRR = GPIO_Pin_7;
			}

			if ( PPM_Signal_Missing > 20 )
				Motor1 = Motor2 = Motor3 = Motor4 = 0;	  //Motor OFF 
		}
		Gyro_Rdy_Signal = 0;
		softbreak = 0;

		Attitude_Correction();  //计算平均加速度值，对加速度进行积分，对陀螺二次积分，算出陀螺与加速度计的瞬时偏差，并瞬时补偿

		if(PPM_UpDate)
			Calculate_Remote_Value();
/*		
		Long_Term_Cnt++;
	   
		if ( Long_Term_Cnt > LONG_FIX_TERM )
		{
			Long_Term_Fusion(); //长期融合
			Long_Term_Cnt = 0;
		}
*/		
		Moto_PID_Controler();


		if ( PPM_Signal_Missing < 20 )
		{	
			while(I2C1->SR2 & 0x0002);	
			MOTO_I2C_START;
		}
		else
			Motor1 = Motor2 = Motor3 = Motor4 = 0;
	}
}







