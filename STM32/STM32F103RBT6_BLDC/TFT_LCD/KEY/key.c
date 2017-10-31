#include "key.h"
#include "sys.h" 
#include "delay.h"
							    
//按键初始化函数

//void KEY_Init(void) //IO初始化
//{ 
// 	GPIO_InitTypeDef GPIO_InitStructure;
//	//初始化KEY0-->GPIOA.13,KEY1-->GPIOA.15  上拉输入
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//使能PORTA,PORTE时钟

//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//PE2~4
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
// 	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4

//	//初始化 WK_UP-->GPIOA.0	  下拉输入
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0

//}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||KEY3==1))
	{
		delay_ms(15);//去抖动 
		key_up=0;
		if(KEY0==0)return 1;
		else if(KEY1==0)return 2;
		else if(KEY2==0)return 3;
		else if(KEY3==1)return 4;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==0)key_up=1; 	    
 	return 0;// 无按键按下
}
