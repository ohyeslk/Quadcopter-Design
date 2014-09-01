#ifndef _ADXL345_H_
#define _ADXL345_H_

/****************************Function Declare*********************************************/
/*****************************************************************************************/
BOOLEAN ADXL345_WriteReg(INT8U Register,INT8U Parameter);
BOOLEAN ADXL345_ReadReg(INT8U Register,INT8U *Data);
BOOLEAN ADXL35_ReadData(INT8U Reg_Pnt,INT8U Num_Byte,INT8U *Buffer);
/******************************************************************************************/

#define ADXL_DEBUG   //Enable ADXL_DEBUG Functions







/***********************************************************************************
Register Address Defines
***********************************************************************************/
#define	DEVICEID_REG		0x00/*  R  Val:11100101  Device ID*/	
// 0x01 ~ 0x1C Resvered
#define	THRESH_TAP_REG		0x1D/*  RW Val:00000000  Tap threshold */

#define	OFSX_REG			0x1E/*  RW Val:00000000  X-axis offset */ 
#define	OFSY_REG			0x1F/*  RW Val:00000000  Y-axis offset */
#define	OFSZ_REG			0x20/*  RW Val:00000000  Z-axis offset*/

#define DUR_REG				0x21/*  RW Val:00000000  Tap duration */
#define	LATENT_REG			0x22/*  RW Val:00000000  Tap latency */
#define	WINDOW_REG			0x23/*  RW Val:00000000  Tap window */
#define	THRESH_ACT_REG		0x24/*  RW Val:00000000  Activity threshold */
#define	THRESH_INACT_REG	0x25/*  RW Val:00000000  Inactivity threshold */

#define TIME_INACT_REG		0x26/*  RW Val:00000000  Inactivity time*/
#define ACT_INACT_CTL_REG	0x27/*  RW Val:00000000  Axis enable control for activity and inactivity detection */
#define	THRESH_FF_REG		0x28/*  RW Val:00000000  Free-fall threshold */
#define	TIME_FF_REG			0x29/*  RW Val:00000000  Free-fall time */
#define	TAP_AXES_REG		0x2A/*  RW Val:00000000  Axis control for single tap/double tap */

#define	ACT_TAP_STATUS		0x2B/*  R  Val:00000000  Source of single tap/double tap*/
#define	BW_RATE_REG			0x2C/*  RW Val:00001010  Data rate and power mode control */
#define POWER_CTL_REG		0x2D/*  RW Val:00000000  Power-saving features control */
#define INT_ENABLE_REG		0x2E/*  RW Val:00000000  Interrupt enable control */
#define INT_MAP_REG			0x2F/*  RW Val:00000000  Interrupt mapping control*/

#define INT_SOURCE_REG		0X30/*  R  Val:00000010  Source of interrupts */

#define DATA_FORMAT_REG		0x31/*  RW Val:00000000  Data format control*/

#define DATAX0_REG			0x32/*  R  Val:00000000  X-Axis Data 0 */
#define DATAX1_REG			0x33/*  R  Val:00000000  X-Axis Data 1*/
#define DATAY0_REG			0x34/*  R  Val:00000000  Y-Axis Data 0 */
#define DATAY1_REG			0x35/*  R  Val:00000000  Y-Axis Data 1 */
#define DATAZ0_REG			0x36/*  R  Val:00000000  Z-Axis Data 0 */
#define DATAZ1_REG			0x37/*  R  Val:00000000  Z-Axis Data 1 */

#define FIFO_CTL_REG		0x38/*  RW Val:00000000  FIFO control */
#define FIFO_STATUS_REG		0x39/*  R  Val:00000000  FIFO status */


/*1*******************************DEVICEID_REG*********************************/
//The DEVID register holds a fixed device ID code of 0xE5
#define ADXL345_ID 0xE5

/*2******************************THRESH_TAP_REG********************************/
//holds the threshold value for tap interrupts. 
#define MAX_THRESH_TAP 32

/*3******************************OFSX OFSY OFSZ********************************/
//set offset adjustments in complement format , scale factor of 15.6 mg/LSB
#define MAX_OFFSET	127 /* 2g */
#define MIN_OFFSET -128 /* -2g*/

/*4*********************************DUR_REG************************************/
//the maximum time that an event must be above the THRESH_TAP threshold to 
//qualify as a tap event.						625 us/LSB   0 = Disable
#define  TAP_DISABLE 0x00

/*5********************************LATENT_REG**********************************/
//representing the wait time from the detection of a tap event to the start 
//of the time window   1.25ms/LSB   0:disables the double tap function
#define DOUBLE_TAP_DISABLE 0x00

/*6********************************WINDOW_REG**********************************/
//representing the amount of time after the expiration of the latency time 
//(determined by the latent register) during which a second valid tap can begin.
//	1.25ms/LSB 0 disables the double tap function

/*7******************************THRESH_ACT_REG********************************/
//holds the threshold value for detecting activity     62.5 mg/LSB

/*8*****************************THRESH_INACT_REG*******************************/
//holds the threshold value for detecting inactivity     62.5 mg/LSB

/*9******************************TIME_INACT_REG********************************/
//representing the amount of time that acceleration must be less than the value
// in the THRESH_INACT register for inactivity to be declared.      1 sec/LSB

/*10***************************ACT_INACT_CTL_REG*******************************/
#define ACT_AC		(1<<7)
#define ACT_DC		(0<<7)
#define ACTX_EN		(1<<6)
#define ACTY_EN		(1<<5)
#define ACTZ_EN		(1<<4)

#define INACT_AC	(1<<3)
#define INACT_DC    (0<<3)
#define INACTX_EN	(1<<2)
#define INACTY_EN	(1<<1)
#define INACTZ_EN	(1<<0)

/*11******************************TIME_FF_REG********************************/
//representing the minimum time that the value of all axes must be less than 
//THRESH_FF to generate a free-fall interrupt.      5 ms/LSB
//Values between 100 ms and 350 ms (0x14 to 0x46) are recommended. 

/*12*****************************TAP_AXES_REG********************************/
//TAP_AXES Control Register
#define SUPPRESS (1<<3)
#define TAP_X_EN (1<<2)
#define	TAP_Y_EN (1<<1)	
#define	TAP_Z_EN (1<<0)

/*13*****************************TAP_AXES_REG********************************/
// indicate the first axis involved in a tap or activity event.  Read Only
#define ACT_X_SOURCE (1<<6)
#define ACT_Y_SOURCE (1<<5)
#define ACT_Z_SOURCE (1<<4)
#define ASLEEP       (1<<3)
#define TAP_X_SOURCE (1<<2) 
#define TAP_Y_SOURCE (1<<1) 
#define TAP_Z_SOURCE (1<<0) 

/*14*****************************BW_RATE_REG********************************/
// Low Power Mode & Select Band Width & OutPut Rate
/* Power Ctrl */
#define LOW_POWER			(1<<4)
#define NORMAL_OPERATION	(0<<4)
/* Band Width   xxxx.xx Hz*/
#define BAND_1600_00    0x0F     //OutPut Rate = 2*Bandwidth
#define BAND_0800_00    0x0E
#define BAND_0400_00	0x0D
#define BAND_0200_00	0x0C
#define BAND_0100_00	0x0B
#define BAND_0050_00	0x0A     //Default
#define BAND_0025_00	0x09
#define BAND_0012_50	0x08
#define BAND_0006_25    0x07
#define BAND_0003_13	0x06
#define BAND_0001_56	0x05
#define BAND_0000_78	0x04
#define BAND_0000_39	0x03
#define BAND_0000_20	0x02
#define BAND_0000_10	0x01
#define BAND_0000_05	0x00

/*15*****************************POWER_CTL_REG********************************/
//Main Power Ctrl Register
#define LINK_EN			(1<<5)	//Enable delays the start of  activity  
								//function untilinactivity is detected
#define AUTO_SLEEP_EN	(1<<4)

#define GO_MEASURE      (1<<3)
#define GO_STANDBY      (0<<3)

#define GO_SLEEP        (1<<2)
#define LEAVE_SLEEP     (0<<2)
/* the frequency of readings in sleep mode */
#define WAKEUP_FREG_8Hz ((0<<1)|(0<<0))
#define WAKEUP_FREG_4Hz ((0<<1)|(1<<0))
#define WAKEUP_FREG_2Hz ((1<<1)|(0<<0))
#define WAKEUP_FREG_1Hz ((1<<1)|(1<<0))

/*16*****************************INT_ENABLE_REG********************************/
//Enable INT
#define DATA_READY_EN (1<<7)
#define SINGLE_TAP_EN (1<<6)
#define DOUBLE_TAP_EN (1<<5)
#define ACTIVITY_EN   (1<<4)
#define INACTIVE_EN   (1<<3)
#define FREE_FALL_EN  (1<<2)
#define WATERMARK_EN  (1<<1)
#define OVERRUN_EN	  (1<<0)

/*16*****************************INT_MAP _REG********************************/
//Config Function Map On INT1 INT2 
#define DATA_READY_INT1 (0<<7)
#define SINGLE_TAP_INT1 (0<<6)
#define DOUBLE_TAP_INT1 (0<<5)
#define ACTIVITY_INT1   (0<<4)
#define INACTIVE_INT1   (0<<3)
#define FREE_FALL_INT1  (0<<2)
#define WATERMARK_INT1	(0<<1)
#define OVERRUN_INT1	(0<<0)

#define DATA_READY_INT2 (1<<7)
#define SINGLE_TAP_INT2 (1<<6)
#define DOUBLE_TAP_INT2 (1<<5)
#define ACTIVITY_INT2   (1<<4)
#define INACTIVE_INT2   (1<<3)
#define FREE_FALL_INT2  (1<<2)
#define WATERMARK_INT2	(1<<1)
#define OVERRUN_INT2	(1<<0)

/*18*****************************INT_SOURCE _REG********************************/
//Interrupt Flag
#define DATA_READY_FLAG (1<<7)  //Mask
#define SINGLE_TAP_FLAG (1<<6)	//Mask
#define DOUBLE_TAP_FLAG (1<<5)	//Mask	
#define ACTIVITY_FLAG   (1<<4)	//Mask
#define INACTIVE_FLAG	(1<<3)	//Mask
#define FREE_FALL_FLAG  (1<<2)	//Mask
#define WATERMARK_FLAG  (1<<1)	//Mask
#define OVERRUN_FLAG	(1<<0)	//Mask

/*17*****************************DATA_FORMAT_REG********************************/
//Control Data Format
#define FORCE_SELF_TEST (1<<7) //Force Enter Self Test Mode
#define SPI_3WIRE_MODE  (1<<6) //0 Select 4-wile SPI Mode (default)

#define INT_ACTIVE_HIGH (0<<5) //Rising Edge 
#define INT_ACTIVE_LOW  (1<<5) //Falling Edge

#define RESERVED	    (0<<4) //Reserved

#define FULL_RES_MODE	(1<<3) //Full Resolution Mode (0:fixed 10bit)

#define DATA_LEFT_JUST	(1<<2) //Data Format left adjust
#define DATA_RIGTH_JUST (0<<2) //Data Format rigth adjust

#define G_RANGE_2G      ((0<<1)|(0<<0))	 //+- 2g range	(default)
#define G_RANGE_4G      ((0<<1)|(1<<0))	 //+- 4g range	
#define G_RANGE_8G      ((1<<1)|(0<<0))	 //+- 8g range	
#define G_RANGE_16G     ((1<<1)|(1<<0))	 //+-16g range

/*18*****************************FIFO_CTL_REG*********************************/
//Control FIFO Behaviour
#define FIFO_MODE_BYPASS  ((0<<7)|(0<<6)) //Not Use FIFO
#define FIFO_MODE_FIFO    ((0<<7)|(1<<6))
#define FIFO_MODE_STREAM  ((1<<7)|(0<<6))
#define FIFO_MODE_TRIGGER ((1<<7)|(1<<6))

#define TRIGGER_LINK_INT1 (0<<5)    //OutPut Trigger Event Map On INT1
#define TRIGGER_LINK_INT2 (1<<5)	//OutPut Trigger Event Map On INT2

#define SAMPLE_BITS 16//define how many FIFO entries are needed for Trigger Interrupt

/*18*****************************FIFO_STATUS _REG*********************************/
//FIFO State   read only
#define FIFO_TRIG_FLAG (1<<7) // Event Flag,For Read Mask
#define RESERVED_BIT   (0<<6) 
#define FIFO_ENTRIES   (0x3F) //Number of Datas In FIFO,For Read Mask

/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
/*********************************************************************************/
/*  For Debug Error */
#define ADXL345_NICE     0
#define ADXL345_ID_ERROR 1
#define ADXL345_AXIS_DAT 2
#define ADXL345_TIMEOUT  3
#define ADXL345_BRRAK    4
#define PRINT_ENTER      5
//#define ADXL345_ID     0xE5   /* Been Defined Formal */

#endif	




