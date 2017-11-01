/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : main.c
* Author             : IMS Systems Lab
* Description        : Main program body.
********************************************************************************

********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#define	  root
#include <Ram.h>

#include "STM103REG.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_MClib.h"
#include "MC_Globals.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

void NVIC_Configuration(void);
void GPIO_Configuration(void);
void RCC_Configuration(void);

unsigned char Res_f=0;	 //上电防止出现过流

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{			//	s16 te;

#ifdef DEBUG
  debug();
#endif
    
  RCC_Configuration();
  GPIO_Configuration(); 


#ifdef THREE_SHUNT  //三个电阻电流取样
  SVPWM_3ShuntInit(); //设置电流取样，PWM周期
#elif defined ICS_SENSORS
  SVPWM_IcsInit();
#elif defined SINGLE_SHUNT
  SVPWM_1ShuntInit();
#endif
  
#ifdef ENCODER
  
   #ifdef OBSERVER_GAIN_TUNING
      STO_StateObserverInterface_Init();
      STO_Init();
   #endif
#elif defined HALL_SENSORS
  
   #ifdef OBSERVER_GAIN_TUNING
      STO_StateObserverInterface_Init();
      STO_Init();
   #endif
#elif defined NO_SPEED_SENSORS
    STO_StateObserverInterface_Init();
    STO_Init();
   #ifdef VIEW_ENCODER_FEEDBACK
      ENC_Init();
   #elif defined VIEW_HALL_FEEDBACK
      HALL_HallTimerInit();
   #endif
#endif

#ifdef DAC_FUNCTIONALITY   
  MCDAC_Init();
#endif

  TB_Init();//定时器初始化
  
  PID_Init(&PID_Torque_InitStructure, &PID_Flux_InitStructure, &PID_Speed_InitStructure);//初始化PID值
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
#ifdef BRAKE_RESISTOR
    MCL_Brake_Init();
#endif
      
  KEYS_Init();
  
  /* TIM1 Counter Clock stopped when the core is halted */
  //DBGMCU_Config(DBGMCU_TIM1_STOP, ENABLE);
  
  // Init Bus voltage and Temperature average  
  MCL_Init_Arrays();
    
 // STM3210C_LCD_Init();
  //LCD_Clear(White);
  //LCD_SetTextColor(Blue);
 // LCD_SetBackColor(White);
    
  //Display_Welcome_Message();   
  NVIC_Configuration(); 
//	LcdReset();
  Res_f=1; //上电完成
  fgDispMode=1;
//wGlobal_Flags |= SPEED_CONTROL;
  while(1)
  { 
 
   KeyProc();
   Timeproc(); 
  	
    switch (State)
    {
      case IDLE:    // Idle state   
      break;
      
      case INIT:
        MCL_Init();
        TB_Set_StartUp_Timeout(3000);//延时1.5S吗
        State = START; 
      break;
        
      case START:  
      //passage to state RUN is performed by startup functions; 
      break;
          
      case RUN:   // motor running       
#ifdef ENCODER
        if(ENC_ErrorOnFeedback() == TRUE)
        {
          MCL_SetFault(SPEED_FEEDBACK);
        }
#elif defined HALL_SENSORS        
        if(HALL_IsTimedOut())
        {
          MCL_SetFault(SPEED_FEEDBACK);
        } 
	//	te=	HALL_GetSpeed();/te/
        if ( HALL_GetSpeed()== HALL_MAX_SPEED)
        {
          MCL_SetFault(SPEED_FEEDBACK);
        } 
#elif defined NO_SPEED_SENSORS
      
#endif     
      break;  
      
      case STOP:    // motor stopped
          // shutdown power         
          /* Main PWM Output Disable */
          TIM_CtrlPWMOutputs(TIM1, DISABLE);
          
          State = WAIT;
          
#ifdef THREE_SHUNT          
          SVPWM_3ShuntAdvCurrentReading(DISABLE);
#endif   
#ifdef SINGLE_SHUNT          
          SVPWM_1ShuntAdvCurrentReading(DISABLE);
#endif
          Stat_Volt_alfa_beta.qV_Component1 = Stat_Volt_alfa_beta.qV_Component2 = 0;
          
#ifdef ICS_SENSORS
          SVPWM_IcsCalcDutyCycles(Stat_Volt_alfa_beta);
#elif defined THREE_SHUNT
          SVPWM_3ShuntCalcDutyCycles(Stat_Volt_alfa_beta);
#endif                                                
          TB_Set_Delay_500us(2000); // 1 sec delay
      break;
      
      case WAIT:    // wait state
          if (TB_Delay_IsElapsed() == TRUE) 
          {
#ifdef ENCODER            
            if(ENC_Get_Mechanical_Speed() ==0)             
            {              
              State = IDLE;              
            }
#elif defined HALL_SENSORS      
            if (HALL_IsTimedOut())
            {               
              State=IDLE;
            } 
#elif defined NO_SPEED_SENSORS
            STO_InitSpeedBuffer();
            State=IDLE; 
#endif            
          }
        break;
    
      case FAULT:                   
          if (MCL_ClearFault() == TRUE)
          {
            if(wGlobal_Flags & SPEED_CONTROL == SPEED_CONTROL)
            {
              bMenu_index = CONTROL_MODE_MENU_1;
            }
            else
            {
              bMenu_index = CONTROL_MODE_MENU_6;
            }       
#if defined NO_SPEED_SENSORS
            STO_InitSpeedBuffer();
#endif            
            State = IDLE;
            wGlobal_Flags |= FIRST_START;
          }
        break;
    
      default:        
        break;
    }
  }
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the TIM1 Pins.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
//  GPIO_InitTypeDef GPIO_InitStructure;

  //RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF, ENABLE);

  //GPIO_DeInit(GPIOF);
  //GPIO_StructInit(&GPIO_InitStructure);
                  
  /* Configure PF.06, PF.07, PF.08 and PF.09 as Output push-pull for debugging
     purposes */
    GPIO_InitTypeDef GPIO_InitStructure;
   

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|
                        	RCC_APB2Periph_ADC1|RCC_APB2Periph_USART1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	/*-----------------GPIOA--------------------*/
	//DI点
	//GPIO_InitStructure.GPIO_Pin =  PAI_KSET;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	
 	//DO 点
	GPIO_InitStructure.GPIO_Pin = PAO_LCDDB0|PAO_LCDEN|PAO_LCDRS;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/*-----------------GPIOB--------------------*/ 
	
	//DI点
	GPIO_InitStructure.GPIO_Pin =  PBI_KDOWN|PBI_KDOWN|PBI_KRIGHT|PBI_KUP|PBI_URTRX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//DO 点
	GPIO_InitStructure.GPIO_Pin =  PBO_LCDCS1|PBO_LCDCS2|PBO_LCDDB5|PBO_LCDDB6|PBO_LCDDB7|PBO_MBREAK|PBO_M_EN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PBI_URTRX;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PBO_URTTX;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*-----------------GPIOC--------------------*/ 
	//DI点
	GPIO_InitStructure.GPIO_Pin =  PCI_DEFAULT|PCI_HELLA|PCI_HELLB|PCI_HELLC|PCI_KLEFT|PCI_KSET;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PCO_IIC_CLK|PCO_IIC_DATA|PCO_LCDDB1|PCO_LCDDB2|PCO_LCDDB3|PCO_LED1;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/*-----------------GPIOD--------------------*/ 
	GPIO_InitStructure.GPIO_Pin = PDO_LCDDB4;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
    
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{ 
  ErrorStatus HSEStartUpStatus;

  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  
  if(HSEStartUpStatus == SUCCESS)
  {
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the Vector Table base address.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
    //printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  }
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
