/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_State_Observer_Interface.c
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : This module implements the State Observer of 
*                      the PMSM B-EMF, thus identifying rotor speed and position
*
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
* 11/07/08 v2.0.1
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
#include "stm32f10x_MClib.h"
#include "MC_Globals.h"
#include "MC_const.h"

#include "MC_PMSM_motor_param.h"
#include "MC_State_Observer_Interface.h"
#include "MC_State_Observer.h"
#include "MC_State_Observer_param.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum 
{
 S_INIT, ALIGNMENT, RAMP_UP
} Start_upStatus_t;

/* Private define ------------------------------------------------------------*/
#define LOWER_THRESHOLD_FACTOR    0.8  //percentage of forced speed
#define UPPER_THRESHOLD_FACTOR    1    //percentage of forced speed

#define LOW_THRESHOLD             (u8) (LOWER_THRESHOLD_FACTOR*10)
#define UP_THRESHOLD              (u8) (UPPER_THRESHOLD_FACTOR*10)

#define HC2_INIT                  (s16)((F1*wK1_LO)/SAMPLING_FREQ)
#define HC4_INIT                  (s16)(((wK2_LO*MAX_CURRENT)/MAX_BEMF_VOLTAGE)\
                                        *F2/SAMPLING_FREQ)
//Do not be modified
#define SLESS_T_ALIGNMENT_PWM_STEPS       (u32) ((SLESS_T_ALIGNMENT * SAMPLING_FREQ)\
                                                                          /1000) 
#define SLESS_ALIGNMENT_ANGLE_S16         (s16)((s32)(SLESS_ALIGNMENT_ANGLE)\
                                                                    * 65536/360)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u16 bConvCounter;
static s16 hAngle = 0;
static u32 wTime = 0;
static s32 wStart_Up_Freq = 0;
static s32 wStart_Up_I;
static s16 hFreq_Inc;
static s32 hI_Inc;

static Start_upStatus_t  Start_Up_State = S_INIT;

#ifdef OBSERVER_GAIN_TUNING
volatile s32 wK1_LO = K1;
volatile s32 wK2_LO = K2;
volatile s16 hPLL_P_Gain = PLL_KP_GAIN;
volatile s16 hPLL_I_Gain = PLL_KI_GAIN;
#endif

/* Private functions ---------------------------------------------------------*/
bool IsObserverConverged(void);
void STO_StartUp_Init(void);

/*******************************************************************************
* Function Name : STO_StateObserverInterface_Init
* Description : It fills and passes to the State Obsever module the data 
*               structure necessary for rotor position observation 
* Input : None
* Output : None
* Return : None
*******************************************************************************/
void STO_StateObserverInterface_Init(void)
{
 StateObserver_Const StateObserver_ConstStruct;
 
 StateObserver_ConstStruct.hC1 = C1;
 StateObserver_ConstStruct.hC3 = C3;
 StateObserver_ConstStruct.hC5 = C5; 
  
   {
    s16 htempk;
    StateObserver_ConstStruct.hF3 = 1;
    htempk = (s16)((100*65536)/(F2*2*PI));
    while (htempk != 0)
    {
      htempk /=2;
      StateObserver_ConstStruct.hF3 *=2;
    }
    StateObserver_ConstStruct.hC6 = (s16)((F2*StateObserver_ConstStruct.hF3*2*
                                                                    PI)/65536);//10000
  }

#ifdef OBSERVER_GAIN_TUNING  
  /* lines below for debug porpose*/
  StateObserver_ConstStruct.hC2 = C2;
  StateObserver_ConstStruct.hC4 = C4;
    
  StateObserver_ConstStruct.hC2 = (s16)((F1*wK1_LO)/SAMPLING_FREQ);
  StateObserver_ConstStruct.hC4 = (s16)((((wK2_LO*MAX_CURRENT)/(MAX_BEMF_VOLTAGE
                                                       ))*F2)/(SAMPLING_FREQ));
  StateObserver_ConstStruct.PLL_P = hPLL_P_Gain;
  StateObserver_ConstStruct.PLL_I = hPLL_I_Gain;
#else
  StateObserver_ConstStruct.hC2 = C2;
  StateObserver_ConstStruct.hC4 = C4;
  StateObserver_ConstStruct.PLL_P = PLL_KP_GAIN;
  StateObserver_ConstStruct.PLL_I = PLL_KI_GAIN;  
#endif    
  StateObserver_ConstStruct.hF1 = F1;
  StateObserver_ConstStruct.hF2 = F2;
  StateObserver_ConstStruct.wMotorMaxSpeed_dpp = MOTOR_MAX_SPEED_DPP;
  StateObserver_ConstStruct.hPercentageFactor = PERCENTAGE_FACTOR;
  
  STO_Gains_Init(&StateObserver_ConstStruct);
}
  
/*******************************************************************************
* Function Name : STO_Check_Speed_Reliability
* Description : Check for the continuity of the speed reliability. If the speed 
*               is continously not reliable, the motor must be stopped 
* Input : None
* Output : None
* Return : boolean value: TRUE if speed is reliable, FALSE otherwise.
*******************************************************************************/
bool STO_Check_Speed_Reliability(void)
{
  static u8 bCounter=0;
  bool baux;
  
  if(STO_IsSpeed_Reliable() == FALSE)
  {
   bCounter++;
   if (bCounter >= RELIABILITY_HYSTERESYS)
   {
     bCounter = 0;
     baux = FALSE;
   }
   else
   {
     baux = TRUE;
   }
  }
  else
  {
   bCounter = 0;
   baux = TRUE;
  }
  return(baux);
}
           
/*******************************************************************************
* Function Name : IsObserverConverged
* Description : Check for algorithm convergence. The speed reliability and the
*               range of the value of the estimated speed are checked. 
* Input : None
* Output : None
* Return : boolean value: TRUE if algortihm converged, FALSE otherwise.
*******************************************************************************/
bool IsObserverConverged(void)
{ 
  s16 hEstimatedSpeed;
  s16 hUpperThreshold;
  s16 hLowerThreshold;

  hEstimatedSpeed = STO_Get_Speed_Hz();
  hEstimatedSpeed = (hEstimatedSpeed < 0 ? -hEstimatedSpeed : hEstimatedSpeed);  
  hUpperThreshold = ((wStart_Up_Freq/65536) * 160)/(POLE_PAIR_NUM * 16);
  hUpperThreshold = (hUpperThreshold < 0 ? -hUpperThreshold : hUpperThreshold);
  hLowerThreshold = ((wStart_Up_Freq/65536) *150) / (POLE_PAIR_NUM * 16);
  hLowerThreshold = (hLowerThreshold < 0 ? -hLowerThreshold : hLowerThreshold);
  
  // If the variance of the estimated speed is low enough...
  if(STO_IsSpeed_Reliable() == TRUE)
  { 
    if(hEstimatedSpeed > MINIMUM_SPEED)
    {
      //...and the estimated value is quite close to the expected value... 
      if(hEstimatedSpeed >= hLowerThreshold)
      {
        if(hEstimatedSpeed <= hUpperThreshold)
        {
          bConvCounter++;
          if (bConvCounter >= NB_CONSECUTIVE_TESTS)
          {
            // ...the algorithm converged.
            return(TRUE);
          }
          else
          {
            return(FALSE);
          }            
        }
        else
        { 
          bConvCounter = 0;
          return(FALSE);
        }              
      }
      else
      { 
        bConvCounter = 0;
        return(FALSE);
      } 
    }
    else
    { 
      bConvCounter = 0;
      return(FALSE);
    } 
  }
  else
  { 
    bConvCounter = 0;
    return(FALSE);
  }    
}

/*******************************************************************************
* Function Name : STO_Get_Speed_Hz
* Description : It returns the motor mechanical speed (Hz*10)
* Input : None.
* Output : None.
* Return : hRotor_Speed_Hz.
*******************************************************************************/
s16 STO_Get_Speed_Hz(void)
{
  return (s16)((STO_Get_Speed()* SAMPLING_FREQ * 10)/(65536*POLE_PAIR_NUM));
}

/*******************************************************************************
* Function Name : STO_Get_Mechanical_Angle
* Description : It returns the rotor position (mechanical angle,s16) 
* Input : None.
* Output : None.
* Return : hRotor_El_Angle/pole pairs number.
*******************************************************************************/
s16 STO_Get_Mechanical_Angle(void)
{
  return ((s16)(STO_Get_Electrical_Angle()/POLE_PAIR_NUM));
}

/*******************************************************************************
* Function Name : STO_Start_Up
* Description : This function implements the ramp-up by forcing a stator current 
*               with controlled amplitude and frequency. If the observer 
*               algorithm converged, it also assign RUN to State variable  
* Input : details the input parameters.
* Output : details the output parameters.
* Return : details the return value.
*******************************************************************************/
void STO_Start_Up(void)
{
  s16 hAux;
#ifdef NO_SPEED_SENSORS_ALIGNMENT
  static u32 wAlignmentTbase=0;
#endif  
  
  switch(Start_Up_State)
  {
  case S_INIT:
    //Init Ramp-up variables
    if (hSpeed_Reference >= 0)
    {
      hFreq_Inc = FREQ_INC;
      hI_Inc = I_INC;
      if (wTime == 0)
      {
        wStart_Up_I = FIRST_I_STARTUP *1024;
      }
    }
    else
    {
      hFreq_Inc = -(s16)FREQ_INC;
      hI_Inc = -(s16)I_INC; 
      if (wTime == 0)
      {
        wStart_Up_I = -(s32)FIRST_I_STARTUP *1024;
      }
    }
    Start_Up_State = ALIGNMENT;
    break;
    
  case ALIGNMENT:
#ifdef NO_SPEED_SENSORS_ALIGNMENT
    wAlignmentTbase++;
    if(wAlignmentTbase <= SLESS_T_ALIGNMENT_PWM_STEPS)
    {                  
      hFlux_Reference = SLESS_I_ALIGNMENT * wAlignmentTbase / 
                                                    SLESS_T_ALIGNMENT_PWM_STEPS;               
      hTorque_Reference = 0;
      
      Stat_Curr_a_b = GET_PHASE_CURRENTS(); 
      Stat_Curr_alfa_beta = Clarke(Stat_Curr_a_b); 
      Stat_Curr_q_d = Park(Stat_Curr_alfa_beta, SLESS_ALIGNMENT_ANGLE_S16);  
      /*loads the Torque Regulator output reference voltage Vqs*/   
      Stat_Volt_q_d.qV_Component1 = PID_Regulator(hTorque_Reference, 
                        Stat_Curr_q_d.qI_Component1, &PID_Torque_InitStructure);  
      /*loads the Flux Regulator output reference voltage Vds*/
      Stat_Volt_q_d.qV_Component2 = PID_Regulator(hFlux_Reference, 
                          Stat_Curr_q_d.qI_Component2, &PID_Flux_InitStructure); 

      RevPark_Circle_Limitation();

      /*Performs the Reverse Park transformation,
      i.e transforms stator voltages Vqs and Vds into Valpha and Vbeta on a 
      stationary reference frame*/

      Stat_Volt_alfa_beta = Rev_Park(Stat_Volt_q_d);

      /*Valpha and Vbeta finally drive the power stage*/ 
      CALC_SVPWM(Stat_Volt_alfa_beta);
    }
    else
    {
      wAlignmentTbase = 0;                
      Stat_Volt_q_d.qV_Component1 = Stat_Volt_q_d.qV_Component2 = 0;
      hTorque_Reference = PID_TORQUE_REFERENCE;
      hFlux_Reference = PID_FLUX_REFERENCE;
      Start_Up_State = RAMP_UP;
      hAngle = SLESS_ALIGNMENT_ANGLE_S16;      
    }
#else
    Start_Up_State = RAMP_UP;    
#endif    
    break;
    
  case RAMP_UP:
    wTime ++;  
    if (wTime <= I_STARTUP_PWM_STEPS)
    {     
      wStart_Up_Freq += hFreq_Inc;
      wStart_Up_I += hI_Inc;
    }
    else if (wTime <= FREQ_STARTUP_PWM_STEPS )
    {
      wStart_Up_Freq += hFreq_Inc;
    }       
    else
    {
      MCL_SetFault(START_UP_FAILURE);
	   //Æô¶¯Ê§°Ü´íÎó
      //Re_initialize Start Up
      STO_StartUp_Init();
    }
    
    //Add angle increment for ramp-up
    hAux = wStart_Up_Freq/65536;
    hAngle = (s16)(hAngle + (s32)(65536/(SAMPLING_FREQ/hAux)));
        
    Stat_Curr_a_b = GET_PHASE_CURRENTS(); 
    Stat_Curr_alfa_beta = Clarke(Stat_Curr_a_b); 
    Stat_Curr_q_d = Park(Stat_Curr_alfa_beta, hAngle);
    
    hAux = wStart_Up_I/1024;
    hTorque_Reference = hAux;       
    hFlux_Reference = 0;
           
    /*loads the Torque Regulator output reference voltage Vqs*/   
    Stat_Volt_q_d.qV_Component1 = PID_Regulator(hTorque_Reference, 
                        Stat_Curr_q_d.qI_Component1, &PID_Torque_InitStructure);
    /*loads the Flux Regulator output reference voltage Vds*/
    Stat_Volt_q_d.qV_Component2 = PID_Regulator(hFlux_Reference, 
                          Stat_Curr_q_d.qI_Component2, &PID_Flux_InitStructure); 
    
    RevPark_Circle_Limitation();
  
    /*Performs the Reverse Park transformation,
    i.e transforms stator voltages Vqs and Vds into Valpha and Vbeta on a 
    stationary reference frame*/
    
    Stat_Volt_alfa_beta = Rev_Park(Stat_Volt_q_d);
  
    /*Valpha and Vbeta finally drive the power stage*/ 
    CALC_SVPWM(Stat_Volt_alfa_beta);
    
    STO_Calc_Rotor_Angle(Stat_Volt_alfa_beta,Stat_Curr_alfa_beta,MCL_Get_BusVolt());
   
    if (IsObserverConverged()==TRUE)
    {      
      PID_Speed_InitStructure.wIntegral = (s32)(hTorque_Reference*256);
      STO_StartUp_Init();  
      State = RUN;
      if ((wGlobal_Flags & SPEED_CONTROL) != SPEED_CONTROL)
      {
        hTorque_Reference = PID_TORQUE_REFERENCE;
        hFlux_Reference = PID_FLUX_REFERENCE;
      }      
    }    
    break;
  default:
    break;
  }    
}

/*******************************************************************************
* Function Name : STO_StartUp_Init
* Description : This private function initializes the sensorless start-up
* Input : details the input parameters.
* Output : details the output parameters.
* Return : details the return value.
*******************************************************************************/
void STO_StartUp_Init(void)
{
  //Re_initialize Start Up
  Start_Up_State = S_INIT;  
  hAngle = 0;
  wTime = 0;
  wStart_Up_Freq = 0;
  bConvCounter = 0; 
}      
      
/*******************************************************************************
* Function Name : STO_Obs_Gains_Update
* Description : This function updates state observer gains after they have been
*               changed by the user interface
* Input : details the input parameters.
* Output : details the output parameters.
* Return : details the return value.
*******************************************************************************/
#ifdef OBSERVER_GAIN_TUNING
void STO_Obs_Gains_Update(void)
{
  StateObserver_GainsUpdate STO_GainsUpdateStruct;

  STO_GainsUpdateStruct.PLL_P = hPLL_P_Gain;
  STO_GainsUpdateStruct.PLL_I = hPLL_I_Gain;
  STO_GainsUpdateStruct.hC2 = HC2_INIT;  
  STO_GainsUpdateStruct.hC4 = HC4_INIT;            
  STO_Gains_Update(&STO_GainsUpdateStruct);
}      
#endif      
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
