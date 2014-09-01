#include "stm32f10x.h"
#include "GlobalConfig.h"
#include "PPM_Signal.h"


#define ACC_AMPLIFY 12

#define LONG_FIX_TERM 128

#define MAX_GAS 160
#define MIN_GAS 20

INT16S StickGier = 0,StickNick = 0, StickRoll = 0,StickGas = 0;  //杆量
INT16S MaxStickNick,MaxStickRoll;  //历史最大杆量

extern   INT16S PPM_in[];


static INT32S Average_FixIntegral_Pitch,Average_FixIntegral_Roll;
static INT32S Average_RawIntegral_Pitch,Average_RawIntegral_Roll; 

static INT32S IntegralFehlerNick, IntegralFehlerRoll;
static INT32S ausgleichNick, ausgleichRoll;
static INT16S LageKorrekturNick, LageKorrekturRoll;  //补偿陀螺仪测量值,补偿中点
static INT8S  GyroAccAbgleich = 32;		//实验修改

static INT8S Driftkomp = 2; //每次改变中立点的限幅,可修改


INT16S GasMischanteil=10,GierMischanteil;

INT8U  Motor1,Motor2,Motor3,Motor4;
INT16S Motor_Tmp1,Motor_Tmp2,Motor_Tmp3,Motor_Tmp4;
		

extern INT16S Acc_Average_Roll;
extern INT16S Acc_Average_Pitch;
extern INT16S Acc_Average_Hoch;

extern INT32S Integral_Acc_Pitch;
extern INT32S Integral_Acc_Roll;
extern INT32S Integral_Acc_Hoch;

extern INT16S Acc_NetValue_Pitch;
extern INT16S Acc_NetValue_Roll;
extern INT16S Acc_NetValue_Hoch;

extern INT32S Integral_Acc_Z;
extern INT32S Integral_Acc_Pitch;
extern INT32S Integral_Acc_Roll;

extern INT16S Gyro_Netural_Pitch,Gyro_Netural_Roll,Gyro_Netural_Yaw;
extern INT16S Gyro_NetValue_Pitch, Gyro_NetValue_Roll, Gyro_NetValue_Yaw;

extern INT32S Fix_Integral_Pitch;
extern INT32S Fix_Integral_Roll;
extern INT32S Fix_Integral_Yaw;	

extern INT32S Raw_Integral_Pitch;
extern INT32S Raw_Integral_Roll;
extern INT32S Raw_Integral_Yaw;

extern INT32S Gyro_Acc_Factor;

extern INT32S Gyro_180_Inregral;

extern INT8U Long_Term_Cnt;

extern INT16S PPM_Net_Val[];
extern INT16S PPM_Netural[];
extern INT16S PPM_diff[];



#define WARP(X)			(X=((X+1)&0x03))
#define WINDOW_DEPTH 4


INT16S Acc_Smooth_Roll[WINDOW_DEPTH] = {0}, Acc_Smooth_Pitch[WINDOW_DEPTH] = {0}, Acc_Smooth_Hoch[WINDOW_DEPTH] = {0};
INT16S Acc_Roll_Tmp,Acc_Pitch_Tmp,Acc_Hoch_Tmp;
volatile INT8U Acc_Pnt_Roll = 0, Acc_Pnt_Pitch = 0,Acc_Pnt_Hoch = 0;


void Calculate_Remote_Value(void)
{
	PPM_Net_Val[K_GAS]   =  PPM_in[K_GAS]   - PPM_Netural[K_GAS];   //油门不会是负的,
	PPM_Net_Val[K_ROLL]  =  (PPM_in[K_ROLL]  - PPM_Netural[K_ROLL])/4;  //横滚量
	PPM_Net_Val[K_NICK] =   (PPM_in[K_NICK]  - PPM_Netural[K_NICK])/4;  //俯仰量
	PPM_Net_Val[K_GIER]  =  (PPM_in[K_GIER]  - PPM_Netural[K_GIER])/4;  //偏航量

//新老数据滤波混合，这里改变的应该是期望角位置  Stick_P = 2
	StickNick  = ( StickNick * 3 + PPM_Net_Val[K_NICK] * 1 ) / 4; //滤波
	StickNick += PPM_diff[K_NICK] * 4; //加入遥控器微分量
		
	StickRoll  = ( StickRoll * 3 + PPM_Net_Val[K_ROLL] * 1 ) / 4;   //滤波
	StickRoll += PPM_diff[K_ROLL] * 4; //加入遥控器微分量

	StickGier =  PPM_Net_Val[K_GIER];
	
	StickGas  =  PPM_Net_Val[K_GAS] ; 	
	if (StickGas < MIN_GAS) StickGas = MIN_GAS; //保证油门输入不会是负的
	if (StickGas > MAX_GAS-20 )	StickGas =  MAX_GAS-20 ;

	GasMischanteil =  StickGas;
		
//用此记录历史上的最大给杆量，如果给杆量很小，则Max数值会不断减小，用于在后面给陀螺仪积分做补偿时 
	if( abs(PPM_in[K_NICK]) > MaxStickNick ) MaxStickNick = abs( PPM_in[K_NICK] );
	else MaxStickNick--;
		
	if( abs(PPM_in[K_ROLL]) > MaxStickRoll ) MaxStickRoll = abs(PPM_in[K_ROLL]); 
	else MaxStickRoll--;
	
}
		
		
		
		
		
		
		
		
		
		
INT16S Filter(INT16S *value)
{
	INT8U  i;
	INT16S vs=0;
	for(i=0;i<WINDOW_DEPTH;i++)
	{
	 	vs+=value[i];
	}
	return vs/WINDOW_DEPTH;
}

INT32S tmp_long, tmp_long2;

void Attitude_Correction(void)
{
	Acc_Smooth_Roll[WARP(Acc_Pnt_Roll) ]  = Acc_Average_Roll;
	Acc_Smooth_Pitch[WARP(Acc_Pnt_Pitch)] = Acc_Average_Pitch;
	Acc_Smooth_Hoch[WARP(Acc_Pnt_Hoch) ]  = Acc_Average_Hoch;	 //滑动滤波
//计算20ms内平均加速度	
	Acc_Roll_Tmp  = Filter(Acc_Smooth_Roll);
	Acc_Pitch_Tmp = Filter(Acc_Smooth_Pitch);
	Acc_Hoch_Tmp  = Filter(Acc_Smooth_Hoch);	                 //这里可以输出 Acc 波形,与 Gyro进行匹配
//计算加速度和陀螺仪积分偏差，并衰减
	tmp_long  = ( Fix_Integral_Pitch / Gyro_Acc_Factor - (INT32S)Acc_Pitch_Tmp );
	tmp_long2 = ( Fix_Integral_Roll  / Gyro_Acc_Factor - (INT32S)Acc_Roll_Tmp ) ;   //瞬时偏差
//如果历史最大摇杆的量比较大,加速度影响较大
	if((MaxStickNick > 15) || (MaxStickRoll > 15))
	{	tmp_long  /= 3;	tmp_long2 /= 3;	}
//如果进行偏航运动，则在衰减3倍		
	if( abs(PPM_in[K_GIER]) > 25 )
	{	tmp_long  /= 3;	tmp_long2 /= 3; }
//限幅
#define AUSGLEICH 500
	if(tmp_long >  AUSGLEICH)   tmp_long  = AUSGLEICH;
	if(tmp_long < -AUSGLEICH)  	tmp_long  =-AUSGLEICH;
	if(tmp_long2 > AUSGLEICH) 	tmp_long2 = AUSGLEICH;
	if(tmp_long2 <-AUSGLEICH)   tmp_long2 =-AUSGLEICH;
//补偿陀螺仪积分
	Fix_Integral_Pitch -= tmp_long;
	Fix_Integral_Roll  -= tmp_long2;	
	
//对积分进行限幅操作,对机身翻转180°后额操作，用于空中翻转

	if(Fix_Integral_Pitch > Gyro_180_Inregral) 
    {Fix_Integral_Pitch = -(Gyro_180_Inregral - 100000L); Raw_Integral_Pitch = Fix_Integral_Pitch;} 
    if(Fix_Integral_Pitch <-Gyro_180_Inregral) 
    {Fix_Integral_Pitch =  (Gyro_180_Inregral - 100000L); Raw_Integral_Pitch = Fix_Integral_Pitch;} 	
	if( Fix_Integral_Roll > Gyro_180_Inregral )  			
    {Fix_Integral_Roll  = -(Gyro_180_Inregral - 100000L); Raw_Integral_Roll  = Fix_Integral_Roll; }
	if( Fix_Integral_Roll < -Gyro_180_Inregral )
	{Fix_Integral_Roll = (Gyro_180_Inregral - 100000L);	  Raw_Integral_Roll = Fix_Integral_Roll;  }	
//对补偿过的角速度的积分再一次积分	
	Average_FixIntegral_Pitch += Fix_Integral_Pitch;    
	Average_FixIntegral_Roll  += Fix_Integral_Roll;
//对未补偿的角速度的积分再一次积分	
	Average_RawIntegral_Pitch += Raw_Integral_Pitch;    
	Average_RawIntegral_Roll  += Raw_Integral_Roll;
//计算加速度计的积分	
	Integral_Acc_Pitch += (ACC_AMPLIFY * Acc_NetValue_Pitch); 		//这里是净值
	Integral_Acc_Roll  += (ACC_AMPLIFY * Acc_NetValue_Roll);  		//这里是净值
	Integral_Acc_Z     +=  Acc_NetValue_Hoch;  //这里减去中立点，中立点是float型。。。。。。。。。。。。。。。。。。。
}
	
	
void Long_Term_Fusion(void)
{
	static INT16S cnt = 0;
	static INT8S  last_n_p, last_n_n, last_r_p, last_r_n;      //控制流程
	static INT32S Fix_Inte_Pitch_History,Fix_Inte_Roll_History;//记录上一次值修正的结果

	INT32S tmp_pitch, tmp_roll;	//暂存放实时误差和长期融合误差
	INT16S tmp_yaw_stick; //Yaw操纵杆 PI计算数值	
	INT16S GierMischanteil,GasMischanteil;  //偏航混合数值，油门混和数值
//求积分平均值		
	Average_FixIntegral_Pitch  /= LONG_FIX_TERM;  //对修正后的积分的积分取平均值
	Average_FixIntegral_Roll   /= LONG_FIX_TERM;  //对修正后的积分的积分取平均值		
// 计算加速度计积分的作用, * GYROACC_FACTOR，为了和陀螺仪数据匹配			
	Integral_Acc_Pitch = ( Gyro_Acc_Factor* Integral_Acc_Pitch ) / LONG_FIX_TERM; // 对加速度积分取平均
	Integral_Acc_Roll  = ( Gyro_Acc_Factor * Integral_Acc_Roll ) / LONG_FIX_TERM; // 对加速度积分取平均
	Integral_Acc_Z     = ( Integral_Acc_Z / LONG_FIX_TERM );                     // 对加速度积分取平均,实际上没用到
/*不考虑补偿的陀螺仪积分的积分-加速度计积分/平衡项*/ 

	IntegralFehlerNick = ( Average_FixIntegral_Pitch - Integral_Acc_Pitch ); 	//长期融合的误差
	ausgleichNick      = IntegralFehlerNick / GyroAccAbgleich;     				//Pitch补偿量衰减
	IntegralFehlerRoll = ( Average_FixIntegral_Roll - Integral_Acc_Roll  ); 	//长期融合的误差
	ausgleichRoll      = IntegralFehlerRoll / GyroAccAbgleich;	   				//Roll补偿量衰减

	LageKorrekturNick = ausgleichNick / LONG_FIX_TERM; //后面的程序中还要进行修正
	LageKorrekturRoll = ausgleichRoll / LONG_FIX_TERM; //后面的程序中还要进行修正
//修正后的值加入到陀螺仪的积分中，可以认为这个参数是系统对于陀螺仪漂移的估计
//历史杆量或者偏航杆较大
	if( (MaxStickNick > 15) || (MaxStickRoll > 15) || (abs(PPM_in[K_GIER]) > 25) ) 
	{	
		LageKorrekturNick /= 2;		LageKorrekturRoll /= 2;	
	}

//陀螺漂移的确定,对陀螺仪漂移的估计过程
	Average_RawIntegral_Pitch /= LONG_FIX_TERM; //未修正的 陀螺仪积分的积分， 求平均数
	Average_RawIntegral_Roll  /= LONG_FIX_TERM; //未修正的 陀螺仪积分的积分， 求平均数
//是没有校正的陀螺仪积分 - 有校正的陀螺仪积分	确定积分误差	
	tmp_pitch  = Raw_Integral_Pitch - Fix_Integral_Pitch;
	tmp_roll = Raw_Integral_Roll  - Fix_Integral_Roll;          //长期误差
			
	IntegralFehlerNick = tmp_pitch;        	//获得误差,下面程序用到
	IntegralFehlerRoll = tmp_roll;			//获得误差,下面程序用到
//将差值加入到Mess_IntegralNick2和Mess_IntegralRoll2中
	Raw_Integral_Pitch -= IntegralFehlerNick; //实际上 = IntegralNick
	Raw_Integral_Roll  -= IntegralFehlerRoll; //将误差输入到未校正的值里面去

#define FEHLER_LIMIT  (LONG_FIX_TERM * 4)
#define FEHLER_LIMIT2 (LONG_FIX_TERM * 16)
#define BEWEGUNGS_LIMIT 20000
//对Pitch轴中立点进行修正
	cnt = 1;
	if( labs( Fix_Inte_Pitch_History - Average_FixIntegral_Pitch ) < BEWEGUNGS_LIMIT )//不是处于剧烈运动状态(悬停)
	{ 
		if( IntegralFehlerNick >  FEHLER_LIMIT2 ) 
		{
			if(last_n_p) //必须连续两次的误差都很大，才能进入下面的if语句
			{
				cnt += labs(IntegralFehlerNick) / FEHLER_LIMIT2; 
				ausgleichNick = IntegralFehlerNick / 8;
				if(ausgleichNick > 5000) 	ausgleichNick = 5000;
				LageKorrekturNick += ausgleichNick / LONG_FIX_TERM;
			} 
			else last_n_p = 1;
		} 
		else  last_n_p = 0;
		//负的情况		
		if(IntegralFehlerNick < -FEHLER_LIMIT2) 
		{
			if(last_n_n)
			{ 
				cnt += labs(IntegralFehlerNick) / FEHLER_LIMIT2; 
				ausgleichNick = IntegralFehlerNick / 8;
				if(ausgleichNick < -5000) 	ausgleichNick = -5000;
				LageKorrekturNick += ausgleichNick / LONG_FIX_TERM;
			} 
			else last_n_n = 1;
		} 
		else  last_n_n = 0;
	} 
	else	cnt = 0;
//误差过大时候，改变陀螺仪的常值误差，每次最多改变EE_Parameter.Driftkomp
	if(cnt > Driftkomp) cnt = Driftkomp;
	if(IntegralFehlerNick >  FEHLER_LIMIT)   Gyro_Netural_Pitch += cnt; //测量值>实际值, 认为中立点偏低
	if(IntegralFehlerNick < -FEHLER_LIMIT)   Gyro_Netural_Pitch -= cnt; //测量值<实际值, 认为中立点偏高

//对Roll轴中立点进行修正
		cnt = 1;
	if(labs(Fix_Inte_Roll_History - Average_FixIntegral_Roll) < BEWEGUNGS_LIMIT)
	{
		if(IntegralFehlerRoll >  FEHLER_LIMIT2) 
		{
			if(last_r_p) 
			{
				cnt += labs(IntegralFehlerRoll) / FEHLER_LIMIT2; 
				ausgleichRoll = IntegralFehlerRoll / 8;
				if(ausgleichRoll > 5000) ausgleichRoll = 5000;
				LageKorrekturRoll += ausgleichRoll / LONG_FIX_TERM;
			} 
			else last_r_p = 1;
		} 
		else  last_r_p = 0;
			//负的情况				
		if( IntegralFehlerRoll < -FEHLER_LIMIT2 ) 
		{
			if(last_r_n) 
			{
				cnt += labs(IntegralFehlerRoll) / FEHLER_LIMIT2; 
				ausgleichRoll = IntegralFehlerRoll / 8;
				if(ausgleichRoll < -5000) ausgleichRoll = -5000;
				LageKorrekturRoll += ausgleichRoll / LONG_FIX_TERM;
			}
			else last_r_n = 1;
		} 
		else  last_r_n = 0;
	} 
	else 	cnt = 0;
		
	if(cnt > Driftkomp) cnt = Driftkomp;  //限幅
	if(IntegralFehlerRoll >  FEHLER_LIMIT)   Gyro_Netural_Roll += cnt;
	if(IntegralFehlerRoll < -FEHLER_LIMIT)   Gyro_Netural_Roll -= cnt; //对Roll轴中立点进行修正
//Pitch和Roll修正完成，将上一次的值储存下来			
	Fix_Inte_Pitch_History = Average_FixIntegral_Pitch;      
	Fix_Inte_Roll_History =  Average_FixIntegral_Roll;      
/*数据清零 加速度计积分每一次都进行清零*/ 	
	Integral_Acc_Pitch = Integral_Acc_Roll = Integral_Acc_Z = 0; //Integral_Acc_Z 未使用
	Average_FixIntegral_Pitch = Average_FixIntegral_Roll = 0;
	Average_RawIntegral_Pitch = Average_RawIntegral_Roll  = 0; //未使用
	Long_Term_Cnt = 0;  //256采样周期才执行一次
}// 长期融合过程结束

	 INT16S sollGier;//标准偏航值
	 float IntegralFaktor = 0.0002; //陀螺仪I参数,可修改 0.00015
	 float GyroFaktor     = 0.06;   //陀螺仪P参数,可修改 
	 INT16S Gyro_PID_Pitch = 0,Gyro_PID_Roll = 0,Gyro_PID_Yaw = 0;	
	
	 INT32S  SummeNick = 0,SummeRoll = 0;  //马达输出PID积分量
	 INT16S  DiffNick  = 0,DiffRoll  = 0;  //马达输出PID微分量
	 float   Ki = 0.001; 	                 //PID积分比例系数
	 INT16S  pd_pitch = 0,pd_roll = 0;     //pitch roll PID输出
	 INT16S  tmp_axis = 0;		         //记录PD控制的上下限

	 INT8U DynamicStability = 50;			 //马达响应速度



void  Moto_PID_Controler(void)
{
#define STICK_YAW_P 6

	INT16S tmp_yaw_stick;
//对偏航进行指数变换，提高操作手感	
	tmp_yaw_stick  =  STICK_YAW_P * ( (INT32S)StickGier * abs(StickGier) ) / 512; 
    tmp_yaw_stick +=  (  STICK_YAW_P * StickGier  ) / 4; // 指数变换
    sollGier = tmp_yaw_stick; //标准偏航值,
//混入遥控偏航参数，实现偏航控制
    Fix_Integral_Yaw -= tmp_yaw_stick;    //对偏航进行人为修改，迫使飞行器认为自己已偏航,可以控制方向
    if( Fix_Integral_Yaw > 500000 )	Fix_Integral_Yaw = 500000;  
    if( Fix_Integral_Yaw <-500000 ) 	Fix_Integral_Yaw =-500000; //偏航积分限幅
//角速度和角度变化的归纳部分,对角速度做PI处理  
	Gyro_PID_Pitch = Fix_Integral_Pitch * IntegralFaktor  + Gyro_NetValue_Pitch * GyroFaktor;
	Gyro_PID_Roll  = Fix_Integral_Roll  * IntegralFaktor  + Gyro_NetValue_Roll  * GyroFaktor;	
    Gyro_PID_Yaw   = Gyro_NetValue_Yaw  * 0.01 + Fix_Integral_Yaw    * IntegralFaktor/4 ;	   //;;;;;;;;;;;;;;16;;;;;;;;;;;;;;;;;;;;;;;;;;
//对控制器输出进行幅度限制 
#define MAX_SENSOR  30000 		//ITG3200 约+-30000
    if(Gyro_PID_Pitch >  MAX_SENSOR) Gyro_PID_Pitch =  MAX_SENSOR;
    if(Gyro_PID_Pitch < -MAX_SENSOR) Gyro_PID_Pitch = -MAX_SENSOR;
    if(Gyro_PID_Roll >  MAX_SENSOR)  Gyro_PID_Roll  =  MAX_SENSOR;
    if(Gyro_PID_Roll < -MAX_SENSOR)  Gyro_PID_Roll  = -MAX_SENSOR;
    if(Gyro_PID_Yaw   >  MAX_SENSOR) Gyro_PID_Yaw   =  MAX_SENSOR;
    if(Gyro_PID_Yaw   < -MAX_SENSOR) Gyro_PID_Yaw   = -MAX_SENSOR;

//油门限幅
	if ( GasMischanteil > MAX_GAS - 25 ) 	GasMischanteil = MAX_GAS - 25;   //限制油门幅度
//计算标准偏航	
    GierMischanteil = Gyro_PID_Yaw - sollGier;//sollGier由遥控器产生，并进行指数变化
//对偏航数值进行限制，尽量避免最后计算出的四个电机的转速小于0
    if( GierMischanteil >  (GasMischanteil/2) )	GierMischanteil =  (GasMischanteil / 2); 
    if( GierMischanteil < -(GasMischanteil/2) ) GierMischanteil = -(GasMischanteil / 2); 
	
    if( GierMischanteil > (MAX_GAS - GasMischanteil) ) GierMischanteil =  (MAX_GAS - GasMischanteil); 
    if( GierMischanteil <-(MAX_GAS - GasMischanteil) ) GierMischanteil = -(MAX_GAS - GasMischanteil);

    if( GasMischanteil < 20 ) GierMischanteil = 0; /*油门本身如果太小了，就限制偏航为0*/ 
//俯仰轴  PI调节器，这个控制算法实际上是位置环为PI控制，速率环为P控制 
//计算pd上下限
	tmp_axis = (	(INT32S)DynamicStability * (INT32S)( GasMischanteil + abs(GierMischanteil)/2 ) ) / 64;
//计算Pitch轴
	DiffNick = Gyro_PID_Pitch - StickNick; //微分值
	
	SummeNick += Fix_Integral_Pitch * IntegralFaktor - StickNick ;   //积分值
	if(SummeNick >  16000) SummeNick =  16000;
    if(SummeNick < -16000) SummeNick = -16000;
	
	pd_pitch = DiffNick; //+ Ki * SummeNick; 	
    if( pd_pitch >  tmp_axis ) pd_pitch =  tmp_axis; 
    if( pd_pitch < -tmp_axis ) pd_pitch = -tmp_axis; 
//已经获得pd_pitch 计算Pitch轴
	DiffRoll = Gyro_PID_Roll - StickRoll ;	
	
	SummeRoll += Fix_Integral_Roll * IntegralFaktor - StickRoll;
    if(SummeRoll >  16000) SummeRoll =  16000;
    if(SummeRoll < -16000) SummeRoll = -16000;
	
    pd_roll = DiffRoll ;//+ Ki * SummeRoll;
    if(pd_roll >  tmp_axis) pd_roll =  tmp_axis; 
    if(pd_roll < -tmp_axis) pd_roll = -tmp_axis; 
//已经获得pd_roll   
//前(1)
	Motor_Tmp1 = GasMischanteil + pd_pitch - GierMischanteil - pd_roll;	
		if ( Motor_Tmp1 > MAX_GAS ) Motor_Tmp1 = MAX_GAS;
		if ( Motor_Tmp1 < MIN_GAS ) Motor_Tmp1 = MIN_GAS;
	Motor1  = (INT8U)Motor_Tmp1;	
//后(2)		
	Motor_Tmp2 = GasMischanteil + pd_pitch + GierMischanteil + pd_roll;	
		if ( Motor_Tmp2 > MAX_GAS ) Motor_Tmp2 = MAX_GAS;
		if ( Motor_Tmp2 < MIN_GAS ) Motor_Tmp2 = MIN_GAS;	
	Motor2  = (INT8U)Motor_Tmp2;
//左(3)
    Motor_Tmp3  = GasMischanteil - pd_pitch + GierMischanteil - pd_roll;		
		if ( Motor_Tmp3 > MAX_GAS ) Motor_Tmp3 = MAX_GAS;
		if ( Motor_Tmp3 < MIN_GAS ) Motor_Tmp3 = MIN_GAS;
	Motor3  = (INT8U)Motor_Tmp3;
//右(4)	
    Motor_Tmp4 = GasMischanteil - pd_pitch - GierMischanteil + pd_roll;	
		if ( Motor_Tmp4 > MAX_GAS ) Motor_Tmp4 = MAX_GAS;
		if ( Motor_Tmp4 < MIN_GAS ) Motor_Tmp4 = MIN_GAS;	
	Motor4  = (INT8U)Motor_Tmp4;
}


