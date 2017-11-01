 
#include "Ram.h"
#include "STM103REG.h"
#include "stm32f10x_lib.h"
#include "MC_Globals.h"

void ShowChar(const unsigned char * CHI,unsigned char width,unsigned char page,unsigned char line);
void ShowNum(S32 number32,U8 numCnt,U8 point,U8 row,U8 line);
void SendFrame(void);
extern volatile s16 hSpeed_Reference;
extern volatile s16 hTorque_Reference;
extern Volt_Components Stat_Volt_alfa_beta;
extern Volt_Components Stat_Volt_q_d;




/***********************************************************
功能： 向LCD写入数据
参数： WDLCD待写入的字符数据
       select用于选择LCD屏的左右,1选择左屏，2选择右屏
***********************************************************/
void WriteDataLCD(U8 WDLCD,U8 select)
{
	switch(select)
	{
		case 0:{LCDCS1ON();LCDCS2OFF();break;}//左
		case 1:{LCDCS1OFF();LCDCS2ON();break;}//右
		default: break;
    }
	LCDENOFF();
	LCDRSON();
	tmpbyte.byte = WDLCD;
	if(tmpbyte.bit.b0) LCDDB0ON();else LCDDB0OFF();
	if(tmpbyte.bit.b1) LCDDB1ON();else LCDDB1OFF();
	if(tmpbyte.bit.b2) LCDDB2ON();else LCDDB2OFF();
	if(tmpbyte.bit.b3) LCDDB3ON();else LCDDB3OFF();
	if(tmpbyte.bit.b4) LCDDB4ON();else LCDDB4OFF();
	if(tmpbyte.bit.b5) LCDDB5ON();else LCDDB5OFF();
	if(tmpbyte.bit.b6) LCDDB6ON();else LCDDB6OFF();
	if(tmpbyte.bit.b7) LCDDB7ON();else LCDDB7OFF();
	
	

	Delay1Us();
	Delay1Us();
	//Delay1Us();
	//Delay1Us();
	LCDENON();
	//Delay1Us();
	Delay1Us();//若晶振速度高，可在此作延时处理
	LCDENOFF(); //下降沿写入数据
	
	//_nop_();    //若晶振速度高，可在此作延时处理
}



/***********************************************************
功能：向LCD写入命令
参数： WDLCD待写入的字符数据
  	   BusyC是否要检查忙标志位选择标志，1为进行，0为不检测
       select用于选择LCD屏的左右,1选择左屏，2选择右屏
***********************************************************/
void WriteCommandLCD(U8 WCLCD , U8 select)
{
	
	switch(select)
	{
		case 0:{LCDCS1ON();LCDCS2OFF();break;}//左
		case 1:{LCDCS1OFF();LCDCS2ON();break;}//右
		default: break;
    }
    LCDENOFF();
    LCDRSOFF();
	
	tmpbyte.byte = WCLCD;
	if(tmpbyte.bit.b0) LCDDB0ON();else LCDDB0OFF();
	if(tmpbyte.bit.b1) LCDDB1ON();else LCDDB1OFF();
	if(tmpbyte.bit.b2) LCDDB2ON();else LCDDB2OFF();
	if(tmpbyte.bit.b3) LCDDB3ON();else LCDDB3OFF();
	if(tmpbyte.bit.b4) LCDDB4ON();else LCDDB4OFF();
	if(tmpbyte.bit.b5) LCDDB5ON();else LCDDB5OFF();
	if(tmpbyte.bit.b6) LCDDB6ON();else LCDDB6OFF();
	if(tmpbyte.bit.b7) LCDDB7ON();else LCDDB7OFF();
	Delay1Us();
	Delay1Us();
	LCDENON();
	Delay1Us();	
	LCDENOFF();
}


/******************************************************************************
功能：清显示RAM区（清屏）
参数：select为驱动控制器选择 1为左 2为右
********************************************************************************/
void ClearLCD(U8 select)
{
	U8 i,j;
	//WriteCommandLCD( LCDBaseAddrX,select);//页面地址设置，并分别写入左右模块中
	//WriteCommandLCD( LCDBaseAddrY,select);//列地址设置，设置为0列，并分别写入左右三个模块中
	for(i=0;i<8;i++)
	{
		if (fgDispMode==0)
		{
			WriteCommandLCD( LCDBaseAddrX|i,select);//页面地址设置，并分别写入左右模块中
			WriteCommandLCD( LCDBaseAddrY,select);//列地址设置，设置为0列，并分别写入左右三个模块中
		}
		for(j=0;j<64;j++)
		{	
			if(fgDispMode==0) //页内清零
			{
				WriteDataLCD( 0 ,select);
			}
			ucLcdBuffer[i][select*64+j]=0;
			
		}
	}
}


/*****************************************************************************
功能：清LCD左右屏
*****************************************************************************/
void ClrAllLcd(void)
{
	ClearLCD(LCDR);
	ClearLCD(LCDL);
}


void DispLCDBuf (U8 select)
{
	U8 i,j;
	//WriteCommandLCD( LCDBaseAddrX,select);//页面地址设置，并分别写入左右模块中
	//WriteCommandLCD( LCDBaseAddrY,select);//列地址设置，设置为0列，并分别写入左右三个模块中
	for(i=0;i<8;i++)
	{
		if (fgDispMode==0)
		{
			WriteCommandLCD( LCDBaseAddrX|i,select);//页面地址设置，并分别写入左右模块中
			WriteCommandLCD( LCDBaseAddrY,select);//列地址设置，设置为0列，并分别写入左右三个模块中
		}
		for(j=0;j<64;j++)
		{		
			WriteDataLCD( ucLcdBuffer[i][select*64+j] ,select);	
		}
	}
}

void ShowDispBuf()
{
	DispLCDBuf(LCDR);
	DispLCDBuf(LCDL);
}

/******************************************************************************
功能：LCD初始化，分为左，右模块初始化
参数：select为驱动控制器选择  1为左  2为右
********************************************************************************/
void LcdReset(void) 
{
	//U8 i;
	//WriteCommandLCD( LCDBaseAddrZ,LCDR);//设置显示起始行为第一行，并分别写入左右模块中,不检测忙标志
	//WriteCommandLCD( LCDBaseAddrZ,LCDL);	//_nop_();    //若晶振速度高，可在此作延时处理
	LCDRSON();
	WriteCommandLCD( LCDBaseAddrZ,LCDR);//设置显示起始行为第一行，并分别写入左右模块中，检测忙标志
	WriteCommandLCD( LCDBaseAddrZ,LCDL);
	ClrAllLcd();//清屏幕
		
	WriteCommandLCD( LCDBaseAddrX,LCDL);//页面地址设置，并分别写入左右模块中
	WriteCommandLCD( LCDBaseAddrY,LCDL);//列地址设置，设置为0列，并分别写入左右三个模块中
	WriteCommandLCD( LCDBaseAddrX,LCDR);//页面地址设置，并分别写入左右模块中
	WriteCommandLCD( LCDBaseAddrY,LCDR);//列地址设置，设置为0列，并分别写入左右三个模块中
	WriteCommandLCD( LCDDispOn,LCDR);//开显示设置，并分别写入左右模块中
	WriteCommandLCD( LCDDispOn,LCDL);
}




void ShowMenuMain()
{
	//fgDispMode=0;
//		LCDRSOFF();
//		LCDENOFF();
//		LCDDB0OFF();
//		LCDDB1OFF();
//		LCDDB2OFF();
//		LCDDB3OFF();
//		LCDDB4OFF();
//		LCDDB5OFF();
//		LCDDB6OFF();
//		LCDDB7OFF();
//		LCDCS1OFF();
//		LCDCS2OFF();
//		
//		LCDRSON();
//		LCDENON();
//		LCDDB0ON();
//		LCDDB1ON();
//		LCDDB2ON();
//		LCDDB3ON();
//		LCDDB4ON();
//		LCDDB5ON();
//		LCDDB6ON();
//		LCDDB7ON();
//		LCDCS1ON();
//		LCDCS2ON();
//	
//		LCDRSOFF();
//		LCDENOFF();
//		LCDDB0OFF();
//		LCDDB1OFF();
//		LCDDB2OFF();
//		LCDDB3OFF();
//		LCDDB4OFF();
//		LCDDB5OFF();
//		LCDDB6OFF();
//		LCDDB7OFF();
//		LCDCS1OFF();
//		LCDCS2OFF();
	
	//ShowChar(&Eng_C[0],5,1,0);
	if(fgDispMode)
	{
	
	ClrAllLcd();
	
 	ShowChar(&Eng_S[0],5,0,0);
	ShowChar(&Eng_E[0],5,0,6);
	ShowChar(&Eng_T[0],5,0,6*2);
	ShowChar(&Eng_S[0],5,0,6*4);
	ShowChar(&Eng_P[0],5,0,6*5);
	ShowChar(&Eng_D[0],5,0,6*6);
	ShowChar(&sign_maohao[0],3,0,6*7);
	ShowNum(hSpeed_Reference * 6,5,0,0,127);
	
	ShowChar(&Eng_C[0],5,1,0);
	ShowChar(&Eng_U[0],5,1,6);
	ShowChar(&Eng_R[0],5,1,6*2);
	ShowChar(&Eng_S[0],5,1,6*4);
	ShowChar(&Eng_P[0],5,1,6*5);
	ShowChar(&Eng_D[0],5,1,6*6);
	ShowChar(&sign_maohao[0],3,1,6*7);
	ShowNum((s16)(STO_Get_Speed_Hz() * 6),5,0,1,127);

	ShowChar(&Eng_T[0],5,2,0);
	ShowChar(&Eng_O[0],5,2,6);
	ShowChar(&Eng_R[0],5,2,6*2);
	ShowChar(&Eng_Q[0],5,2,6*3);
	ShowChar(&Eng_U[0],5,2,6*4);
	ShowChar(&Eng_E[0],5,2,6*5);
	ShowChar(&sign_maohao[0],3,2,6*6);
	ShowNum(hTorque_Reference,5,0,2,127);

	ShowChar(&Eng_A[0],5,3,0);
	ShowChar(&Eng_l[0],5,3,6);
	ShowChar(&Eng_f[0],5,3,6*2);
	ShowChar(&Eng_a[0],5,3,6*3);
	ShowChar(&Eng_V[0],5,3,6*5);
	ShowChar(&Eng_o[0],5,3,6*6);
	ShowChar(&Eng_l[0],5,3,6*7);
	ShowChar(&Eng_t[0],5,3,6*8);
	ShowChar(&sign_maohao[0],3,3,6*9);
	ShowNum(Stat_Volt_q_d.qV_Component1,5,0,3,127);

	ShowChar(&Eng_B[0],5,4,0);
	ShowChar(&Eng_e[0],5,4,6);
	ShowChar(&Eng_t[0],5,4,6*2);
	ShowChar(&Eng_a[0],5,4,6*3);
	ShowChar(&Eng_V[0],5,4,6*5);
	ShowChar(&Eng_o[0],5,4,6*6);
	ShowChar(&Eng_l[0],5,4,6*7);
	ShowChar(&Eng_t[0],5,4,6*8);
	ShowChar(&sign_maohao[0],3,4,6*9);
	ShowNum(Stat_Volt_q_d.qV_Component2,5,0,4,127);
	fgDispMode =0;
	ShowDispBuf();
	}
	else
	{
		ShowNum(hSpeed_Reference * 6,4,0,0,127);
		ShowNum((s16)(STO_Get_Speed_Hz() * 6),4,0,1,127);
		ShowNum(hTorque_Reference,5,0,2,127);
		ShowNum(Stat_Volt_q_d.qV_Component1,5,0,3,127);
		ShowNum(Stat_Volt_q_d.qV_Component2,5,0,4,127);
		
		//ShowNum(MotorA.State,1,0,2,127);
		
		//ShowNum(BEMF_Cnt,3,0,3,127);
			//ShowChar(&Eng_T[0],5,6,6*7);
		
		          ShowChar(&Eng_N[0],5,6,6*7);
	            ShowChar(&Eng_O[0],5,6,6*8);
            	//ShowChar(&Eng_ [0],5,6,6*9);
		
		
             	ShowChar(&Eng_H[0],5,6,6*10);
	            ShowChar(&Eng_A[0],5,6,6*11);
            	ShowChar(&Eng_L[0],5,6,6*12);
	            ShowChar(&Eng_L[0],5,6,6*13);
            //	ShowChar(&Eng_R[0],5,6,6*12);
		 
		 
		
		
		 	        ShowChar(&Eng_T[0],5,5,6*15);
             	ShowChar(&Eng_A[0],5,5,6*16);
	            ShowChar(&Eng_N[0],5,5,6*17);
            	ShowChar(&Eng_K[0],5,5,6*18);
	            ShowChar(&Eng_O[0],5,5,6*19);
            	ShowChar(&Eng_R[0],5,5,6*20);


				      ShowChar(&Eng_T[0],5,7,0);
             	ShowChar(&Eng_A[0],5,7,6);
	            ShowChar(&Eng_N[0],5,7,6*2);
            	ShowChar(&Eng_K[0],5,7,6*3);
	            ShowChar(&Eng_O[0],5,7,6*4);
            	ShowChar(&Eng_R[0],5,7,6*5);
	}
}


/******************************************************************************
功能：设置数据写入或读出的地址，通过页号和列数可以唯一确定一个地址
参数：Column表示列号(Y)，Page表示页号(X)
********************************************************************************/
void SetXY(U8 Column,U8 Page)
{	
	U8 x,y;
	U8 select=0;
	switch (Column&0x40) //完成左右屏的判断	，根据列号完成区分,<64为左屏,>=64为右屏
 	{   
 	 	case 0x00: {select = 0;fgLcdCS1=0; break;}/*为左区 */
  		case 0x40: {select = 1;fgLcdCS1=1;break;}/*为右区 */
 	}
	y = Column&0x3F|0x40; /* col.and.0x3f.or.Set Y Address*/
 	x = Page&0x07|0xB8; /* row.and.0x07.or.set Page */	
	WriteCommandLCD( x,select);//设置页面地址
	WriteCommandLCD( y,select);//设置列地址
	WriteCommandLCD( LCDBaseAddrZ,select);
}

void ShowBlank(unsigned char width,unsigned char row,unsigned char line)
{
	unsigned char i;
	//SetLine(line);
	//SetPage(row);
	if (fgDispMode==0)
	SetXY(line,row);
	for(i=0;i<width;i++) 
	{	
		if (fgDispMode==0)
		{
			if (fgLcdCS1==LCDL) 
			{	if (line+i>=64) 
				{	//SetLine(line+i);
				//SetPage(row);
				SetXY(line+i,row);
				}
			}
			WriteDataLCD(0,fgLcdCS1);
		}
		ucLcdBuffer[row][line+i]=0;
		
	}

}


/*
*********************************************************************************************************
* 函数名称：NumToArray
* 函数功能：数字转换成数组
* 入口参数：number32:转换的数字
* 出口参数：Null
*********************************************************************************************************
*/
void NumToArray(S32 number32)
{
	U32 num;
	if(number32<0)
	{
		N2ABuf.Fuhao=0x01;
		num=(-1)*number32;
	}
	else
	{
		N2ABuf.Fuhao=0x00;
		num=number32;
	}
	N2ABuf.Buf[0]=(U8)(num%10);
	num/=10;
	N2ABuf.Buf[1]=(U8)(num%10);
	num/=10;
	N2ABuf.Buf[2]=(U8)(num%10);
	num/=10;
	N2ABuf.Buf[3]=(U8)(num%10);
	num/=10;
	N2ABuf.Buf[4]=(U8)(num%10);
	num/=10;
	N2ABuf.Buf[5]=(U8)(num%10);
	num/=10;
	N2ABuf.Buf[6]=(U8)(num%10);
	num/=10;
	N2ABuf.Buf[7]=(U8)(num%10);
	N2ABuf.Buf[8]=(U8)(num/10);
	
}


void ShowChar(const unsigned char *CHI,unsigned char width,unsigned char page,unsigned char line)
{
	U8 i;
	if (fgDispMode==0)
   	SetXY(line,page);
	for(i=0;i<width;i++) 
	{	
		if (fgDispMode==0)
		{
			if (fgLcdCS1==LCDL) 
			{	
				if (line+i>=64) 
				{	
					SetXY(line+i,page);
				}
			}
		WriteDataLCD(*(CHI+i),fgLcdCS1);
		//WriData(*(CHI+i));
		}
		ucLcdBuffer[page][line+i]=*(CHI+i);
	}
}

void ShowNum(S32 number32, U8 numCnt, U8 point, U8 row ,U8 line)

{
	U32 temp32;
	U8 num0,num1,num2,num3,num4,num5,num6,num7,num8;
	//先清空原来所显示数值所占的空间
	if(point>0)
	{
		if(number32<0)
		{
			//当所要显示的参数为负数时，清空时应把小数点和负号所占的列也考虑进去
			ShowBlank((numCnt*6+1*3+6+1),row,(line-numCnt*6-1*3-6));
		}
		else
		{
			//当存在小数位时，清空时应把小数点所占的列也考虑进去
			ShowBlank((numCnt*6+1*3+1),row,(line-numCnt*6-1*3));
		}		
	}
	else
	{
		if(number32<0)
		{
			//当所要显示的参数为负数时，清空时应把负号所占的列也考虑进去
			ShowBlank((numCnt*6+6+1),row,(line-numCnt*6-6));
		}
		else
		{
			 ShowBlank((numCnt*6+1),row,(line-numCnt*6));
		}		
	}
	
/*
	if (number32<0) 
		temp32=number32*(-1);
	else 
		temp32=number32;
*/
	NumToArray(number32);  //将该数转换为字符数组，同时也是更新全局变量N2ABuf.Buf[]
	num0=N2ABuf.Buf[0];
	num1=N2ABuf.Buf[1];
	num2=N2ABuf.Buf[2];
	num3=N2ABuf.Buf[3];
	num4=N2ABuf.Buf[4];
	num5=N2ABuf.Buf[5];
	num6=N2ABuf.Buf[6];
	num7=N2ABuf.Buf[7];
	num8=N2ABuf.Buf[8];
/*
	num0=(U8)(temp32%10);
	temp32/=10;
	num1=(U8)(temp32%10);
	temp32/=10;
	num2=(U8)(temp32%10);
	temp32/=10;
	num3=(U8)(temp32%10);
	num4=(U8)(temp32/10);
*/
	if (number32<0) 
		temp32=number32*(-1);
	else 
		temp32=number32;

	switch(point)
	{
		case 0:
			ShowChar(&number8x5[num0][0],5,row,(line-1*6+1))	;
			if((temp32>9)&&(numCnt>1))
				ShowChar(&number8x5[num1][0],5,row,(line-2*6+1))	;
			if((temp32>99)&&(numCnt>2))
				ShowChar(&number8x5[num2][0],5,row,(line-3*6+1))	;
			if((temp32>999)&&(numCnt>3))
				ShowChar(&number8x5[num3][0],5,row,(line-4*6+1))	;
			if((temp32>9999)&&(numCnt>4))
				ShowChar(&number8x5[num4][0],5,row,(line-5*6+1))	;
			if((temp32>99999)&&(numCnt>5))
				ShowChar(&number8x5[num4][0],5,row,(line-6*6+1))	;
			if((temp32>999999)&&(numCnt>6))
				ShowChar(&number8x5[num4][0],5,row,(line-7*6+1))	;
			if(number32<0)
			{
				if((temp32>999999)&&(numCnt>6))
					ShowChar(&sign_fu[0],5,row,(line-8*6+1))	;
				else if((temp32>99999)&&(numCnt>5))
					ShowChar(&sign_fu[0],5,row,(line-7*6+1))	;
				else if((temp32>9999)&&(numCnt>4))
					ShowChar(&sign_fu[0],5,row,(line-6*6+1))	;
				else if((temp32>999)&&(numCnt>3))
					ShowChar(&sign_fu[0],5,row,(line-5*6+1))	;
				else if((temp32>99)&&(numCnt>2))
					ShowChar(&sign_fu[0],5,row,(line-4*6+1))	;
				else if((temp32>9)&&(numCnt>1))
					ShowChar(&sign_fu[0],5,row,(line-3*6+1))	;
				else
					ShowChar(&sign_fu[0],5,row,(line-2*6+1))	;
			}
			break;
		case 1:
			ShowChar(&number8x5[num0][0],5,row,(line-1*6+1))	;
			ShowChar(&sign_dot[0],2,row,line-1*6-1*3+1);
			ShowChar(&number8x5[num1][0],5,row,(line-2*6-1*3+1))	;
			if((temp32>99)&&(numCnt>2))
				ShowChar(&number8x5[num2][0],5,row,(line-3*6-1*3+1))	;
			if((temp32>999)&&(numCnt>3))
				ShowChar(&number8x5[num3][0],5,row,(line-4*6-1*3+1))	;
			if((temp32>9999)&&(numCnt>4))
				ShowChar(&number8x5[num4][0],5,row,(line-5*6-1*3+1))	;
			if((temp32>99999)&&(numCnt>5))
				ShowChar(&number8x5[num4][0],5,row,(line-6*6-1*3+1))	;
			if((temp32>999999)&&(numCnt>6))
				ShowChar(&number8x5[num4][0],5,row,(line-7*6-1*3+1))	;
			if(number32<0)
			{
				if((temp32>999999)&&(numCnt>6))
					ShowChar(&sign_fu[0],5,row,(line-8*6-1*3+1))	;
				else if((temp32>99999)&&(numCnt>5))
					ShowChar(&sign_fu[0],5,row,(line-7*6-1*3+1))	;
				else if((temp32>9999)&&(numCnt>4))
					ShowChar(&sign_fu[0],5,row,(line-6*6-1*3+1))	;
				else if((temp32>999)&&(numCnt>3))
					ShowChar(&sign_fu[0],5,row,(line-5*6-1*3+1))	;
				else if((temp32>99)&&(numCnt>2))
					ShowChar(&sign_fu[0],5,row,(line-4*6-1*3+1))	;
				else 
					ShowChar(&sign_fu[0],5,row,(line-3*6-1*3+1))	;	
			}
			break;
		case 2:
			ShowChar(&number8x5[num0][0],5,row,(line-1*6+1))	;
			ShowChar(&number8x5[num1][0],5,row,(line-2*6+1))	;
			ShowChar(&sign_dot[0],2,row,line-2*6-1*3+1);
			ShowChar(&number8x5[num2][0],5,row,(line-3*6-1*3+1))	;
			if((temp32>999)&&(numCnt>3))
				ShowChar(&number8x5[num3][0],5,row,(line-4*6-1*3+1))	;
			if((temp32>9999)&&(numCnt>4))
				ShowChar(&number8x5[num4][0],5,row,(line-5*6-1*3+1))	;
			if((temp32>99999)&&(numCnt>5))
				ShowChar(&number8x5[num5][0],5,row,(line-6*6-1*3+1))	;
			if((temp32>999999)&&(numCnt>6))
				ShowChar(&number8x5[num6][0],5,row,(line-7*6-1*3+1))	;
			if(number32<0)
			{
				if((temp32>999999)&&(numCnt>6))
					ShowChar(&sign_fu[0],5,row,(line-8*6-1*3+1))	;
				else if((temp32>99999)&&(numCnt>5))
					ShowChar(&sign_fu[0],5,row,(line-7*6-1*3+1))	;
				else if((temp32>9999)&&(numCnt>4))
					ShowChar(&sign_fu[0],5,row,(line-6*6-1*3+1))	;
				else if((temp32>999)&&(numCnt>3))
					ShowChar(&sign_fu[0],5,row,(line-5*6-1*3+1))	;
				else
					ShowChar(&sign_fu[0],5,row,(line-4*6-1*3+1))	;
			}
			break;
		case 3:
			ShowChar(&number8x5[num0][0],5,row,(line-1*6+1))	;
			ShowChar(&number8x5[num1][0],5,row,(line-2*6+1))	;
			ShowChar(&number8x5[num2][0],5,row,(line-3*6+1))	;
			ShowChar(&sign_dot[0],2,row,line-3*6-1*3+1);
			ShowChar(&number8x5[num3][0],5,row,(line-4*6-1*3+1))	;
			//if(temp32>999)
			//	ShowChar(&number8x5[num4][0],5,row,(line-5*6-1*3+1))	;
			if((temp32>9999)&&(numCnt>4))
				ShowChar(&number8x5[num4][0],5,row,(line-5*6-1*3+1))	;
			if((temp32>99999)&&(numCnt>5))
				ShowChar(&number8x5[num5][0],5,row,(line-6*6-1*3+1))	;
			if((temp32>999999)&&(numCnt>6))
				ShowChar(&number8x5[num6][0],5,row,(line-7*6-1*3+1))	;
			if((temp32>9999999)&&(numCnt>7))
				ShowChar(&number8x5[num7][0],5,row,(line-8*6-1*3+1))	;
			if((temp32>99999999)&&(numCnt>8))
				ShowChar(&number8x5[num8][0],5,row,(line-8*6-1*3+1))	;
			if(number32<0)
			{
				if((temp32>9999999)&&(numCnt>7))
					ShowChar(&sign_fu[0],5,row,(line-9*6-1*3+1))	;
				else if((temp32>999999)&&(numCnt>6))
					ShowChar(&sign_fu[0],5,row,(line-8*6-1*3+1))	;
				else if((temp32>99999)&&(numCnt>5))
					ShowChar(&sign_fu[0],5,row,(line-7*6-1*3+1))	;
				else if((temp32>9999)&&(numCnt>4))
					ShowChar(&sign_fu[0],5,row,(line-6*6-1*3+1))	;				
				else
					ShowChar(&sign_fu[0],5,row,(line-6*6-1*3+1))	;
			}
			break;
		default:
			break;
	}
}




int pid(int nonce,int aim)
{
	static int ek_2=0;
	static int ek_1=0;
	static int ek=0;
//	int ec;	
	int uk; 
	
	ek=aim-nonce;
//	ec=ek/T;
	uk=kp*(ek-ek_1+ki*ek+kd*(ek-2*ek_1+ek_2));
	ek_2=ek_1;
	ek_1=ek;
	if(uk>20)
	{
		uk=20;
	}
	if(uk<-20)
	{
		uk=-20;
	}
	return (uk);
}


void SpeedPidCtl()
{
		S32 siTemp;
		U16 temp;
		
		siLast_e = siThis_e;
		siThis_e = usSetSpd-speed_1;
		
		siLast_delta_e = siThis_delta_e;
		siThis_delta_e = siThis_e - siLast_e;
		siDoub_delta_e = siThis_delta_e - siLast_delta_e;
		siTemp =  (siThis_delta_e*kp+siThis_e*ki+kd*siDoub_delta_e);//PID值放大100倍
		if ((usSetSpd-speed_1)>50)
		{
			temp=1000;
		}
		else
		{
			temp=200;
		}
		
		
			if (siTemp < 0)	// PID输出结果小于零
			{
				siTemp = -siTemp;// 取绝对值
				if (siTemp >= temp)
				{
					siPwidth -= (temp/100);
				}
				else
				{
					//ucTemp = (U8)siTemp;
//						if (ucTemp > 0x3f)
//						{
//							ucTemp = 0x3f;
//						}
					siPwidth -= (siTemp/100);
				}
			}
			else	// PID输出结果大于等于零
			{
				if (siTemp >= temp)
				{
					siPwidth += (temp/100);
				}
				else 
				{
					//ucTemp = (U8)siTemp;
//						if (ucTemp > 0x3f)
//						{
//							ucTemp = 0x3f;
//						}
					siPwidth += (siTemp/100);
				}
			}

		if ((siPwidth < 0))
		{
			siPwidth = 0;
		}
		else if (siPwidth>= (Period-200))
		{
			siPwidth = (Period-200);
		}			
	}


void Timeproc()
{
//		if ((fgSendABKaa==0)&&fg2S)
//		{
//			fgAdConv=0;
//			ucST++;
//			if(ucST>1)
//			{
//			ucST=0;
//			ucSendTime++;
//		
//		
//			//SendFrame();
//			
//			if (ucSendTime>1000)
//			{
//				//fgSendA=0;
//				fgSendABKaa=1;
//				ucSendTime=0;
//			}
//			}
//		}
	
	if (fgT10Ms ==0)
	 {
		 return;
	 }
	//SendFrame();
	 fgT10Ms = 0;
	 ucT1S++;
	 ucT100ms++;
	 if(ucT100ms>=10)
	 {
	 	
			//My_PWM=200;
			//SendFrame1();
		

		
		
		
		ucT100ms=0;
	 }
	 if(ucT1S>=100) //每秒的事件处理
	 {
	 	
	 	
		
		fg2S=1;
		ucT10S++;
		if (ucT10S>=10)
		{
			ucT10S=0;
			fg2S=1;
		}
		//	My_PWM+=pid(speed_1,aim_speed)/((speed_1/My_PWM)+1);	
//			if(My_PWM<0)
//			My_PWM=0;
//			if(My_PWM>2000)			    
//			My_PWM=1999;
		
		
	 	ShowMenuMain();
		ucT1S=0;
		
	 }
}





