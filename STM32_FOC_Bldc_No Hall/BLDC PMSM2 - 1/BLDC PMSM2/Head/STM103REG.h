
#ifndef STM103REG
#define STM103REG
#include "TYPESTRUCT.h"


/**********************************************/
/* MACRO DEFINE */
#define __UMEM8(addr)  (*((volatile unsigned char  *)addr))
#define __UMEM16(addr) (*((volatile unsigned short *)addr))
#define __UMEM32(addr) (*((volatile unsigned long  *)addr))

/* SPECIAL REGISTER DEFINE */
#define    PASET	__UMEM32(0x40010810)
#define    PARESET	__UMEM32(0x40010814)
#define    PAIN		__UMEM32(0x40010808)

#define    PBSET	__UMEM32(0x40010C10)
#define    PBRESET	__UMEM32(0x40010C14)
#define    PBIN		__UMEM32(0x40010C08)

#define    PCSET	__UMEM32(0x40011010)
#define    PCRESET	__UMEM32(0x40011014)
#define    PCIN		__UMEM32(0x40011008)

#define    PDOUT	__UMEM32(0x4001140C)
#define    PDSET	__UMEM32(0x40011410)
#define    PDRESET	__UMEM32(0x40011414)
#define    PDIN		__UMEM32(0x40011408)

#define    PESET	__UMEM32(0x40011810)
#define    PERESET	__UMEM32(0x40011814)
#define    PEIN		__UMEM32(0x40011808)

/*************************引脚宏定义****************************/
/******************GPIOA********************/
#define PAIA_BUS_VOLT			GPIO_Pin_0 
#define PAIA_BEMFA				GPIO_Pin_1
#define PAIA_BEMFB				GPIO_Pin_2
#define PAIA_BEMFC				GPIO_Pin_3
#define PAIA_IA					GPIO_Pin_4
#define PAIA_IB					GPIO_Pin_5
#define PAIA_ITOTAL 			GPIO_Pin_6
#define PAI_KSET 				GPIO_Pin_7

#define PAO_T1CH1 				GPIO_Pin_8
#define PAO_T1CH2 				GPIO_Pin_9
#define PAO_T1CH3 				GPIO_Pin_10
#define PAO_LCDRS 				GPIO_Pin_11
#define PAO_LCDEN 				GPIO_Pin_12
#define PAO_LCDDB0 				GPIO_Pin_15
/******************GPIOB********************/
#define PBI_KRIGHT				GPIO_Pin_0 
#define PBI_KUP					GPIO_Pin_1
#define PBO_LCDDB5				GPIO_Pin_3
#define PBO_LCDDB6				GPIO_Pin_4
#define PBO_LCDDB7				GPIO_Pin_5
#define PBO_URTTX				GPIO_Pin_6
#define PBI_URTRX				GPIO_Pin_7
#define PBO_LCDCS1				GPIO_Pin_8

#define PBO_LCDCS2 				GPIO_Pin_9
#define PBI_KDOWN 				GPIO_Pin_10
#define PBO_MBREAK 				GPIO_Pin_11
#define PBO_M_EN 				GPIO_Pin_12
#define PBO_T1CH1N 				GPIO_Pin_13
#define PBO_T1CH2N 				GPIO_Pin_14
#define PBO_T1CH3N 				GPIO_Pin_15

/******************GPIOC********************/
#define PCO_IIC_CLK				GPIO_Pin_0 
#define PCO_IIC_DATA			GPIO_Pin_1
#define PCO_LED1				GPIO_Pin_2
#define PCIA_SPEEDAD			GPIO_Pin_3
//#define PCI_KFUN				GPIO_Pin_4
#define PCI_KLEFT				GPIO_Pin_5
#define PCI_HELLA				GPIO_Pin_6
#define PCI_HELLB				GPIO_Pin_7

#define PCI_HELLC 				GPIO_Pin_8
#define PCI_DEFAULT 			GPIO_Pin_9
#define PCO_LCDDB1 				GPIO_Pin_10
#define PCO_LCDDB2 				GPIO_Pin_11
#define PCO_LCDDB3				GPIO_Pin_12
//#define PCO_T1CH2N 				GPIO_Pin_14
#define PCI_KSET 				GPIO_Pin_15
/******************GPIOD********************/
#define PDO_LCDDB4				GPIO_Pin_2 








/**********************************
   IO定义
***********************************/

/****************P0 Define********************************/

#define LCDRSON()     PASET = PAO_LCDRS
#define LCDRSOFF()    PARESET = PAO_LCDRS

#define LCDENON()      PASET = PAO_LCDEN
#define LCDENOFF()     PARESET = PAO_LCDEN

#define LCDDB0ON()      PASET = PAO_LCDDB0
#define LCDDB0OFF()    	PARESET = PAO_LCDDB0

#define T1CH1ON()      PASET = PAO_T1CH1
#define T1CH1OFF()    	PARESET = PAO_T1CH1

#define T1CH2ON()      PASET = PAO_T1CH2
#define T1CH2OFF()    	PARESET = PAO_T1CH2

#define T1CH3ON()      PASET = PAO_T1CH3
#define T1CH3OFF()    	PARESET = PAO_T1CH3



#define KEY_SET 		((*(volatile TYPE_DWORD*)&PAIN).bit.b7)






//#define LED1    ((*(volatile TYPE_DWORD*)&PBOUT).bit.b10)
#define LCDDB5ON()    	PBSET = PBO_LCDDB5
#define LCDDB5OFF()   	PBRESET = PBO_LCDDB5

#define LCDDB6ON()    	PBSET = PBO_LCDDB6
#define LCDDB6OFF()   	PBRESET = PBO_LCDDB6

#define LCDDB7ON()    	PBSET = PBO_LCDDB7
#define LCDDB7OFF()   	PBRESET = PBO_LCDDB7

#define LCDCS1ON()     	PBSET = PBO_LCDCS1
#define LCDCS1OFF()    	PBRESET = PBO_LCDCS1


#define LCDCS2ON()     	PBSET = PBO_LCDCS2
#define LCDCS2OFF()    	PBRESET = PBO_LCDCS2

#define T1CH1NON()     	PBSET = PBO_T1CH1N
#define T1CH1NOFF()    	PBRESET = PBO_T1CH1N

#define T1CH2NON()     	PBSET = PBO_T1CH2N
#define T1CH2NOFF()    	PBRESET = PBO_T1CH2N

#define T1CH3NON()     	PBSET = PBO_T1CH3N
#define T1CH3NOFF()    	PBRESET = PBO_T1CH3N




#define MBREAKON()     	PBSET = PBO_MBREAK
#define MBREAKOFF()    	PBRESET = PBO_MBREAK

#define MENON()     	PBSET = PBO_M_EN
#define MENOFF()    	PBRESET = PBO_M_EN


#define KEY_RIGHT 		((*(volatile TYPE_DWORD*)&PBIN).bit.b0)
#define KEY_UP 			((*(volatile TYPE_DWORD*)&PBIN).bit.b1)
#define KEY_DOWN 		((*(volatile TYPE_DWORD*)&PBIN).bit.b10)


/***************GPIOC********************/

#define IICSCLON()    	PCSET = PCO_IIC_CLK
#define IICSCLOFF()   	PCRESET = PCO_IIC_CLK

#define IICSDAON()    	PCSET = PCO_IIC_DATA
#define IICSDAOFF()   	PCRESET = PCO_IIC_DATA

#define LEDON()    		PCSET = PCO_LED1
#define LEDOFF()   		PCRESET = PCO_LED1


#define LCDDB1ON()    	PCSET = PCO_LCDDB1
#define LCDDB1OFF()   	PCRESET = PCO_LCDDB1

#define LCDDB2ON()    	PCSET = PCO_LCDDB2
#define LCDDB2OFF()   	PCRESET = PCO_LCDDB2

#define LCDDB3ON()    	PCSET = PCO_LCDDB3
#define LCDDB3OFF()   	PCRESET = PCO_LCDDB3
#define MDEFAULT()		((*(volatile TYPE_DWORD*)&PCIN).bit.b9)

#define KEY_FUN 			((*(volatile TYPE_DWORD*)&PCIN).bit.b15)
#define KEY_LEFT 		((*(volatile TYPE_DWORD*)&PCIN).bit.b5)

/***************GPIOD********************/
#define LCDDB4ON()    	PDSET = PDO_LCDDB4
#define LCDDB4OFF()   	PDRESET = PDO_LCDDB4















#endif

