
#include<reg52.h> 
#include "COMMON.h"
#include "LCD12864.h"
#include "IO.h"

void LCD_SendU8(u8 u8Data)
{
	u8 idata i;
    for(i=0;i<8;i++)
    {
          LCD_SCLK = 0;
          u8Data=u8Data<<1;        
          LCD_SID = CY;              
          LCD_SCLK = 1;
          LCD_SCLK = 0;
    }		
}
u8 LCD_RetU8()
{
	u8 idata i,u8H,u8L;
    u8H = 0;
    u8L = 0;
    for(i=0;i<8;i++)
    {
		
      	u8H=u8H<<1;
        LCD_SCLK = 0;
        LCD_SCLK = 1;            
        LCD_SCLK = 0;
        if(LCD_SID)u8H = 1;
    }
    for(i=0;i<8;i++)
    {
        u8L=u8L<<1;
		
        LCD_SCLK = 0;
        LCD_SCLK = 1;
        LCD_SCLK = 0;
        if(LCD_SID) u8L = 1;
    }
    return ((0xf0&u8H)+(0x0f&u8L));	
}

void LCD_WaitBF()
{
	LCD_SendU8(0xfc);
	while(0x80&LCD_RetU8());
}

void LCD_WRCmd(u8 u8Cmd)
{
	LCD_WaitBF();
	LCD_SendU8(0xf8);              
    LCD_SendU8(0xf0&u8Cmd);        
    LCD_SendU8(0xf0&u8Cmd<<4);		
}


void LCD_WRDat(u8 u8Dat)
{
	LCD_WaitBF();
	LCD_SendU8(0xfa);              
    LCD_SendU8(0xf0&u8Dat);        
    LCD_SendU8(0xf0&u8Dat<<4);
}


void LCD_Init()
{
	delayms(20);
	LCD_WRCmd(0x30); 
	delayms(20);
	LCD_WRCmd(0x06); 
	delayms(20);
	LCD_WRCmd(0x01); 
	delayms(20);
	LCD_WRCmd(0x0C);
	delayms(20); 			
	LCD_WRCmd(0x02); 
	delayms(20);
}

void LCD_CLR()
{
	u8 idata CLRtable[16] = {0x20,0x20,0x20,0x20,
					   0x20,0x20,0x20,0x20,
					   0x20,0x20,0x20,0x20,
					   0x20,0x20,0x20,0x20};
	LCD_NLet(0x80, CLRtable, 8);
	LCD_NLet(0x90, CLRtable, 8);
	LCD_NLet(0x88, CLRtable, 8);
	LCD_NLet(0x98, CLRtable, 8);
}


void LCD_NLet(u8 ads, u8* table, u8 n)
{
	u8 idata i;
	LCD_WRCmd(ads);
	for(i=0;i<n;i++)
	{
		LCD_WRDat(table[2*i]);
		LCD_WRDat(table[2*i+1]);
	}
}

void LCD_NChar(u8 ads, u8* table, u8 n)
{
	u8 idata i;
	LCD_WRCmd(ads);
	for(i=0;i<n;i++)
	{
		LCD_WRDat(table[i]);
	}	
}