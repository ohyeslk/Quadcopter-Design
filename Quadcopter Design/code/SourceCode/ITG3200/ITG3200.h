#ifndef _ITG3200_H_
#define _ITG3200_H_

#define ITG_DEBUG                 //Enable ITG3200_Dbug
//#define ITG_DEBUG_WAVE                //For Wave Syac

/***********************************************************************************
Register Address Defines
***********************************************************************************/
#define DEVICE_ADDRESS_REG  0x00  /* RW  Contain 7bit Device Address */
/* 0x01 ~ 0x14 Reserved */
#define SAMPLE_RATE_REG		0x15  /* RW  Sample Rate Divide */
#define SENSOR_SETING_REG	0X16  /* RW  Set Device Sensor Papameter */        
#define INT_CONFIG_REG      0x17  /* RW  Interrupt Config Of The Device */
/* 0x18 ~ 0x19 Reserved */
#define INT_STATUS_REG      0x1A  /* R   Contain Interrupt State */

#define TEMPERATURE_H_REG   0x1B  /* R   Contain Temperature High Byte */	
#define TEMPERATURE_L_REG   0x1C  /* R   Contain Temperature Low  Byte */

#define GYRO_XOUT_H_REG     0x1D  /* R   Contain X OUT High Byte */
#define GYRO_XOUT_L_REG    	0x1E  /* R   Contain X OUT Low  Byte */

#define GYRO_YOUT_H_REG     0x1F  /* R   Contain Y OUT High Byte */
#define GYRO_YOUT_L_REG    	0x20  /* R   Contain Y OUT Low  Byte */

#define GYRO_ZOUT_H_REG     0x21  /* R   Contain Z OUT High Byte */
#define GYRO_ZOUT_L_REG    	0x22  /* R   Contain Z OUT  Low  Byte */

#define POWER_MAMAGE_REG    0x3E  /* RW  Contain Power Management Seting Bits */

/***********************************************************************************
Register Bits Defines
***********************************************************************************/

/*****    DEVICE_ADDRESS_REG    *****/
#define	_7BIT_ADDDRESS  0x69
#define ITG_WRITE_ADDR  0xD2 
#define ITG_READ_ADDR   0xD3    //For I2C Read & Write

/*****    SAMPLE_RATE_REG    *****/
#define	PWON_RESET 0x00
#define SAMPLE_RATE 7    /* Sample rate divider: 0 to 255 */  
//	Fsample = Finternal / (divider+1), where Finternal is either 1kHz or 8kHz 

/*****    SENSOR_SETING_REG    *****/
//Bit7~Bit5 Reserved
//Full Scaler
#define FULL_SCALER_0000 ((0<<4)|(0<<3)) //Reserved
#define FULL_SCALER_0001 ((0<<4)|(1<<3)) //Reserved
#define FULL_SCALER_0010 ((1<<4)|(0<<3)) //Reserved
#define FULL_SCALER_2000 ((1<<4)|(1<<3)) //+-2000 */sec
//Digital Lowpass Filter
#define LOW_PASS_256Hz   ((0<<2)|(0<<1)|(0<<0)) //Internal Sample Rate 8K
#define LOW_PASS_188Hz   ((0<<2)|(0<<1)|(1<<0)) //Internal Sample Rate 1K
#define LOW_PASS_098Hz   ((0<<2)|(1<<1)|(0<<0)) //Internal Sample Rate 1K
#define LOW_PASS_042Hz   ((0<<2)|(1<<1)|(1<<0)) //Internal Sample Rate 1K
#define LOW_PASS_020Hz   ((1<<2)|(0<<1)|(0<<0)) //Internal Sample Rate 1K
#define LOW_PASS_010Hz   ((1<<2)|(0<<1)|(1<<0)) //Internal Sample Rate 1K
#define LOW_PASS_005Hz   ((1<<2)|(1<<1)|(0<<0)) //Internal Sample Rate 1K
#define LOW_PASS_RESEV   ((1<<2)|(1<<1)|(1<<0)) //Internal Sample Rate 1K

/*****    INT_CONFIG_REG     *****/
//configures the interrupt operation of the device
#define DEFAULT_CONFIG  0x00
#define INT_PIN_ACT_LOW (1<<7) //else Active High
#define INT_PIN_OD      (1<<6) //Else Push-Pull

#define LATCH_INT_EN    (1<<5) //Latch Until Interrupt Is Cleared ,Else 50us Pluse
#define LATCH_CLEAR_ANY (1<<4) //Any Register Read To Clear,Else Status Register Read Only 
#define BIT3_RESERVED   (0<<3) //Must Be 0
#define ITG_RDY_EN      (1<<2) //Enable interrupt when device is ready
#define BIT1_RESERVED   (0<<1) //Must Be 0
#define RAW_RDY_EN      (1<<0) //Enable interrupt when data is available 

/*****    INT_STATUS_REG    *****/
#define ITG_RDY_FLAG_MASK      (1<<2) //PLL Readly
#define RAW_DATA_RDY_FLAG_MASK (1<<0) //Raw data is ready
//Interrupt Status bits get cleared as determined by INT_CONFIG_REG
//Else Reserved

/*****    DATA_REG    *****/
//TEMPERATURE_H_REG   
// TEMPERATURE_L_REG 
// GYRO_XOUT_H_REG  
// GYRO_XOUT_L_REG  
// GYRO_YOUT_H_REG   
// GYRO_YOUT_L_REG 
// GYRO_ZOUT_H_REG  
// GYRO_ZOUT_L_REG      //Contain Valaid Data For Read

/*****    POWER_MAMAGE_REG    *****/
#define POWER_DEFAULT 0x00
#define ITG_RESET (1<<7) //Reset Device and internal registers 
#define ITG_SLEEP (1<<6) //Enter Sleep Mode
#define X_STANDBY (1<<5)
#define Y_STANDBY (1<<4)
#define Z_STANDBY (1<<3)

#define CLK_SEL_INTERNAL ((0<<2)|(0<<1)|(0<<0)) //Internal Oscillator  (Default)
#define CLK_SEL_PLL_X    ((0<<2)|(0<<1)|(1<<0)) //PLL with X Gyro reference 
#define CLK_SEL_PLL_Y    ((0<<2)|(1<<1)|(0<<0)) //PLL with Y Gyro reference 
#define CLK_SEL_PLL_Z    ((0<<2)|(1<<1)|(1<<0)) //PLL with Z Gyro reference 
#define CLK_SEL_PLL32768 ((1<<2)|(0<<1)|(0<<0)) //PLL with external 32.768kHz reference 
#define CLK_SEL_PLL19.2M ((1<<2)|(0<<1)|(1<<0)) //PLL with external 19.2MHz reference 
#define CLK_SEL_RESEV1   ((1<<2)|(1<<1)|(0<<0)) //Reserved
#define CLK_SEL_RESEV2   ((1<<2)|(1<<1)|(1<<0)) //Reserved
//t is highly recommended that the device is configured to use one of the 
//gyros (or an external clock) as the clock reference, due to the improved stability.




/***********************************************************************************
 Defines For Debug
***********************************************************************************/
#define ITG3200_NICE 0
#define ITG_TIMEOUT  1
#define ITG3200_DAT  2



#endif













