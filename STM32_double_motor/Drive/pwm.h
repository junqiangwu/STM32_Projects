#ifndef __PWM_H
#define __PWM_H
#include "sys.h"


#define PWM3_SET  GPIO_SetBits(GPIOC, GPIO_Pin_1)//PC1 
#define PWM3_RESET  GPIO_SetBits(GPIOC, GPIO_Pin_1)//PC1 

void PWM_Init(u16 arr,u16 psc);
int Motor1(int Nextpoint);
int Motor2(int Nextpoint);
#endif
