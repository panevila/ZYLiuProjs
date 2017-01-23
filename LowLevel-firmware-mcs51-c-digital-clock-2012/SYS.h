
#include<reg52.h>
#include<common.h>



extern flag		F_Timeup;
extern u16		F_Process;
extern flag		F_Alarm;
extern flag 	F_TMode;
extern flag		F_Return;
extern flag		F_AlarmEN;

extern u16		sysTimer;

extern u8		NumBit;
extern msg		msg_Keys;

void SYS_Main();
void SYS_Init();
void SYS_CLR();
void SYS_Pointer();
void SYS_Prep();

void SYS_Menu();
void SYS_Default();
void SYS_CLRScrOnce();

void SYS_SubM_Fts();
void SYS_SubM_Set();

void SYS_SSubM_TIM();
void SYS_SSubM_rvTIM();
void SYS_SSubM_SetAlarm();

void SYS_SV_Alarm();

void SYS_Success();
void SYS_Timeup();

void SYS_Info();