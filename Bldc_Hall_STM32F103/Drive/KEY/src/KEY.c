#include "main.h"

void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA,PORTC时钟
 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOA.0

} 

u16 KEY_Time=0;
vu16 KEY_UP[3]={1,1,1};

u8 KEY_Scan1(u8 mode)
{	 
	//static u8 key_up=1;   //按键按松开标志
	//if(mode)key_up=1;  //支持连按		  
	if( KEY_UP[0]&& KEY_UP[1]&& KEY_UP[2]&&(KEY0==0||KEY1==0||KEY2==0))
	{
		delay_us(500);//去抖动 
		if(KEY0==0)  
        {   KEY_UP[0]=0;   
        }
		else if(KEY1==0)
        {   KEY_UP[1]=0;       
        }   
        else if(KEY2==0)
        {   KEY_UP[2]=0;       
        }
	}
    else if(KEY0==1&&KEY1==1&&KEY2==1)   //KEY0_s
       {
     if( KEY_UP[0]==0){
           if((KEY_Time<=100)&&(KEY_Time>=0))				
             {
                 KEY_UP[0]=1; 
                 KEY_Time=0; 
                 return KEY0_s;      
             }
          else if (KEY_Time>100)	
            {	
               KEY_UP[0]=1;  
               KEY_Time=0;
               return KEY0_l;
           } 
       }
               
       if( KEY_UP[1]==0){
           if((KEY_Time<=100)&&(KEY_Time>=0))				
             {
                 KEY_UP[1]=1; 
                 KEY_Time=0; 
                 return KEY1_s;      
             }
          else if (KEY_Time>100)	
            {	
               KEY_UP[1]=1;  
               KEY_Time=0;
               return KEY1_l;
           }  
       }
     
     if( KEY_UP[2]==0){
       if((KEY_Time<=100)&&(KEY_Time>=0))				
         {
              KEY_UP[2]=1;
             KEY_Time=0; 
             return KEY2_s;      
         }
      else if (KEY_Time>100)	
        {	  
            KEY_UP[2]=1;
            KEY_Time=0;
           return KEY2_l;
       }   
     }
        KEY_UP[0]=1;
        KEY_UP[1]=1;
        KEY_UP[2]=1;     
   }
       
	return 0;// 无按键按下
}

