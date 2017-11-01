/*
************************************************************************
*                北京邮电大学 K60 学习例程                                        
* 文件名 :[outputdata.c] 
* 描  述 ：模拟示波器驱动程序
*设计改进：snowwolf
*  Email ：snowwolf2012@qq.com
* 说  明 ：模拟示波器驱动程序
***********************************************************************
*/
#include "outputdata.h"
#include "USART2.h"

int OutData[4] = {0};

/********************************************************************
** 函数名称: CRC_CHECK
** 功能描述: 加校验
** 入口：
** 出口：
** 说明:  
*********************************************************************/

unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
{
    unsigned short CRC_Temp;
    unsigned char i,j;
    CRC_Temp = 0xffff;

    for (i=0;i<CRC_CNT; i++){      
        CRC_Temp ^= Buf[i];
        for (j=0;j<8;j++) {
            if (CRC_Temp & 0x01)
                CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
            else
                CRC_Temp = CRC_Temp >> 1;
        }
    }
    return(CRC_Temp);
}
/********************************************************************
** 函数名称: OutPut_Data()
** 功能描述: 数据输出
** 入口：
** 出口：
** 说明:  
*********************************************************************/

void OutPut_Data(void)
{
  int temp[4] = {0};
  unsigned int temp1[4] = {0};
  unsigned char databuf[10] = {0};
  unsigned char i;
  unsigned short CRC16 = 0;
  for(i=0;i<4;i++)
   {
    
    temp[i]  = (int)OutData[i];
    temp1[i] = (unsigned int)temp[i];
    
   }
   
  for(i=0;i<4;i++) 
  {
    databuf[i*2]   = (unsigned char)(temp1[i]%256);
    databuf[i*2+1] = (unsigned char)(temp1[i]/256);
  }
  
  CRC16 = CRC_CHECK(databuf,8);
  databuf[8] = CRC16%256;
  databuf[9] = CRC16/256;
  
  for(i=0;i<10;i++){
	  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); 
      USART_SendData(USART1,databuf[i]);
  }
}


