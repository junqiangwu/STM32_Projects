#ifndef _PID_H
#define _PID_H




extern int Motor_PIDAdjust(int NextPoint);
extern int Motor_Slave_PIDAdjust(int NextPoint);

extern void Motor_Pid_Init(void);
extern void Motor_Slave_Pid_Init(void);
#endif
