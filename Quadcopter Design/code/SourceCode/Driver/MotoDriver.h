#ifndef _MOTODRIVER_H_
#define _MOTODRIVER_H_

void MOTO_I2C_GPIO_Configuration(void);
void MOTO_I2C_delay(void);
BOOLEAN MOTO_I2C_Start(void) ;
void MOTO_I2C_Stop(void);
void MOTO_I2C_Ack(void);
void MOTO_I2C_NoAck(void);
BOOLEAN MOTO_I2C_WaitAck(void);   
void MOTO_I2C_SendByte(INT8U SendByte);
INT8U MOTO_I2C_ReceiveByte(void);


#endif






