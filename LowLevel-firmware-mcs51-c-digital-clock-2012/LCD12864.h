#include "COMMON.h"

void LCD_SendU8(u8 u8Data);
u8 LCD_RetU8();
void LCD_WaitBF();
void LCD_WRCmd(u8 u8Cmd);
void LCD_WRDat(u8 u8Dat);

void LCD_Init();
void LCD_CLR();


void LCD_NLet(u8 ads, u8* table, u8 n);
void LCD_NChar(u8 ads, u8* table, u8 n);