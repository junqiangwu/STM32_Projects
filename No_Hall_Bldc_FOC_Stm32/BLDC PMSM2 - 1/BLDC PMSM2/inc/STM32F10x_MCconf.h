/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : STM32F10x_MCconf.h
* Author             : FOCSDK development tool ver. 1.0.0 by IMS Systems Lab
* Creation date      : Mon Aug 26 21:58:31 2013
* Description        : Motor Control Library configuration file.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_MCCONF_H
#define __STM32F10x_MCCONF_H


/********************   Current sampling technique definition   ***************/
/*   Define here the technique utilized for sampling the three-phase current  */

/* Current sensing by ICS (Isolated current sensors) */
//#define ICS_SENSORS

/* Current sensing by Three Shunt resistors */
#define THREE_SHUNT

/* Current sensing by Single Shunt resistor */
//#define SINGLE_SHUNT

/*******************  Position sensing technique definition  ******************/
/* Define here the type of rotor position sensing utilized for both Field     */
/*                    Oriented Control and speed regulation                   */

  /* Position sensing by quadrature encoder */
//#define ENCODER

  /* Position sensing by Hall sensors */
//#define HALL_SENSORS

/* Sensorless position sensing  */
#define NO_SPEED_SENSORS
/* When in sensorless operation define here if you also want to acquire any */
/* position sensor information                                              */
//#define VIEW_HALL_FEEDBACK
//#define VIEW_ENCODER_FEEDBACK
  
/* When in sensorless operation define here if you want to perform an       */
/* alignment before the ramp-up                                             */
#define NO_SPEED_SENSORS_ALIGNMENT

/************************** FOC methods **************************************/
/* Internal Permanent Magnet Motors Maximum-Torque-per-Ampere strategy */
//#define IPMSM_MTPA

/* Flux weakening operations allowed */
//#define FLUX_WEAKENING

/* Feed forward current regulation based on known motor parameters */
//#define FEED_FORWARD_CURRENT_REGULATION

/**************************   Brake technique   *******************************/
/*      Define here the if you want to enable brake resistor management       */
/*          MANDATORY in case of operation in flux weakening region           */
  
/* Uncomment to enable brake resistor management feature */
//#define BRAKE_RESISTOR

/********************    PIDs differential terms enabling  ********************/
/*        Define here if you want to enable PIDs differential terms           */

/* Uncomment to enable differential terms of PIDs */           
//#define DIFFERENTIAL_TERM_ENABLED
  
/********************  PIDs gains tuning operations enabling  *****************/
/*  Define here if you want to tune currents (Id, Iq) PIDs, Luenberger State  */
/*                             Observer and PLL gains                         */                              

/* Uncomment to enable the generation of a square-wave shaped reference Iq */
//#define FLUX_TORQUE_PIDs_TUNING
  
/* Uncomment to enable the tuning of Luenberger State Observer and PLL gains*/ 
//#define OBSERVER_GAIN_TUNING

/***********************   DAC functionality enabling  ************************/

/*Uncomment to enable DAC functionality feature through TIM3 output channels*/
//#define DAC_FUNCTIONALITY

/******************************************************************************/
/* Check-up of the configuration validity*/
#if ( (defined(ICS_SENSORS)) && (defined(THREE_SHUNT)) )
#error "Invalid configuration: Two current sampling techniques selected"
#endif

#if ( (defined(ICS_SENSORS)) && (defined(SINGLE_SHUNT)) )
#error "Invalid configuration: Two current sampling techniques selected"
#endif

#if ( (defined(SINGLE_SHUNT)) && (defined(THREE_SHUNT)) )
#error "Invalid configuration: Two current sampling techniques selected"
#endif

#if ( (!defined(ICS_SENSORS)) && (!defined(THREE_SHUNT)) && (!defined(SINGLE_SHUNT)) )
#error "Invalid setup: No sampling technique selected"
#endif

#if ( (defined(ENCODER)) && (defined (HALL_SENSORS)) || (defined(ENCODER)) &&\
    (defined (NO_SPEED_SENSORS)) || (defined(NO_SPEED_SENSORS)) && (defined\
    (HALL_SENSORS)))
#error "Invalid configuration: Two position sensing techniques selected"
#endif

#if ( (!defined(ENCODER)) && (!defined (HALL_SENSORS)) && (!defined\
                                                            (NO_SPEED_SENSORS)))
#error "Invalid configuration: No position sensing technique selected"
#endif

#if ((defined VIEW_HALL_FEEDBACK) && (!defined NO_SPEED_SENSORS))
#error "Invalid configuration: VIEW_HALL_FEEDBACK supported only in\
                                                           sensorless operation"
#endif

#if ((defined VIEW_ENCODER_FEEDBACK) && (!defined NO_SPEED_SENSORS))
#error "Invalid configuration: VIEW_ENCODER_FEEDBACK supported only in\
                                                           sensorless operation"
#endif

#if ((defined FLUX_TORQUE_PIDs_TUNING) && (defined NO_SPEED_SENSORS))
#error "Invalid configuration: FLUX_TORQUE_PIDs_TUNING not supported in\
                                                           sensorless operation"
#endif

#if ((defined VIEW_HALL_FEEDBACK) && (defined VIEW_ENCODER_FEEDBACK))
#error "Invalid configuration: Two position sensing techniques selected"
#endif

#if ((!defined NO_SPEED_SENSORS) && (defined NO_SPEED_SENSORS_ALIGNMENT))
#warning "No speed sensors alignment has been disabled"
#undef NO_SPEED_SENSORS_ALIGNMENT
#endif

#endif /* __STM32F10x_MCCONF_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
