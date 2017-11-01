#ifndef __TIM_H
#define __TIM_H
#include "stm32f10x.h"

extern u8 RUN_MODE;
extern float Zheng_W;
extern u16 Fang_T;


void TIM2_Init(u16 arr,u16 psc);
void TIM4_Init(u16 arr,u16 psc);
#endif
