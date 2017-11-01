/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : readme.txt
* Author             : MCD Application Team
* Version            : V2.0
* Date               : 05/23/2008
* Description        : This sub-directory contains all the user-modifiable files 
*                      needed to create a new project linked with the STM32F10x  
*                      Firmware Library and working with RealView Microcontroller
*                      Development Kit(RVMDK) software toolchain (Version 3.21 and later).
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED 
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

Directory contents
===================
- Project.Uv2/.Opt: A pre-configured project file with the provided library structure
                    that produces an executable image with RVMDK.
              
- stm32f10x_vector.s: This file contains the vector table for STM32F10x, Stack
                      and Heap definition. User can also enable the use of external
                      SRAM mounted on STM3210E-EVAL board as data memory.                
                      
- cortexm3_macro.s: Instruction wrappers for special Cortex-M3 instructions. 

- note.txt        : Steps to follow when using the default startup file provided 
                    by RVMDK when creating new projects.
      
How to use it
=============
- Open the Project.Uv2 project
- In the build toolbar select the project config:
    - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
    - STM3210E-EVAL: to configure the project for STM32 High-density devices
- Rebuild all files: Project->Rebuild all target files
- Load project image: Debug->Start/Stop Debug Session
- Run program: Debug->Run (F5)

NOTE:
 - Medium-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 32 and 128 Kbytes.
 - High-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 256 and 512 Kbytes.
    
******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE******
