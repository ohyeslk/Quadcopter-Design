#include "stm32f10x.h"
#include "GlobalConfig.h"
#include "PPM_Signal.h"


#define ACC_AMPLIFY 12

#define LONG_FIX_TERM 128

#define MAX_GAS 160
#define MIN_GAS 20

INT16S StickGier = 0,StickNick = 0, StickRoll = 0,StickGas = 0;  //����
INT16S MaxStickNick,MaxStickRoll;  //��ʷ������

extern   INT16S PPM_in[];


static INT32S Average_FixIntegral_Pitch,Average_FixIntegral_Roll;
static INT32S Average_RawIntegral_Pitch,Average_RawIntegral_Roll; 

static INT32S IntegralFehlerNick, IntegralFehlerRoll;
static INT32S ausgleichNick, ausgleichRoll;
static INT16S LageKorrekturNick, LageKorrekturRoll;  //���������ǲ���ֵ,�����е�
static INT8S  GyroAccAbgleich = 32;		//ʵ���޸�

static INT8S Driftkomp = 2; //ÿ�θı���������޷�,���޸�


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
	PPM_Net_Val[K_GAS]   =  PPM_in[K_GAS]   - PPM_Netural[K_GAS];   //���Ų����Ǹ���,
	PPM_Net_Val[K_ROLL]  =  (PPM_in[K_ROLL]  - PPM_Netural[K_ROLL])/4;  //�����
	PPM_Net_Val[K_NICK] =   (PPM_in[K_NICK]  - PPM_Netural[K_NICK])/4;  //������
	PPM_Net_Val[K_GIER]  =  (PPM_in[K_GIER]  - PPM_Netural[K_GIER])/4;  //ƫ����

//���������˲���ϣ�����ı��Ӧ����������λ��  Stick_P = 2
	StickNick  = ( StickNick * 3 + PPM_Net_Val[K_NICK] * 1 ) / 4; //�˲�
	StickNick += PPM_diff[K_NICK] * 4; //����ң����΢����
		
	StickRoll  = ( StickRoll * 3 + PPM_Net_Val[K_ROLL] * 1 ) / 4;   //�˲�
	StickRoll += PPM_diff[K_ROLL] * 4; //����ң����΢����

	StickGier =  PPM_Net_Val[K_GIER];
	
	StickGas  =  PPM_Net_Val[K_GAS] ; 	
	if (StickGas < MIN_GAS) StickGas = MIN_GAS; //��֤�������벻���Ǹ���
	if (StickGas > MAX_GAS-20 )	StickGas =  MAX_GAS-20 ;

	GasMischanteil =  StickGas;
		
//�ô˼�¼��ʷ�ϵ����������������������С����Max��ֵ�᲻�ϼ�С�������ں���������ǻ���������ʱ 
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
	Acc_Smooth_Hoch[WARP(Acc_Pnt_Hoch) ]  = Acc_Average_Hoch;	 //�����˲�
//����20ms��ƽ�����ٶ�	
	Acc_Roll_Tmp  = Filter(Acc_Smooth_Roll);
	Acc_Pitch_Tmp = Filter(Acc_Smooth_Pitch);
	Acc_Hoch_Tmp  = Filter(Acc_Smooth_Hoch);	                 //���������� Acc ����,�� Gyro����ƥ��
//������ٶȺ������ǻ���ƫ���˥��
	tmp_long  = ( Fix_Integral_Pitch / Gyro_Acc_Factor - (INT32S)Acc_Pitch_Tmp );
	tmp_long2 = ( Fix_Integral_Roll  / Gyro_Acc_Factor - (INT32S)Acc_Roll_Tmp ) ;   //˲ʱƫ��
//�����ʷ���ҡ�˵����Ƚϴ�,���ٶ�Ӱ��ϴ�
	if((MaxStickNick > 15) || (MaxStickRoll > 15))
	{	tmp_long  /= 3;	tmp_long2 /= 3;	}
//�������ƫ���˶�������˥��3��		
	if( abs(PPM_in[K_GIER]) > 25 )
	{	tmp_long  /= 3;	tmp_long2 /= 3; }
//�޷�
#define AUSGLEICH 500
	if(tmp_long >  AUSGLEICH)   tmp_long  = AUSGLEICH;
	if(tmp_long < -AUSGLEICH)  	tmp_long  =-AUSGLEICH;
	if(tmp_long2 > AUSGLEICH) 	tmp_long2 = AUSGLEICH;
	if(tmp_long2 <-AUSGLEICH)   tmp_long2 =-AUSGLEICH;
//���������ǻ���
	Fix_Integral_Pitch -= tmp_long;
	Fix_Integral_Roll  -= tmp_long2;	
	
//�Ի��ֽ����޷�����,�Ի���ת180������������ڿ��з�ת

	if(Fix_Integral_Pitch > Gyro_180_Inregral) 
    {Fix_Integral_Pitch = -(Gyro_180_Inregral - 100000L); Raw_Integral_Pitch = Fix_Integral_Pitch;} 
    if(Fix_Integral_Pitch <-Gyro_180_Inregral) 
    {Fix_Integral_Pitch =  (Gyro_180_Inregral - 100000L); Raw_Integral_Pitch = Fix_Integral_Pitch;} 	
	if( Fix_Integral_Roll > Gyro_180_Inregral )  			
    {Fix_Integral_Roll  = -(Gyro_180_Inregral - 100000L); Raw_Integral_Roll  = Fix_Integral_Roll; }
	if( Fix_Integral_Roll < -Gyro_180_Inregral )
	{Fix_Integral_Roll = (Gyro_180_Inregral - 100000L);	  Raw_Integral_Roll = Fix_Integral_Roll;  }	
//�Բ������Ľ��ٶȵĻ�����һ�λ���	
	Average_FixIntegral_Pitch += Fix_Integral_Pitch;    
	Average_FixIntegral_Roll  += Fix_Integral_Roll;
//��δ�����Ľ��ٶȵĻ�����һ�λ���	
	Average_RawIntegral_Pitch += Raw_Integral_Pitch;    
	Average_RawIntegral_Roll  += Raw_Integral_Roll;
//������ٶȼƵĻ���	
	Integral_Acc_Pitch += (ACC_AMPLIFY * Acc_NetValue_Pitch); 		//�����Ǿ�ֵ
	Integral_Acc_Roll  += (ACC_AMPLIFY * Acc_NetValue_Roll);  		//�����Ǿ�ֵ
	Integral_Acc_Z     +=  Acc_NetValue_Hoch;  //�����ȥ�����㣬��������float�͡�������������������������������������
}
	
	
void Long_Term_Fusion(void)
{
	static INT16S cnt = 0;
	static INT8S  last_n_p, last_n_n, last_r_p, last_r_n;      //��������
	static INT32S Fix_Inte_Pitch_History,Fix_Inte_Roll_History;//��¼��һ��ֵ�����Ľ��

	INT32S tmp_pitch, tmp_roll;	//�ݴ��ʵʱ���ͳ����ں����
	INT16S tmp_yaw_stick; //Yaw���ݸ� PI������ֵ	
	INT16S GierMischanteil,GasMischanteil;  //ƫ�������ֵ�����Ż����ֵ
//�����ƽ��ֵ		
	Average_FixIntegral_Pitch  /= LONG_FIX_TERM;  //��������Ļ��ֵĻ���ȡƽ��ֵ
	Average_FixIntegral_Roll   /= LONG_FIX_TERM;  //��������Ļ��ֵĻ���ȡƽ��ֵ		
// ������ٶȼƻ��ֵ�����, * GYROACC_FACTOR��Ϊ�˺�����������ƥ��			
	Integral_Acc_Pitch = ( Gyro_Acc_Factor* Integral_Acc_Pitch ) / LONG_FIX_TERM; // �Լ��ٶȻ���ȡƽ��
	Integral_Acc_Roll  = ( Gyro_Acc_Factor * Integral_Acc_Roll ) / LONG_FIX_TERM; // �Լ��ٶȻ���ȡƽ��
	Integral_Acc_Z     = ( Integral_Acc_Z / LONG_FIX_TERM );                     // �Լ��ٶȻ���ȡƽ��,ʵ����û�õ�
/*�����ǲ����������ǻ��ֵĻ���-���ٶȼƻ���/ƽ����*/ 

	IntegralFehlerNick = ( Average_FixIntegral_Pitch - Integral_Acc_Pitch ); 	//�����ںϵ����
	ausgleichNick      = IntegralFehlerNick / GyroAccAbgleich;     				//Pitch������˥��
	IntegralFehlerRoll = ( Average_FixIntegral_Roll - Integral_Acc_Roll  ); 	//�����ںϵ����
	ausgleichRoll      = IntegralFehlerRoll / GyroAccAbgleich;	   				//Roll������˥��

	LageKorrekturNick = ausgleichNick / LONG_FIX_TERM; //����ĳ����л�Ҫ��������
	LageKorrekturRoll = ausgleichRoll / LONG_FIX_TERM; //����ĳ����л�Ҫ��������
//�������ֵ���뵽�����ǵĻ����У�������Ϊ���������ϵͳ����������Ư�ƵĹ���
//��ʷ��������ƫ���˽ϴ�
	if( (MaxStickNick > 15) || (MaxStickRoll > 15) || (abs(PPM_in[K_GIER]) > 25) ) 
	{	
		LageKorrekturNick /= 2;		LageKorrekturRoll /= 2;	
	}

//����Ư�Ƶ�ȷ��,��������Ư�ƵĹ��ƹ���
	Average_RawIntegral_Pitch /= LONG_FIX_TERM; //δ������ �����ǻ��ֵĻ��֣� ��ƽ����
	Average_RawIntegral_Roll  /= LONG_FIX_TERM; //δ������ �����ǻ��ֵĻ��֣� ��ƽ����
//��û��У���������ǻ��� - ��У���������ǻ���	ȷ���������	
	tmp_pitch  = Raw_Integral_Pitch - Fix_Integral_Pitch;
	tmp_roll = Raw_Integral_Roll  - Fix_Integral_Roll;          //�������
			
	IntegralFehlerNick = tmp_pitch;        	//������,��������õ�
	IntegralFehlerRoll = tmp_roll;			//������,��������õ�
//����ֵ���뵽Mess_IntegralNick2��Mess_IntegralRoll2��
	Raw_Integral_Pitch -= IntegralFehlerNick; //ʵ���� = IntegralNick
	Raw_Integral_Roll  -= IntegralFehlerRoll; //��������뵽δУ����ֵ����ȥ

#define FEHLER_LIMIT  (LONG_FIX_TERM * 4)
#define FEHLER_LIMIT2 (LONG_FIX_TERM * 16)
#define BEWEGUNGS_LIMIT 20000
//��Pitch���������������
	cnt = 1;
	if( labs( Fix_Inte_Pitch_History - Average_FixIntegral_Pitch ) < BEWEGUNGS_LIMIT )//���Ǵ��ھ����˶�״̬(��ͣ)
	{ 
		if( IntegralFehlerNick >  FEHLER_LIMIT2 ) 
		{
			if(last_n_p) //�����������ε����ܴ󣬲��ܽ��������if���
			{
				cnt += labs(IntegralFehlerNick) / FEHLER_LIMIT2; 
				ausgleichNick = IntegralFehlerNick / 8;
				if(ausgleichNick > 5000) 	ausgleichNick = 5000;
				LageKorrekturNick += ausgleichNick / LONG_FIX_TERM;
			} 
			else last_n_p = 1;
		} 
		else  last_n_p = 0;
		//�������		
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
//������ʱ�򣬸ı������ǵĳ�ֵ��ÿ�����ı�EE_Parameter.Driftkomp
	if(cnt > Driftkomp) cnt = Driftkomp;
	if(IntegralFehlerNick >  FEHLER_LIMIT)   Gyro_Netural_Pitch += cnt; //����ֵ>ʵ��ֵ, ��Ϊ������ƫ��
	if(IntegralFehlerNick < -FEHLER_LIMIT)   Gyro_Netural_Pitch -= cnt; //����ֵ<ʵ��ֵ, ��Ϊ������ƫ��

//��Roll���������������
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
			//�������				
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
		
	if(cnt > Driftkomp) cnt = Driftkomp;  //�޷�
	if(IntegralFehlerRoll >  FEHLER_LIMIT)   Gyro_Netural_Roll += cnt;
	if(IntegralFehlerRoll < -FEHLER_LIMIT)   Gyro_Netural_Roll -= cnt; //��Roll���������������
//Pitch��Roll������ɣ�����һ�ε�ֵ��������			
	Fix_Inte_Pitch_History = Average_FixIntegral_Pitch;      
	Fix_Inte_Roll_History =  Average_FixIntegral_Roll;      
/*�������� ���ٶȼƻ���ÿһ�ζ���������*/ 	
	Integral_Acc_Pitch = Integral_Acc_Roll = Integral_Acc_Z = 0; //Integral_Acc_Z δʹ��
	Average_FixIntegral_Pitch = Average_FixIntegral_Roll = 0;
	Average_RawIntegral_Pitch = Average_RawIntegral_Roll  = 0; //δʹ��
	Long_Term_Cnt = 0;  //256�������ڲ�ִ��һ��
}// �����ںϹ��̽���

	 INT16S sollGier;//��׼ƫ��ֵ
	 float IntegralFaktor = 0.0002; //������I����,���޸� 0.00015
	 float GyroFaktor     = 0.06;   //������P����,���޸� 
	 INT16S Gyro_PID_Pitch = 0,Gyro_PID_Roll = 0,Gyro_PID_Yaw = 0;	
	
	 INT32S  SummeNick = 0,SummeRoll = 0;  //������PID������
	 INT16S  DiffNick  = 0,DiffRoll  = 0;  //������PID΢����
	 float   Ki = 0.001; 	                 //PID���ֱ���ϵ��
	 INT16S  pd_pitch = 0,pd_roll = 0;     //pitch roll PID���
	 INT16S  tmp_axis = 0;		         //��¼PD���Ƶ�������

	 INT8U DynamicStability = 50;			 //�����Ӧ�ٶ�



void  Moto_PID_Controler(void)
{
#define STICK_YAW_P 6

	INT16S tmp_yaw_stick;
//��ƫ������ָ���任����߲����ָ�	
	tmp_yaw_stick  =  STICK_YAW_P * ( (INT32S)StickGier * abs(StickGier) ) / 512; 
    tmp_yaw_stick +=  (  STICK_YAW_P * StickGier  ) / 4; // ָ���任
    sollGier = tmp_yaw_stick; //��׼ƫ��ֵ,
//����ң��ƫ��������ʵ��ƫ������
    Fix_Integral_Yaw -= tmp_yaw_stick;    //��ƫ��������Ϊ�޸ģ���ʹ��������Ϊ�Լ���ƫ��,���Կ��Ʒ���
    if( Fix_Integral_Yaw > 500000 )	Fix_Integral_Yaw = 500000;  
    if( Fix_Integral_Yaw <-500000 ) 	Fix_Integral_Yaw =-500000; //ƫ�������޷�
//���ٶȺͽǶȱ仯�Ĺ��ɲ���,�Խ��ٶ���PI����  
	Gyro_PID_Pitch = Fix_Integral_Pitch * IntegralFaktor  + Gyro_NetValue_Pitch * GyroFaktor;
	Gyro_PID_Roll  = Fix_Integral_Roll  * IntegralFaktor  + Gyro_NetValue_Roll  * GyroFaktor;	
    Gyro_PID_Yaw   = Gyro_NetValue_Yaw  * 0.01 + Fix_Integral_Yaw    * IntegralFaktor/4 ;	   //;;;;;;;;;;;;;;16;;;;;;;;;;;;;;;;;;;;;;;;;;
//�Կ�����������з������� 
#define MAX_SENSOR  30000 		//ITG3200 Լ+-30000
    if(Gyro_PID_Pitch >  MAX_SENSOR) Gyro_PID_Pitch =  MAX_SENSOR;
    if(Gyro_PID_Pitch < -MAX_SENSOR) Gyro_PID_Pitch = -MAX_SENSOR;
    if(Gyro_PID_Roll >  MAX_SENSOR)  Gyro_PID_Roll  =  MAX_SENSOR;
    if(Gyro_PID_Roll < -MAX_SENSOR)  Gyro_PID_Roll  = -MAX_SENSOR;
    if(Gyro_PID_Yaw   >  MAX_SENSOR) Gyro_PID_Yaw   =  MAX_SENSOR;
    if(Gyro_PID_Yaw   < -MAX_SENSOR) Gyro_PID_Yaw   = -MAX_SENSOR;

//�����޷�
	if ( GasMischanteil > MAX_GAS - 25 ) 	GasMischanteil = MAX_GAS - 25;   //�������ŷ���
//�����׼ƫ��	
    GierMischanteil = Gyro_PID_Yaw - sollGier;//sollGier��ң����������������ָ���仯
//��ƫ����ֵ�������ƣ�������������������ĸ������ת��С��0
    if( GierMischanteil >  (GasMischanteil/2) )	GierMischanteil =  (GasMischanteil / 2); 
    if( GierMischanteil < -(GasMischanteil/2) ) GierMischanteil = -(GasMischanteil / 2); 
	
    if( GierMischanteil > (MAX_GAS - GasMischanteil) ) GierMischanteil =  (MAX_GAS - GasMischanteil); 
    if( GierMischanteil <-(MAX_GAS - GasMischanteil) ) GierMischanteil = -(MAX_GAS - GasMischanteil);

    if( GasMischanteil < 20 ) GierMischanteil = 0; /*���ű������̫С�ˣ�������ƫ��Ϊ0*/ 
//������  PI����������������㷨ʵ������λ�û�ΪPI���ƣ����ʻ�ΪP���� 
//����pd������
	tmp_axis = (	(INT32S)DynamicStability * (INT32S)( GasMischanteil + abs(GierMischanteil)/2 ) ) / 64;
//����Pitch��
	DiffNick = Gyro_PID_Pitch - StickNick; //΢��ֵ
	
	SummeNick += Fix_Integral_Pitch * IntegralFaktor - StickNick ;   //����ֵ
	if(SummeNick >  16000) SummeNick =  16000;
    if(SummeNick < -16000) SummeNick = -16000;
	
	pd_pitch = DiffNick; //+ Ki * SummeNick; 	
    if( pd_pitch >  tmp_axis ) pd_pitch =  tmp_axis; 
    if( pd_pitch < -tmp_axis ) pd_pitch = -tmp_axis; 
//�Ѿ����pd_pitch ����Pitch��
	DiffRoll = Gyro_PID_Roll - StickRoll ;	
	
	SummeRoll += Fix_Integral_Roll * IntegralFaktor - StickRoll;
    if(SummeRoll >  16000) SummeRoll =  16000;
    if(SummeRoll < -16000) SummeRoll = -16000;
	
    pd_roll = DiffRoll ;//+ Ki * SummeRoll;
    if(pd_roll >  tmp_axis) pd_roll =  tmp_axis; 
    if(pd_roll < -tmp_axis) pd_roll = -tmp_axis; 
//�Ѿ����pd_roll   
//ǰ(1)
	Motor_Tmp1 = GasMischanteil + pd_pitch - GierMischanteil - pd_roll;	
		if ( Motor_Tmp1 > MAX_GAS ) Motor_Tmp1 = MAX_GAS;
		if ( Motor_Tmp1 < MIN_GAS ) Motor_Tmp1 = MIN_GAS;
	Motor1  = (INT8U)Motor_Tmp1;	
//��(2)		
	Motor_Tmp2 = GasMischanteil + pd_pitch + GierMischanteil + pd_roll;	
		if ( Motor_Tmp2 > MAX_GAS ) Motor_Tmp2 = MAX_GAS;
		if ( Motor_Tmp2 < MIN_GAS ) Motor_Tmp2 = MIN_GAS;	
	Motor2  = (INT8U)Motor_Tmp2;
//��(3)
    Motor_Tmp3  = GasMischanteil - pd_pitch + GierMischanteil - pd_roll;		
		if ( Motor_Tmp3 > MAX_GAS ) Motor_Tmp3 = MAX_GAS;
		if ( Motor_Tmp3 < MIN_GAS ) Motor_Tmp3 = MIN_GAS;
	Motor3  = (INT8U)Motor_Tmp3;
//��(4)	
    Motor_Tmp4 = GasMischanteil - pd_pitch - GierMischanteil + pd_roll;	
		if ( Motor_Tmp4 > MAX_GAS ) Motor_Tmp4 = MAX_GAS;
		if ( Motor_Tmp4 < MIN_GAS ) Motor_Tmp4 = MIN_GAS;	
	Motor4  = (INT8U)Motor_Tmp4;
}


