#include "GlobalConfig.h"
#include "ADXL345.h"
#include "stdio.h"

#ifdef ADXL_DEBUG

void ADXL_Debug(ERROR_SENSOR ErrorCode,void *p);

void ADXL_Debug(ERROR_SENSOR ErrorCode,void *p)
{
	p = p;
	switch(ErrorCode)
	{
		case ADXL345_NICE:
			printf("ADXL345_WorkWell\r\n");
			break;	
		case ADXL345_ID_ERROR:
			printf("ADXL35_ID_ERROR=%0x\r\n",(*(INT8U *)p));
			break;
		case ADXL345_AXIS_DAT:
/*			printf("X=%0x,%0x  ",	((INT8U *)p)[1],((INT8U *)p)[0]);
			printf("Y=%0x,%0x  ",	((INT8U *)p)[3],((INT8U *)p)[2]);
			printf("Z=%0x,%0x\r\n",	((INT8U *)p)[5],((INT8U *)p)[4]);    */
			printf("X=%d,",	(INT16S)(((INT8U *)p)[1]<<8|((INT8U *)p)[0]));
			printf("Y=%d,",	(INT16S)(((INT8U *)p)[3]<<8|((INT8U *)p)[2]));
			printf("Z=%d\r\n",(INT16S)(((INT8U *)p)[5]<<8|((INT8U *)p)[4]));	 
			break;
		case ADXL345_TIMEOUT:
			printf("ADXL345_DataTimeOut\r\n");
			break;
		case ADXL345_BRRAK:
			printf("ADXL345_Broken\r\n");
			break;
		case PRINT_ENTER:
			printf("\r\n\r\n");
			break;
		default:
			break;
	}	
}		
#endif
