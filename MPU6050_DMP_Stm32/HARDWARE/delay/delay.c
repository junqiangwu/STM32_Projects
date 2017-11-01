/*-------------------------------------
	      延时配置程序
包括us,ms,时钟初始化都在本程序能够完成
已经将这几个程序封装成库,拿来即可使用
		  2013年7月22日
-------------------------------------*/
//#define UCOS_DELAY_OPEN		  //延时选择开关
  #define NO_UCOS_DELAY_OPEN

#include "delay.h"
#include "misc.h"
#include "stm32f10x.h"

/*-------------------------------------
	 	   裸机的延时 
-------------------------------------*/
#ifdef NO_UCOS_DELAY_OPEN
unsigned int TimeDelay;	 //声明的全局变量

void DelayInit(void)
{
	if(SysTick_Config(SystemCoreClock / 1000000)) //配置为1us一次中断
	{
		while(1);  	//如果配置失败则死循环
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //失能滴答定时器
}

void delayus(unsigned int us)
{
	TimeDelay = us;	 						   //us赋值给全局变量TD
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  //使能滴答定时器
	while(TimeDelay != 0);					   //在中断中自减
}	

void delayms(unsigned int ms)
{
	delayus(ms * 1000);		//1ms = 1000us
}

void SysTick_Handler(void)						//在中断中自减
{
	if (TimeDelay != 0)
	{ 
		TimeDelay--;
	}	
}
#endif


/*-------------------------------------
	 	   跑系统的延时 
-------------------------------------*/
#ifdef UCOS_DELAY_OPEN

#include "ucos_ii.h"
unsigned int reload;
static unsigned char  fac_us=0;
static unsigned short int fac_ms=0;
void DelayInit()
{	
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us = SystemCoreClock/8000000;	//为系统时钟的1/8 
	
	reload=SystemCoreClock/8000000;		//每秒钟的计数次数 单位为K	   
	reload*=1000000/OS_TICKS_PER_SEC;						
	fac_ms=1000/OS_TICKS_PER_SEC;		//代表ucos可以延时的最少单位	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   
	SysTick->LOAD=reload; 		
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;	
}

void SysTick_Handler(void)	  //使用UCOS时
{				   
	OSIntEnter();		
    OSTimeTick();                   
    OSIntExit();        
}

void delayus(unsigned int nus)
{		
	unsigned int ticks;
	unsigned int told,tnow,tcnt=0;
	unsigned int reload=SysTick->LOAD;	//LOAD的值	    	 
	ticks=nus*fac_us; 			//需要的节拍数	  		 
	tcnt=0;
	told=SysTick->VAL;        	//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;//时间超过/等于要延迟的时间,则退出.
		}  
	}; 									    
}

void delayms(unsigned short int nms)
{	
	if(OSRunning==TRUE)//如果os已经在跑了	    
	{		  
		if(nms>=fac_ms)//延时的时间大于ucos的最少时间周期 
		{
   			OSTimeDly(nms/fac_ms);//ucos延时
		}
		nms%=fac_ms;				//ucos已经无法提供这么小的延时了,采用普通方式延时    
	}
	delayus((unsigned int)(nms*1000));	//普通方式延时,此时ucos无法启动调度.
}
#endif

void Software_Delay(unsigned int sec)
{
	for(;sec>0;sec--);
}
