
#include<reg52.h> 
#include "COMMON.h"
#include "DS1302.h"
#include "IO.h"


void DS1302_SendU8(u8 U8dat)
{
	u8 idata i;
	DS1302_SCLK=0;
	for(i=0;i<8;i++)
	{
		if(U8dat&0x01)
			DS1302_IO=1;
		else 
		    DS1302_IO=0;
		DS1302_SCLK=1;
		U8dat>>=1;
		DS1302_SCLK=0;
	}		
}

u8 DS1302_GetU8()
{
  	u8 idata i;
	u8 idata u8Dat;
	DS1302_SCLK=0;
	for(i=0;i<8;i++)
	{
		u8Dat>>=1;
		if(DS1302_IO)
			u8Dat|=0x80;
		else 
		    DS1302_IO&=0x7f; 		 		
		DS1302_SCLK=1;
		DS1302_SCLK=0;
	} 	
	return u8Dat;
}

void DS1302_WR(u8 REG, u8 dat)
{
	u8 idata temp;
	temp = dat;
	DS1302_CS=1;
	DS1302_SendU8(REG);
	DS1302_SendU8(dat);
	DS1302_CS=0;		
}
u8 DS1302_RD(u8 REG)
{
	u8 idata Dat;
	DS1302_CS=1;
	DS1302_SendU8(REG);
	Dat = DS1302_GetU8();
	DS1302_CS=0;
	return Dat;
}