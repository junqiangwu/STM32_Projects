#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "PID.h"
#include "pwm.h"
#include "TIM.h"

#define Out_Speed_Max  3999	   //2500占空比

void Speed_PIDInit(Pid_struct *motor_pid)      // PID初始化   起始占空比设置为600    后面实际调试
{ 
	motor_pid->speed_target = 0;
	motor_pid->out_duty = 0;
	
	motor_pid->speed_p = 0.06;
	motor_pid->speed_i = 0.0002;
	motor_pid->speed_d = 0;
	
	motor_pid->ek_0 	= 0;	
	motor_pid->ek_1 	= 0;	
	motor_pid->ek_sum = 0; 
}

int Speed_PIDAdjust(Pid_struct *motor_pid,int Targe,int Feedback_speed)   //Next_point 目标速度  
{
    float de;
		
		motor_pid->speed_target = Targe;   //Target 目标速度
    motor_pid->ek_0= motor_pid->speed_target - Feedback_speed;//  目标值-反馈速度   
    de=motor_pid->ek_0-motor_pid->ek_1;
    motor_pid->ek_sum+=motor_pid->ek_0;
    
  if(motor_pid->speed_i!=0){    //积分上限处理  
    if( motor_pid->ek_sum>(Out_Speed_Max/motor_pid->speed_i))   motor_pid->ek_sum=Out_Speed_Max/motor_pid->speed_i;     
   } 
    motor_pid->out_duty = motor_pid->ek_0*motor_pid->speed_p + motor_pid->speed_i*motor_pid->ek_sum+de*motor_pid->speed_d;  //PI
    
    if( motor_pid->out_duty >= Out_Speed_Max) 		motor_pid->out_duty= Out_Speed_Max;
    if(motor_pid->out_duty <= (-1) * Out_Speed_Max)		motor_pid->out_duty =(-1)*Out_Speed_Max;
	   
     motor_pid->ek_1=motor_pid->ek_0;		 
     return (motor_pid->out_duty);
}

// PIDOUT：PID输出控制参数，为PWM宽度的调整量 
//Kp：比例调节参数 
//Ki：积分调节参数 
//Kd：微分调节参数 
//error：误差量（为期望速度-实际速度，可以为正数或负数） 
//d_error：本次误差量与上次误差量之差（error-pre_error） 
//dd_error：上次误差量与上上次误差量之差（pre_error-pre_pre_error）
