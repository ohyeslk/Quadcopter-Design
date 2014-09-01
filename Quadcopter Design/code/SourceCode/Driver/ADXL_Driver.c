#include "GlobalConfig.h"
#include "stm32f10x.h"
#include "ADXL_Driver.h"





BOOLEAN ADXL_Read_Multi_Driver(INT8U Reg_Pnt,INT8U Num_Byte,INT8U *Buffer)
{
	INT8U i;
	(GPIOB->BRR) = GPIO_Pin_12;                             		//  CS Low To Start Transmation

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI2,  ((INT16U)(Reg_Pnt|0xC0)<<8)|0xFF ); 	//0xFF Dummy
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

	Buffer[0] = (INT8U)( (SPI2->DR) & 0x00FF );
	for(i=1; i<Num_Byte; i+=2)
	{
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
		SPI_I2S_SendData(SPI2,  ((INT16U)(Reg_Pnt|0xC0)<<8)|0xFF ); 	//0xFF Dummy
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

			Buffer[i]   = (INT8U)( (SPI2->DR) >>8 );
			Buffer[i+1] = (INT8U)( (SPI2->DR) & 0x00FF );
	}
	(GPIOB->BSRR) = GPIO_Pin_12;
	return 	0;
}	

BOOLEAN ADXL_Read_Driver(INT8U Register,INT8U *Data)
{
	(GPIOB->BRR) = GPIO_Pin_12;                             //  CS Low To Start Transmation 
 	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI2,  ((INT16U)(Register|0x80)<<8)|0xFF ); //0xFF Dummy
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);	
	
	(GPIOB->BSRR) = GPIO_Pin_12;                             // CS High To End Transmation 	
	
	(*Data) = (INT8U)( (SPI2->DR) & 0x00FF );                //Read Data
	return 0;
}
	
	
BOOLEAN ADXL_WriteReg_Driver(INT8U Register,INT8U Parameter)
{
	volatile INT16U temp;

	(GPIOB->BRR) = GPIO_Pin_12;                             //  CS Low To Start Transmation 
	
 	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI2,  ((INT16U)(Register&0x3F)<<8)|Parameter );
 	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

	temp = (SPI2->DR) ;

	(GPIOB->BSRR) = GPIO_Pin_12;  
	                           // CS High To End Transmation 

	return 0;
}



void ADXL345_Driver_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  	 //Enable GPIOB CLOCK
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);     //Enable SPI2 Clock 

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15; //PB13-SCLK & PB15-MOSI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;          //AF_PUSH_POLL
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;   			 //PB14-MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 //InPut Floating
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;   			 //PB12-CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 	 //Out Push Poll
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	(GPIOB->BSRR) = GPIO_Pin_12;                             //Set CS High 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //GPIOB_PIN_0 Acc INT1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
				     
/****************************************************************************************/	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //Full Duplex
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;        //CPOL = 1
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;       //CPHA = 1
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   //Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;  //1MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //MSB First
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE); 

}
	
	
	
	
	
	
	
