#ifndef __MAIN_H
#define __MAIN_H

#include "RCC.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_it.h"
#include "BLDC.h"
#include "KEY.h"
#include "USART2.h"
#include "ADC.h"
#include "PID.h"
#include "TIM.h"
#include "Encode.h"
#include "EXTI.h"
#include "delay.h"
#include "lcd.h"	 
#include "spi.h"
#include "stm32f10x_it.h"
#include "core_cm3.h"
#include "Flash.h"

#define FLASH_SAVE_ADDR  0x8000000+1024*127	//设置FLASH 保存地址(必须为偶数， 
extern u8 Flash_Buff[2];                                       //且其值要大于本代码所占用FLASH的大小+0X08000000)

#define MAX_V_BUS 210
#define MIN_V_BUS 160
extern u8  Mileage;
extern u8 ERROR_flag;
extern u8 Ref_Lcd_flag;
extern u8 state; 
extern u8 ERROR_flag;
extern u32 TIMER;

void FRAME_INIT(void);
void FRAME_INIT1(void);
void Ref_LCD(void);
void  NVIC_Config(void);
void  USART_Reci(void);
void mes_show(u16 a,float b,u8 c,float d,u8 e);
void mes_show1(float a,float b,u8 c,float d,u16 e,u16 f);
void KEY_Scan(void);
void Adjust_Speed(void);
#endif

