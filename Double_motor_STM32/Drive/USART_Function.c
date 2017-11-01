#include "USART_Function.h"
#include "USART2.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"

void cmd_help_func(int argc,char *argv[]){
     uprintf(USART1,"error2222\n");
}

void set_speed_p(int argc,char *argv[]){
  	float param;
	if (argc <2){
		uprintf(USART1,"error!exp:position_p 2\n");
	}
	param = atof (argv[1]);
    
    uprintf(USART1,"%f",param);
}
