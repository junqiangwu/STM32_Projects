#ifndef _PID_H
#define _PID_H


#define Out_Speed_Max  2200	   //2500占空比
#define Out_Speed_Min   0

#define MAX_Speed_Targe  11000
#define MIN_Speed_Targe  0

typedef struct  
{ 
	int speed_target;     //设定目标 
  int out_duty;  // 输出占空比
	
	float speed_p; 		//比例常数
	float speed_i; 		//积分常数
	float speed_d;     //微分参数
	
	float ek_0;		  //当前误差
	float ek_sum; 	// 积分项
}Pid_struct;


extern void Speed_PIDInit(Pid_struct *motor_pid);
extern int Speed_PIDAdjust(Pid_struct *motor_pid,int Targe);

#endif
