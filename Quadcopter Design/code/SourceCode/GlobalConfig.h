#ifndef _GlobalConfig_h_
#define _GlobalConfig_h_

#include <stdlib.h> //Use For Math ABS Purpuse
//Integer Typedef 
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef INT8U          ERROR_SENSOR;
typedef INT8U          ERROR_PPM;

#define NULL 		   (void *)0

#define FALSE          0   //boolean 
#define TRUE           1   //boolean 


#endif

