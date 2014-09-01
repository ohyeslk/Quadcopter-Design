/****************************************TIMER4_CH3******************************************/
#include "GlobalConfig.h"
#include "PPM_Signal.h"
#include "stm32f10x_conf.h"



volatile INT16S PPM_in[MAX_PPM_CHANNELS];


static INT16U PPM_RawData[MAX_PPM_CHANNELS] = {0};
static INT16U Temp_Raw = 0;
static INT8U  PPM_Signal_Missing = 0; 
static INT8U  Singal_Sync        = HEAD_SYNC;
static INT8U  Pointer            = 0;

void PPM_Capture_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,  ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,   ENABLE);  
//GPIO Init	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;	         			//PB8 Timer4 CH3
  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);		    				//A端口 
//Timer Init
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;            //每次输入捕捉后都产生中断
	TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	
	TIM_ITConfig(TIM4, TIM_IT_CC3, ENABLE);                  //Enable TIM Capture Compare 3 Interrupt source	
//Timer Count Speed Prescaler	
	TIM_PrescalerConfig(TIM4, 15, TIM_PSCReloadMode_Immediate); //
//Set CNT To 0	
	TIM4->CNT = 0x0000;
	TIM_Cmd(TIM4, ENABLE);
	
 

//Interrupt Enable
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //优先级不用太高
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}	

ERROR_PPM  Load_PPM_Value(INT16U *PPM_In,INT8U Chanel_Nums)
{
	if ( PPM_Signal_Missing > MAX_PPM_ERROR )
	{
		return (PPM_SIGNAL_MISSING);
	}
	return (PPM_SIGNAL_NICE);
}



void TIM4_IRQHandler(void)
{ 
	Temp_Raw = TIM4->CCR3;         //Get Capture Value
	TIM4->CNT = 0x0000;

	
	if ( (TIM4->SR) &  TIM_FLAG_Update ) //Timer Over Flow About 15ms
	{
		PPM_Signal_Missing+=5;
		TIM4->SR = (INT16U)( ~(TIM_IT_Update|TIM_IT_CC3) ); //Clear Flag
	#ifdef  PPM_DEBUG
		PPM_Debug(TIMER_OVERFLOW,NULL);
	#endif	
		return;
	}
	
	if ( Singal_Sync != 0 )
	{
		if ( (Temp_Raw > FRAM_GAP_TIME-8000) && (Temp_Raw < FRAM_GAP_TIME+8000) )
		{
			Singal_Sync = Pointer = 0;  //One Cycle Finished
			if ( PPM_Signal_Missing != 0 )
				PPM_Signal_Missing--;         //If OK,
		}
		else
		{
			PPM_Signal_Missing++; //Incorrect PPM Signal, May Be Signal Lost
		}				
	}
	else
	{	
		if ( (Temp_Raw > MIN_CHANNEL_TIME) && (Temp_Raw < MAX_CHANNEL_TIME)	)
		{
			PPM_RawData[Pointer] = Temp_Raw;
			
		}
		else	
			PPM_Signal_Missing++; //Incorrect PPM Signal, May Be Signal Lost
		
		if ( (++Pointer) >= MAX_PPM_CHANNELS )
		{
			Singal_Sync = HEAD_SYNC;//1
			Pointer = 0;
//		#ifdef  PPM_DEBUG
//			PPM_Debug(PPM_DATA,(void *)PPM_RawData);
//		#endif	
		}	
	}
//	#ifdef  PPM_DEBUG
//		PPM_Debug(PPM_MISSING,(void *)(&PPM_Signal_Missing));
//	#endif			
	
	TIM4->SR = (INT16U)(~TIM_IT_CC3); //Clear Pending Bits
}


void Test_Load_PPM_Thr(INT16S * Buffer)
{
	  (*Buffer) =  ( (*Buffer)*3 +  ((INT16S)PPM_RawData[2]- 4500) )/4;
}
