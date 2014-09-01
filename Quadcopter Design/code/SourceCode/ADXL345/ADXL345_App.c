#include "stm32f10x.h"
#include "GlobalConfig.h"
#include "ITG3200.h"
#include "ADXL345.h"

#define ACC_AMPLIFY 12
INT32S  Gyro_Acc_Factor = 306 ;



INT32S  Acc_Average_Pitch=0,Acc_Average_Roll=0,Acc_Average_Hoch = 0;    //三轴加速度平均测量值
INT16S  Acc_Value_Pitch=0, Acc_Value_Roll=0, Acc_Value_Hoch = 0;      //三轴加速度瞬时测量值
INT16S  Acc_NetValue_Pitch=0, Acc_NetValue_Roll=0,   Acc_NetValue_Hoch = 0; //三轴加速度净输出值
INT16S  Acc_Netural_Pitch=0,Acc_Netural_Roll = 0;      
float   Acc_Netural_Z = 0.000;									 	  //加速度计中立点 (Float 型）
INT32S  Integral_Acc_Pitch=0,Integral_Acc_Roll=0, Integral_Acc_Z = 0;      //X Y Z 轴加速度积分 Integral_Acc_Z 并未用到
INT32S  Acc_Integral_Hoch;  //代替Mess_Integral_Hoch

INT8U Acc_Data_Buf[6] = 0;



/**************************************************************************************************************
设定加速度计中立点
**************************************************************************************************************/
void Acc_Set_Neutral(void)
{
	Acc_NetValue_Pitch = 0;
	Acc_NetValue_Roll  = 0;
	Acc_NetValue_Hoch  = 235;   //已经通过实验获得,不需要通过程序自我测定
}


/**************************************************************************************************************
对加速度数据进行处理
获得每次测量的加速度值，并且积分
**************************************************************************************************************/
void Acc_Load_Average(void) //终端中执行
{
/* 加速度计获得的平均值 */
	ADXL35_ReadData(DATAX0_REG,6,Acc_Data_Buf); //Get Data
	Acc_Value_Pitch  = (INT16S)( Acc_Data_Buf[1]<<8)|Acc_Data_Buf[0];
	Acc_Value_Roll   = (INT16S)( Acc_Data_Buf[3]<<8)|Acc_Data_Buf[2];
	Acc_Value_Hoch   = (INT16S)( Acc_Data_Buf[5]<<8)|Acc_Data_Buf[4]; //Junc Data
	
	Acc_NetValue_Pitch = Acc_Value_Pitch - Acc_Netural_Pitch;
	Acc_NetValue_Roll  = Acc_Netural_Roll - Acc_Value_Roll;	
	Acc_NetValue_Hoch  = Acc_Value_Hoch - Acc_Value_Roll;	
	
	Acc_Average_Pitch = ( Acc_Average_Pitch + (ACC_AMPLIFY * Acc_NetValue_Pitch) ) / 2L; //加权平均 放大了12倍
	Acc_Average_Roll  = ( Acc_Average_Roll  + (ACC_AMPLIFY * Acc_NetValue_Roll ) ) / 2L; //加权平均
	Acc_Average_Hoch  = ( Acc_Average_Hoch  +  Acc_NetValue_Hoch ) / 2L;

}

/*******************************************************************************************
ADXL345 Data Ready Interrupt 5ms 执行一次 执行时间约 xxxus
********************************************************************************************/

volatile Error_ADXL = 0;
void EXTI0_IRQHandler(void)
{
//	读取数据，或者DMA
	Acc_Load_Average();
	EXTI->PR = EXTI_Line0; //清除标志位
	Error_ADXL = 0;
	GPIOC->BRR = GPIO_Pin_6;	   //ADXL Works WRONG
}


