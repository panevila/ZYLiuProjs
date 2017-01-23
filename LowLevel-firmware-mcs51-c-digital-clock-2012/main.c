#include<reg52.h> 
#include "COMMON.h"
#include "SYS.h"
#include "IO.h"
#include "LCD12864.h"
#include "INT.h"



void main(void) 
{  
	SYS_Init();
	
	while(1)
	{
		SYS_Main();

	}
		
}