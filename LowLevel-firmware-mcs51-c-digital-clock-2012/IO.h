#include<reg52.h>
#include "COMMON.h"

/*------------------------------IO------------------------------*/

#define BF 						0x80
//URT
//sbit LCD_CS			=			P1^2;
sbit LCD_SID			=			P2^7;
sbit LCD_SCLK			=			P2^6;

//DS1302
sbit DS1302_IO		=			P1^3;
sbit DS1302_CS		=			P1^4;
sbit DS1302_SCLK		=			P1^2;

//key
sbit Key_A0			=			P2^3;
sbit Key_A1		   	=			P2^5;
sbit Key_A2			=			P2^4;

sbit AlarmBeep		=			P1^1;