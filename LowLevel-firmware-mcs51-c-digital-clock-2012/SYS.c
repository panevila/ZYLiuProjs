
#include<reg52.h> 
#include "LCD12864.h"
#include "COMMON.h"
#include "LCDClock.h"
#include "DS1302.h"
#include "SYS.h"
#include "INT.h"
#include "IO.h"


msg msg_Keys;
u16 F_Process;
flag F_Timeup;
flag F_TMode;
flag F_Return;
flag F_AlarmEN;

flag idata F_Selected;
flag idata F_TIMState;

u8 idata temp;
u8 idata Ttemp;



void SYS_Init()
{
	AlarmBeep = 0;
	LCD_Init();
	INT_EX0_Init();
	INT_TIM0_Init();
	INT_TIM1_Init();
	INT_TIM1_Cmd();
	Clock_Alarm_CLR();

	F_HourMode = 1;	
	F_Process = 0;
	msg_Keys = 8;
	F_Selected = 1;
	F_Return = 0;
	F_AlarmEN = 0;

	uYEAR = 0;
	uMONTH = 0;
	uDATE = 0;
	uHOUR = 0;
	uMINUTE = 0;
	uSECOND = 0;

	F_Timeup = 0;
	F_TIMState = 0;
	Clock_Tim_CLR();
}

void SYS_Prep()
{
	 if(!msg_Keys) 
	{
		msg_Keys = 8;
	}
	if(temp!=F_Process) 		
	{
	//-------------------------菜单改变服务-----------------//
		LCD_CLR();
		F_Selected = 1;
		Clock_GetForSet();
		if(Ttemp!=F_TMode)
		{
			Clock_Tim_CLR();
			F_TIMState = 0;
		}

	}
	if(F_Timeup)
	{
		F_Timeup = 0;
		F_Process = 1120;
	
	}
	if(F_Return&&F_Process)
	{
		F_Return = 0;
		F_Process = 0;
	}
	temp = F_Process;
	Ttemp = F_TMode;
}
void SYS_Main()
{
	while(1)
	{
		Clock_Alarm_Chk();
		if(cHOUR==aHOUR&&cMINUTE==aMINUTE&&F_AlarmEN&&F_Process!=113)
		{
			F_Process = 1120;
			Clock_Alarm_CLR();
			F_AlarmEN = 0;
		}
		SYS_Prep();
		switch(F_Process)
		{
			
			case 0:
			{
				SYS_Default();
				break;
			}
			case 1:
			{
				SYS_Menu();
				break;
			}
			case 11:
			{
				SYS_SubM_Fts();
				break;
			}
			case 12:
			{
				SYS_SubM_Set();
				break;
			}
			case 13:
			{
				SYS_Init();
				F_Process = 121;
				break;
			}
			case 14:
			{
				SYS_Info();
				break;
			}
			case 111:
			{
				SYS_SSubM_TIM();
				break;
			}
			case 112:
			{
				SYS_SSubM_rvTIM();
				break;
			}
			case 113:
			{
				SYS_SSubM_SetAlarm();
				break;
			}
			case 1120:
			{
				SYS_Timeup();
				break;
			}
			case 121:
			{
				SYS_Success();
				delayms(1000);
				F_Process = 0;
				break;
			}
		

			default: ;
		}
			
	}
}

//------------------------菜单指针-----------------------//
void SYS_Pointer()
{
	u8 idata sign[2];
	u8 idata signCLR[2];
	sign[0] = 0x20;
	sign[1] = 0x10;
	signCLR[0] = 0x20;
	signCLR[1] = 0x20;
	switch(F_Selected)
	{
		case 1:
		{
			LCD_NLet(0x80,sign,1);
			LCD_NLet(0x90,signCLR,1);
			LCD_NLet(0x88,signCLR,1);
			LCD_NLet(0x98,signCLR,1);

			break;
		}
		case 2:
		{
			LCD_NLet(0x90,sign,1);
			LCD_NLet(0x80,signCLR,1);
			LCD_NLet(0x88,signCLR,1);
			LCD_NLet(0x98,signCLR,1);

			break;
		}
		case 3:
		{
			LCD_NLet(0x88,sign,1);
			LCD_NLet(0x80,signCLR,1);
			LCD_NLet(0x90,signCLR,1);
			LCD_NLet(0x98,signCLR,1);

			break;
		}
		case 4:
		{
			LCD_NLet(0x98,sign,1);
			LCD_NLet(0x80,signCLR,1);
			LCD_NLet(0x90,signCLR,1);
			LCD_NLet(0x88,signCLR,1);

			break;
		}
		default: ;
	}
}
//------------------------待机界面-----------------------//
void SYS_Default()
{
	u8 idata AlarmS[2];
	u8 idata AlarmNS[2];
	AlarmS[0] = 0x0E;
	AlarmS[1] = 0x20;
	AlarmNS[0] = 0x20;
	AlarmNS[1] = 0x20;
	//msg_Keys = 8;
	AlarmBeep = 0;
	Clock_UpdateTime();
	Clock_ShowTime();
	Clock_ShowDate();

	if(F_AlarmEN)	LCD_NLet(0x87,AlarmS,1);
	else	LCD_NLet(0x87,AlarmNS,1);	

	
	switch(msg_Keys)
	{
		case 1: 
		{
			F_Process = 1;
			msg_Keys = 8;
			delayms(100);
			break;
		}
	
		default:;
	}
	 
}

//------------------------主菜单-----------------------//
void SYS_Menu()
{	
	
	u8 idata txt1[]="常用功能";
	u8 idata txt2[]="调整时间日期";	
	u8 idata txt3[]="复位计时";
	u8 idata txt4[]="程序信息";
	

	LCD_NChar(0x81,txt1,8);
	LCD_NChar(0x91,txt2,12);
	LCD_NChar(0x89,txt3,8);
	LCD_NChar(0x99,txt4,8);
	
	SYS_Pointer();

	switch(msg_Keys)
	{
		case 1:
		{
			switch(F_Selected)
			{
				case 1: 
				{
					F_Process = 11;
					break;
				}
				case 2: 
				{
					F_Process = 12;
					break;
				}
				case 3:
				{
					F_Process = 13;
					break;
				} 
				case 4: 
				{
					F_Process = 14;
					break;
				}
			}
		}
		case 3: 
		{
			if(F_Selected == 4)F_Selected = 1;
			else F_Selected++;
			msg_Keys = 8;
			break;
		}
		case 2: 
		{
			if(F_Selected == 1)F_Selected = 4;
			else F_Selected--;
			msg_Keys = 8;
			break;
		}
		case 6: 
		{
			F_Process = 0;
			delayms(100);
			msg_Keys = 8;
			break;
		}
		case 7: 
		{
			F_Process = 0;
			delayms(100);
			msg_Keys = 8;
			break;
		}
		default:;
	}
}

//------------------------功能菜单-----------------------//
void SYS_SubM_Fts()
{
	u8 idata txt1[]="秒表";	
	u8 idata txt2[]="定时器";
	u8 idata txt3[]="设置闹钟";	

	LCD_NChar(0x81,txt1,4);
	LCD_NChar(0x91,txt2,6);
	LCD_NChar(0x89,txt3,8);

	SYS_Pointer();
	switch(msg_Keys)
	{
		case 1:
		{
			switch(F_Selected)
			{
				case 1: 
				{
					F_Process = 111;
					F_TMode = 0;
					msg_Keys = 8;
					break;
				}
				case 2: 
				{
					F_Process = 112;
					F_TMode = 1;
					msg_Keys = 8;
					break;
				}
				case 3: 
				{
					F_Process = 113;
					msg_Keys = 8;
					break;
				}
				case 4: ;
			}
		}
		case 3: 
		{
			if(F_Selected == 4)F_Selected = 1;
			else F_Selected++;
			msg_Keys = 8;
			break;
		}
		case 2: 
		{
			if(F_Selected == 1)F_Selected = 4;
			else F_Selected--;
			msg_Keys = 8;
			break;
		}
		case 6: 
		{
			F_Process = 1;
			delayms(100);
			msg_Keys = 8;
			break;
		}
		case 7: 
		{
			F_Process = 0;
			delayms(100);
			msg_Keys = 8;
			break;
		}
		default:;
	}

}

//-----------------------时间设置界面-------------------------//
void SYS_SubM_Set()
{	
	u8 idata PtLeft[2] = {0x1E,0x20};
	u8 idata PtRight[2] = {0x20,0x1E};
	u8 idata PtCLR[2] = {0x20,0x20};


	//按键分配
	switch(msg_Keys)
	{
		case 1:
		{
			if(F_HourMode&&F_AMPM&&uHOUR!=12) uHOUR+=12;
			Clock_SetTime(uYEAR,uMONTH,uDATE,uHOUR,uMINUTE,uSECOND,F_HourMode);
			msg_Keys = 8;
			F_Process = 121;
			break;
		}
		case 2:
		{
			if(F_Selected == 1)F_Selected = 7;
			else F_Selected--;
			msg_Keys = 8;
			break;
		}

		case 3:
		{
			if(F_Selected == 7)F_Selected = 1;
			else F_Selected++;
			msg_Keys = 8;
			break;
		}

		case 4:
		{
		    //------------------------------------------------------------
			switch(F_Selected)
			{
				case 1:
				{
					if(uMONTH == 1)uMONTH = 12;
					else if(uMONTH == 0)uMONTH = 9;
					else uMONTH --;
					break;
				}

				case 2:
				{

					if(uDATE == 1)
					{

						if(uMONTH == 2)
						{
							if(!(uYEAR%4))uDATE = 29;
							else uDATE = 28;
						}

						else if(uMONTH == 4||uMONTH == 6||uMONTH == 9||uMONTH == 11)uDATE = 30;
						else uDATE = 31;

					}
					else if(uDATE == 0)uDATE = 9;	
					else uDATE--;	
					break;		
				}
					
				case 3: 
				{
					if(!uYEAR)uYEAR = 99;
					else uYEAR --;
					break;
				}
				case 4: 
				{
					if(!F_HourMode&&!uHOUR) uHOUR = 23;
					else if(F_HourMode&&!uHOUR&&!F_AMPM) uHOUR = 11;
					else if(F_HourMode&&uHOUR == 1&&F_AMPM) uHOUR = 12;
					else if(F_HourMode&&!uHOUR&&F_AMPM) uHOUR = 1;
					else uHOUR --;
					break;
				}
				case 5: 
				{
					if(!uMINUTE)uMINUTE = 59;
					else uMINUTE --;
					break;
				}
				case 6: 
				{
					if(!uSECOND)uSECOND = 59;
					else uSECOND --;
					break;
				}
				case 7: 
				{
					if(F_HourMode&&F_AMPM) F_AMPM = 0;
					else if(F_HourMode &&(!F_AMPM)) F_HourMode = 0;
					else if(!F_HourMode) 
					{
						F_HourMode = 1;
						F_AMPM = 1;
					}
					else 
					{
						F_HourMode = 0;
					}
						 
					break;
				}
				default:;
			}
			//-------------------------------------------------------------
			msg_Keys = 8;
			break;
		}

		case 5:
		{
			//------------------------------------------------------------
			switch(F_Selected)
			{
				case 1:
				{	
					if(uMONTH == 12)uMONTH = 1;
					else uMONTH++;
					break;		
				}
		
				case 2:
				{
					if(uMONTH == 2)
					{
						if(!(uYEAR%4))
						{
							if(uDATE == 29)uDATE = 1;
							else uDATE++;
						}
						else 
						{
							if(uDATE == 28)uDATE = 1;
							else if(uDATE == 29)uDATE = 1;
							else uDATE++;
						}
					}
					else if(uMONTH==4||uMONTH==6||uMONTH==9||uMONTH==11)
					{
						if(uDATE == 30) uDATE = 1;
						else uDATE++;
					}
					else
					{
						if(uDATE == 31)uDATE = 1;
						else uDATE++;
					}
						
					break;
				}	
					
				case 3: 
				{
					if(uYEAR == 99)uYEAR = 0;
					else uYEAR++;
					break;
				}
				case 4: 
				{
					if(!F_HourMode&&uHOUR==23)uHOUR = 0;
					else if(F_HourMode&&!F_AMPM&&uHOUR==11)uHOUR=0;
					else if(F_HourMode&&F_AMPM&&uHOUR==12)uHOUR=1;
					else if(F_HourMode&&!F_AMPM&&uHOUR==12)uHOUR=11;
					else uHOUR++;
					break;
				}
				case 5: 
				{
					if(uMINUTE==59)uMINUTE=0;
					else uMINUTE++;
					break;
				}
				case 6: 
				{
					if(uSECOND==59)uSECOND=0;
					else uSECOND++;
					break;
				}
				case 7: 
				{
					if(!F_HourMode)
					{
						F_HourMode = 1;
						F_AMPM = 0;
					}
					else if(F_HourMode&&!F_AMPM)
					{
						F_AMPM = 1;
					}
					else F_HourMode = 0;
					break;
				}
				default:;
			}
			//-------------------------------------------------------------
			msg_Keys = 8;
			break;
		}	
			
		case 6: 
		{
			F_Process = 1;
			delayms(100);
			msg_Keys = 8;
			break;
		}
		case 7: 
		{
			F_Process = 0;
			delayms(100);
			msg_Keys = 8;
			break;
		}
		default:;
	}

	//指针显示
	switch(F_Selected)
	{
		case 1:
		{
			LCD_NLet(0x90,PtRight,1);
			LCD_NLet(0x92,PtCLR,1);	
			LCD_NLet(0x94,PtCLR,1);	
			LCD_NLet(0x99,PtCLR,1);	
			LCD_NLet(0x9B,PtCLR,1);	
			LCD_NLet(0x9C,PtCLR,1);	
			LCD_NLet(0x9E,PtCLR,1);
			break;		
		}

		case 2:
		{
			LCD_NLet(0x90,PtCLR,1);
			LCD_NLet(0x92,PtLeft,1);	
			LCD_NLet(0x94,PtCLR,1);	
			LCD_NLet(0x99,PtCLR,1);	
			LCD_NLet(0x9B,PtCLR,1);	
			LCD_NLet(0x9C,PtCLR,1);	
			LCD_NLet(0x9E,PtCLR,1);	
			break;
		}	
			
		case 3: 
		{
			LCD_NLet(0x90,PtCLR,1);
			LCD_NLet(0x92,PtCLR,1);	
			LCD_NLet(0x94,PtRight,1);	
			LCD_NLet(0x99,PtCLR,1);	
			LCD_NLet(0x9B,PtCLR,1);	
			LCD_NLet(0x9C,PtCLR,1);	
			LCD_NLet(0x9E,PtCLR,1);	
			break;
		}
		case 4: 
		{
			LCD_NLet(0x90,PtCLR,1);
			LCD_NLet(0x92,PtCLR,1);	
			LCD_NLet(0x94,PtCLR,1);	
			LCD_NLet(0x99,PtRight,1);	
			LCD_NLet(0x9B,PtCLR,1);	
			LCD_NLet(0x9C,PtCLR,1);	
			LCD_NLet(0x9E,PtCLR,1);
			break;
		}
		case 5: 
		{
			LCD_NLet(0x90,PtCLR,1);
			LCD_NLet(0x92,PtCLR,1);	
			LCD_NLet(0x94,PtCLR,1);	
			LCD_NLet(0x99,PtCLR,1);	
			LCD_NLet(0x9B,PtLeft,1);	
			LCD_NLet(0x9C,PtCLR,1);	
			LCD_NLet(0x9E,PtCLR,1);
			break;
		}
		case 6: 
		{
			LCD_NLet(0x90,PtCLR,1);
			LCD_NLet(0x92,PtCLR,1);	
			LCD_NLet(0x94,PtCLR,1);	
			LCD_NLet(0x99,PtCLR,1);	
			LCD_NLet(0x9B,PtCLR,1);	
			LCD_NLet(0x9C,PtRight,1);	
			LCD_NLet(0x9E,PtCLR,1);
			break;
		}
		case 7: 
		{
			LCD_NLet(0x90,PtCLR,1);
			LCD_NLet(0x92,PtCLR,1);	
			LCD_NLet(0x94,PtCLR,1);	
			LCD_NLet(0x99,PtCLR,1);	
			LCD_NLet(0x9B,PtCLR,1);	
			LCD_NLet(0x9C,PtCLR,1);	
			LCD_NLet(0x9E,PtRight,1);
			break;
		}
		default:;
	}
	Clock_ShowSetT();

	
	
		
}







//------------------------成功消息提示---------------------//
void SYS_Success()
{
	u8 idata txt1[]="操作成功";	
	LCD_NChar(0x92,txt1,8);
}
//------------------------秒表------------------------//
void SYS_SSubM_TIM()
{  	
	u8 idata txt1[]="秒表";	
	LCD_NChar(0x80,txt1,4);
	switch(msg_Keys)
	{
		case 1:
		{
			if(!F_TIMState)
			{
				INT_TIM0_Cmd();
				TR0 = 1;
				F_TIMState = 2;	
			}
			else if(F_TIMState == 1)
			{
				 F_TIMState = 2;
			}
			else if(F_TIMState == 2)
			{
				 F_TIMState = 1;
			}
			delayms(100);
			msg_Keys = 8;
			break;
		}
		case 2:
		{
			if(F_TIMState == 1)
			{
				Clock_Tim_CLR();
			}
			msg_Keys = 8;
			break;
		}
		case 6: 
		{
			F_Process = 11;
			delayms(100);
			msg_Keys = 8;
			break;
		}
		case 7: 
		{
			F_Process = 0;
			delayms(100);
			msg_Keys = 8;
			break;
		}
		default:;
	}
	switch(F_TIMState)
	{
		case 1:
		{
			TR0 = 0;
			break;
		}
		case 2:
		{
			TR0 = 1;
			break;
		}
		
		default:;
	}
	Clock_Tim_Disp();
}

void SYS_SSubM_rvTIM()
{
	u8 idata txt1[]="定时器";
	u8 idata PtLeft[2] = {0x1E,0x20};
	u8 idata PtRight[2] = {0x20,0x1E};
	u8 idata PtCLR[2] = {0x20,0x20};
		
	LCD_NChar(0x80,txt1,6);
	switch(msg_Keys)
	{
		case 1:
		{
			if(!F_TIMState)
			{
				INT_TIM0_Cmd();
				TR0 = 1;
				F_TIMState = 2;
				//开始走时 
				LCD_NLet(0x99,PtCLR,1);
				LCD_NLet(0x9B,PtCLR,1);	
				LCD_NLet(0x9C,PtCLR,1);
			}
			else if(F_TIMState == 1)
			{
				 F_TIMState = 2;   //继续
			}
			else if(F_TIMState == 2)
			{
				 F_TIMState = 1;   //停止
			}
			delayms(100);
			msg_Keys = 8;
			break;
		}
		case 2:
		{
			if(F_TIMState == 1)
			{
				F_TIMState = 0;
			}
			else if(!F_TIMState)
			{
				if(F_Selected==1) F_Selected = 3;
				else F_Selected --;
			}
			msg_Keys = 8;
			break;
		}
		case 3:
		{
			if(F_Selected==3) F_Selected = 1;
			else F_Selected ++;
			msg_Keys = 8;
			break;
		}
		case 4:
		{
			switch(F_Selected)
			{
				case 1:
				{
					if(tHOUR == 0) tHOUR = 99;
					else tHOUR --;
					break;
				}
				case 2:
				{
					if(tMINUTE == 0) tMINUTE = 59;
					else tMINUTE --;
					break;
				}
				case 3:
				{
					if(tSECOND == 0) tSECOND = 59;
					else tSECOND --;
					break;
				}
			}
			msg_Keys = 8;
			break;
		}
		case 5:
		{
			switch(F_Selected)
			{
				case 1:
				{
					if(tHOUR == 99) tHOUR = 0;
					else tHOUR ++;
					break;
				}
				case 2:
				{
					if(tMINUTE == 59) tMINUTE = 0;
					else tMINUTE ++;
					break;
				}
				case 3:
				{
					if(tSECOND == 59) tSECOND = 0;
					else tSECOND ++;
					break;
				}
			}
			msg_Keys = 8;
			break;
		}
		case 6: 
		{
			F_Process = 11;
			delayms(100);
			msg_Keys = 8;
			break;
		}
		case 7: 
		{
			F_Process = 0;
			delayms(100);
			msg_Keys = 8;
			break;
		}
		default:;
	}

	switch(F_TIMState)
	{
		case 0:
		{
			//显示设置指针
			switch(F_Selected)
			{
				case 1:
				{
					LCD_NLet(0x99,PtRight,1);
					LCD_NLet(0x9B,PtCLR,1);	
					LCD_NLet(0x9C,PtCLR,1);	
					break;		
				}
		
				case 2:
				{
					LCD_NLet(0x99,PtCLR,1);
					LCD_NLet(0x9B,PtLeft,1);	
					LCD_NLet(0x9C,PtCLR,1);	
					break;
				}	
					
				case 3: 
				{
					LCD_NLet(0x99,PtCLR,1);
					LCD_NLet(0x9B,PtCLR,1);	
					LCD_NLet(0x9C,PtRight,1);
					break;
				}
				default:;
			}
			TR0 = 0;
			break;
		}
		case 1:
		{
			TR0 = 0;
			break;
		}
		case 2:
		{
			TR0 = 1;
			break;
		}
		
		default:;
	}
	Clock_rvTim_Disp();
}

void SYS_Timeup()
{
	 u8 idata txt1[]="时间到";
	 LCD_NChar(0x8A,txt1,6);
	 TR0 = 0;
	 F_TIMState = 0;
	 Clock_Tim_CLR();
	 AlarmBeep = 1;
	 if(msg_Keys>=1&&msg_Keys<=7)
	 {
	 	F_Process = 0;
	 }
}

void SYS_SSubM_SetAlarm()
{	
	u8 idata title[] = "闹钟设置";
	u8 idata PtLeft[2] = {0x1E,0x20};
	u8 idata PtRight[2] = {0x20,0x1E};
	u8 idata PtCLR[2] = {0x20,0x20};
	u8 idata sign[2];
	u8 idata signCLR[2];
	sign[0] = 0x20;
	sign[1] = 0x10;
	signCLR[0] = 0x20;
	signCLR[1] = 0x20;

	
	LCD_NChar(0x80,title,8);		

	switch(msg_Keys)
	{
		case 1:
		{
			F_Process = 0;
			msg_Keys = 8;
			break;
		}
		case 2:
		{
		
			if(F_Selected==1) F_Selected = 3;
			else F_Selected --;
			msg_Keys = 8;
			break;
			
		}
		case 3:
		{
			if(F_Selected==3) F_Selected = 1;
			else F_Selected ++;
			msg_Keys = 8;
			break;
		}
		case 4:
		{
			switch(F_Selected)
			{
				case 1:
				{
					if(aHOUR == 0) aHOUR = 23;
					else aHOUR --;
					break;
				}
				case 2:
				{
					if(aMINUTE == 0) aMINUTE = 59;
					else aMINUTE --;
					break;
				}
				case 3:
				{
					F_AlarmEN = ~F_AlarmEN;
				}
			}
			msg_Keys = 8;
			break;
		}
		case 5:
		{
			switch(F_Selected)
			{
				case 1:
				{
					if(aHOUR == 23) aHOUR = 0;
					else aHOUR ++;
					break;
				}
				case 2:
				{
					if(aMINUTE == 59) aMINUTE = 0;
					else aMINUTE ++;
					break;
				}
				case 3:
				{
					F_AlarmEN = ~F_AlarmEN;
				}
			}
			msg_Keys = 8;
			break;
		}
		case 6: 
		{
			F_Process = 11;
			delayms(100);
			msg_Keys = 8;
			break;
		}
		case 7: 
		{
			F_Process = 0;
			Clock_Alarm_CLR();
			delayms(100);
			msg_Keys = 8;
			break;
		}
		default:;
	}

	
	switch(F_Selected)
	{
		case 1:
		{
			LCD_NLet(0x9A,PtRight,1);
			LCD_NLet(0x9C,PtCLR,1);
			LCD_NLet(0x90,PtCLR,1);	
			break;		
		}
		
		case 2:
		{
			LCD_NLet(0x9A,PtCLR,1);	
			LCD_NLet(0x9C,PtLeft,1);
			LCD_NLet(0x90,PtCLR,1);		
			break;
		}	
					
		case 3: 
		{
			LCD_NLet(0x9A,PtCLR,1);	
			LCD_NLet(0x9C,PtCLR,1);
			LCD_NLet(0x90,sign,1);	
			break;
		}
		default:;
	}
	Clock_Alarm_Disp();
}

void SYS_Info()
{
	 u8 idata txt1[] = "版本信息";
	 u8 idata txt2[4];
	 u8 idata txt3[8]={0x50,0x61,0x6E,0x65,0x76,0x69,0x6C,0x61};

	 txt2[0] = 0x76;
	 txt2[1] = 0x30;
	 txt2[2] = 0x2E;
	 txt2[3] = 0x39;

	 LCD_NChar(0x80,txt1,8);
	 LCD_NLet(0x8A,txt2,2);
	 LCD_NLet(0x9C,txt3,4);
	 switch(msg_Keys)
	{
		case 1:
		{
			F_Process = 0;
			msg_Keys = 8;
			break;
		}
		
		case 6: 
		{
			F_Process = 1;
			delayms(100);
			msg_Keys = 8;
			break;
		}
		case 7: 
		{
			F_Process = 0;
			Clock_Alarm_CLR();
			delayms(100);
			msg_Keys = 8;
			break;
		}
		default:;
	}

}