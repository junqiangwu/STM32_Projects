/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_hall.c
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : Module handling speed feedback provided by three Hall 
*                      sensors
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
* 26/06/08 v2.0.1
* 27/06/08 v2.0.2
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
#include "stm32f10x_lib.h"
#include "stm32f10x_hall.h"
#include "MC_hall_prm.h"
#include "MC_Globals.h"
#include "stm32f10x_MClib.h"
#include "stm32f10x_it.h"
/* Private define ------------------------------------------------------------*/
#define HALL_MAX_SPEED_FDBK (u16)(HALL_MAX_SPEED_FDBK_RPM/6 * POLE_PAIR_NUM)
#define HALL_MIN_SPEED_FDBK (u16)(HALL_MIN_SPEED_FDBK_RPM/6* POLE_PAIR_NUM)
#define LOW_RES_THRESHOLD   ((u16)0x5500u)// If capture below, ck prsc decreases
#define	ROTOR_SPEED_FACTOR  ((u32)((CKTIM*10)) / 3)
#define PSEUDO_FREQ_CONV    ((u32)(ROTOR_SPEED_FACTOR / (SAMPLING_FREQ * 10)) * 0x10000uL)
#define SPEED_OVERFLOW      ((u32)(ROTOR_SPEED_FACTOR / HALL_MAX_SPEED_FDBK))
#define MAX_PERIOD          ((u32)(ROTOR_SPEED_FACTOR / HALL_MIN_SPEED_FDBK))
#define HALL_COUNTER_RESET  ((u16) 0)
#define S16_PHASE_SHIFT     (s16)(HALL_PHASE_SHIFT * 65536/360)
#define S16_120_PHASE_SHIFT (s16)(65536/3)
#define S16_60_PHASE_SHIFT  (s16)(65536/6)

#define STATE_0 (u8)0
#define STATE_1 (u8)1
#define STATE_2 (u8)2
#define STATE_3 (u8)3
#define STATE_4 (u8)4
#define STATE_5 (u8)5
#define STATE_6 (u8)6
#define STATE_7 (u8)7

#define NEGATIVE          (s8)-1
#define POSITIVE          (s8)1
#define NEGATIVE_SWAP     (s8)-2
#define POSITIVE_SWAP     (s8)2
#define ERROR             (s8)127

#define GPIO_MSK (u8)0x07
#define ICx_FILTER (u8) 0x0B // 11 <-> 1333 nsec 

#define TIMx_PRE_EMPTION_PRIORITY 2
#define TIMx_SUB_PRIORITY 0

/* if (HALL_SENSORS_PLACEMENT == DEGREES_120)
The sequence of the states is {STATE_5,STATE_1,STATE_3,STATE_2,STATE_6,STATE_4}
else if (HALL_SENSORS_PLACEMENT == DEGREES_60)
the sequence is {STATE_1,STATE_3,STATE_7,STATE_6,STATE_4,STATE_0}*/

// Here is practically assigned the timer for Hall handling
#if defined(TIMER2_HANDLES_HALL)
    #define HALL_TIMER TIM2
#elif defined(TIMER3_HANDLES_HALL)
    #define HALL_TIMER TIM3
#else // TIMER4_HANDLES_HALL
    #define HALL_TIMER TIM4
#endif

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef struct {
	u16 hCapture;
	u16 hPrscReg;
        s8 bDirection;
	} SpeedMeas_s;

typedef struct {
        u32 wPeriod;
        s8 bDirection;
        } PeriodMeas_s;
/* Private variables ---------------------------------------------------------*/

volatile SpeedMeas_s SensorPeriod[HALL_SPEED_FIFO_SIZE];// Holding the last captures
vu8 bSpeedFIFO_Index;   // Index of above array
vu8 bGP1_OVF_Counter;   // Count overflows if prescaler is too low
vu16 hCaptCounter;      // Counts the number of captures interrupts
volatile PeriodMeas_s PeriodMeas;

volatile bool RatioDec;
volatile bool RatioInc;
volatile bool DoRollingAverage;
volatile bool InitRollingAverage;
volatile bool HallTimeOut;
static s16 hElectrical_Angle; 
static s16 hRotorFreq_dpp;
#if (defined HALL_SENSORS || defined VIEW_HALL_FEEDBACK)
static s8 bSpeed;
#endif
/* Private function prototypes -----------------------------------------------*/
PeriodMeas_s GetLastHallPeriod(void);
PeriodMeas_s GetAvrgHallPeriod(void);
void HALL_StartHallFiltering(void);
u16  HALL_GetCaptCounter(void);
void HALL_ClrCaptCounter(void);

u8  ReadHallState(void); 
/*******************************************************************************
* Function Name  : Hall_HallTimerInit
* Description    : Initializes the timer handling Hall sensors feedback
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/




/*******************************************************************************
* ROUTINE Name : HALL_InitHallMeasure
*
* Description : Clear software FIFO where are "pushed" latest speed information
*           This function must be called before starting the motor to initialize
*	    the speed measurement process.
*
* Input       : None
* Output      : None
* Return      : None
* Note        : First measurements following this function call will be done
*               without filtering (no rolling average).
*******************************************************************************/
void HALL_InitHallMeasure( void )
{
   // Mask interrupts to insure a clean intialization

   TIM_ITConfig(HALL_TIMER, TIM_IT_CC1, DISABLE);
    
   RatioDec = FALSE;
   RatioInc = FALSE;
  // DoRollingAverage = FALSE;  
   DoRollingAverage = TRUE;

   InitRollingAverage = FALSE;
   HallTimeOut = FALSE;

   hCaptCounter = 0;
   bGP1_OVF_Counter = 0;

   for (bSpeedFIFO_Index=0; bSpeedFIFO_Index < HALL_SPEED_FIFO_SIZE; 
                                                             bSpeedFIFO_Index++)
   {
      SensorPeriod[bSpeedFIFO_Index].hCapture = U16_MAX;
      SensorPeriod[bSpeedFIFO_Index].hPrscReg = HALL_MAX_RATIO;
      SensorPeriod[bSpeedFIFO_Index].bDirection = POSITIVE;
   }

   // First measurement will be stored in the 1st array location
   bSpeedFIFO_Index = HALL_SPEED_FIFO_SIZE-1;

   // Re-initialize partly the timer
   HALL_TIMER->PSC = HALL_MAX_RATIO;
   
   HALL_ClrCaptCounter();
     
   TIM_SetCounter(HALL_TIMER, HALL_COUNTER_RESET);
   
   TIM_Cmd(HALL_TIMER, ENABLE);

   TIM_ITConfig(HALL_TIMER, TIM_IT_CC1, ENABLE);

}


/*******************************************************************************
* ROUTINE Name : HALL_GetSpeed
*
* Description : This routine returns Rotor frequency with [0.1Hz] definition.
*		Result is given by the following formula:
*		Frotor = K x (Fosc / (Capture x number of overflow)))
*		where K depends on the number of motor poles pairs
*
* Input    : None
* Output   : None
* Returns  : Rotor mechanical frequency, with 0.1Hz resolution.
* Comments : Result is zero if speed is too low (glitches at start for instance)
*           Excessive speed (or high freq glitches will result in a pre-defined
*           value returned.
* Warning : Maximum expectable accuracy depends on CKTIM: 72MHz will give the
* 	    best results.
*******************************************************************************/
s16 HALL_GetSpeed ( void )
{ 
  s32 wAux;
  
  if( hRotorFreq_dpp == HALL_MAX_PSEUDO_SPEED)
  {
    return (HALL_MAX_SPEED);
  }
  else
  {
    wAux = ((hRotorFreq_dpp* SAMPLING_FREQ * 10)/(65536*POLE_PAIR_NUM));
    return (s16)wAux;
  }
}

/*******************************************************************************
* ROUTINE Name : Hall_GetRotorFreq
*
* Description : This routine returns Rotor frequency with an unit that can be
*               directly integrated to get the speed in the field oriented
*               control loop.
*
* Input    : None
* Output   : None
* Returns  : Rotor mechanical frequency with rad/PWM period unit
*             (here 2*PI rad = 0xFFFF).
* Comments : Result is zero if speed is too low (glitches at start for instance)
*           Excessive speed (or high freq glitches will result in a pre-defined
*           value returned.
* Warning : Maximum expectable accuracy depends on CKTIM: 72MHz will give the
* 	    best results.
*******************************************************************************/
s16 HALL_GetRotorFreq ( void )
{
   PeriodMeas_s PeriodMeasAux;

   if ( DoRollingAverage)
   {
      PeriodMeasAux = GetAvrgHallPeriod();
   }
   else
   {  // Raw period
      PeriodMeasAux = GetLastHallPeriod();
   }

   if (HallTimeOut == TRUE)
   {
      hRotorFreq_dpp = 0;
   }
   else
   {
     if(PeriodMeasAux.bDirection != ERROR)
     //No errors have been detected during rotor speed information extrapolation          
     {
        if ( HALL_TIMER->PSC >= HALL_MAX_RATIO )/* At start-up or very low freq */
        {                           /* Based on current prescaler value only */
           hRotorFreq_dpp = 0;
        }
        else
        {
           if( PeriodMeasAux.wPeriod > MAX_PERIOD) /* Speed is too low */
           {
              hRotorFreq_dpp = 0;
           }
           else
           {  /*Avoid u32 DIV Overflow*/
              if ( PeriodMeasAux.wPeriod > (u32)SPEED_OVERFLOW )
              {
                if (HALL_GetCaptCounter()<2)// First capture must be discarded
                {
                  hRotorFreq_dpp=0;
                }
                else                  
                {
                   hRotorFreq_dpp = (s16)((u16) (PSEUDO_FREQ_CONV /
                                                          PeriodMeasAux.wPeriod));
                   hRotorFreq_dpp *= PeriodMeasAux.bDirection;               
                }
              }
              else
              {
                hRotorFreq_dpp = HALL_MAX_PSEUDO_SPEED;
              }
           }
        }
     }          
   }

   return (hRotorFreq_dpp);
}


/*******************************************************************************
* ROUTINE Name : HALL_ClrTimeOut
*
* Description     : Clears the flag indicating that that informations are lost,
*                   or speed is decreasing sharply.
* Input           : None
* Output          : Clear HallTimeOut
* Return          : None
*******************************************************************************/
void HALL_ClrTimeOut(void)
{
   HallTimeOut = FALSE;
}


/*******************************************************************************
* ROUTINE Name : HALL_IsTimedOut
*
* Description     : This routine indicates to the upper layer SW that Hall 
*                   sensors information disappeared or timed out.
* Input           : None
* Output          : None
* Return          : boolean, TRUE in case of Time Out
* Note            : The time-out duration depends on timer pre-scaler,
*                   which is variable; the time-out will be higher at low speed.
*******************************************************************************/
bool HALL_IsTimedOut(void)
{
   return(HallTimeOut);
}


/*******************************************************************************
* ROUTINE Name : Hall_GetCaptCounter
*
* Description     : Gives the number of Hall sensors capture interrupts since last call
*                   of the HALL_ClrCaptCounter function.
* Input           : None
* Output          : None
* Return          : u16 integer (Roll-over is prevented in the input capture
*                   routine itself).
*******************************************************************************/
u16 HALL_GetCaptCounter(void)
{
   return(hCaptCounter);
}


/*******************************************************************************
* ROUTINE Name : HALL_ClrCaptCounter
*
* Description     : Clears the variable holding the number of capture events.
* Input           : None
* Output          : hCaptCounter is cleared.
* Return          : None
*******************************************************************************/
void HALL_ClrCaptCounter(void)
{
   hCaptCounter = 0;
}


/*******************************************************************************
* ROUTINE Name : GetLastHallPeriod
*
* Description     : returns the rotor pseudo-period based on last capture
* Input           : None
* Output          : None
* Return          : rotor pseudo-period, as a number of CKTIM periods
*******************************************************************************/
PeriodMeas_s GetLastHallPeriod(void)
{
      PeriodMeas_s PeriodMeasAux;
      u8 bLastSpeedFIFO_Index;

   // Store current index to prevent errors if Capture occurs during processing
   bLastSpeedFIFO_Index = bSpeedFIFO_Index;

   // This is done assuming interval between captures is higher than time
   // to read the two values
   PeriodMeasAux.wPeriod = SensorPeriod[bLastSpeedFIFO_Index].hCapture;
   PeriodMeasAux.wPeriod *= (SensorPeriod[bLastSpeedFIFO_Index].hPrscReg + 1);
   
   PeriodMeasAux.bDirection = SensorPeriod[bLastSpeedFIFO_Index].bDirection;
   return (PeriodMeasAux);
}


/*******************************************************************************
* ROUTINE Name : GetAvrgHallPeriod
*
* Description    : returns the rotor pseudo-period based on 4 last captures
* Input          : None
* Output         : None
* Return         : averaged rotor pseudo-period, as a number of CKTIM periods
* Side effect: the very last period acquired may not be considered for the
* calculation if a capture occurs during averaging.
*******************************************************************************/
PeriodMeas_s GetAvrgHallPeriod(void)
{
    u32 wFreqBuffer, wAvrgBuffer, wIndex;
    PeriodMeas_s PeriodMeasAux;

  wAvrgBuffer = 0;

  for ( wIndex = 0; wIndex < HALL_SPEED_FIFO_SIZE; wIndex++ )
  {
     // Disable capture interrupts to have presc and capture of the same period
     HALL_TIMER->DIER &= ~TIM_IT_CC1; // NB:Std libray not used for perf issues
     
     wFreqBuffer = SensorPeriod[wIndex].hCapture;
     wFreqBuffer *= (SensorPeriod[wIndex].hPrscReg + 1);
     
     HALL_TIMER->DIER |= TIM_IT_CC1;   // NB:Std libray not used for perf issue
     wAvrgBuffer += wFreqBuffer;	// Sum the whole periods FIFO
     PeriodMeasAux.bDirection = SensorPeriod[wIndex].bDirection;
  }
  // Round to upper value
  wAvrgBuffer = (u32)(wAvrgBuffer + (HALL_SPEED_FIFO_SIZE/2)-1);  
  wAvrgBuffer /= HALL_SPEED_FIFO_SIZE;        // Average value	

  PeriodMeasAux.wPeriod = wAvrgBuffer;
  
  return (PeriodMeasAux);
}


/*******************************************************************************
* ROUTINE Name : HALL_StartHallFiltering
*
* Description : Set the flags to initiate hall speed values smoothing mechanism.
* Input       : None
* Output      : The result of the next capture will be copied in the whole array
*               to have 1st average = last value.
* Return      : None
* Note: The initialization of the FIFO used to do the averaging will be done
*       when the next input capture interrupt will occur.
*******************************************************************************/
void HALL_StartHallFiltering( void )
{
   InitRollingAverage = TRUE;
}

/*******************************************************************************
* ROUTINE Name : HALL_ReadHallState
*
* Description : Read the GPIO Input used for Hall sensor IC and return the state  
* Input       : None
* Output      : None
* Return      : STATE_X
*
*******************************************************************************/

u8 ReadHallState(void)
{
  u8 ReadValue;
#if defined(TIMER2_HANDLES_HALL)  

   ReadValue= (u8)(GPIO_ReadInputData(GPIOA));
  
   ReadValue = ReadValue & GPIO_MSK;
  
#elif defined(TIMER3_HANDLES_HALL)
  
  ReadValue = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)<<2;
  ReadValue |= GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)<<1;
  ReadValue |= GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);

#elif defined(TIMER4_HANDLES_HALL)
  ReadValue = ((u8)(GPIO_ReadInputData(GPIOA))>>6) & GPIO_MSK;
#endif
  
  return(ReadValue);
}

/*******************************************************************************
* ROUTINE Name : HALL_GetElectricalAngle
*
* Description : Export the variable containing the latest angle updated by IC 
*               interrupt
* Input       : None
* Output      : None
* Return      : Electrical angle s16 format
*
*******************************************************************************/
s16 HALL_GetElectricalAngle(void)
{
  return(hElectrical_Angle);
}

/*******************************************************************************
* ROUTINE Name : HALL_IncElectricalAngle
*
* Description : Increment the variable containing the rotor position information.
*               This function is called at each FOC cycle for integrating 
*               the speed information
* Input       : None
* Output      : None
* Return      : Electrical angle s16 format
*
*******************************************************************************/
void HALL_IncElectricalAngle(void)
{ 
  static s16 hPrevRotorFreq;
 
  if (hRotorFreq_dpp != HALL_MAX_PSEUDO_SPEED)
  {
    hElectrical_Angle += hRotorFreq_dpp;
    hPrevRotorFreq = hRotorFreq_dpp;
  }
  else
  {
    hElectrical_Angle += hPrevRotorFreq;
  }
}

/*******************************************************************************
* ROUTINE Name : HALL_Init_Electrical_Angle
*
* Description : Read the logic level of the three Hall sensor and individuates   
*               this way the position of the rotor (+/- 30∞). Electrical angle 
*               variable is then initialized
*
* Input       : None
* Output      : None
* Return      : Electrical angle s16 format
*
*******************************************************************************/
void HALL_Init_Electrical_Angle(void)
{
#if (HALL_SENSORS_PLACEMENT == DEGREES_120) 
 switch(ReadHallState())
 {
  case STATE_5:
    hElectrical_Angle = (s16)(S16_PHASE_SHIFT+S16_60_PHASE_SHIFT/2);
    break;
  case STATE_1:
    hElectrical_Angle =(s16)(S16_PHASE_SHIFT+S16_60_PHASE_SHIFT+
                                                          S16_60_PHASE_SHIFT/2);
    break;
  case STATE_3:
    hElectrical_Angle =(s16)(S16_PHASE_SHIFT+S16_120_PHASE_SHIFT+
                                                          S16_60_PHASE_SHIFT/2);      
    break;
  case STATE_2:
    hElectrical_Angle =(s16)(S16_PHASE_SHIFT-S16_120_PHASE_SHIFT-
                                                          S16_60_PHASE_SHIFT/2);      
    break;
  case STATE_6:
    hElectrical_Angle =(s16)(S16_PHASE_SHIFT-S16_60_PHASE_SHIFT-
                                                          S16_60_PHASE_SHIFT/2);          
    break;
  case STATE_4:
    hElectrical_Angle =(s16)(S16_PHASE_SHIFT-S16_60_PHASE_SHIFT/2);          
    break;    
  default:    
    break;
  }
#elif (HALL_SENSORS_PLACEMENT == DEGREES_60)
 switch(ReadHallState())
 {  
  case STATE_1:
    hElectrical_Angle =(s16)(S16_PHASE_SHIFT+S16_60_PHASE_SHIFT/2);
    break;
  case STATE_3:
    hElectrical_Angle =(s16)(S16_PHASE_SHIFT+S16_60_PHASE_SHIFT+
                                                          S16_60_PHASE_SHIFT/2);
    break;
  case STATE_7:
    hElectrical_Angle =(s16)(S16_PHASE_SHIFT+S16_120_PHASE_SHIFT+
                                                          S16_60_PHASE_SHIFT/2);      
    break;
  case STATE_6:
    hElectrical_Angle =(s16)(S16_PHASE_SHIFT-S16_120_PHASE_SHIFT-
                                                          S16_60_PHASE_SHIFT/2);      
    break;
  case STATE_4:
    hElectrical_Angle =(s16)(S16_PHASE_SHIFT-S16_60_PHASE_SHIFT-
                                                          S16_60_PHASE_SHIFT/2);          
    break;
  case STATE_0:
    hElectrical_Angle =(s16)(S16_PHASE_SHIFT-S16_60_PHASE_SHIFT/2);          
    break;    
  default:    
    break;
  }
#endif
}

/*******************************************************************************
* Function Name  : TIMx_IRQHandler
* Description    : This function handles both the capture event and Update event 
*                  interrupt handling the hall sensors signal period measurement
*                  
*                  - On 'CAPTURE' event case:
*                    The spinning direction is extracted
*                    The electrical angle is updated (synchronized)
*                    If the average is initialized, the last captured measure is
*                    copied into the whole array.
*                    Period captures are managed as following:
*                    If too low, the clock prescaler is decreased for next measure
*                    If too high (ie there was overflows), the result is
*                    re-computed as if there was no overflow and the prescaler is
*                    increased to avoid overflows during the next capture
*                   
*                  - On 'UPDATE' event case:
*                    This function handles the overflow of the timer handling
*                    the hall sensors signal period measurement.
* Input          : 
*                  - On 'CAPTURE' event case:
*                    None
*                   
*                  - On 'UPDATE' event case: 
*                    None
*
* Output         : 
*                  - On 'CAPTURE' event case:
*                   Updates the array holding the 4 latest period measures, reset
*                   the overflow counter and update the clock prescaler to
*                   optimize the accuracy of the measurement.
*                   
*                  - On 'UPDATE' event case:
*                    Updates a Counter of overflows, handled and reset when next
*                    capture occurs. 
*
* Return         : None (Interrupt Service routine)
*******************************************************************************/
#if (defined HALL_SENSORS || defined VIEW_HALL_FEEDBACK)
#if defined(TIMER2_HANDLES_HALL)
void TIM2_IRQHandler(void)
#elif defined(TIMER3_HANDLES_HALL)
void TIM3_IRQHandler(void)
#else // TIMER4_HANDLES_HALL
void TIM4_IRQHandler(void)
#endif

{
  static u8  bHallState; 
  u8 bPrevHallState;

// Check for the source of TIMx int - Capture or Update Event - 
  if ( TIM_GetFlagStatus(HALL_TIMER, TIM_FLAG_Update) == RESET )
  {
    // A capture event generated this interrupt
    bPrevHallState = bHallState;
    bHallState = ReadHallState();
#if (HALL_SENSORS_PLACEMENT == DEGREES_120)    
    switch(bHallState)
    {
      case STATE_5:
        if (bPrevHallState == STATE_5)
        {
	 //a speed reversal occured 
         if(bSpeed<0)
         {
           bSpeed = POSITIVE_SWAP;
         }
         else
         {
           bSpeed = NEGATIVE_SWAP;
         }
        }
        else   
          if (bPrevHallState == STATE_6)
          {
           bSpeed = POSITIVE;
          }
          else 
            if (bPrevHallState == STATE_3)
            {
              bSpeed = NEGATIVE;
            }
		// Update angle
        if(bSpeed<0)
        {
          hElectrical_Angle = (s16)(S16_PHASE_SHIFT+S16_60_PHASE_SHIFT);
        }
        else if(bSpeed!= ERROR)
        {
          hElectrical_Angle = S16_PHASE_SHIFT;  
        }
        break;
             
    case STATE_3:
        if (bPrevHallState == STATE_3)
        {
		 //a speed reversal occured
         if(bSpeed<0)
         {
           bSpeed = POSITIVE_SWAP;
         }
         else
         {
           bSpeed = NEGATIVE_SWAP;
         }
        }
        else
          if (bPrevHallState == STATE_5)
          {
           bSpeed = POSITIVE;
          }
          else 
            if (bPrevHallState == STATE_6)
            {
              bSpeed = NEGATIVE;
            }
		// Update of the electrical angle
        if(bSpeed<0)
        {
          hElectrical_Angle = (s16)(S16_PHASE_SHIFT+S16_120_PHASE_SHIFT+
                                                            S16_60_PHASE_SHIFT);
        }
        else if(bSpeed!= ERROR)
        {
          hElectrical_Angle =(s16)(S16_PHASE_SHIFT + S16_120_PHASE_SHIFT);
        }
        break;  
      
      case STATE_6: 
        if (bPrevHallState == STATE_6)
        {
         if(bSpeed<0)
         {
           bSpeed = POSITIVE_SWAP;
         }
         else
         {
           bSpeed = NEGATIVE_SWAP;
         }
        }
        
        if (bPrevHallState == STATE_3)
        {
         bSpeed = POSITIVE; 
        }
        else 
          if (bPrevHallState == STATE_5)
          {
            bSpeed = NEGATIVE;
          }  
        if(bSpeed<0)
        {
          hElectrical_Angle =(s16)(S16_PHASE_SHIFT - S16_60_PHASE_SHIFT);  
        }
        else if(bSpeed!= ERROR)
        {
          hElectrical_Angle =(s16)(S16_PHASE_SHIFT - S16_120_PHASE_SHIFT); 
        }
        break;
        
      default:
        bSpeed = ERROR;
        break;
    }
#elif (HALL_SENSORS_PLACEMENT == DEGREES_60)    
    switch(bHallState)
    {
      case STATE_3:
        if (bPrevHallState == STATE_3)
        {
         if(bSpeed<0)
         {
           bSpeed = POSITIVE_SWAP;
         }
         else
         {
           bSpeed = NEGATIVE_SWAP;
         }
        }
        else          
          if (bPrevHallState == STATE_0)
          {
           bSpeed = POSITIVE;
          }
          else 
            if (bPrevHallState == STATE_6)
            {
              bSpeed = NEGATIVE;              
            }
        if(bSpeed<0)
        {
          hElectrical_Angle = (s16)(S16_PHASE_SHIFT+S16_120_PHASE_SHIFT);
        }
        else if(bSpeed!= ERROR)
        {
          hElectrical_Angle = (s16)(S16_PHASE_SHIFT+S16_60_PHASE_SHIFT);
        }
        break;
             
      case STATE_6:
        if (bPrevHallState == STATE_6)
        {
         if(bSpeed<0)
         {
           bSpeed = POSITIVE_SWAP;
         }
         else
         {
           bSpeed = NEGATIVE_SWAP;
         }
        } 
        else
          if (bPrevHallState == STATE_3)
          {
           bSpeed = POSITIVE;           
          }
          else 
            if (bPrevHallState == STATE_0)
            {
              bSpeed = NEGATIVE;
            }
        if(bSpeed<0)
        {
          hElectrical_Angle = (s16)(S16_PHASE_SHIFT-S16_120_PHASE_SHIFT);
        }
        else if(bSpeed!= ERROR)
        {
          hElectrical_Angle =(s16)(S16_PHASE_SHIFT + S16_120_PHASE_SHIFT+
                                                            S16_60_PHASE_SHIFT);
        }
        break;  
      
      case STATE_0:
        if (bPrevHallState == STATE_0)
        {
         if(bSpeed<0)
         {
           bSpeed = POSITIVE_SWAP;
         }
         else
         {
           bSpeed = NEGATIVE_SWAP;
         }
        } 
        else
          if (bPrevHallState == STATE_6)
          {
           bSpeed = POSITIVE;
          }
          else 
            if (bPrevHallState == STATE_3)
            {
              bSpeed = NEGATIVE;
            }
        
        if(bSpeed<0)
        {
          hElectrical_Angle =(s16)(S16_PHASE_SHIFT );  
        }
        else if(bSpeed!= ERROR)
        {
          hElectrical_Angle =(s16)(S16_PHASE_SHIFT - S16_60_PHASE_SHIFT);  
        }                      
        break;
        
      default:
        bSpeed = ERROR;
        break;
    }
#endif
   // A capture event occured, it clears the flag  	
	TIM_ClearFlag(HALL_TIMER, TIM_FLAG_CC1);
   
   // used for discarding first capture
   if (hCaptCounter < U16_MAX)
   {
      hCaptCounter++;
   }

   // Compute new array index
   if (bSpeedFIFO_Index != HALL_SPEED_FIFO_SIZE-1)
   {
      bSpeedFIFO_Index++;
   }
   else
   {
      bSpeedFIFO_Index = 0;
   }

   //Timeout Flag is cleared when receiving an IC
   HALL_ClrTimeOut();
   
   // Store the latest speed acquisition
   if (bGP1_OVF_Counter != 0)	// There was counter overflow before capture
   {
        u32 wCaptBuf;
        u16 hPrscBuf;

      wCaptBuf = (u32)TIM_GetCapture1(HALL_TIMER);        
      
      hPrscBuf = HALL_TIMER->PSC;

      while (bGP1_OVF_Counter != 0)
      {
         wCaptBuf += 0x10000uL;// Compute the real captured value (> 16-bit)
         bGP1_OVF_Counter--;
         // OVF Counter is 8-bit and Capt is 16-bit, thus max CaptBuf is 24-bits
      }
      while(wCaptBuf > U16_MAX)
      {
         wCaptBuf /= 2;		// Make it fit 16-bit using virtual prescaler
         // Reduced resolution not a problem since result just slightly < 16-bit
         hPrscBuf = (hPrscBuf * 2) + 1;
         if (hPrscBuf > U16_MAX/2) // Avoid Prsc overflow
         {
            hPrscBuf = U16_MAX;
            wCaptBuf = U16_MAX;
         }
      }
      SensorPeriod[bSpeedFIFO_Index].hCapture = wCaptBuf;
      SensorPeriod[bSpeedFIFO_Index].hPrscReg = hPrscBuf;
      SensorPeriod[bSpeedFIFO_Index].bDirection = bSpeed;
      if (RatioInc)
      {
         RatioInc = FALSE;	// Previous capture caused overflow
         // Don't change prescaler (delay due to preload/update mechanism)
      }
      else
      {
         if ((HALL_TIMER->PSC) < HALL_MAX_RATIO) // Avoid OVF w/ very low freq
         {
            (HALL_TIMER->PSC)++; // To avoid OVF during speed decrease
            RatioInc = TRUE;	  // new prsc value updated at next capture only
         }
      }
   }
   else		// No counter overflow
   {
      u16 hHighSpeedCapture, hClockPrescaler;   

      hHighSpeedCapture = (u32)TIM_GetCapture1(HALL_TIMER);
        
      SensorPeriod[bSpeedFIFO_Index].hCapture = hHighSpeedCapture;
      SensorPeriod[bSpeedFIFO_Index].bDirection = bSpeed;
      // Store prescaler directly or incremented if value changed on last capt
      hClockPrescaler = HALL_TIMER->PSC;

      // If prsc preload reduced in last capture, store current register + 1
      if (RatioDec)  // and don't decrease it again
      {
         SensorPeriod[bSpeedFIFO_Index].hPrscReg = (hClockPrescaler)+1;
         RatioDec = FALSE;
      }
      else  // If prescaler was not modified on previous capture
      {
         if (hHighSpeedCapture >= LOW_RES_THRESHOLD)// If capture range correct
         {
            SensorPeriod[bSpeedFIFO_Index].hPrscReg = hClockPrescaler;
         }
         else
         {
            if(HALL_TIMER->PSC == 0) // or prescaler cannot be further reduced
            {
               SensorPeriod[bSpeedFIFO_Index].hPrscReg = hClockPrescaler;
            }
            else  // The prescaler needs to be modified to optimize the accuracy
            {
               SensorPeriod[bSpeedFIFO_Index].hPrscReg = hClockPrescaler;
               (HALL_TIMER->PSC)--;	// Increase accuracy by decreasing prsc
               // Avoid decrementing again in next capt.(register preload delay)
               RatioDec = TRUE;
            }
         }
      }
   }
    
   if (InitRollingAverage)
   {
        u16 hCaptBuf, hPrscBuf;
        s8 bSpeedAux;
        u32 wIndex;
      // Read last captured value and copy it into the whole array
      hCaptBuf = SensorPeriod[bSpeedFIFO_Index].hCapture;
      hPrscBuf = SensorPeriod[bSpeedFIFO_Index].hPrscReg;
      bSpeedAux = SensorPeriod[bSpeedFIFO_Index].bDirection;
      
      for (wIndex = 0; wIndex != HALL_SPEED_FIFO_SIZE-1; wIndex++)
      {
         SensorPeriod[wIndex].hCapture = hCaptBuf;
         SensorPeriod[wIndex].hPrscReg = hPrscBuf;
         SensorPeriod[wIndex].bDirection = bSpeedAux;
      }
      InitRollingAverage = FALSE;
      // Starting from now, the values returned by MTC_GetRotorFreq are averaged
      DoRollingAverage = TRUE;
    }
   
  //Update Rotor Frequency Computation
   hRotorFreq_dpp = HALL_GetRotorFreq();
  
  }
  else 
  {
    TIM_ClearFlag(HALL_TIMER, TIM_FLAG_Update);  
  	// an update event occured for this interrupt request generation
    if (bGP1_OVF_Counter < U8_MAX)
    {
       bGP1_OVF_Counter++;
    }
  
    if (bGP1_OVF_Counter >= HALL_MAX_OVERFLOWS)		 //≥¨ ±√ª∑¥”¶£¨‘ÚÀŸ∂»∑¥¿° ß∞‹
    {
       HallTimeOut = TRUE;
       hRotorFreq_dpp = 0;
    }    
  }
}

#endif // HALL_SENSORS defined

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
