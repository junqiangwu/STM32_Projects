#include "Ram.h"
#include "MC_Globals.h"


void ProcSetKey()
{
		//usSetSpd=usSetSpdBk;//按确认键速度控制才生效
}

void ProcFunKey()
{
	if (fgRun==0)
	{
	 State= INIT;
	 fgRun=1;
	}
	else
	{
		fgRun=0;
		State= STOP;
	}
}
void ProcLeftKey()
{
	hTorque_Reference +=100;
}

void ProcRightKey()
{
	
	hTorque_Reference -=100;
}
void ProcUpKey()
{
	hSpeed_Reference += 10;
}
void ProcDownKey()
{
	
	hSpeed_Reference -= 10;
}

/*
*********************************************************************************************************
* 函数名称：keyScan
* 函数功能：按键扫描程序
* 入口参数：Null
* 出口参数：Null
*********************************************************************************************************
*/
	void KeyProc()
	{
		U8 i;
			if (fgKeyDeal == 0)//8ms 检测一次
			{
				return;
			}
			
			fgKeyDeal =0;
			if (ucKeyPressTime != 0)//长按的时间定义
			{
				ucKeyPressTime--;
			}	
			fgKeyDeal = 0;
			switch (ucKeyDat)  //判断那个键按下
			{
				case 0:
					i = 0;
					break;
				case SetKey:
					i = SetKey;
					break;
				
				case FunKey:
					i = FunKey;
					break;
				case LeftKey:
					i = LeftKey;
					break;
				case RightKey:
					i = RightKey;
					break;
				case UpKey:
					i = UpKey;
					break;
				//case 34:
				//	i = Fun_SetCombKey;
				//	break;
				case DownKey:
					i =  DownKey;
					break;
				
				
				
				
				default:
					i = OtherKey;
					break;
			}
			if (i == ucPreKey)//判断是否跟上个键一样
			{
				if (ucKeyDebouce != 0)//去抖动,时间6*8
				{
					ucKeyDebouce--;
					return;
				}
				if (fgBusy)
				{
					if (i == NoKey)
					{
						fgBusy = 0; 			// 此处处理释放有效键
		//				checkFan2();
						ucKeyNew = 0;
						ucKeyOld = 0;
					}
				}
				else
				{
					if ((i != NoKey) && (i != OtherKey))
					{
						fgBusy = 1;
						ucKeyNew = i;
						ucKeyOld = i;
					}
					
				}
			}
			else
			{	
				ucPreKey = i;
				ucKeyDebouce = KeyDebouce2; 	// 72ms
			}
		///////////////////////////////////////////////////
			if (ucKeyNew != 0)
			{
		
				if (ucKeyOld != 0)
				{
					ucKeyOld = 0;
					switch (ucKeyNew)
						{
							case SetKey:		ProcSetKey(); 	  //不同的按键不同的处理程序//返回按键
								break;		
							
							case FunKey:	
								//联网的时候不能按启动键，手动的时候也不行
								ProcFunKey();
								break;
							case LeftKey:		ProcLeftKey();	//下键处理程序		  
								break;
							case RightKey:		ProcRightKey(); //右键处理程序
								break;
							case UpKey: 		ProcUpKey();	//确认键处理程序	
								break;
							case DownKey:		ProcDownKey();	//左键处理程序	
								break;
							default:
								break;
						}
					
					
					
				}
				else
				{
					
				}
			}
}




