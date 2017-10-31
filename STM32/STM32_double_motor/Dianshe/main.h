#ifndef __MAIN_H
#define __MAIN_H

#include "RCC.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_it.h"
#include "KEY.h"
#include "USART2.h"
#include "PID.h"
#include "TIM.h"
#include "delay.h"
#include "lcd.h"	 
#include "spi.h"
#include "stm32f10x_it.h"
#include "core_cm3.h"
#include "Flash.h"

#define FLASH_SAVE_ADDR  0x8000000+1024*127	//设置FLASH 保存地址(必须为偶数， 
extern u8 Flash_Buff[2];                                       //且其值要大于本代码所占用FLASH的大小+0X08000000)

#define POSITION_MODE  0	
#define SPEED_MODE  0	


extern u32 SET_SPEED;
extern u8 Adjust_Mode_Flag;

void  NVIC_Config(void);
void  USART_Reci(void);
void KEY_Scan(void);

#endif

