/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_FOC_Drive.c
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : This file provides all the PMSM FOC drive functions.
* 
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
* 14/07/08 v2.0.1
* 28/08/08 v2.0.2
* 04/09/08 v2.0.3
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
#include "MC_FOC_Drive.h"
#include "MC_PMSM_motor_param.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FW_KDDIV        1
#define FW_KD_GAIN      0
#define FW_D_TERM_INIT  0
#define VOLTAGE_SAMPLING_BUFFER 128
/* Private macro -------------------------------------------------------------*/
#define SATURATION_TO_S16(a)    if (a > S16_MAX)              \
                                {                             \
                                  a = S16_MAX;                \
                                }                             \
                                else if (a < -S16_MAX)        \
                                {                             \
                                  a = -S16_MAX;               \
                                }                             \
/* Private functions ---------------------------------------------------------*/
/* Private variable ----------------------------------------------------------*/
static volatile Curr_Components Stat_Curr_q_d_ref;
static Curr_Components Stat_Curr_q_d_ref_ref;

#ifdef FEED_FORWARD_CURRENT_REGULATION
static Volt_Components Stat_Volt_q_d_3;
static Volt_Components Stat_Volt_q_d_2;
#endif

#ifdef FLUX_WEAKENING
static Volt_Components Stat_Volt_q_d_1;
s16 hFW_V_Ref = FW_VOLTAGE_REF;
s16 hFW_P_Gain = FW_KP_GAIN;
s16 hFW_I_Gain = FW_KI_GAIN;
s16 hVMagn = 0;
#endif

/*******************************************************************************
* Function Name : FOC_Init
* Description   : The purpose of this function is to initialize to proper values
*                 all the variables related to the field-oriented control
*                 algorithm. To be called once prior to every motor startup.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void FOC_Init (void)
{
#ifdef FLUX_WEAKENING  
  FOC_FluxRegulatorInterface_Init();//初始化PID值
  
  Stat_Volt_q_d_1.qV_Component1 = 0;
  Stat_Volt_q_d_1.qV_Component2 = 0;

  hVMagn = 0;
#endif

#ifdef IPMSM_MTPA
  FOC_MTPAInterface_Init();
#endif  
  
#ifdef FEED_FORWARD_CURRENT_REGULATION  
  Stat_Volt_q_d_3.qV_Component1 = 0;
  Stat_Volt_q_d_3.qV_Component2 = 0; 
  Stat_Volt_q_d_2.qV_Component1 = 0;
  Stat_Volt_q_d_2.qV_Component2 = 0;
  FOC_FF_CurrReg_Init(CONSTANT1_Q,CONSTANT1_D,CONSTANT2);  
#endif
  
  Stat_Curr_q_d_ref_ref.qI_Component1 = 0;
  Stat_Curr_q_d_ref_ref.qI_Component2 = 0;  
  
  Stat_Curr_q_d_ref.qI_Component1 = 0;
  Stat_Curr_q_d_ref.qI_Component2 = 0;
}

/*******************************************************************************
* Function Name : FOC_Model
* Description   : The purpose of this function is to perform PMSM torque and 
*                 flux regulation, implementing the FOC vector algorithm.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void FOC_Model(void)
{
#ifdef FEED_FORWARD_CURRENT_REGULATION  
  Volt_Components Stat_Volt_q_d_4;
  s32 wtemp;
#endif
  
#if defined HALL_SENSORS
  //Integrate Speed for rotor angle update
  HALL_IncElectricalAngle();
#endif    
  
  /**********STARTS THE VECTOR CONTROL ************************/  
 
  Stat_Curr_a_b = GET_PHASE_CURRENTS();//取得当前电流ia,ib
  
  Stat_Curr_alfa_beta = Clarke(Stat_Curr_a_b);//clarke 变换成i_alfa,i_beta
  
  Stat_Curr_q_d = Park(Stat_Curr_alfa_beta,GET_ELECTRICAL_ANGLE);  

#ifdef NO_SPEED_SENSORS  
  STO_Calc_Rotor_Angle(Stat_Volt_alfa_beta,Stat_Curr_alfa_beta,MCL_Get_BusVolt());
#endif

#ifdef FEED_FORWARD_CURRENT_REGULATION   
  /*loads the Torque Regulator output reference voltage Vqs*/   
  Stat_Volt_q_d_4.qV_Component1 = PID_Regulator(Stat_Curr_q_d_ref_ref.qI_Component1,
                         Stat_Curr_q_d.qI_Component1,&PID_Torque_InitStructure);                                                   

  
  /*loads the Flux Regulator output reference voltage Vds*/
  Stat_Volt_q_d_4.qV_Component2 = PID_Regulator(Stat_Curr_q_d_ref_ref.qI_Component2,
                         Stat_Curr_q_d.qI_Component2,&PID_Flux_InitStructure);  

 
  wtemp = (s32)(Stat_Volt_q_d_4.qV_Component1 + Stat_Volt_q_d_3.qV_Component1);
  
  SATURATION_TO_S16(wtemp);
  
  Stat_Volt_q_d.qV_Component1 = (s16)wtemp;  

  wtemp = (s32)(Stat_Volt_q_d_4.qV_Component2 + Stat_Volt_q_d_3.qV_Component2);
  
  SATURATION_TO_S16(wtemp);
  
  Stat_Volt_q_d.qV_Component2 = (s16)wtemp;
  
#else
  /*loads the Torque Regulator output reference voltage Vqs*/   
  Stat_Volt_q_d.qV_Component1 = PID_Regulator(Stat_Curr_q_d_ref_ref.qI_Component1, 
                        Stat_Curr_q_d.qI_Component1, &PID_Torque_InitStructure);

  
  /*loads the Flux Regulator output reference voltage Vds*/
  Stat_Volt_q_d.qV_Component2 = PID_Regulator(Stat_Curr_q_d_ref_ref.qI_Component2, 
                          Stat_Curr_q_d.qI_Component2, &PID_Flux_InitStructure);  
#endif
  
  //circle limitation
  RevPark_Circle_Limitation();//限制Stat_Volt_q_d 的值
 
  /*Performs the Reverse Park transformation,
  i.e transforms stator voltages Vqs and Vds into Valpha and Vbeta on a 
  stationary reference frame*/

  Stat_Volt_alfa_beta = Rev_Park(Stat_Volt_q_d);

  /*Valpha and Vbeta finally drive the power stage*/ 
  CALC_SVPWM(Stat_Volt_alfa_beta);
  
#ifdef FEED_FORWARD_CURRENT_REGULATION  
  Stat_Volt_q_d_2.qV_Component1 = (s16)((Stat_Volt_q_d_2.qV_Component1*
                                  (VOLTAGE_SAMPLING_BUFFER-1)+
                                  Stat_Volt_q_d_4.qV_Component1)/
                                  VOLTAGE_SAMPLING_BUFFER);
  Stat_Volt_q_d_2.qV_Component2 = (s16)((Stat_Volt_q_d_2.qV_Component2*
                                  (VOLTAGE_SAMPLING_BUFFER-1)+
                                  Stat_Volt_q_d_4.qV_Component2)/
                                  VOLTAGE_SAMPLING_BUFFER);
#endif 
#ifdef FLUX_WEAKENING
  
  Stat_Volt_q_d_1.qV_Component1 = (s16)((Stat_Volt_q_d_1.qV_Component1*
                                  (VOLTAGE_SAMPLING_BUFFER-1)+
                                  Stat_Volt_q_d.qV_Component1)/
                                  VOLTAGE_SAMPLING_BUFFER);
  Stat_Volt_q_d_1.qV_Component2 = (s16)((Stat_Volt_q_d_1.qV_Component2*
                                  (VOLTAGE_SAMPLING_BUFFER-1)+
                                  Stat_Volt_q_d.qV_Component2)/
                                  VOLTAGE_SAMPLING_BUFFER);
#endif  
}

/*******************************************************************************
* Function Name   : FOC_CalcFluxTorqueRef
* Description     : This function provides current components Iqs* and Ids* to be
*                   used as reference values (by the FOC_Model function) when in
*                   speed control mode
* Input           : None.
* Output          : None.
* Return          : None.
*******************************************************************************/
void FOC_CalcFluxTorqueRef(void)
{
  Stat_Curr_q_d_ref.qI_Component1 = PID_Regulator(hSpeed_Reference,
                                    GET_SPEED_0_1HZ,&PID_Speed_InitStructure);

#ifdef IPMSM_MTPA  
  Stat_Curr_q_d_ref.qI_Component2 = FOC_MTPA(Stat_Curr_q_d_ref.qI_Component1);  
#else
  Stat_Curr_q_d_ref.qI_Component2 = 0;
#endif

#ifdef FLUX_WEAKENING
  {
    s16 hVoltageLimit_Reference;
    
    Curr_Components Stat_Curr_q_d_temp;
    
    hVoltageLimit_Reference = (s16)((hFW_V_Ref*MAX_MODULE)/1000);
    
    Stat_Curr_q_d_temp = FOC_FluxRegulator(Stat_Curr_q_d_ref,Stat_Volt_q_d_1,
                                           hVoltageLimit_Reference);
    
    PID_Speed_InitStructure.wLower_Limit_Integral =
                            -((s32)(Stat_Curr_q_d_temp.qI_Component1)*SP_KIDIV);
    PID_Speed_InitStructure.wUpper_Limit_Integral = 
                             ((s32)(Stat_Curr_q_d_temp.qI_Component1)*SP_KIDIV);
    
    if (Stat_Curr_q_d_ref.qI_Component1 > Stat_Curr_q_d_temp.qI_Component1)
    {
      Stat_Curr_q_d_ref_ref.qI_Component1 = Stat_Curr_q_d_temp.qI_Component1;
    }
    else if (Stat_Curr_q_d_ref.qI_Component1 < -Stat_Curr_q_d_temp.qI_Component1)
    {
      Stat_Curr_q_d_ref_ref.qI_Component1 = -Stat_Curr_q_d_temp.qI_Component1;
    }
    else
    {
      Stat_Curr_q_d_ref_ref.qI_Component1 = Stat_Curr_q_d_ref.qI_Component1;
    }
    
    Stat_Curr_q_d_ref_ref.qI_Component2 = Stat_Curr_q_d_temp.qI_Component2;
  }
  hVMagn = FOC_FluxRegulator_Update(hFW_P_Gain, hFW_I_Gain);
    
#else
  Stat_Curr_q_d_ref_ref = Stat_Curr_q_d_ref;
#endif

#ifdef FEED_FORWARD_CURRENT_REGULATION
  Stat_Volt_q_d_3 = FOC_FF_CurrReg(Stat_Curr_q_d_ref_ref,Stat_Volt_q_d_2,
                                        GET_SPEED_DPP,MCL_Get_BusVolt());
#endif
  
  hTorque_Reference = Stat_Curr_q_d_ref_ref.qI_Component1;
  hFlux_Reference = Stat_Curr_q_d_ref_ref.qI_Component2;  
}

/*******************************************************************************
* Function Name   : FOC_TorqueCntrl
* Description     : This function provides current components Iqs* and Ids* to be
*                   used as reference values (by the FOC_Model function) when in
*                   Torque control mode
* Input           : None.
* Output          : None.
* Return          : None.
*******************************************************************************/
void FOC_TorqueCtrl(void)
{
  Stat_Curr_q_d_ref_ref.qI_Component1 = hTorque_Reference;
  Stat_Curr_q_d_ref_ref.qI_Component2 = hFlux_Reference;
#ifdef FEED_FORWARD_CURRENT_REGULATION
  Stat_Volt_q_d_3 = FOC_FF_CurrReg(Stat_Curr_q_d_ref_ref,Stat_Volt_q_d_2,
                                          GET_SPEED_DPP,MCL_Get_BusVolt());
#endif  
}

/*******************************************************************************
* Function Name   : FOC_MTPAInterface_Init
* Description     : According to the motor parameters written in the MTPA
*                   section of MC_PMSM_motor_param.h, it initializes all the
*                   variables related to the MTPA trajectory generator to proper
*                   values (FOC_MTPA function). It has to be called at least
*                   once before the first motor startup.
* Input           : None.
* Output          : None.
* Return          : None.
*******************************************************************************/
#ifdef IPMSM_MTPA
void FOC_MTPAInterface_Init(void)
{
  MTPA_Const MTPA_InitStructure = {SEGDIV,ANGC,OFST};
  FOC_MTPA_Init(MTPA_InitStructure, ID_DEMAG);
}
#endif

/*******************************************************************************
* Function Name   : FOC_FluxRegulatorInterface_Init
* Description     : According to the motor parameters written in the
*                   fluxweakening section of MC_PMSM_motor_param.h,
*                   it initializes all the variables related to flux-weakening
*                   operations to proper values (FOC_Flux_Regulator function).
*                   It has to be called before every motor startup.
* Input           : None.
* Output          : None.
* Return          : None.
*******************************************************************************/
#ifdef FLUX_WEAKENING
void FOC_FluxRegulatorInterface_Init(void)
{
  PID_Struct_t PI_Stat_Volt_InitStructure;
  
  PI_Stat_Volt_InitStructure.hKp_Gain = hFW_P_Gain;
  PI_Stat_Volt_InitStructure.hKp_Divisor = FW_KPDIV;
  PI_Stat_Volt_InitStructure.hKi_Gain = hFW_I_Gain;
  PI_Stat_Volt_InitStructure.hKi_Divisor = FW_KIDIV; 
  //Lower Limit for Output limitation
  PI_Stat_Volt_InitStructure.hLower_Limit_Output = ID_DEMAG;
  //Upper Limit for Output limitation
  PI_Stat_Volt_InitStructure.hUpper_Limit_Output = 0;
  PI_Stat_Volt_InitStructure.wLower_Limit_Integral = 
                              PI_Stat_Volt_InitStructure.hLower_Limit_Output *
                              PI_Stat_Volt_InitStructure.hKi_Divisor;   //Lower Limit for Integral term limitation
  PI_Stat_Volt_InitStructure.wUpper_Limit_Integral = 0;   //Lower Limit for Integral term limitation
  PI_Stat_Volt_InitStructure.wIntegral = 0;
  
  PI_Stat_Volt_InitStructure.hKd_Gain = FW_KD_GAIN;
  PI_Stat_Volt_InitStructure.hKd_Divisor = FW_KDDIV;
  PI_Stat_Volt_InitStructure.wPreviousError = FW_D_TERM_INIT;
  
  FOC_FluxRegulator_Init(&PI_Stat_Volt_InitStructure,NOMINAL_CURRENT);  
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
