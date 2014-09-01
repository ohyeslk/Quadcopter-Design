#include "GlobalConfig.h"
#include "ITG3200.h"
#include "stdio.h"


#ifdef ITG_DEBUG	

void ITG_Debug(ERROR_SENSOR ErrorCode,void *p);

void ITG_Debug(ERROR_SENSOR ErrorCode,void *p)
{
	INT8U i;	
	p = p;

	switch(ErrorCode)
	{
		case ITG3200_NICE:
			printf("GYRO_Device_OK\r\n");
			break;
		case ITG_TIMEOUT:	
			printf("WaitACKTimeOut\r\n");
			break;
		case ITG3200_DAT:
/*		printf("X=%0x,%0x  ",((INT8U *)p)[0],((INT8U *)p)[1]);
		printf("Z=%0x,%0x  ",((INT8U *)p)[2],((INT8U *)p)[3]);
		printf("Y=%0x,%0x\r\n",((INT8U *)p)[4],((INT8U *)p)[5]);  */ 
		#ifdef ITG_DEBUG_WAVE
			USART_Send_Buffer(0x7F);
			USART_Send_Buffer(0x7F);
			USART_Send_Buffer(0xEF);
			USART_Send_Buffer( ((INT8U *)p)[2] );
			USART_Send_Buffer( ((INT8U *)p)[3] );
			USART_Send_Buffer( ((INT8U *)p)[4] );
			USART_Send_Buffer( ((INT8U *)p)[5] );
			USART_Send_Buffer( ((INT8U *)p)[6] );
			USART_Send_Buffer( ((INT8U *)p)[7] );
			for (i=0;i<40;i++)USART_Send_Buffer( 0x00 );				
			USART_Send_Buffer( 0x7F+0x7F+0xEF+((INT8U *)p)[2]+((INT8U *)p)[3]+
											  ((INT8U *)p)[4]+((INT8U *)p)[5]+
											  ((INT8U *)p)[6]+((INT8U *)p)[7] );		
		#else
			printf("T=%d,",		(INT16S)( ((INT8U *)p)[0]<<8|((INT8U *)p)[1]) );
			printf("X=%d,",		(INT16S)( ((INT8U *)p)[2]<<8|((INT8U *)p)[3]) );
			printf("Y=%d,",		(INT16S)( ((INT8U *)p)[4]<<8|((INT8U *)p)[5]) );
			printf("Z=%d\r\n",	(INT16S)( ((INT8U *)p)[6]<<8|((INT8U *)p)[7]) );
		#endif		 
			break;
		default:
			break;
	}	
}		
#endif





