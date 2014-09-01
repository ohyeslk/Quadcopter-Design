//捷联惯导
#include <math.h>
#include <iostream.h>
#include <stdio.h>

#define PI 3.1415926
#define WIE 7.2921158e-5//弧度每秒
#define RAD PI/180
#define G 9.8
#define Re 6.378393e6
#define dRe 0.15678e-6
#define e 0.3367e-2

//void Initial();
void Initial(double C[3][3],double T[3][3],double Q[4],double POS[3],double V[2],double Angl[3],double Wiep[3],double WEPP[2]);
void INITCMAT(double C[3][3],double POS[3]);
void INITQ(double T[3][3],double Q[4]);
void INITWIEP(double C[3][3]);
void INITWEPP(double C[3][3], double V[2]);
void calcG(double C[3][3],double h);
void calcV(double V[2]);
void calcWPBB(double Wpbb[3],double Wibb[3], double T[3][3],double Wepp[3], double Wiep[3],double WEPP[2]);
//void updConvQ(double Wpbb[3]);
void updConvQ(double Wpbb[3],double  Conv_Q[4][4]);
void updateQ(double Wpbb1[3],double Wpbb2[3],double Wpbb3[3],double Q[4],double t1);
void unitQ(double Q[4]);
void calaT(double Q[4],double T[3][3]);
void convacce(double T[3][3],double fp[3],double fb[3]);
void updateVp(double fp[3],double Wiep[3],double Wepp[3],double V[3],double t2);
//void updateVp(double fp[3],double Wiep[3],double Wepp[3],double Init_V[3],double t2);
void calaWEPP(double C[3][3], double V[2],double WEPP[2]);
void updateC(double C[3][3],double WEPP[2],double t2);
//void updateC(double Init_C[3][3],double C[3][3],double Wepp[2],double t2);
void calaWIEP(double Wiep[3],double C[3][3],double Wie[3]);
void calapos(double POS[3],double C[3][3]);
void calczitai(double Angl[3],double T[3][3],double alpha);

void mulit_MAT_VEC1(double mulit[2],double a[2][2],double b[2]);
void mulit_MAT_VEC2(double mulit[3],double a[3][3],double b[3]);
void mulit_MAT_VEC3(double mulit[4],double a[4][4],double b[4]);
void mulit_NUM_VEC1(double mulit[4],double n,double a[4]);
void mulit_NUM_VEC2(double mulit[3],double n,double a[3]);
void mulit_NUM_MAT1(double mulit[3][3],double n,double a[3][3]);
void add_VECTOR1(double sum[3],double a[3],double b[3]);
void add_VECTOR2(double sum[4],double a[4],double b[4]);
void add_mVECTOR(double sum[4],double a[4],double b[4],double c[4],double d[4],double e1[4]);
void sub_VECTOR1(double diff[3],double a[3],double b[3]);
void trans_MATRIX1(double a[3][3],double a_trans[3][3]);
void mulit_MATRIX(double mulit[3][3],double a[3][3],double b[3][3]);
void add_MATRIX(double sum[3][3],double a[3][3],double b[3][3]);
  
void main()

{
	double LAT=45*RAD;
    double t1=0.01;
	double t2=0.04;
	int I=0;
    int m,n;
	double T[3][3];
    double Wepp[3];
    double WEPP[2];
	double Wiep[3];
    double Q[4];
    double fp[3];    
	double C[3][3];
    double V[2];
	double Wie[3];
	double h=0;
	double Angl[3];
	double alpha=0;
	double POS[3];   
	double x=0;
    double Wf[18];
    double Wibb1[3],Wibb2[3],Wibb3[3],fb[3];
    double Wpbb1[3],Wpbb2[3],Wpbb3[3];
	  
	  //Initial();
    Initial(C,T,Q,POS,V,Angl,Wiep,WEPP);
	cout<<C[0][0]<<" "<<C[0][1]<<" "<<C[0][2]<<"\n";
    cout<<C[1][0]<<" "<<C[1][1]<<" "<<C[1][2]<<"\n";
    cout<<C[2][0]<<" "<<C[2][1]<<" "<<C[2][2]<<"\n";
  	
  	while(x<10)

     { 
     	I=I+1;
		cout<<"I="<<I<<"\n";
    	for(m=0;m<18;m++)
		{ 
			
			Wf[m]=10*cos(x);
			x+=0.001;
		}
          //cout<<"x="<<x<<"Wf="<<Wf[i]<<"\n";
          for(n=0;n<3;n++)
 	                     {
 		                     Wibb1[0+n]=Wf[3+n];
 	                       Wibb2[0+n]=Wf[9+n];
                         Wibb3[0+n]=Wf[15+n];
                         fb[n]=Wf[n];
                        }
                        
        		       	 
		            calcWPBB(Wpbb1,Wibb1,T,Wepp,Wiep,WEPP);//姿态速率的计算；
		            calcWPBB(Wpbb2,Wibb2,T,Wepp,Wiep,WEPP);
		            calcWPBB(Wpbb3,Wibb3,T,Wepp,Wiep,WEPP);
		            updateQ(Wpbb1,Wpbb2,Wpbb3,Q,t1);//四元数Q的即时修正
		            if (I%8==0)
			             {unitQ(Q); }//四元数Q的归一化
			          else
			           	 {
			           	 	calaT(Q,T);  //矩阵T的计算					   
	                  convacce(T,fp,fb);
	                 }//加速度的坐标转换
	              if (I%4==0)
	  	             {
	  	              updateVp(fp,Wiep,Wepp,V,t2);
	  	              calaWEPP(C,V,WEPP);
					  cout<<"WEPP="<<WEPP[0]<<" "<<WEPP[1]<<"\n";
	  	              updateC(C,Wepp,t2);
					  cout<<C[0][0]<<" "<<C[0][1]<<" "<<C[0][2]<<"\n";
                      cout<<C[1][0]<<" "<<C[1][1]<<" "<<C[1][2]<<"\n";
					  cout<<C[2][0]<<" "<<C[2][1]<<" "<<C[2][2]<<"\n";
	 	              calaWIEP(Wiep,C,Wie);	//地球速率的计算
					  calcG(C,h);  //计算重力加速度g的初始值
	                   if  (I%8==0)
					    {
							calczitai(Angl,T,alpha);   //姿态计算
				       cout<<"Angl="<<Angl[0]<<" "<<Angl[1]<<" "<<Angl[2]<<"\n";
					   }

	    	            else 
                        {
                        	calapos(POS,C);//位置计算         
                            calcV(V);//计算地速
						cout<<"POS="<<POS[0]<<" "<<POS[1]<<" "<<POS[2]<<"\n";
						cout<<"V="<<V[0]<<" "<<V[1]<<"\n";
						}
						
				  }                      

              }       		
            
         }  

	//计算矩阵T的初始值
//void INITTMAT()
//{
	 //double T[3][3]={{1,0,0},{0,1,0},{0,0,1}};

      
	 //T[0][0]=1/(G*WIE*cos(LAT))*(Wibb[2]*fb[1]-Wibb[1]*fb[2]);
   //T[0][1]=1/(G*WIE*cos(LAT))*(Wibb[0]*fb[2]-Wibb[2]*fb[0]);
   //T[0][2]=1/(G*WIE*cos(LAT))*(Wibb[1]*fb[0]-Wibb[0]*fb[1]);
   //T[1][0]=fb[0]/G*tan(LAT)+Wibb[0]/WIE*1/cos(LAT);
   //T[1][1]=fb[1]/G*tan(LAT)+Wibb[1]/WIE*1/cos(LAT);
   //T[1][2]=fb[2]/G*tan(LAT)+Wibb[2]/WIE*1/cos(LAT);
   //T[2][0]=-fb[0]/G;
   //T[2][1]=-fb[1]/G;
   //T[2][2]=-fb[2]/G;
 // }
//计算矩阵C的初始值
//POS[0]=ALPHA0;POS[1]=LAT0;POS[2]=LONG0;
void INITCMAT(double C[3][3],double POS[3])

{
		
		C[0][0]=-sin(POS[0])*sin(POS[1])*cos(POS[2])-cos(POS[0])*sin(POS[2]);
        C[0][1]=-sin(POS[0])*sin(POS[1])*sin(POS[2])+cos(POS[0])*sin(POS[2]);
        C[0][2]=sin(POS[0])*cos(POS[1]);
        
		C[1][0]=-cos(POS[0])*sin(POS[1])*cos(POS[2])+sin(POS[0])*sin(POS[2]);
        C[1][1]=cos(POS[0])*sin(POS[1])*sin(POS[2])-sin(POS[0])*cos(POS[2]);
        C[1][2]=cos(POS[0])*cos(POS[1]);
        
		C[2][0]=cos(POS[1])*cos(POS[2]);
        C[2][1]=cos(POS[1])*sin(POS[2]);
        C[2][2]=sin(POS[1]);
}



//计算初始四元数Q

void INITQ(double T[3][3],double Q[4])

{

  double qn[4];
	double abs_q[4];
           
       qn[1]=1+T[0][0]-T[1][1]-T[2][2];        //计算绝对值方程；
       qn[2]=1-T[0][0]+T[1][1]-T[2][2];
	   qn[3]=1-T[0][0]-T[1][1]+T[2][2];
	   qn[0]=1-qn[1]*qn[1]-qn[2]*qn[2]-qn[3]*qn[3];
           
           abs_q[0]=sqrt(qn[0]);               
		   abs_q[1]=1/2*sqrt(qn[1]);
		   abs_q[2]=1/2*sqrt(qn[2]);
		   abs_q[3]=1/2*sqrt(qn[3]);

		   Q[0]=abs_q[0];             //确定q0,q1,q2,q3符号；
		   if (T[2][1]-T[1][2]>=0)
			   Q[1]=abs_q[1];
		   else 	
				 Q[1]=-abs_q[1];
			            
			 if (T[0][2]-T[2][0]>=0)
			   Q[2]=abs_q[2];
		   else 
		   	 Q[2]=-abs_q[2];	
			      
			 if (T[1][0]-T[0][1]>=0)
			   Q[3]=abs_q[3];
		   else 
			   Q[3]=-abs_q[3]; 
			          }       
			          
			          
//计算矩阵C的初始值

      
    //计算平台系地球速率WIEP的初始值
    void INITWIEP(double C[3][3])
    {
       	double WIEP[3];
        int i;
    
    	       for(i=0;i<3;i++)
    	       {
    	       	WIEP[i]=WIE*C[i][2];
    	        }
    	         }
    	         
    //计算平台系位置速率WEPP的初始值
    void INITWEPP(double C[3][3], double V[2])
    { 

    	double WEPP[2];
       	double conv_wepp[2][2];
    	       
    	       conv_wepp[0][0]=-2*e*dRe*C[0][2]*C[1][2];
    	       conv_wepp[0][1]=dRe*(1-e*C[2][2]*C[2][2]+2*e*C[1][2]*C[1][2]);
    	       conv_wepp[1][0]=dRe*(1-e*C[2][2]*C[2][2]+2*e*C[0][2]*C[0][2]);
    	       conv_wepp[1][1]=2*e*dRe*C[0][2]*C[1][2];
    	       
    	       mulit_MAT_VEC1(WEPP,conv_wepp,V);
    }

    	  //计算重力加速度g0的初始值    
    void calcG(double C[3][3],double h)
    {
    	double g;
    	const double g0=9.7803;
    	const double gx=0.051799;
    	const double hr=0.94114e-6;
        	
    	g=g0+gx*C[2][2]-hr*h;
    }
    	
  //计算地速的初始值
    void calcV(double V[2])
    {
    	double v;
      double sumV;
    	
    	sumV=V[0]*V[0]+V[1]*V[1];
    	v=sqrt(sumV);
    	
    }
      		
  
  
  //姿态速率WPbb的计算
  void calcWPBB(double Wpbb[3],double Wibb[3], double T[3][3],double Wepp[3], double Wiep[3],double WEPP[2])
  {  	         
    double We_ip[3];
    double transT[3][3];
    double Tw[3];
	
    Wepp[0]=WEPP[0];
	Wepp[1]=WEPP[1];
	Wepp[2]=0;
   add_VECTOR1(We_ip,Wiep,Wepp);
   trans_MATRIX1(T,transT);  
   mulit_MAT_VEC2(Tw,transT,We_ip);
   sub_VECTOR1(Wpbb,Wibb,Tw); 
  }
  //更新Q转换阵；
  void updConvQ(double Wpbb[3],double  Conv_Q[4][4])
   {   	
     Conv_Q[0][0]=0;
     Conv_Q[0][1]=-Wpbb[0];
     Conv_Q[0][2]=-Wpbb[1];
     Conv_Q[0][3]=-Wpbb[2];
     
     Conv_Q[1][0]=Wpbb[0];
     Conv_Q[1][1]=0;
     Conv_Q[1][2]=Wpbb[2];
     Conv_Q[1][3]=-Wpbb[1];
     
     Conv_Q[2][0]=Wpbb[1];    
     Conv_Q[2][1]=-Wpbb[2];
     Conv_Q[2][2]=0;
     Conv_Q[2][3]=Wpbb[0];
     
     Conv_Q[3][0]=Wpbb[2];
     Conv_Q[3][1]=Wpbb[1];
     Conv_Q[3][2]=-Wpbb[0];
     Conv_Q[3][3]=0;
    }
//四元数Q的即时修正
 void updateQ(double Wpbb1[3],double Wpbb2[3],double Wpbb3[3],double Q[4],double t1)
     {
       double Conv_Q[4][4];    
	   double K1[4];
	   double K2[4];
	   double K3[4];
	   double K4[4];
       double douK1[4],douK2[4],douK3[4],douK4[4];
       double T5K1[4],T5K2[4],T5K3[4];
       double Init_Q[4];
       double t5;
   
	        t5=t1/2;     	    	
     
        //四阶龙格库塔法
           	Init_Q[0]=Q[0]; 
            Init_Q[1]=Q[1];
			Init_Q[2]=Q[2];
			Init_Q[3]=Q[3];
			updConvQ(Wpbb1,Conv_Q);
     	   	mulit_MAT_VEC3(douK1,Conv_Q,Init_Q); //得到2*K1；Conv_Q[4][4]应为0时刻的；
     	   	mulit_NUM_VEC1(K1,0.5,douK1);//得到K1；Conv_Q[4][4]应为0时刻的；
     	   	mulit_NUM_VEC1(T5K1,t5,K1);//得到t1/2*K1;
     	    add_VECTOR2(Q,Init_Q,T5K1); //得到 0.5*t1时刻的K1斜率下的Q[4]的估计值  	 
         	
            updConvQ(Wpbb2,Conv_Q);
     	    mulit_MAT_VEC3(douK2,Conv_Q,Q); //得到2*K2；Conv_Q[4][4]应为0.5*t1时刻的；
     	    mulit_NUM_VEC1(K2,0.5,douK2);//得到K2；
     	    mulit_NUM_VEC1(T5K2,t5,K2);//得到t1/2*K2;
     	    add_VECTOR2(Q,Init_Q,T5K2);	 //得到 0.5*t1时刻的K2斜率下的new_Q[4]的估计值   	
       	    
     	    mulit_MAT_VEC3(douK3,Conv_Q,Q); //得到2*K3; Conv_Q[4][4]应为0.5*t1时刻的；
     	    mulit_NUM_VEC1(K3,0.5,douK3);//得到K2；
     	    mulit_NUM_VEC1(T5K3,t1,K3);//得到t1*K3;
     	    add_VECTOR2(Q,Init_Q, T5K3);	 //得到 t1时刻的K3斜率下的new_Q[4]的估计值   	
 
            updConvQ(Wpbb3,Conv_Q);
     	    mulit_MAT_VEC3(douK4,Conv_Q,Q); //得到2*K4;Conv_Q[4][4]应为t1时刻的；
     	   	mulit_NUM_VEC1(K4,0.5,douK4);//得到K4；Conv_Q[4][4]应为t1时刻的；
     	   	
     
     	    
     	    add_mVECTOR(Q,Init_Q,K1,K2,K3,K4);
     	         	    
     	    
     	  }
    	       
    //四元数Q的归一化
    //Q=Q0/根号下（q0*q0+q1*q1*q2*q2+q3*q3)
   	    void unitQ(double Q[4])
	      {
			double squQ,unq;
	      	double unit_Q[4];
	      	int i;
	      	
	      	for(i=0;i<4;i++)
	      	{ 
	      		squQ=Q[0]*Q[0]+Q[1]*Q[1]+Q[2]*Q[2]+Q[3]*Q[3];
	      		unq=1/sqrt(squQ);
	      	    unit_Q[i]=Q[i]*unq;
	      	}
	      }		
	     
	      //矩阵T的计算
	        void calaT(double Q[4],double T[3][3])
	      {
	      	      	
	      	
	      	T[0][0]=Q[0]*Q[0]+Q[1]*Q[1]+Q[2]*Q[2]+Q[3]*Q[3];
	      	T[0][1]=2*(Q[1]*Q[2]-Q[0]*Q[3]);
	      	T[0][2]=2*(Q[1]*Q[3]+Q[0]*Q[1]);
	      	
	      	T[1][0]=2*(Q[1]*Q[2]+Q[0]*Q[3]);
	      	T[1][1]=Q[0]*Q[0]-Q[1]*Q[1]+Q[2]*Q[2]-Q[3]*Q[3];
	      	T[1][2]=2*(Q[2]*Q[3]-Q[0]*Q[1]);
	      	
	      	T[2][0]=2*(Q[1]*Q[3]-Q[0]*Q[2]);
	      	T[2][1]=2*(Q[2]*Q[3]+Q[0]*Q[1]);
	      	T[2][2]=Q[0]*Q[0]-Q[1]*Q[1]-Q[2]*Q[2]+Q[3]*Q[3];
	   }
	   
	   //加速度的坐标转换
	  void convacce(double T[3][3],double fp[3],double fb[3])
	   {
	   		   	
	   	 mulit_MAT_VEC2(fp,T,fb);
	   	}
	   	
	   	//速度Vp的即时修正
	 void updateVp(double fp[3],double Wiep[3],double Wepp[3],double V[3],double t2)
	   	{
	   		double KV[3];
	   		double Init_V[3];		
	   		double g[3];
	   		double WW[3][3];
	   		double WV[3];
	        double GWV[3];
	   	    double t2KV[3];
	        t2=0.04;
		
	   		WW[0][0]=0;
	   		WW[0][1]=2*Wiep[2];
	   		WW[0][2]=-(2*Wiep[1]+Wepp[1]);
	   		WW[1][0]=-2*Wiep[2];
	   		WW[1][1]=0;	   		
	   		WW[1][2]=2*Wiep[0]+Wepp[0];
	   		WW[2][0]=2*Wiep[1]+Wepp[1];
	   		WW[2][1]=-2*Wiep[0]+Wepp[0];
	   		WW[2][2]=0;
	   		
	   		g[0]=0;
	   		g[1]=0;
	   		g[2]=9.8;
	   		
			Init_V[0]=V[0];
			Init_V[1]=V[1];
			Init_V[2]=V[2];

			//一阶欧拉法

	   		mulit_MAT_VEC2(WV,WW,Init_V);
	   		add_VECTOR1(GWV,g,WV);
	   		sub_VECTOR1(KV,fp,GWV); 
	   		mulit_NUM_VEC2(t2KV,t2,KV);
	   		add_VECTOR1(V,t2KV,Init_V);
	   	
	   	}
 
	   		//位置速率的计算
	   		void calaWEPP(double C[3][3], double V[2],double WEPP[2])
    { 

       	      double conv_wepp[2][2];
    	       conv_wepp[0][0]=-2*e*dRe*C[0][2]*C[1][2];
    	       
    	       conv_wepp[0][1]=dRe*(1-e*C[2][2]*C[2][2]+2*e*C[1][2]*C[1][2]);
    	       conv_wepp[1][0]=dRe*(1-e*C[2][2]*C[2][2]+2*e*C[0][2]*C[0][2]);
    	       conv_wepp[1][1]=2*e*dRe*C[0][2]*C[1][2];
    	       
    	       mulit_MAT_VEC1(WEPP,conv_wepp,V);
    }
    //位置矩阵C的即时修正
    void updateC(double C[3][3],double WEPP[2],double t2)
    {
    	double WEPP0[3][3];
        double KC[3][3];
        double t2KC[3][3];
        double Init_C[3][3];
    	
    	WEPP0[0][0]=0;
	   	WEPP0[0][1]=0;	
	    WEPP0[0][2]=-WEPP[1];
	    WEPP0[1][0]=0;
	    WEPP0[1][1]=0;
	    WEPP0[1][2]=WEPP[0];
	    WEPP0[2][0]=WEPP[1];
	    WEPP0[2][1]=-WEPP[0];
	    WEPP0[2][2]=0;

   	//一阶欧拉法
	     Init_C[0][0]=C[0][0];
         Init_C[0][1]=C[0][1];
         Init_C[0][2]=C[0][2];
		 Init_C[1][0]=C[1][0];
		 Init_C[1][1]=C[1][1];
		 Init_C[1][2]=C[1][2];
		 Init_C[2][0]=C[2][0];
		 Init_C[2][1]=C[2][1];
		 Init_C[2][2]=C[2][2];

		 mulit_MATRIX(KC,WEPP0,Init_C);
	     mulit_NUM_MAT1(t2KC,t2,KC);
	     add_MATRIX(C,t2KC,Init_C);
	   
	    
	  }
	  //地球速率的计算
	  void calaWIEP(double Wiep[3],double C[3][3],double Wie[3])
	  {
	 	  	
	  	Wie[0]=0;
	  	Wie[1]=0;
	  	Wie[2]=WIE;
	  	
	  	mulit_MAT_VEC2(Wiep,C,Wie);
	  }
	  	  
    //位置计算
    
  void calapos(double POS[3],double C[3][3])
    {
    	double fai,lamda,alpha;
     	 	
        fai=asin(C[2][2]);
    	lamda=atan(C[2][1]/C[2][0]);
    	alpha=atan(C[0][2]/C[1][2]);
    	
    	POS[0]=fai;
    	
    	if (C[2][0]>0)
    		POS[1]=lamda;
    		else
    			{if (lamda<0)
    				  POS[1]=lamda+PI;
    			else 
    				  POS[1]=lamda-PI;
    				}
    			
    	if (C[1][2]<0)
    		POS[2]=alpha+PI;
       	else
       		{
       			if (alpha>0)
       			    POS[2]=alpha;
       			else
       			    POS[2]=alpha+2*PI;
       		}
       	}
       	//姿态计算
       void calczitai(double Angl[3],double T[3][3],double alpha)
    {
       
    	double thta,gama,pusai0=0,pusaiG,pusai;
		
        thta=asin(T[2][1]);
    	gama=atan(-T[2][0]/T[2][2]);
    	pusai=atan(-T[0][1]/T[1][1]);
    	
    	Angl[0]=thta;
    	
    	if (T[2][2]>=0)
    		Angl[1]=gama;
    		else
    			{if (gama<0)
    				  Angl[1]=gama+PI;
    			else 
    				  Angl[1]=gama-PI;
    				}
    			
    	if (T[1][1]<0)
    		pusaiG=pusai0+PI;
       	else
       		{
       			if (pusai0>=0)
       			  pusaiG=pusai0;
       			else
       			  pusaiG=pusai0+2*PI;
       		}
       		
       		pusai=pusaiG+alpha;
       		if(pusai<2*PI)
       		   Angl[2]=pusai;
       		else
       		Angl[2]=pusai-2*PI;
       	}

	   void Initial(double C[3][3],double T[3][3],double Q[4],double POS[3],double V[2],double Angl[3],double Wiep[3],double WEPP[2])
{	
   
       //POS[0]=ALPHA0;POS[1]=LAT0;POS[2]=LONG0;
   double h=0;//初始化高度为0；       
   double Wie[3];
   
   T[0][0]=T[1][1]=T[2][2]=1;
   T[0][1]=T[0][2]=T[1][0]=T[1][2]=T[2][0]=T[2][1]=0;//初始化T矩阵；
   V[0]=V[1]=0;//初始化速度为0；
   POS[0]=0;//初始游动方位角为0；
   POS[1]=45*RAD;//初始纬度为45度；
   POS[2]=126*RAD;//初始经度为126度；
   
   //INITTMAT();
   calczitai(Angl,T,0);//初始化姿态角；
   INITQ(T,Q);//初始化四元数；
   INITCMAT(C,POS);//初始化C矩阵；
   calaWIEP(Wiep,C,Wie);//初始化地球速率;
   calaWEPP(C,V,WEPP);//初始化位置速率WEPP;
   calcG(C,h);//初始化重力加速度；
   calcV(V);//初始化地速V;
  }


	   //矩阵与向量乘法
//矩阵a为2行2列,矩阵b为2行1列,相乘得到矩阵mulit为2行1列。
 void mulit_MAT_VEC1(double mulit[2],double a[2][2],double b[2])
{
   int i,j;
   
      for (i=0;i<2;i++)
       {
       	mulit[i]=0;
       	for(j=0;j<2;j++)
       	    {                  
             mulit[i]+=a[i][j]*b[j];
            }
       }
 }
  //矩阵与向量乘法
//矩阵a为3行3列,矩阵b为3行1列,相乘得到矩阵mulit为3行1列。
 void mulit_MAT_VEC2(double mulit[3],double a[3][3],double b[3])
{
   int i,j;
   
      for (i=0;i<3;i++)
       {
       	mulit[i]=0;
       	for(j=0;j<3;j++)
       	    {                  
             mulit[i]+=a[i][j]*b[j];
            }
       }
 }
 //矩阵与向量乘法
//矩阵a为4行4列,矩阵b为4行1列,相乘得到矩阵mulit为4行1列。
 void mulit_MAT_VEC3(double mulit[4],double a[4][4],double b[4])
{
   int i,j;
   
      for (i=0;i<4;i++)
       {
       	mulit[i]=0;
       	for(j=0;j<4;j++)
       	    {                  
             mulit[i]+=a[i][j]*b[j];
            }
       }
 }
 //数与向量相乘
 //4*1向量
 void mulit_NUM_VEC1(double mulit[4],double n,double a[4])
  {
    	int i;
  	
  	for(i=0;i<4;i++)
    	       {
    	       	mulit[i]=n*a[i];
    	        }
    	         }
  //数与向量相乘
 //3*1向量
 void mulit_NUM_VEC2(double mulit[3],double n,double a[3])
  {
  	
  	int i;
  	
  	for(i=0;i<3;i++)
    	       {
    	       	mulit[i]=n*a[i];
    	        }
    	         }
    	         
    	         
 //数与矩阵相乘
 //3*3矩阵
 void mulit_NUM_MAT1(double mulit[3][3],double n,double a[3][3])
  {
  
  	int i,j;
  	
  	for(i=0;i<3;i++)
    	       {
    	       	for(j=0;j<3;j++)
    	       	  {
    	       	    mulit[i][j]=n*a[i][j];
    	           }
    	        }
   }
  //向量加法
  //3*1向量相加
 void add_VECTOR1(double sum[3],double a[3],double b[3])  
 {
 
  int i;
  for (i=0;i<3;i++)
      {
      	sum[i]=a[i]+b[i];
     
      }
      
   }
    //向量加法
  //4*1向量相加
 void add_VECTOR2(double sum[4],double a[4],double b[4])  
 {
 
  int i;
  for (i=0;i<4;i++)
      {
      	sum[i]=a[i]+b[i];
     
      }
      
   }
    //5个向量加法Y1=Y0+t1/6(K1+2K2+2K3+K4)
   void add_mVECTOR(double sum[4],double a[4],double b[4],double c[4],double d[4],double e1[4] )  
 {
	  double A=0.01;
      int i;
  
  for (i=0;i<4;i++)
      {
      	sum[i]=a[i]+A/6*(b[i]+2*c[i]+2*d[i]+e1[i]);
    
      }
      
   }
    
     //向量减法
     //3*1向量
      
   void sub_VECTOR1(double diff[3],double a[3],double b[3])  
 {
 
  int i;
  
  for (i=0;i<3;i++)
      {
      	diff[i]=a[i]-b[i];
  
      }
      
   } 
    
    //矩阵转置
    //3*3矩阵
   
 void trans_MATRIX1(double a[3][3],double a_trans[3][3])
   {
   	
    int i,j;
    
    for (i=0;i<3;i++)
       {
       	for(j=0;j<3;j++)
       	   {
       	   	a_trans[j][i]=a[i][j];
       	   }
       	}
    }
    //矩阵乘法
//矩阵a为3行3列,矩阵b为3行3列,相乘得到矩阵mulit为3行3列。

void mulit_MATRIX(double mulit[3][3],double a[3][3],double b[3][3])
{
      int i,j,k;
   
      for (i=0;i<3;i++)
       {
       	for(k=0;k<3;k++)
       	mulit[i][k]=0;
           for(j=0;j<3;j++)
          { 
          mulit[i][k]=mulit[i][k]+a[i][j]*b[j][k];
          }
       }
 }
 //矩阵加法
 //矩阵a为3行3列,矩阵b为3行3列,相乘得到矩阵sum为3行3列。
 void add_MATRIX(double sum[3][3],double a[3][3],double b[3][3])
{
  
   int i,j;
   
      for (i=0;i<3;i++)
         {
            for(j=0;j<3;j++)
             { 
              sum[i][j]=a[i][j]+b[i][j];
             }
          }
 }
