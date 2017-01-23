
#include<reg52.h>
#include "LCD12864.h"
#include "COMMON.h"
#include "LCDClock.h"
#include "DS1302.h"
#include "IO.h"
#include "INT.h"
#include "SYS.h"
					
flag F_HourMode;
flag F_AMPM;

u8 YEAR;
u8 MONTH;
u8 DATE;
u8 DAY;

u8 HOUR;
u8 MINUTE;
u8 SECOND;

u8 uYEAR;
u8 uMONTH;
u8 uDATE;

u8 uHOUR;
u8 uMINUTE;
u8 uSECOND;

u8 uSECD100;


u8 tHOUR;
u8 tMINUTE;
u8 tSECOND;

u8 tSECD100;

u8 aHOUR;
u8 aMINUTE;

u8 cHOUR;
u8 cMINUTE;


flag F_uHourMode;
flag F_uAMPM;


void Clock_DispDate(u8 year,u8 month,u8 date)
{
	u8 idata datet[10];

	datet[0] = 0x30+(month/10);	
	datet[1] = 0x30+(month%10);

	datet[2] = 0x2F;				 

	datet[3] = 0x30+(date/10);
	datet[4] = 0x30+(date%10);

	datet[5] = 0x2F;

	datet[6] = 0x32;
	datet[7] = 0x30;

	datet[8] = 0x30+(year/10);
	datet[9] = 0x30+(year%10);


   
	LCD_NLet(0x80,datet,5);	
}

void Clock_DispTime(u8 hour,u8 min,u8 sec)
{
	u8 idata time[8];
	u8 idata AMPM[2];
	
	time[0] = 0x30+(hour/10);
	if(!(hour/10))	time[0] = 0x20;	
	time[1] = 0x30+(hour%10);

	time[2] = 0x3A;				 

	time[3] = 0x30+(min/10);
	time[4] = 0x30+(min%10);

	time[5] = 0x3A;

	time[6] = 0x30+(sec/10);;
	time[7] = 0x30+(sec%10);

	LCD_NLet(0x89,time,4);
	
	if(F_HourMode)
	{	
		AMPM[1] = 0x4D;
		if(F_AMPM)
			AMPM[0] = 0x50;
		else
			AMPM[0] = 0x41;
	}
	else
	{
		AMPM[0] = 0x20;
		AMPM[1] = 0x20;
	}
	LCD_NLet(0x8E,AMPM,1);
}


void Clock_SetTime(u8 year,u8 month,u8 date,u8 hour,u8 min,u8 sec,u8 mode)
{	
	u8 idata i;
	F_HourMode = mode;
	
	for(i=0;i<2;i++)
	{
		SECOND = Tf_H_BCD(sec);
		MINUTE = Tf_H_BCD(min);
		if(!F_HourMode)
		{
			HOUR = Tf_H_BCD(hour);
		}
		else
		{
			if(hour>12)
			{
				hour -= 12;
				HOUR = Tf_H_BCD(hour)|0xA0;
				F_AMPM = 1;
				hour += 12;
			}
			else
			{
				HOUR = Tf_H_BCD(hour)|0x80;
				F_AMPM = 0;
			}
		
		}

		YEAR = Tf_H_BCD(year);
		MONTH = Tf_H_BCD(month);
		DATE = Tf_H_BCD(date);

	
		DS1302_WR(0x8E,0x00);	
		DS1302_WR(0x80,0x80);
		DS1302_WR(0x80,SECOND);
		DS1302_WR(0x82,MINUTE);
		DS1302_WR(0x84,HOUR);
		DS1302_WR(0x86,DATE);
		DS1302_WR(0x88,MONTH);
		DS1302_WR(0x8C,YEAR);
		DS1302_WR(0x8E,0x80);
	}
			
}
void Clock_UpdateTime()
{
	SECOND = DS1302_RD(0x81);
	MINUTE = DS1302_RD(0x83);
	HOUR = DS1302_RD(0x85);	
	F_AMPM = ((HOUR&0x20)>>5)&0x01;	

	DATE = DS1302_RD(0x87);
	MONTH = DS1302_RD(0x89);
	YEAR = DS1302_RD(0x8D);			
}
void Clock_ShowTime()
{
	if(!F_HourMode)
		Clock_DispTime(Tf_BCD_H(HOUR),Tf_BCD_H(MINUTE),Tf_BCD_H(SECOND));
	else
	{
		u8 idata hour12;
		hour12 = Tf_BCD_H(HOUR&0x1f);
		Clock_DispTime(hour12,Tf_BCD_H(MINUTE),Tf_BCD_H(SECOND));
	}	
}

void Clock_ShowDate()
{
	Clock_DispDate(Tf_BCD_H(YEAR),Tf_BCD_H(MONTH),Tf_BCD_H(DATE));
}

void Clock_ShowSetT()
{

	Clock_DispTime(uHOUR,uMINUTE,uSECOND);
	Clock_DispDate(uYEAR,uMONTH,uDATE);
}

void Clock_GetForSet()
{
	uYEAR = Tf_BCD_H(YEAR);
	uMONTH = Tf_BCD_H(MONTH);
	uDATE = Tf_BCD_H(DATE);
	if(!F_HourMode)uHOUR = Tf_BCD_H(HOUR);
	else uHOUR = Tf_BCD_H(HOUR&0x1f);
	uMINUTE = Tf_BCD_H(MINUTE);
	uSECOND = Tf_BCD_H(SECOND);
}


void Clock_Tim_CLR()
{
	t0ms = 0;
	tSECD100 = 0;
	tSECOND = 0;
	tMINUTE = 0;
	tHOUR = 0;
}
void Clock_Tim_Disp()
{
	u8 idata time[12];
	time[0] = 0x30+(tHOUR/10);
	if(!(tHOUR/10))	time[0] = 0x20;	
	time[1] = 0x30+(tHOUR%10);
	time[2] = 0x3A;				 
	time[3] = 0x30+(tMINUTE/10);
	time[4] = 0x30+(tMINUTE%10);
	time[5] = 0x3A;
	time[6] = 0x30+(tSECOND/10);
	time[7] = 0x30+(tSECOND%10);
	time[8] = 0x3A;
	time[9] = 0x30+(tSECD100/10);
	time[10] = 0x30+(tSECD100%10);
	time[11] = 0x20;

	LCD_NLet(0x89,time,6);
}

void Clock_rvTim_Disp()
{
	u8 idata time[8];
	time[0] = 0x30+(tHOUR/10);
	if(!(tHOUR/10))	time[0] = 0x20;	
	time[1] = 0x30+(tHOUR%10);
	time[2] = 0x3A;				 
	time[3] = 0x30+(tMINUTE/10);
	time[4] = 0x30+(tMINUTE%10);
	time[5] = 0x3A;
	time[6] = 0x30+(tSECOND/10);
	time[7] = 0x30+(tSECOND%10);
	LCD_NLet(0x89,time,4);
}

void Clock_Alarm_Disp()
{
	u8 idata time[6];
	u8 idata status[] = "闹钟状态";
	u8 idata aEnable[] = "启用";
	u8 idata aDisable[] = "禁用";
	time[0] = 0x30+(aHOUR/10);
	if(!(aHOUR/10))	time[0] = 0x20;	
	time[1] = 0x30+(aHOUR%10);
	time[2] = 0x3A;				 
	time[3] = 0x30+(aMINUTE/10);
	time[4] = 0x30+(aMINUTE%10);
	time[5] = 0x20;
	LCD_NLet(0x8A,time,3);
	LCD_NChar(0x91,status,8);
	if(F_AlarmEN)LCD_NChar(0x96,aEnable,4);
	else LCD_NChar(0x96,aDisable,4);
}

void Clock_Alarm_CLR()
{
	aHOUR = 0;
	aMINUTE = 0;
	F_AlarmEN = 0;
}

void Clock_Alarm_Chk()
{
	u8 idata temp;
	if(!F_HourMode)cHOUR = Tf_BCD_H(HOUR);
	else
	{
		temp = Tf_BCD_H(HOUR&0x1f);
		if(F_AMPM&&temp<12) cHOUR = temp+12;
		else cHOUR = temp;
	}
	cMINUTE = Tf_BCD_H(MINUTE);
}