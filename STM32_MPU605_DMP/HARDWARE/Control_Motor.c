#include "pwm.h"
#include "PID.h"
#include "math.h"
#include "TIM.h"
#include "DataScope_DP.h"
#include "delay.h"
#include "Control_Motor.h"

//     TIM3->CCR1=1000;     // 电机1 CCR1>0 吸风   CCR2>0  吹风
//     TIM3->CCR2=0;   
//      
//     TIM3->CCR3=1000;     //电机2 CCR3>0 吹风   CCR4>0  吸风
//     TIM3->CCR4=0;
     
//  TIM2->CCR3=2000;     //电机3 CCR3>0 吸风  CCR4>0  吹风
//  TIM2->CCR4=0;
     
//  TIM2->CCR1=0;     //电机4  CCR2>0 吹风   CCR1>0  吸风
//  TIM2->CCR2=1000;
     
/*
   1/3 X轴 pitch 3 为正方向
   
   2/4 Y轴 roll  4 为正方向
*/

/*
   电机1/2  IN1/2/3/4  接 PA6/7  PB0/1
   电机3/4  IN1/2/3/4  接 PA1/0/2/3  
   
   TIM3  CCR1/2 控制2号电机，CCR1>0  
   TIM3  CCR3/4 控制1号电机，CCR3>0  吸风   
   
   TIM2  CCR1/2  控制4号电机
   PWM_X_13   占空比>0，角度大于目标  往1电机方向移动  3吹  1 吸
 
*/
int PWM_Y_13(int Nextpoint){   //1、3电机  Y轴  3方向角度为负  
   if(Nextpoint<0){    // 角度小于目标，往正方向移动  3 吸 1 吹
     
     TIM2->CCR4=0;       //电机3 CCR3>0 吸风  CCR4>0  吹风
     TIM2->CCR3=-Nextpoint;  
       
     TIM3->CCR3=0;    // 电机1 CCR3>0  吸风   CCR4>0  吹风
     TIM3->CCR4=-Nextpoint;       
   }
  else{   
       
     TIM2->CCR4=Nextpoint;     
     TIM2->CCR3=0;  
       
     TIM3->CCR4=0;   
     TIM3->CCR3=Nextpoint;         
    } 
    return 0;   }

int PWM_X_24(int Nextpoint){   // 2、4电机  4为正方向
  if(Nextpoint<0){      // 往2 方向移动  2 吸风  4 吹风

     TIM2->CCR1=0;     //电机4  CCR2>0 吹风   CCR1>0  吸风
     TIM2->CCR2=-Nextpoint;      
     TIM3->CCR2=-Nextpoint;  //电机2 CCR3>0 吹风   CCR4>0  吸风
     TIM3->CCR1=0;  
    }else{
     
     TIM2->CCR1=Nextpoint;     //电机4  CCR2>0 吹风   CCR1>0  吸风
     TIM2->CCR2=0;
      
     TIM3->CCR2=0;  //电机2 CCR3>0 吹风   CCR4>0  吸风
     TIM3->CCR1=Nextpoint;    
    }
   return 0;   }


float R = 20.0; 					 //半径设置(cm)
float angle = 40.0;					 //摆动角度设置(°)
float B=0;

extern float pitch,roll;
extern Pid_struct motor13_parameter,motor24_parameter;

/*------------------------------------------
 函数功能:第1问PID计算
------------------------------------------*/
void Test_1(void)
{
	const float priod = 1410.0;  //单摆周期(毫秒)
	static uint32_t MoveTimeCnt = 0;
	float set_y = 0.0;
	float A = 0.0;
	float Normalization = 0.0;
	float Omega = 0.0;
				
	MoveTimeCnt += 5;							 //每5ms运算1次
	Normalization = (float)MoveTimeCnt / priod;	 //对单摆周期归一化
	Omega = 2.0*3.14159*Normalization;			 //对2π进行归一化处理
	A = atan((R/102.0f))*57.2958f;				 //根据摆幅求出角度A,102为摆杆距离地面长度cm
	set_y = A*sin(Omega);                        //计算出当前摆角 	
	
	Speed_PIDInit(&motor13_parameter,30,0.2,100);   
	Speed_PIDInit(&motor13_parameter,30,0.2,100);   	

  PWM_Y_13(Speed_PIDAdjust(&motor13_parameter,set_y,roll));  //x方向0   y方向跟踪set_y，微调摆动周期
  PWM_X_24(Speed_PIDAdjust(&motor24_parameter,0,pitch));           
}


/*------------------------------------------
 函数功能:第2问PID计算
------------------------------------------*/
void Test_2(void)
{
	const float priod = 1905.0;  //单摆周期(毫秒)
	static uint32_t MoveTimeCnt = 0;
	float set_y = 0.0;
	float A = 0.0;
	float Normalization = 0.0;
	float Omega = 0.0;
				
	MoveTimeCnt +=5;							 //每5ms运算1次
	Normalization = (float)MoveTimeCnt / priod;	 //对单摆周期归一化
	Omega = 2.0*3.14159*Normalization;			 //对2π进行归一化处理
  A = atan((R/110.0f))*57.2958f;				 //根据摆幅求出角度A（57.3转换）,102摆杆离地高度
  set_y = A*sin(Omega);               //计算出当前摆角 	
   
 	Speed_PIDInit(&motor13_parameter,60,0.2,200);   
	Speed_PIDInit(&motor13_parameter,60,0.2,200);   	

  PWM_Y_13(Speed_PIDAdjust(&motor13_parameter,set_y,roll));  //x方向0   y方向跟踪set_y，微调摆动周期
  PWM_X_24(Speed_PIDAdjust(&motor24_parameter,0,pitch));     // 调节R   调节摆动幅度大小
}


/*------------------------------------------
 函数功能:第3问PID计算
------------------------------------------*/
void Test_3(void)
{
	const float priod = 1795.0;  //单摆周期(毫秒)
	             //相位补偿 0, 10   20   30   40   50   60   70   80   90   100  110  120  130  140  150  160  170 180
	const float Phase[19]= {0,-0.1,-0.05,0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.05,0.05,0.05,0.07,0};
	static uint32_t MoveTimeCnt = 0;
	float set_x = 0.0;
	float set_y = 0.0;
	float Ax = 0.0;
	float Ay = 0.0;
	float A = 0.0;
	uint32_t pOffset = 0;
	float Normalization = 0.0;
	float Omega = 0.0;
	
	pOffset = (uint32_t)(angle/10.0f);			 //相位补偿数组下标
        
	MoveTimeCnt += 5;							 //每5ms运算1次
	Normalization = (float)MoveTimeCnt / priod;	 //对单摆周期归一化    
	Omega = 2.0*3.14159*Normalization;			 //对2π进行归一化处理
	A = atan((R/102.0f))*57.2958f;              //根据摆幅求出角度A（57.3转换）,102摆杆离地高度
        
	Ax = A*cos(angle*0.017453);	 //计算出X方向摆幅分量0.017453为弧度转换
	Ay = A*sin(angle*0.017453);	 //计算出Y方向摆幅分量
	set_x = Ax*sin(Omega); 		 //计算出X方向当前摆角
	set_y = Ay*sin(Omega+Phase[pOffset]); //计算出Y方向当前摆角	 有一定漂移
	
 	Speed_PIDInit(&motor13_parameter,60,0.2,200);   
	Speed_PIDInit(&motor13_parameter,60,0.2,200);   	

  PWM_Y_13(Speed_PIDAdjust(&motor13_parameter,set_y,roll));  //x方向0   y方向跟踪set_y，微调摆动周期
  PWM_X_24(Speed_PIDAdjust(&motor24_parameter,set_x,pitch));     // 调节R   调节摆动幅度大小 
	
}


/*------------------------------------------
 函数功能:第4问PID计算
------------------------------------------*/ 
void Test_4(void)
{	
	if(abs(pitch)<45.0 && abs(roll)<45.0)	//小于45度才进行制动
	{
	
 	Speed_PIDInit(&motor13_parameter,60,0.2,200);   
	Speed_PIDInit(&motor13_parameter,60,0.2,200);   	

  PWM_Y_13(Speed_PIDAdjust(&motor13_parameter,0,roll));  //小于45度时，pid调节x_y方向角度为0  
  PWM_X_24(Speed_PIDAdjust(&motor24_parameter,0,pitch));   
	}
	else	
	{
    // 系统停止运行
	}
}



/*------------------------------------------
 函数功能:第5问PID计算
------------------------------------------*/
void Test_5(void)
{
	const float priod = 1410.0;  //单摆周期(毫秒)
	static uint32_t MoveTimeCnt = 0;
	float set_x = 0.0;
	float set_y = 0.0;
	float A = 0.0;
	float phase = 0.0;
	float Normalization = 0.0;
	float Omega = 0.0;
	
	MoveTimeCnt += 5;							 //每5ms运算1次
	Normalization = (float)MoveTimeCnt / priod;	 //对单摆周期归一化
	Omega = 2.0*3.14159*Normalization;			 //对2π进行归一化处理				
	A = atan((R/88.0f))*57.2958f;    //根据半径求出对应的振幅A
	
//   Set_Motor_PID(120,0,2);   // 第一题的PID设置
 //   Set_PID_2(120,0,2);
}