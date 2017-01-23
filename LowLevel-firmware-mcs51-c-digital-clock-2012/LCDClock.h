extern u8 YEAR;
extern u8 MONTH;
extern u8 DATE;

extern u8 HOUR;
extern u8 MINUTE;
extern u8 SECOND;

extern flag F_HourMode;
extern flag F_AMPM;


extern u8 uYEAR;
extern u8 uMONTH;
extern u8 uDATE;

extern u8 uHOUR;
extern u8 uMINUTE;
extern u8 uSECOND;

extern u8 uSECD100;

extern u8 tHOUR;
extern u8 tMINUTE;
extern u8 tSECOND;

extern u8 tSECD100;


extern u8 aHOUR;
extern u8 aMINUTE;

extern u8 cHOUR;
extern u8 cMINUTE;




void Clock_DispDate(u8 year,u8 month,u8 date);
void Clock_DispTime(u8 hour,u8 min,u8 sec);
void Clock_SetTime(u8 year,u8 month,u8 date,u8 hour,u8 min,u8 sec,u8 mode);
void Clock_UpdateTime();
void Clock_ShowTime();
void Clock_ShowDate();

void Clock_ShowSetT();
void Clock_GetForSet();


void Clock_Tim_CLR();
void Clock_Tim_Disp();
					
void Clock_rvTim_Disp();

void Clock_Alarm_Disp();
void Clock_Alarm_CLR();
void Clock_Alarm_Chk();
