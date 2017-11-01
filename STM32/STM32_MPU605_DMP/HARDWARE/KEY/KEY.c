#include "stm32f10x.h"
#include "delay.h"
#include "key.h"
#include "USART2.h"
/*******************************************************
*******矩阵键盘设置相应题号及功能
*******************************************************/
void KEY_Init(void) //初始化矩阵键盘要使用的GPIO口。
{

 	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       //定义PB8到PB11为上拉输入、、。
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;       //定义PB12到PB15为下拉输入。
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_Init(GPIOB,&GPIO_InitStructure);		
}

extern u8 RUN_MODE;

int KEY_Scan(void) //实现矩阵键盘。返回值为，各按键的键值，此键值由用户自己定义。
{
	u8 KeyVal=0;	 //keyVal为最后返回的键值。
    
	GPIO_Write(GPIOB,(GPIOB->ODR & 0xf0ff | 0x0f00)); //先让PB8到PB11全部输出高。
	
	// 消抖
	if((GPIOB->IDR & 0xf000)==0x0000)  //如果PB12到PB15全为0，则没有键按下。此时，返回值为-1.
    {   return -1;    }
	else
	{	
	    delay_ms(5);    //延时5ms去抖动。
	  if((GPIOB->IDR & 0xf000)==0x0000)//如果延时5ms后，PB12到PB15又全为0，则，刚才引脚的电位变化是抖动产生的.
        return -1;   }
    
  
  GPIO_Write(GPIOB,(GPIOB->ODR & 0xf0ff | 0x0100));	//让PB11到PB8输出二进制的0001.

		switch(GPIOB->IDR & 0xf000)//对PB12到PB15的值进行判断，以输出不同的键值。
			{
				case 0x1000: KeyVal=1; RUN_MODE=1;   uprintf(USART1," 设置为第一题\r\n");  break;
				case 0x2000: KeyVal=2; RUN_MODE=2;   uprintf(USART1," 设置为第二题!\r\n");  break;
				case 0x4000: KeyVal=3; RUN_MODE=3;   uprintf(USART1," 设置为第三题!\r\n");	break;
				case 0x8000: KeyVal=10;	break;
			}
	   
	GPIO_Write(GPIOB,(GPIOB->ODR & 0xf0ff | 0x0200));	//让PB11到PB8输出二进制的0010.
		switch(GPIOB->IDR & 0xf000)		        //对PB12到PB15的值进行判断，以输出不同的键值。
		{
			case 0x1000: KeyVal=4; RUN_MODE=4;  uprintf(USART1," 设置为第四题!\r\n");	break;
			case 0x2000: KeyVal=5; RUN_MODE=5;  uprintf(USART1," 设置为第五题!\r\n");	break;
			case 0x4000: KeyVal=6; RUN_MODE=6;  uprintf(USART1," 设置为第六题!\r\n"); break;
			case 0x8000: KeyVal=11;	break;
		}

	GPIO_Write(GPIOB,(GPIOB->ODR & 0xf0ff | 0x0400));	//让PB11到PB8输出二进制的0100.
		switch(GPIOB->IDR & 0xf000)		        //对PB12到PB15的值进行判断，以输出不同的键值。
		{
			case 0x1000: KeyVal=7;	break;
			case 0x2000: KeyVal=8;	break;
			case 0x4000: KeyVal=9;	break;
			case 0x8000: KeyVal=12;	break;
		}
 
	 GPIO_Write(GPIOB,(GPIOB->ODR & 0xf0ff | 0x0800));	//让PB11到PB8输出二进制的1000.
		switch(GPIOB->IDR & 0xf000)		        //对PB12到PB15的值进行判断，以输出不同的键值。
		{
			case 0x1000: KeyVal=14;	break;
			case 0x2000: KeyVal=0;	break;
			case 0x4000: KeyVal=15;	break;
			case 0x8000: KeyVal=13;	break;
		}			
	return KeyVal;		
}

