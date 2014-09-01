#ifndef _I2C_DRIVER_H_
#define _I2C_DRIVER_H_

void I2C_GPIO_Configuration(void);
void I2C_delay(void);
BOOLEAN I2C_Start(void) ;
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);
BOOLEAN I2C_WaitAck(void);   
void I2C_SendByte(INT8U SendByte);
INT8U I2C_ReceiveByte(void);


#endif




