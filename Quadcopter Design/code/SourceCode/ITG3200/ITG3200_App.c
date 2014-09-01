


#include "GlobalConfig.h"
#include "ITG3200.h"
#include "stm32f10x.h"

volatile INT8U Gyro_Rdy_Signal = 0;     //�����Ǹ���5�α�־�ź�

INT32S Gyro_180_Inregral = 5898240/2L;  //ת��180�Ȼ���ֵ
INT8U  Gyro_Data_Buf[8]; 			   //�ݴ������Ƕ���

INT16S  Gyro_Netural_Yaw,   Gyro_Netural_Roll,   Gyro_Netural_Pitch;  //����������������
INT16S  Gyro_Value_Yaw,     Gyro_Value_Roll,     Gyro_Value_Pitch;    //����������δ���������ֵ
INT16S  Gyro_NetValue_Yaw,  Gyro_NetValue_Roll,  Gyro_NetValue_Pitch; //���������Ǿ����ֵ 

INT32S  Raw_Integral_Yaw=0,   Raw_Integral_Roll=0,   Raw_Integral_Pitch=0;  //δ���������������ǻ���,			 Yawδʹ��
INT32S  Fix_Integral_Yaw=0,   Fix_Integral_Roll=0,   Fix_Integral_Pitch=0;  //�����˼��ٶ������������ǻ���


INT32S Raw_Integral_Yaw_Tmp, Raw_Integral_Roll_Tmp, Raw_Integral_Pitch_Tmp;
INT32S Fix_Integral_Yaw_Tmp, Fix_Integral_Roll_Tmp, Fix_Integral_Pitch_Tmp; //�ݴ� �����ǻ���ֵ

INT32S LageKorrekturRoll=0, LageKorrekturNick = 0;         //�����ǲ���������256�β�����ִ��



ERROR_SENSOR Gyro_Set_Neutral(void)  //�趨������Ӧ���ڿ��������ж�ǰִ��
{
	INT16U  i,time;
	INT32S  roll_tmp=0,pitch_tmp=0,yaw_tmp = 0;
	ITG3200_ReadData(TEMPERATURE_H_REG,8,(INT8U*)Gyro_Data_Buf);//������Ҫ	
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

	if ( ( Gyro_Netural_Roll > 100  || Gyro_Netural_Roll < -100 ) || //�����������㳬����ֵ
		 ( Gyro_Netural_Pitch > 100 || Gyro_Netural_Pitch < -100 )||
		 ( Gyro_Netural_Yaw > 100   || Gyro_Netural_Yaw < -100   ) )
	{	return 1; }
	return 0;
}


/************************************************************************************
1ms ִ��һ�� ��� ԭ�����ǻ���ֵ��������Ļ���ֵ���� ���ٶȾ�ֵ 
���������жϺ�������
*************************************************************************************/ 


void Gyro_Integral(void) //Test Speed 
{
	INT32S coupling_pitch, coupling_roll;	
// �����������ݼ�ȥ��ֵ���õ�ʵ�ʵĽ����ʵı���
	ITG3200_ReadData(TEMPERATURE_H_REG,8,(INT8U*)Gyro_Data_Buf); //Read All 8 Register
//	Gyro_Value_Tmp = (INT16S)( (Gyro_Data_Buf[0]<<8) | Gyro_Data_Buf[1]);
	Gyro_Value_Roll    = (INT16S)( (Gyro_Data_Buf[2]<<8) | Gyro_Data_Buf[3]);
	Gyro_Value_Pitch   = (INT16S)( (Gyro_Data_Buf[4]<<8) | Gyro_Data_Buf[5]);
	Gyro_Value_Yaw     = (INT16S)( (Gyro_Data_Buf[6]<<8) | Gyro_Data_Buf[7]);	
//��þ����ٶ�	
	Gyro_NetValue_Yaw   = Gyro_Value_Yaw   - Gyro_Netural_Yaw ;
    Gyro_NetValue_Roll  = Gyro_Value_Roll  - Gyro_Netural_Roll;
    Gyro_NetValue_Pitch = Gyro_Value_Pitch - Gyro_Netural_Pitch; 
//Yaw ��ƫ��ֱ�ӻ���
    Raw_Integral_Yaw +=  Gyro_NetValue_Yaw; 	//δ���Ǽ��ٶȼ������Ļ���ֵ
    Fix_Integral_Yaw +=  Gyro_NetValue_Yaw;     //���ٶȼ�������Ļ���ֵ	
//Yawƫ���Ժ���������Ӱ��,���
    coupling_pitch  = Fix_Integral_Pitch / 4096L;
    coupling_pitch *= Gyro_NetValue_Yaw;
    coupling_pitch *= 11L;  //125  				��� Parameter_AchsKopplung1
    coupling_pitch /= 2048L;

	coupling_roll = Fix_Integral_Roll / 4096L;
    coupling_roll *= Gyro_NetValue_Yaw;
    coupling_roll *= 11L;//125    ��� Parameter_AchsKopplung1
    coupling_roll /= 2048L;
//��ϸ��������,����Ӱ��

    Gyro_NetValue_Roll +=  coupling_pitch;     //+
	Gyro_NetValue_Roll += ( coupling_roll * 64 )/512L; 	//Parameter_AchsGegenKopplung1
	
	Gyro_NetValue_Pitch -= coupling_roll;
    Gyro_NetValue_Pitch -= ( coupling_pitch * 64 )/512L;   //Parameter_AchsGegenKopplung1

//�Ը����������л���	
    Raw_Integral_Roll +=  Gyro_NetValue_Roll;
    Fix_Integral_Roll +=  Gyro_NetValue_Roll - LageKorrekturRoll; //��ȥƫ�ʵʱ���ٶ���������������������������������������������������������������   
	
	Raw_Integral_Pitch += Gyro_NetValue_Pitch;
    Fix_Integral_Pitch += Gyro_NetValue_Pitch - LageKorrekturNick; //��ȥƫ�ʵʱ���ٶ���������������������������������������������������������������
//�������������̽ϴ���˲����жϳ������	
}



/*******************************************************************************************
ITG3200 Data Ready Interrupt 1ms ִ��һ�� ִ��ʱ��Լ xxxus
********************************************************************************************/
void EXTI2_IRQHandler(void)
{
//	GPIOC->BSRR = GPIO_Pin_6|GPIO_Pin_7; //ʾ����������
	
	Gyro_Integral();  					 //��ʱxxus, �������ǽ��л��֣����һ�õ���ξ�ֵ
	Gyro_Rdy_Signal++;

//	GPIOC->BRR = GPIO_Pin_6|GPIO_Pin_7;  //ʾ����������
	EXTI->PR = EXTI_Line2; //�����־λ
}








