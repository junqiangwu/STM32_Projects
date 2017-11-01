#ifndef _KEY_H
#define _KEY_H
#include "stm32f10x.h"

#define KEY0 PBin(8)   //PB8
#define KEY1 PBin(9)   //PB9
#define KEY2 PBin(10)   //PA10
#define KEY3 PBin(11)   //PA11
#define KEY4 PBin(12)   //PA12
#define KEY5 PBin(13)   //PA13
#define KEY6 PBin(14)   //PA14
#define KEY7 PBin(15)   //PA15

void KEY_Init(void);//IO≥ı ºªØ

int KEY_Scan(void);

#endif
