
#include <reg52.h>
#include "INT.h"
#include "SYS.h"
#include "LCDClock.h"
#include "IO.h"

u16 t0ms;
u16 t1ms;
u8 TIM_Modify;

//--------------------EX0--------------------//

void INT_EX0_Init()
{
	IE |= (1<<7)|(1<<0);
	TCON &= 0<<0;
}

void ISR_EX0() interrupt 0
{
	u8 idata A2,A1,A0;

	t1ms = 0;
	if(!Key_A2) A2 = 1;
	else A2 = 0;

	if(!Key_A1)	A1 = 1;
	else A1 = 0;

	if(!Key_A0)	A0 = 1;
	else A0 = 0;
	msg_Keys = (A2<<2)+(A1<<1)+A0;
	delayms(100);		
}

//--------------------TIM0--------------------//

void INT_TIM0_Init()
{
	IE |= (1<<7)|(1<<1);
	TMOD |= 0x01;
	t0ms = 0;
	TIM_Modify = 0x0A;
}


void INT_TIM0_Cmd()
{
	t0ms = 0;
	TH0 = 0xF8;
	TL0 = 0xCD + TIM_Modify;
	TCON |= 1<<4;
}

void ISR_TIM0() interrupt 1
{
	TH0 = 0xF8;
	TL0 = 0xCD + TIM_Modify;
	if(!F_TMode)
	{
		if(t0ms>10)
		{
			if(tSECD100==99)
			{
				if(tSECOND == 59)
				{
					if(tMINUTE == 59)
					{
						if(tHOUR == 99)tHOUR = 0;
						else tHOUR++;
						tMINUTE = 0;
					}
					else tMINUTE++;
					tSECOND = 0;
				}	
				else tSECOND++;	
				tSECD100 = 0;			
			}
			else tSECD100++;
			t0ms = 0;
		}
		else t0ms++; 
	}

	else if(F_TMode==1)
	{
		if(t0ms>=1000)
		{
			if(!tSECOND)
			{
				if(!tMINUTE)
				{
					if(!tHOUR)
					{
						F_Timeup = 1;
					}
					else tHOUR--;
					tMINUTE = 59;
				}
				else tMINUTE--;
				tSECOND = 59;
			}
			else tSECOND--;
			t0ms = 0;
		}
		else t0ms++;
	}	
}
void INT_TIM1_Init()
{
	IE |= (1<<7)|(1<<3);
	TMOD |= 0x10;
	t1ms = 0;
	TIM_Modify = 0x0A;
}


void INT_TIM1_Cmd()
{
	t1ms = 0;
	TH1 = 0xF8;
	TL1 = 0xCD + TIM_Modify;
	TCON |= 1<<6;
	TR1 = 1;
}

void ISR_TIM1() interrupt 3
{			  
	TH0 = 0xF8;
	TL0 = 0xCD + TIM_Modify;
	t1ms++;
	if(t1ms>=10000)
	{	
		t1ms = 0;
		F_Return = 1;
	}		
}