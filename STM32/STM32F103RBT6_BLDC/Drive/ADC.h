#ifndef	 _ADC_H
#define  _ADC_H
#include "stm32f10x.h"

void Adc_Init(void);
void DMA_Configuration(void);
void Get_Adc_Average(void); 
u16 Get_Adc(u8 ch,u8 times);
#endif

