
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_it.h"
#include "delay.h"
#include "BLDC.h"
#include "main.h"

/**********************************************************************
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
**********************************************************************/

/*********全局变量***********/
FlagStatus Direction = SET;
FlagStatus zheng_fan = RESET;
extern u8 duzhuan_value;	

void BLDC_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
 	 
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;  //TIM1输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;						   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
			 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;  //TIM1互补输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;						   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;              //TIM3的霍尔输入
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                     
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);    //重映射到 PB6/7/8
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;              //TIM3的霍尔输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                         //PB12  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  
    
}

void BLDC_TIM1Config(void)
{
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;			   //基本结构体变量定义
   TIM_OCInitTypeDef  		TIM_OCInitStructure;             //输出结构体变量定义
   TIM_BDTRInitTypeDef  	TIM_BDTRInitStructure;			     //死区刹车结构体变量定义
   
   TIM_DeInit(TIM1);   //缺省值

   TIM_TimeBaseStructure.TIM_Prescaler = 2;					   //TIM基本初始化
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned2;//中央对齐计数模式,输出比较标志位只有在比较器向上计算被设置
   TIM_TimeBaseStructure.TIM_Period = 2500 - 1;					   //PWM 16K
   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
   TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;				   

   TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);   //初始化TIM1 时间基数

   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 		   //TIM输出通道初始化
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; 
   TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;                  
   TIM_OCInitStructure.TIM_Pulse =0; 
    
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
   TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;         
   TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//TIM_OCIdleState_Set;
   TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;          
  
   TIM_OC1Init(TIM1,&TIM_OCInitStructure); 

   TIM_OCInitStructure.TIM_Pulse =0;        //初始化TIM1 的通道2
   TIM_OC2Init(TIM1,&TIM_OCInitStructure);

   TIM_OCInitStructure.TIM_Pulse =0;
   TIM_OC3Init(TIM1,&TIM_OCInitStructure);

   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 		   //TIM输出通道4初始化，用来触发AD注入采样
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;                   
   TIM_OCInitStructure.TIM_Pulse =1500; 
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;           
  
   TIM_OC4Init(TIM1,&TIM_OCInitStructure); 
 
   TIM_SelectOutputTrigger(TIM1,TIM_TRGOSource_OC4Ref);
 
   TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;	//死区刹车初始化
   TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
   TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; 
   TIM_BDTRInitStructure.TIM_DeadTime = 100;
   TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;	 //如打开，开机无输出且状态紊乱？？？？
   TIM_BDTRInitStructure.TIM_BreakPolarity =  TIM_BreakPolarity_Low ;
   TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;

   TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure);

   TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);  //使能捕获比较寄存器预装载（通道1）

   TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);	 //使能捕获比较寄存器预装载（通道2）

   TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Enable);	 //使能捕获比较寄存器预装载（通道3）
   
   TIM_SelectInputTrigger(TIM1, TIM_TS_ITR2);        //输入触发源选择TIM3   
    
	 // TIM_CCPreloadControl(TIM1,ENABLE);   // CCPC1预装载使能
   // TIM_SelectCOM(TIM1,ENABLE);
      
   // TIM_ITConfig(TIM1, TIM_IT_CC4 ,ENABLE);    //触发AD采样，采集相电流
    
     TIM_SelectInputTrigger(TIM1, TIM_TS_ITR2);        //输入触发源选择TIM3   
  
   //TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Trigger);//从模式选择 触发	  

     TIM_CtrlPWMOutputs(TIM1,ENABLE);
		 
	//   TIM_ClearITPendingBit(TIM1, TIM_IT_Break|TIM_IT_COM);
	//   TIM_ITConfig(TIM1, TIM_IT_Break | TIM_IT_COM ,ENABLE);
		 TIM_ClearITPendingBit( TIM1, TIM_IT_COM);
		 TIM_ITConfig(TIM1, TIM_IT_COM ,ENABLE);
     TIM_Cmd(TIM1,ENABLE);  
}


void BLDC_TIM3Config(void)
{
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;			   //基本结构体变量定义
   TIM_ICInitTypeDef  TIM_ICInitStructure;                      //定义结构体变量
   TIM_OCInitTypeDef  TIM_OCInitStructure;                     //输出结构体变量定义
      
   TIM_DeInit(TIM3);

   TIM_TimeBaseStructure.TIM_Prescaler = 71;				   //TIM基本初始化
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBaseStructure.TIM_Period =65535;     //只能通过霍尔信号变化清零
   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
   TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

   TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);     
   
   TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;            //选择通道1
   TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; //输入上升沿捕获  
   TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_TRC;  //配置通道为输入，并映射到哪里
   TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;       //输入捕获预分频值
   TIM_ICInitStructure.TIM_ICFilter = 10;                      //输入滤波器带宽设置

   TIM_ICInit(TIM3, &TIM_ICInitStructure);                     //输入通道配置

   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 		    //TIM输出通道初始化
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;             
   TIM_OCInitStructure.TIM_Pulse =100; 
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      
  
   TIM_OC2Init(TIM3,&TIM_OCInitStructure);

   TIM_SelectHallSensor(TIM3,ENABLE);      // //TIMx CH1/2/3 通道异或产生TRGO
   
   TIM_SelectInputTrigger(TIM3, TIM_TS_TI1F_ED);               //输入触发源选择 TI1F_ED    
  
   TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);             //从模式 复位 

   TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);//主从模式选择        
   
   TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_OC2Ref); //选择输出触发模式(TRGO端)，通过OC2触发com事件
   
}

void huanxiang(void)
{
  u8 irq_zhuanzi; 

  irq_zhuanzi =(u8)((GPIOC->IDR&0x000000c0)>>6);	//读位置 1100 0000 C6/7  0011
  irq_zhuanzi|=(u8)((GPIOC->IDR&0x00000100)>>6);   // 1 0000 0000 C8  0100
  irq_zhuanzi &= 0x07;   // 二进制求与  0000  0111
	
  switch(irq_zhuanzi)				 //根据转子位置，决定CCER输出相位和转子字偏移量
    {
	  case 0x06:{	                      												
			  	  TIM1->CCER=0x3081;  //1,4  ba   
			    };break;
	  case 0x02:{
				  TIM1->CCER=0x3180;	  //4,5	   bc     	   
	            };break;							
  	case 0x03:{
				  TIM1->CCER=0x3108;	//2,5    ac
			    };break;
	  case 0x01:{
				  TIM1->CCER=0x3018;	//2,3	    ab      		
	            };break;
	  case 0x05:{
				  TIM1->CCER=0x3810;	//3,6	    cb      		
	            };break;
	  case 0x04:{
	             			
				  TIM1->CCER=0x3801;    //1,6	  ca        		
	            };break;
				default:break;
  }	 
}


/**************启动******************/
void BLDC_Start1(void)
{   u8 a=0;
    while (a<100){
      a++;
    BLDC_Start();
    delay_ms(5);
    BLDC_Stop();
    delay_ms(2);
    BLDC_Start();
    }
}

void BLDC_Start(void)
{
   TIM_ITConfig(TIM3, TIM_IT_Trigger, ENABLE);      //开定时器中断  
   TIM1->SMCR|=0x0006;        //开TIM1的输入触发	
   TIM1->DIER=0x0040;         //开TIM1的触发中断
   huanxiang();			      //调用换向函数，启动	
   TIM3->CR1|=0x0001;		  //开TIM3
   TIM3->DIER|=0x0050;		  //开TIM3中断  
}

extern u8 Assist;
extern u8 Mileage;
extern u8 datatemp[2];
void BLDC_Stop(void)
{

	
   TIM1->SMCR&=0xfff8;		  //关闭TIM1的输入触发
   TIM1->CCER=0;              //关闭TIM1的六路输出
   TIM1->CCER=0x0ccc;         //打开三路下管，进行能耗制动
   TIM1->CCER=0;              //关闭TIM1的六路输出，关刹车		  
   TIM3->CR1&=0xfffe;         //关闭TIM3						  
   TIM3->CNT=0;               //清TIM3的计数器				   
   TIM3->DIER&=0xffaf;		  //关TIM3中断       
}


