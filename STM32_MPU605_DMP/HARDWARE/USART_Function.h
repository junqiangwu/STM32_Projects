#ifndef _USART_Function_H
#define _USART_Function_H
#include "stm32f10x.h"


void cmd_help_func(int argc,char *argv[]);


void Stop(int argc,char *argv[]);
void Start(int argc,char *argv[]);

void set_speed_p(int argc,char *argv[]);
void set_speed_i(int argc,char *argv[]);
void set_speed_d(int argc,char *argv[]);

void set_speed_p_2(int argc,char *argv[]);
void set_speed_i_2(int argc,char *argv[]);
void set_speed_d_2(int argc,char *argv[]);

void set_test(int argc,char *argv[]);
#endif


