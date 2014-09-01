


#include "GlobalConfig.h"
#include "ITG3200.h"
#include "stm32f10x.h"

volatile INT8U Gyro_Rdy_Signal = 0;     //陀螺仪更新5次标志信号

INT32S Gyro_180_Inregral = 5898240/2L;  //转动180度积分值
INT8U  Gyro_Data_Buf[8]; 			   //暂存陀螺仪读数

INT16S  Gyro_Netural_Yaw,   Gyro_Netural_Roll,   Gyro_Netural_Pitch;  //三轴陀螺仪中立点
INT16S  Gyro_Value_Yaw,     Gyro_Value_Roll,     Gyro_Value_Pitch;    //三轴陀螺仪未减中立点的值
INT16S  Gyro_NetValue_Yaw,  Gyro_NetValue_Roll,  Gyro_NetValue_Pitch; //三轴陀螺仪净输出值 

INT32S  Raw_Integral_Yaw=0,   Raw_Integral_Roll=0,   Raw_Integral_Pitch=0;  //未考虑修正的陀螺仪积分,			 Yaw未使用
INT32S  Fix_Integral_Yaw=0,   Fix_Integral_Roll=0,   Fix_Integral_Pitch=0;  //考虑了加速度修正的陀螺仪积分


INT32S Raw_Integral_Yaw_Tmp, Raw_Integral_Roll_Tmp, Raw_Integral_Pitch_Tmp;
INT32S Fix_Integral_Yaw_Tmp, Fix_Integral_Roll_Tmp, Fix_Integral_Pitch_Tmp; //暂存 陀螺仪积分值

INT32S LageKorrekturRoll=0, LageKorrekturNick = 0;         //陀螺仪补偿量，在256次采样后执行



ERROR_SENSOR Gyro_Set_Neutral(void)  //设定中立点应该在开启陀螺中断前执行
{
	INT16U  i,time;
	INT32S  roll_tmp=0,pitch_tmp=0,yaw_tmp = 0;
	ITG3200_ReadData(TEMPERATURE_H_REG,8,(INT8U*)Gyro_Data_Buf);//丢弃不要	
	for (i=0; i<1000; i++)
	{
		ITG3200_ReadData(TEMPERATURE_H_REG,8,(INT8U*)Gyro_Data_Buf); 
		Gyro_Value_Roll    = (INT16S)( (Gyro_Data_Buf[2]<<8) | Gyro_Data_Buf[3]);
		Gyro_Value_Pitch   = (INT16S)( (Gyro_Data_Buf[4]<<8) | Gyro_Data_Buf[5]);
		Gyro_Value_Yaw     = (INT16S)( (Gyro_Data_Buf[6]<<8) | Gyro_Data_Buf[7]);		
		roll_tmp  += Gyro_Value_Roll;
		pitch_tmp += Gyro_Value_Pitch;
		yaw_tmp   += Gyro_Value_Yaw;
	
		for ( time=0;time<0x1FFF;time++ ) //Wait For Data Readly
			;
	}

	Gyro_Netural_Roll  = (INT16S)( roll_tmp /1000 );
	Gyro_Netural_Pitch = (INT16S)( pitch_tmp/1000 );
	Gyro_Netural_Yaw   = (INT16S)( yaw_tmp  /1000 );

	if ( ( Gyro_Netural_Roll > 100  || Gyro_Netural_Roll < -100 ) || //陀螺仪中立点超出常值
		 ( Gyro_Netural_Pitch > 100 || Gyro_Netural_Pitch < -100 )||
		 ( Gyro_Netural_Yaw > 100   || Gyro_Netural_Yaw < -100   ) )
	{	return 1; }
	return 0;
}


/************************************************************************************
1ms 执行一次 获得 原陀螺仪积分值，修正后的积分值，和 角速度净值 
被陀螺仪中断函数调用
*************************************************************************************/ 


void Gyro_Integral(void) //Test Speed 
{
	INT32S coupling_pitch, coupling_roll;	
// 将陀螺仪数据减去常值误差，得到实际的角速率的倍数
	ITG3200_ReadData(TEMPERATURE_H_REG,8,(INT8U*)Gyro_Data_Buf); //Read All 8 Register
//	Gyro_Value_Tmp = (INT16S)( (Gyro_Data_Buf[0]<<8) | Gyro_Data_Buf[1]);
	Gyro_Value_Roll    = (INT16S)( (Gyro_Data_Buf[2]<<8) | Gyro_Data_Buf[3]);
	Gyro_Value_Pitch   = (INT16S)( (Gyro_Data_Buf[4]<<8) | Gyro_Data_Buf[5]);
	Gyro_Value_Yaw     = (INT16S)( (Gyro_Data_Buf[6]<<8) | Gyro_Data_Buf[7]);	
//获得净角速度	
	Gyro_NetValue_Yaw   = Gyro_Value_Yaw   - Gyro_Netural_Yaw ;
    Gyro_NetValue_Roll  = Gyro_Value_Roll  - Gyro_Netural_Roll;
    Gyro_NetValue_Pitch = Gyro_Value_Pitch - Gyro_Netural_Pitch; 
//Yaw 对偏航直接积分
    Raw_Integral_Yaw +=  Gyro_NetValue_Yaw; 	//未考虑加速度计修正的积分值
    Fix_Integral_Yaw +=  Gyro_NetValue_Yaw;     //加速度计修正后的积分值	
//Yaw偏航对横滚俯仰轴的影响,耦合
    coupling_pitch  = Fix_Integral_Pitch / 4096L;
    coupling_pitch *= Gyro_NetValue_Yaw;
    coupling_pitch *= 11L;  //125  				耦合 Parameter_AchsKopplung1
    coupling_pitch /= 2048L;

	coupling_roll = Fix_Integral_Roll / 4096L;
    coupling_roll *= Gyro_NetValue_Yaw;
    coupling_roll *= 11L;//125    耦合 Parameter_AchsKopplung1
    coupling_roll /= 2048L;
//耦合俯仰横滚轴,航向影响

    Gyro_NetValue_Roll +=  coupling_pitch;     //+
	Gyro_NetValue_Roll += ( coupling_roll * 64 )/512L; 	//Parameter_AchsGegenKopplung1
	
	Gyro_NetValue_Pitch -= coupling_roll;
    Gyro_NetValue_Pitch -= ( coupling_pitch * 64 )/512L;   //Parameter_AchsGegenKopplung1

//对俯仰横滚轴进行积分	
    Raw_Integral_Roll +=  Gyro_NetValue_Roll;
    Fix_Integral_Roll +=  Gyro_NetValue_Roll - LageKorrekturRoll; //减去偏差（实时加速度误差。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。）   
	
	Raw_Integral_Pitch += Gyro_NetValue_Pitch;
    Fix_Integral_Pitch += Gyro_NetValue_Pitch - LageKorrekturNick; //减去偏差（实时加速度误差。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。）
//由于陀螺仪量程较大，因此不再判断超量情况	
}



/*******************************************************************************************
ITG3200 Data Ready Interrupt 1ms 执行一次 执行时间约 xxxus
********************************************************************************************/
void EXTI2_IRQHandler(void)
{
//	GPIOC->BSRR = GPIO_Pin_6|GPIO_Pin_7; //示波器测速用
	
	Gyro_Integral();  					 //耗时xxus, 对陀螺仪进行积分，并且获得第五次净值
	Gyro_Rdy_Signal++;

//	GPIOC->BRR = GPIO_Pin_6|GPIO_Pin_7;  //示波器测速用
	EXTI->PR = EXTI_Line2; //清除标志位
}








