#ifndef _PPM_SIGNAL_H_
#define _PPM_SIGNAL_H_

//#define PPM_DEBUG       //Enable Debug



#ifdef PPM_DEBUG
void PPM_Debug(INT8U Error_State, void *p);
#endif

void PPM_Capture_Init(void);



#define K_GIER 	3
#define K_GAS  	2
#define K_ROLL 	0
#define K_NICK  1


#define MAX_PPM_CHANNELS 6
#define FRAM_GAP_TIME    (45000/14)    //3214 About 10ms
#define MAX_CHANNEL_TIME ((9000+1000)/14) //about 2ms
#define MIN_CHANNEL_TIME ((4500-1000)/14) //About 1ms
#define HEAD_SYNC 1
#define PPM_DATA  0
#define MAX_PPM_ERROR 20


#define TIMER_OVERFLOW 0
#define PPM_MISSING    1


#define PPM_SIGNAL_MISSING 1
#define PPM_SIGNAL_NICE    0

#endif





