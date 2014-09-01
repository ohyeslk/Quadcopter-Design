/****************************************TIMER4_CH3******************************************/
#include "GlobalConfig.h"
#include "PPM_Signal.h"
#include "stm32f10x_conf.h"



volatile INT16S PPM_Net_Val[MAX_PPM_CHANNELS];//遥控量 - 初始量 = 净量，用于混合
volatile INT16S PPM_in[MAX_PPM_CHANNELS];	//遥控器 输入捕捉获得的数据
volatile INT16S PPM_diff[MAX_PPM_CHANNELS]; //本次遥控量与上一次遥控量的差值 
volatile INT16S PPM_Netural[MAX_PPM_CHANNELS];//遥控器输出中立点
volatile INT16S Tmp_Fixed;
volatile INT8U  PPM_UpDate = 0;


static INT16U Temp_Raw = 0;
volatile INT8U  PPM_Signal_Missing = 0; 
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //4 pre-emption priority & 4 subpriority
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //优先级不用太高
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}	




void TIM4_IRQHandler(void)
{ 
	Temp_Raw = (TIM4->CCR3)/14;         //Get Capture Value
	TIM4->CNT = 0x0000;
//处理定时器溢出的问题	
	if ( (TIM4->SR) &  TIM_FLAG_Update ) //Timer Over Flow About 15ms
	{
		PPM_Signal_Missing++;
		Pointer = 0;
		Singal_Sync = HEAD_SYNC;
		TIM4->SR = (INT16U)( ~(TIM_IT_Update|TIM_IT_CC3) ); //Clear Flag
	#ifdef  PPM_DEBUG
		PPM_Debug(TIMER_OVERFLOW,NULL);
	#endif	
		return;
	}
//遥控器10ms发送一帧，一帧6通道	
	if ( Singal_Sync != PPM_DATA )//Recieve Head Data -> 10ms
	{
		if ( (Temp_Raw > FRAM_GAP_TIME-600) && (Temp_Raw < FRAM_GAP_TIME+600) )
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
	else//Recieve Channel Data  -> 1~2ms
	{	
		if ( (Temp_Raw > MIN_CHANNEL_TIME) && (Temp_Raw < MAX_CHANNEL_TIME)	)
		{
			Tmp_Fixed = (3 * (PPM_in[Pointer]) + Temp_Raw) / 4;	
			
			if ( Tmp_Fixed > Temp_Raw+1 ) 	    Tmp_Fixed--; 
			else if ( Tmp_Fixed < Temp_Raw-1 )	Tmp_Fixed++; 
            
			if( PPM_Signal_Missing == 0 )   //If No Signal Error 
					PPM_diff[Pointer] = ((Tmp_Fixed - PPM_in[Pointer]) / 3) * 3;
			else PPM_diff[Pointer] = 0;		
			PPM_in[Pointer] = Tmp_Fixed; 	
		}
		else	
			PPM_Signal_Missing++; //Incorrect PPM Signal, May Be Signal Lost
		
		if ( (++Pointer) >= MAX_PPM_CHANNELS )
		{
			Singal_Sync = HEAD_SYNC; 	Pointer = 0;
		}
		
		if ( Pointer> 3 ) PPM_UpDate = 1; //		
	}
	TIM4->SR = (INT16U)(~TIM_IT_CC3); //Clear Pending Bits

	if ( PPM_Signal_Missing > 50) PPM_Signal_Missing = 50;
}


void PPM_Set_Netural(void)
{
	INT8U i;

LOOP:
	while( (PPM_Signal_Missing !=0) && (PPM_UpDate == 0) ) 	//Wait PPM Stable & Well To Recieve
		;
	PPM_UpDate = 0;
	for (i=0;i<	MAX_PPM_CHANNELS;i++)
	{
		if(	 PPM_in[i] >256 && PPM_in[i] < 700)
			PPM_Netural[i] = PPM_in[i];
		else
			goto LOOP;
	}
}




