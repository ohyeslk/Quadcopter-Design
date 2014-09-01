#ifndef _HMC5883L_H_
#define _HMC5883L_H_

#define HMC_DEBUG		//Enable Debug

#define HMC5883L_NICE   0
#define HMC5883L_BREAK  1
#define HMC5883L_NO_RDY 2
#define UNKNOWDEVICE    3
#define HMC_TIMEOUT     4
#define HMC_AXIS_DAT    5
#define HMC_ID_ERROR    6

#define HMCWRITE_ADD    0x3C
#define HMCREAD_ADD     0x3D
#define HMC_7BIT_ADD    0x1F


/********************Address Register*************************/
#define CONFG_REG_A 0x00  	/* 0 R/W  */
#define CONFG_REG_B	0x01	/* 1 R/W  */

#define MODE_REG 	0x02	/* 2 R/W  */

#define DATA_X_MSB	0x03	/* 3 R    */
#define DATA_X_LSB	0x04	/* 4 R    */

#define DATA_Z_MSB	0x05	/* 5 R    */
#define DATA_Z_LSB	0x06	/* 6 R    */

#define DATA_Y_MSB	0x07	/* 6 R    */
#define DATA_Y_LSB	0x08	/* 8 R    */

#define STATUS_REG  0x09	/* 9 R    */
#define IDEN_REG_A	0x0A	/* 10 R   */
#define IDEN_REG_B	0x0B	/* 11 R   */
#define IDEN_REG_C	0x0C	/* 12 R   */



/**********************Register Configration ***************/ 
/* CRA       data output rate and measurement config     */
//Number of Samples Averaged Per Measurement
#define CRA_MASK       (~(1<<7))      //Bit7 Must Be 0 For Correct Operation
#define AVE_MEASURE_1  ((0<<6)|(0<<5))//1 Samples           
#define AVE_MEASURE_2  ((0<<6)|(1<<5))//2 Samples
#define AVE_MEASURE_4  ((1<<6)|(0<<5))//4 Samples
#define AVE_MEASURE_8  ((1<<6)|(1<<5))//8 Samples
//Data OutPut Rate (Continue)  Format (xx.xx) Hz
#define OUTRATE_0075   ((0<<4)|(0<<3)|(0<<2))//0.75HZ
#define OUTRATE_0150   ((0<<4)|(0<<3)|(1<<2))//1.5HZ
#define OUTRATE_0300   ((0<<4)|(1<<3)|(0<<2))//3HZ  
#define OUTRATE_0750   ((0<<4)|(1<<3)|(1<<2))//7.5HZ
#define OUTRATE_1500   ((1<<4)|(0<<3)|(0<<2))//15HZ_Defualt
#define OUTRATE_3000   ((1<<4)|(0<<3)|(1<<2))//30HZ
#define OUTRATE_7500   ((1<<4)|(1<<3)|(0<<2))//75HZ
#define RESERVED       ((1<<4)|(1<<3)|(1<<2))//Reserved
//Measurement Mode 
#define MODE_NORMAL    ((0<<1)|(0<<0))//Default Mode Normal
#define MODE_P_BIAS    ((0<<1)|(1<<0))//Positive bias configuration
#define MODE_N_BIAS    ((1<<1)|(0<<0))//Negative bias configuration
#define MODE_RESV      ((1<<1)|(1<<0))//Reserved

/* CRB           Setting the device gain     */
//Gain Control  Format (xx.xx) Ga
#define GAIN_0088   ((0<<7)|(0<<6)|(0<<5))//Gain:1370    +-0.88Ga
#define GAIN_0130	((0<<7)|(0<<6)|(1<<5))//Default:1090 +-1.3Ga
#define GAIN_0190	((0<<7)|(1<<6)|(0<<5))//Gain:820     +-1.9Ga
#define GAIN_0250	((0<<7)|(1<<6)|(1<<5))//Gain:660     +-0.88Ga
#define GAIN_0400	((1<<7)|(0<<6)|(0<<5))//Gain:440     +-0.88Ga
#define GAIN_0470	((1<<7)|(0<<6)|(1<<5))//Gain:390     +-0.88Ga
#define GAIN_0560	((1<<7)|(1<<6)|(0<<5))//Gain:330     +-0.88Ga
#define GAIN_0810	((1<<7)|(1<<6)|(1<<5))//Gain:230     +-0.88Ga
// Mask Must be 0
#define CRB_MASK    (~(((1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0)))

/*  MR         Operating mode of the device */
//Must Be 0
#define MR_MASK       (~((1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<3)|(1<<2)))
#define MODE_CONTINUE ((0<<1)|(0<<0))//Continue Measurement
#define MODE_SINGLE	  ((0<<1)|(1<<0))//Single Measurement(Default)
#define MODE_IDLE1 	  ((1<<1)|(0<<0))//IDLE
#define MODE_IDLE2    ((1<<1)|(1<<0))//IDLE

/* SR     Status Register  Read Only*/
#define SRMASK   0x03        
#define LOCK_MASK 0x02
#define RDY_MASK  0x01  //RDY PIN Show the sate of Status Register

/* IDR   Identification Register  Read Only**/
//Value
#define IDRD_A 'H'   //Identification ID
#define IDRD_B '4'   //Identification ID
#define IDRD_C '3'   //Identification ID


#endif



















