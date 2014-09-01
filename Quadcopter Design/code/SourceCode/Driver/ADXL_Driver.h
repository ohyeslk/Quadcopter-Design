#ifndef _ADXL345_DRIVER_H_
#define _ADXL345_DRIVER_H_

BOOLEAN ADXL_Read_Multi_Driver(INT8U Reg_Pnt,INT8U Num_Byte,INT8U *Buffer);
BOOLEAN ADXL_WriteReg_Driver(INT8U Register,INT8U Parameter);
BOOLEAN ADXL_Read_Driver(INT8U Register,INT8U *Data);
void ADXL345_Driver_Init(void);

#endif



