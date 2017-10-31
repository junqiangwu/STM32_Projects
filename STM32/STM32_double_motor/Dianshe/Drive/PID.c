#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "PID.h"
#include "main.h"
#include "pwm.h"

#define ADC_Speed_Max   600	 //2500占空比
#define ADC_Speed_Min   2200
  
typedef struct PI 
{ 
	int Target;     //设定目标 
	int Uk;			//当前速度
    int Set_pwmvalue;  // 输出占空比
	double P; 		//比例常数
	double I; 		//积分常数
	int ek_0;		//当前误差
	int ek_sum; 	// 积分项
}PI;


static PI Motor; 
static PI *Motor_Pid = &Motor; 

void Motor_Pid_Init(void)      // PID初始化   起始占空比设置为600    后面实际调试
{ 
	Motor_Pid->Target = 0;
	Motor_Pid->Uk		= 0;
    Motor_Pid->Set_pwmvalue     = 0; 
	Motor_Pid->ek_0 	= 0;	
	Motor_Pid->ek_sum = 0; 
	Motor_Pid->P 		= 6;  //比例常数
	Motor_Pid->I 		= 0.25; 	 //积分常数
}

s32 Feedback_Motor=0;		   //速度反馈值
s32 Feedback_Motor_Slave=0;		   //速度反馈值

int Motor_PIDAdjust(int NEXT_POINT)   //Next_point 目标速度  
{    
  
    
	Motor_Pid->Target = NEXT_POINT;   //Target 目标速度
   
    Motor_Pid->ek_0= Motor_Pid->Target - Feedback_Motor;//  目标值-反馈速度
    
    Motor_Pid->ek_sum+=Motor_Pid->ek_0;
    
  if(Motor_Pid->I!=0){    //积分上限处理  
    if( Motor_Pid->ek_sum>(4000/Motor_Pid->I))
     {
       Motor_Pid->ek_sum=4000/Motor_Pid->I;
     }   
  }
  
    Motor_Pid->Set_pwmvalue = Motor_Pid->ek_0*Motor_Pid->P + Motor_Pid->I*Motor_Pid->ek_sum;  //PI
    
    if( Motor_Pid->Set_pwmvalue >= 3999)
	{
		Motor_Pid->Set_pwmvalue= 3999;
	}
	
    if(Motor_Pid->Set_pwmvalue <= -3999)
	{
		Motor_Pid->Set_pwmvalue =-3999;
	}  
    
     Motor1(Motor_Pid->Set_pwmvalue);
    	
    return(Motor_Pid->Set_pwmvalue);
}



static PI Motor_Slave; 
static PI *Motor_Slave_Pid = &Motor_Slave; 

void Motor_Slave_Pid_Init(void)      // PID初始化   起始占空比设置为600    后面实际调试
{ 
	Motor_Slave_Pid->Target = 0;
	Motor_Slave_Pid->Uk		= 0;
    Motor_Slave_Pid->Set_pwmvalue     = 0; 
	Motor_Slave_Pid->ek_0 	= 0;	
	Motor_Slave_Pid->ek_sum = 0; 
	Motor_Slave_Pid->P 		=6;  //比例常数
	Motor_Slave_Pid->I 		= 0.25; 	 //积分常数
}

 u8  Motor_Slave_Position_Kp=0;
 s32  Motor_Position_ek_0=0;
 s32  Motor_Position=0;
 s16  Motor_Position_Stop=0;
 s16  Motor_Position_Speed=0;
int Motor_Slave_PIDAdjust(int NEXT_POINT)   //Next_point 目标速度  
{

    
  if(Adjust_Mode_Flag==POSITION_MODE){
    Motor_Position_ek_0=NEXT_POINT-Motor_Position; 
    Motor_Position_Stop=1000;
    if(Motor_Position_ek_0>0){
      if(Motor_Position_ek_0>Motor_Position_Stop){
         Motor_Position_Speed=3000;
      }else{
        Motor_Position_Speed= Motor_Position_ek_0*3000/10;   // 误差*最大速度/比例系数
      }
    }else {
         if(abs(Motor_Position_ek_0)>Motor_Position_Stop){
             Motor_Position_Speed=-3000;
          }else{
            Motor_Position_Speed=-(Motor_Position_ek_0*3000/10);   // 误差*最大速度/比例系数
          } 
     }         
    }
 
    
	Motor_Slave_Pid->Target = NEXT_POINT;   //Target 目标速度
   
    Motor_Slave_Pid->ek_0= Motor_Slave_Pid->Target - Feedback_Motor_Slave;//  目标值-反馈速度
    
    Motor_Slave_Pid->ek_sum+=Motor_Slave_Pid->ek_0;
    
  if(Motor_Slave_Pid->I!=0){    //积分上限处理  
    if( Motor_Slave_Pid->ek_sum>(4000/Motor_Slave_Pid->I))
     {
       Motor_Slave_Pid->ek_sum=4000/Motor_Slave_Pid->I;
     }   
  }
  
    Motor_Slave_Pid->Set_pwmvalue = Motor_Slave_Pid->ek_0*Motor_Slave_Pid->P + Motor_Slave_Pid->I*Motor_Slave_Pid->ek_sum;  //PI
    
    if( Motor_Slave_Pid->Set_pwmvalue >= 3000)
	{
		Motor_Slave_Pid->Set_pwmvalue= 3000;
	}
	
    if(Motor_Slave_Pid->Set_pwmvalue <= -3000)
	{
		Motor_Slave_Pid->Set_pwmvalue =-30000;
	}  
     
    Motor2(Motor_Slave_Pid->Set_pwmvalue);   
	
    return(Motor_Slave_Pid->Set_pwmvalue);
}





//void SpeedAdjust()
//{
//    long d_error,dd_error,error;        //声明变量
//    error = (int)(speed_ept - now_speed);           //计算本次误差(期待速度-当前速度)
//    d_error = error - pre_error;        //本次误差与上次误差之差
//    dd_error = pre_error - pre_pre_error;       //上次误差与上上次误差之差
//    pre_error = error;          //将本次误差赋值给上次误差(下次计算用)
//    pre_pre_error = pre_error;      //将上次误差赋值给上上次误差(下次计算用)
//    pwm_tmp = pwm_tmp + PID_P*d_error + PID_I*error + PID_D*dd_error;       //计算pwm宽度调整量
//    if(now_speed>22)        //如果当前速度大于22
//    {
//        Set_DCMotor(0,0);       //不调整电机
//    }
//    else        //否则
//    {
//        if(pwm_tmp>=0)      //如果pwm宽度调整为正(增加宽度)
//        {
//            Set_DCMotor(pwm_tmp,0);     //增加直流电机转速
//        }
//        else        //否则
//        {
//            Set_DCMotor(pwm_tmp,1);     //降低直流电机转速
//        }
//    }
//    before_speed = now_speed;       //讲本次速度赋值给上次速度(下次使用)
//}


// PIDOUT：PID输出控制参数，为PWM宽度的调整量 
//Kp：比例调节参数 
//Ki：积分调节参数 
//Kd：微分调节参数 
//error：误差量（为期望速度-实际速度，可以为正数或负数） 
//d_error：本次误差量与上次误差量之差（error-pre_error） 
//dd_error：上次误差量与上上次误差量之差（pre_error-pre_pre_error）
