/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_PID_regulators.c
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : This file contains the software implementation for the
                       PI(D) regulators.
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
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

/* Standard include ----------------------------------------------------------*/

#include "stm32f10x_lib.h"
#include "stm32f10x_MClib.h"
#include "stm32f10x_type.h"
#include "MC_Globals.h"

#define PID_SPEED_REFERENCE  (u16)(PID_SPEED_REFERENCE_RPM/6)

typedef signed long long s64;

/*******************************************************************************
* Function Name  : PID_Init
* Description    : Initialize PID coefficients for torque, flux and speed loop: 
                   Kp_Gain: proportional coeffcient 
                   Ki_Gain: integral coeffcient 
                   Kd_Gain: differential coeffcient 
* Input          : Pointer 1 to Torque PI structure,  
                   Pointer 2 to Flux PI structure,  
                   Pointer 3 to Speed PI structure  
* Output         : None
* Return         : None
*******************************************************************************/
void PID_Init (PID_Struct_t *PID_Torque, PID_Struct_t *PID_Flux, 
                                                        PID_Struct_t *PID_Speed)
{
  hTorque_Reference = PID_TORQUE_REFERENCE;

  PID_Torque->hKp_Gain    = PID_TORQUE_KP_DEFAULT;
  PID_Torque->hKp_Divisor = TF_KPDIV;  

  PID_Torque->hKi_Gain = PID_TORQUE_KI_DEFAULT;
  PID_Torque->hKi_Divisor = TF_KIDIV;
  
  PID_Torque->hKd_Gain = PID_TORQUE_KD_DEFAULT;
  PID_Torque->hKd_Divisor = TF_KDDIV;
  PID_Torque->wPreviousError = 0;
  
  PID_Torque->hLower_Limit_Output=S16_MIN;   //Lower Limit for Output limitation
  PID_Torque->hUpper_Limit_Output= S16_MAX;   //Upper Limit for Output limitation
  PID_Torque->wLower_Limit_Integral = S16_MIN * TF_KIDIV;
  PID_Torque->wUpper_Limit_Integral = S16_MAX * TF_KIDIV;
  PID_Torque->wIntegral = 0;
 
  /**************************************************/
  /************END PID Torque Regulator members*******/
  /**************************************************/

  /**************************************************/
  /************PID Flux Regulator members*************/
  /**************************************************/

  PID_Flux->wIntegral = 0;  // reset integral value 

  hFlux_Reference = PID_FLUX_REFERENCE;

  PID_Flux->hKp_Gain    = PID_FLUX_KP_DEFAULT;
  PID_Flux->hKp_Divisor = TF_KPDIV;  

  PID_Flux->hKi_Gain = PID_FLUX_KI_DEFAULT;
  PID_Flux->hKi_Divisor = TF_KIDIV;
  
  PID_Flux->hKd_Gain = PID_FLUX_KD_DEFAULT;
  PID_Flux->hKd_Divisor = TF_KDDIV;
  PID_Flux->wPreviousError = 0;
  
  PID_Flux->hLower_Limit_Output=S16_MIN;   //Lower Limit for Output limitation
  PID_Flux->hUpper_Limit_Output= S16_MAX;   //Upper Limit for Output limitation
  PID_Flux->wLower_Limit_Integral = S16_MIN * TF_KIDIV;
  PID_Flux->wUpper_Limit_Integral = S16_MAX * TF_KIDIV;
  PID_Flux->wIntegral = 0;
  
  /**************************************************/
  /************END PID Flux Regulator members*********/
  /**************************************************/

  /**************************************************/
  /************PID Speed Regulator members*************/
  /**************************************************/


  PID_Speed->wIntegral = 0;  // reset integral value 

  hSpeed_Reference = PID_SPEED_REFERENCE;

  PID_Speed->hKp_Gain    = PID_SPEED_KP_DEFAULT;
  PID_Speed->hKp_Divisor = SP_KPDIV;  

  PID_Speed->hKi_Gain = PID_SPEED_KI_DEFAULT;
  PID_Speed->hKi_Divisor = SP_KIDIV;
  
  PID_Speed->hKd_Gain = PID_SPEED_KD_DEFAULT;
  PID_Speed->hKd_Divisor = SP_KDDIV;
  PID_Speed->wPreviousError = 0;
  
  PID_Speed->hLower_Limit_Output= -IQMAX;   //Lower Limit for Output limitation
  PID_Speed->hUpper_Limit_Output= IQMAX;   //Upper Limit for Output limitation
  PID_Speed->wLower_Limit_Integral = -IQMAX * SP_KIDIV;
  PID_Speed->wUpper_Limit_Integral = IQMAX * SP_KIDIV;
  PID_Speed->wIntegral = 0;
  /**************************************************/
  /**********END PID Speed Regulator members*********/
  /**************************************************/

}

/*******************************************************************************
* Function Name  : PID_Speed_Coefficients_update
* Description    : Update Kp, Ki & Kd coefficients of speed regulation loop 
                   according to motor speed. See "MC_PID_Param.h" for parameters 
                   setting
* Input          : s16 
                   Mechanical motor speed with 0.1 Hz resolution 
                   eg: 10Hz <-> (s16)100
* Output         : None
* Return         : None
*******************************************************************************/
void PID_Speed_Coefficients_update(s16 motor_speed, PID_Struct_t *PID_Struct)
{
if ( motor_speed < 0)  
{
  motor_speed = (u16)(-motor_speed);   // absolute value only
}

if ( motor_speed <= Freq_Min )    // motor speed lower than Freq_Min? 
{
  PID_Struct->hKp_Gain = Kp_Fmin;   
  PID_Struct->hKi_Gain = Ki_Fmin;

  #ifdef DIFFERENTIAL_TERM_ENABLED
  PID_Struct->hKd_Gain =Kd_Fmin;
  #endif
}
else if ( motor_speed <= F_1 )
{
  PID_Struct->hKp_Gain = Kp_Fmin + (s32)(alpha_Kp_1*(motor_speed - Freq_Min) / 1024);
  PID_Struct->hKi_Gain = Ki_Fmin + (s32)(alpha_Ki_1*(motor_speed - Freq_Min) / 1024);

  #ifdef DIFFERENTIAL_TERM_ENABLED
  PID_Struct->hKd_Gain = Kd_Fmin + (s32)(alpha_Kd_1*(motor_speed - Freq_Min) / 1024);
  #endif
}
else if ( motor_speed <= F_2 )
{
  PID_Struct->hKp_Gain = Kp_F_1 + (s32)(alpha_Kp_2 * (motor_speed-F_1) / 1024);
  PID_Struct->hKi_Gain = Ki_F_1 + (s32)(alpha_Ki_2 * (motor_speed-F_1) / 1024);

  #ifdef DIFFERENTIAL_TERM_ENABLED
  PID_Struct->hKd_Gain = Kd_F_1 + (s32)(alpha_Kd_2 * (motor_speed-F_1) / 1024);
  #endif
}
else if ( motor_speed <= Freq_Max )
{
  PID_Struct->hKp_Gain = Kp_F_2 + (s32)(alpha_Kp_3 * (motor_speed-F_2) / 1024);
  PID_Struct->hKi_Gain = Ki_F_2 + (s32)(alpha_Ki_3 * (motor_speed-F_2) / 1024);

  #ifdef DIFFERENTIAL_TERM_ENABLED
  PID_Struct->hKd_Gain = Kd_F_2 + (s32)(alpha_Kd_3 * (motor_speed-F_2) / 1024);
  #endif
}
else  // motor speed greater than Freq_Max? 
{
  PID_Struct->hKp_Gain = Kp_Fmax;
  PID_Struct->hKi_Gain = Ki_Fmax;

  #ifdef DIFFERENTIAL_TERM_ENABLED
  PID_Struct->hKd_Gain = Kd_Fmax;
  #endif
}
}

/*******************************************************************************
* Function Name  : PID_Regulator
* Description    : Compute the PI(D) output for a PI(D) regulation.
* Input          : Pointer to the PID settings (*PID_Flux)
                   Speed in s16 format
* Output         : s16
* Return         : None
*******************************************************************************/
s16 PID_Regulator(s16 hReference, s16 hPresentFeedback, PID_Struct_t *PID_Struct)
{
  s32 wError, wProportional_Term,wIntegral_Term, houtput_32;
  s64 dwAux; 
#ifdef DIFFERENTIAL_TERM_ENABLED    
  s32 wDifferential_Term;
#endif    
  // error computation
  wError= (s32)(hReference - hPresentFeedback);
 
  // Proportional term computation
  wProportional_Term = PID_Struct->hKp_Gain * wError;

  // Integral term computation
  if (PID_Struct->hKi_Gain == 0)
  {
    PID_Struct->wIntegral = 0;
  }
  else
  { 
    wIntegral_Term = PID_Struct->hKi_Gain * wError;
    dwAux = PID_Struct->wIntegral + (s64)(wIntegral_Term);
    
    if (dwAux > PID_Struct->wUpper_Limit_Integral)
    {
      PID_Struct->wIntegral = PID_Struct->wUpper_Limit_Integral;
    }
    else if (dwAux < PID_Struct->wLower_Limit_Integral)
          { 
            PID_Struct->wIntegral = PID_Struct->wLower_Limit_Integral;
          }
          else
          {
           PID_Struct->wIntegral = (s32)(dwAux);
          }
  }
  // Differential term computation
#ifdef DIFFERENTIAL_TERM_ENABLED
  {
  s32 wtemp;
  
  wtemp = wError - PID_Struct->wPreviousError;
  wDifferential_Term = PID_Struct->hKd_Gain * wtemp;
  PID_Struct->wPreviousError = wError;    // store value 
  }
  houtput_32 = (wProportional_Term/PID_Struct->hKp_Divisor+ 
                PID_Struct->wIntegral/PID_Struct->hKi_Divisor + 
                wDifferential_Term/PID_Struct->hKd_Divisor); 

#else  
  houtput_32 = (wProportional_Term/PID_Struct->hKp_Divisor+ 
                PID_Struct->wIntegral/PID_Struct->hKi_Divisor);
#endif
  
    if (houtput_32 >= PID_Struct->hUpper_Limit_Output)
      {
      return(PID_Struct->hUpper_Limit_Output);		  			 	
      }
    else if (houtput_32 < PID_Struct->hLower_Limit_Output)
      {
      return(PID_Struct->hLower_Limit_Output);
      }
    else 
      {
        return((s16)(houtput_32)); 		
      }
}		   

/******************** (C) COPYRIGHT 2008 STMicroelectronics *******************/


