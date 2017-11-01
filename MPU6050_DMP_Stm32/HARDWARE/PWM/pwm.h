#ifndef __PWM_H
#define __PWM_H
#include "sys.h"


#define PWM3_SET  GPIO_SetBits(GPIOC, GPIO_Pin_1)//PC1 
#define PWM3_RESET  GPIO_SetBits(GPIOC, GPIO_Pin_1)//PC1 

void PWM_Init_2(u16 arr,u16 psc);
void PWM_Init(u16 arr,u16 psc);
int Motor1(int Nextpoint);
int Motor2(int Nextpoint);

void Mode_1(void);
void Mode_2(void);
void Mode_3(void);
void Mode_4(void);
void Mode_5(void);
#endif
