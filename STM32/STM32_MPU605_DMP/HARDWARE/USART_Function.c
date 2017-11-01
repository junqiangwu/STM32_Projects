#include "USART_Function.h"
#include "USART2.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "PID.h"
#include "Control_Motor.h"

extern u8 RUN_MODE;
extern Pid_struct motor13_parameter,motor24_parameter;

void Stop(int argc,char *argv[]){
    PWM_Y_13(0);
    PWM_X_24(0);
    RUN_MODE=0;
    uprintf(USART1,"结束运行\n");
}

extern u8 Print;
void Start(int argc,char *argv[]){
    float param;
	if (argc <2){
		uprintf(USART1,"error!set_speed_p\n");
	}
    Print=atof (argv[1]);
    uprintf(USART1,"开始运行\n");
}

void set_speed_p(int argc,char *argv[]){
  	float param;
	if (argc <2){
		uprintf(USART1,"error!set_speed_p\n");
	}    
      motor13_parameter.speed_p =atof (argv[1]); 
      uprintf(USART1,"   motor_pid%f\n",    motor13_parameter.speed_p);
}


void set_speed_i(int argc,char *argv[]){
  	float param;
	if (argc <2){
		uprintf(USART1,"error!set_speed_p\n");
	}   
     motor13_parameter.speed_i =atof (argv[1]);
     uprintf(USART1,"  motor_pid->I%f\n",   motor13_parameter.speed_i );
}


void set_speed_d(int argc,char *argv[]){
  	float param;
	if (argc <2){
		uprintf(USART1,"error!set_speed_p\n");
	}
    
   motor13_parameter.speed_d =atof (argv[1]);
   uprintf(USART1,"  motor_pid->D%f\n",  motor13_parameter.speed_d );
}


void set_speed_p_2(int argc,char *argv[]){
  	float param;
	if (argc <2){
		uprintf(USART1,"error!set_speed_p\n");
	}
    
    motor24_parameter.speed_p = atof (argv[1]);
    uprintf(USART1," motor24_parameter.speed_p %f\n", motor24_parameter.speed_p);
}

void set_speed_i_2(int argc,char *argv[]){
  	float param;
	if (argc <2){
		uprintf(USART1,"error!set_speed_p\n");
	} 
   motor24_parameter.speed_i =atof (argv[1]);
   uprintf(USART1," motor24_parameter.speed_i \r%f\n", motor24_parameter.speed_i);
}

void set_speed_d_2(int argc,char *argv[]){
  	float param;
	if (argc <2){
		uprintf(USART1,"error!set_speed_p\n");
	} 
   motor24_parameter.speed_d =atof (argv[1]);
   uprintf(USART1," motor24_parameter.speed_d %f\n", motor24_parameter.speed_d);
}

void set_test(int argc,char *argv[]){
  	float param;
	if (argc <2){
		uprintf(USART1,"error!set_test\n");
	}
    else{
       RUN_MODE = atof (argv[1]);
       uprintf(USART1,"设置运行模式 %d\n",RUN_MODE);
  }  }

