#include "COMMON.h"
#include<reg52.h> 
#include "INT.h"


void delayms(u16 t)
{
	int i;
	int j;
	for(i=0;i<t;i++)
		for(j=0;j<246;j++);
}

u8 Tf_BCD_H(u8 BCDdata)
{
	u8 temp,Hdata;
	temp = (BCDdata>>4)&0x0f;
	BCDdata &= 0x0f; 
	Hdata = BCDdata+temp*10;
	return Hdata;
}

u8 Tf_H_BCD(u8 HEXdata)
{
	u8 bcdH,bcdL;
	bcdH = ((HEXdata/10)<<4)&0xf0;
	bcdL = HEXdata%10;
	bcdL = bcdH+bcdL;
	return bcdL; 
}

void TIM_Delayms(u16 t)
{
	TR0 = 0;
	INT_TIM0_Cmd();
	while(1)
	{
		if(t0ms > t)break;
	}
}
