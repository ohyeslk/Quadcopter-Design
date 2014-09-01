#include "GlobalConfig.h"
#include "HMC5883L.h"
#include "I2C_Driver.h"

BOOLEAN HMC5883L_WriteReg(INT8U Register,INT8U Parameter);  		//Send 0x3C + address +Parameter
BOOLEAN HMC5883L_ReadData(INT8U Reg_Pnt,INT8U Num_Byte,INT8U *Buffer);	//Send 0x3C+Pnt & 0x3D + Num_Byte & Return Bytes
INT8U HMC5883L_ReadReg(INT8U Register);  		     		//send 0x3D & Return Register Value
extern void HMC_Debug(ERROR_SENSOR ErrorCode,void *p);

BOOLEAN HMC5883L_WriteReg(INT8U Register,INT8U Parameter)
{
	if (!I2C_Start()) 
		return FALSE; 
	I2C_SendByte(HMCWRITE_ADD);//Send WriteAddress
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

INT8U HMC5883L_ReadReg(INT8U Register)
{
	INT8U tmp;
	
	if (!I2C_Start()) 
		return FALSE;	
	I2C_SendByte(HMCWRITE_ADD);//Send WriteAddress For Set Register Pointer	
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	}
	I2C_SendByte(Register);
	I2C_WaitAck();
	
	if (!I2C_Start()) 
		return FALSE;	
	I2C_SendByte(HMCREAD_ADD);
	I2C_WaitAck();
	tmp = I2C_ReceiveByte();
	I2C_NoAck();
	I2C_Stop(); 	
	return (tmp);
}

BOOLEAN HMC5883L_ReadData(INT8U Reg_Pnt,INT8U Num_Byte,INT8U *Buffer)
{
	INT8U count;
	if (!I2C_Start()) 
		return FALSE;	
	I2C_SendByte(HMCWRITE_ADD);//Send WriteAddress For Set Register Pointer	
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	}
	I2C_SendByte(Reg_Pnt);
	I2C_WaitAck();
	
	if (!I2C_Start()) 
		return FALSE;	
	I2C_SendByte(HMCREAD_ADD);
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


ERROR_SENSOR HMC5883L_SelfTest(void)
{
	INT8U  buffer[3][2];//X Z Y Data
	INT8U  Device_ID[3];
	INT16U  sum[3]  =  {0};
	INT32U  timeout = 0;
	INT8U   i,j,state;
	
	HMC5883L_ReadData(IDEN_REG_A,3,Device_ID);
	if ( (Device_ID[0]!=IDRD_A) || (Device_ID[1]!=IDRD_B) || (Device_ID[2]!=IDRD_C) )
	{
		#ifdef HMC_DEBUG
			HMC_Debug(HMC_ID_ERROR,NULL);
		#endif	
		return (UNKNOWDEVICE);
	}

	HMC5883L_WriteReg(CONFG_REG_A, AVE_MEASURE_8|OUTRATE_7500|MODE_P_BIAS);
	HMC5883L_WriteReg(CONFG_REG_B, GAIN_0470);
	HMC5883L_WriteReg(MODE_REG, MODE_CONTINUE);	
	
	for (i=0; i< 10; i++)
	{
		timeout = 0;
		do      //Wait Device Data Readly
		{
			state =  HMC5883L_ReadReg(STATUS_REG);
			if ( (timeout++) > 0xFFF )  //About 70ms
			{
			#ifdef HMC_DEBUG
				HMC_Debug(HMC_TIMEOUT,NULL);
			#endif	
				return (HMC5883L_NO_RDY);
			}
		}while(!(state&RDY_MASK));
		
		HMC5883L_ReadData(DATA_X_MSB,0x06,(INT8U *)buffer);  //Send 0x3D 0x06  & return 6 Bytes
		#ifdef HMC_DEBUG
			HMC_Debug(HMC_AXIS_DAT,(void*)buffer);
		#endif				
		for(j=0;j<3;j++)
		{
			sum[j] += ( (buffer[j][0]<<8)|buffer[j][1] );
		}

		for (timeout=0; timeout<0xFfFFF; timeout++);
	}
	for (j=0; j<3; j++)
	{
		sum[j] = sum[j]/10;
		if ( (sum[j]<243)||(sum[j]>575) )
		{
		#ifdef HMC_DEBUG
			HMC_Debug(HMC5883L_BREAK,NULL);
		#endif			
			return (HMC5883L_BREAK);
		}
	}
	HMC5883L_WriteReg(CONFG_REG_A, AVE_MEASURE_8|OUTRATE_7500|MODE_NORMAL); //Exit Selftest Mode
	HMC5883L_WriteReg(MODE_REG, MODE_IDLE1);                                 //IDLE Mode	
	#ifdef HMC_DEBUG
		HMC_Debug(HMC5883L_NICE,NULL);
	#endif				
	return (HMC5883L_NICE);	
}



ERROR_SENSOR HMC5883L_Init(void)
{
	INT32U timeout;
	INT8U  buffer[3][2];//X Z Y Data
	INT8U  state;
	HMC5883L_WriteReg(CONFG_REG_A, AVE_MEASURE_8|OUTRATE_7500|MODE_NORMAL);
	HMC5883L_WriteReg(CONFG_REG_B, GAIN_0130);
	HMC5883L_WriteReg(MODE_REG, MODE_CONTINUE);	
	
	while(1)
	{
		timeout = 0;
		do      //Wait Device Data Readly
		{
			state =  HMC5883L_ReadReg(STATUS_REG);
			if ( (timeout++) > 0xFFFF )  //About 70ms
			{
			#ifdef HMC_DEBUG
				HMC_Debug(HMC_TIMEOUT,NULL);
			#endif	
				return (HMC5883L_NO_RDY);
			}
		}while(!(state&RDY_MASK));
		
		HMC5883L_ReadData(DATA_X_MSB,0x06,(INT8U *)buffer);  //Send 0x3D 0x06  & return 6 Bytes
		#ifdef HMC_DEBUG
			HMC_Debug(HMC_AXIS_DAT,(void*)buffer);
		#endif	

		for(timeout=0;timeout<0xFFFFF;timeout++);
	}
}		

