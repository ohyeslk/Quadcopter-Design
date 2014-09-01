#include "GlobalConfig.h"
#include "PPM_Signal.h"
#include "stdio.h"

#ifdef PPM_DEBUG


void PPM_Debug(INT8U Error_State, void *p)
{
	INT8U i;
	p = p;
	switch(Error_State)
	{
		case TIMER_OVERFLOW:
			printf("TimerOverFlow\r\n");
			break;
		case PPM_DATA:
//			for (i=0; i<MAX_PPM_CHANNELS; i++)
//			{
				printf("%d  ",((INT16U *)p)[0] );
//			}
			printf("\r\n");
			break;
		case PPM_MISSING:
			printf("Err_Cnt=%d\r\n",*((INT16U *)p));
			break;
		default: break;
	}		
}			

#endif
			


