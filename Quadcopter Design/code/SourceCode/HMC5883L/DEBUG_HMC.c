#include "HMC5883L.h"

#ifdef HMC_DEBUG

#include "GlobalConfig.h"
#include "stdio.h"
void HMC_Debug(ERROR_SENSOR ErrorCode,void *p);

void HMC_Debug(ERROR_SENSOR ErrorCode,void *p)
{
	p = p;
	switch(ErrorCode)
	{
		case HMC_ID_ERROR:
			printf("HMC_ID_ERROR\r\n");
			break;
		case HMC_TIMEOUT:	
			printf("WaitRdyTimeOut\r\n");
			break;
		case HMC_AXIS_DAT:
/*		printf("X=%0x,%0x  ",((INT8U *)p)[0],((INT8U *)p)[1]);
		printf("Z=%0x,%0x  ",((INT8U *)p)[2],((INT8U *)p)[3]);
		printf("Y=%0x,%0x\r\n",((INT8U *)p)[4],((INT8U *)p)[5]);  */ 
			printf("X=%d,",		(INT16S)( ((INT8U *)p)[0]<<8|((INT8U *)p)[1]) );
			printf("Z=%d,",		(INT16S)( ((INT8U *)p)[2]<<8|((INT8U *)p)[3]) );
			printf("Y=%d\r\n",	(INT16S)( ((INT8U *)p)[4]<<8|((INT8U *)p)[5]) );	 
			break;
		case HMC5883L_BREAK:
			printf("HMC5883L_Broken\r\n");
			break;
		case HMC5883L_NICE:
			printf("HMC_Device_OK\r\n");
			break;
		default:
			break;
	}	
}		
#endif


