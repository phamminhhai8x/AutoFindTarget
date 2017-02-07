
#include "myOS.h"

#define TIME_COUNT01MS	01U
#define TIME_COUNT10MS	10U
#define TIME_COUNT100MS	100U
#define TIME_COUNT200MS	200U
#define TIME_COUNT0MS	0U

uchar8 cUC8_Count01ms  = 0;
uchar8 cUC8_Count10ms  = 0;
uchar8 cUC8_Count100ms = 0;
uchar8 cUC8_Count200ms = 0;

/********************************************************************/
/* Function			: OS_control									*/
/* Description		: set flag to run JOB in JOB mode 				*/
/* Argument			: void											*/
/* Return			: void										 	*/
/********************************************************************/
void OS_control(void)
{
	cUC8_Count01ms++;
	cUC8_Count10ms++;
	cUC8_Count100ms++;
	cUC8_Count200ms++;
	if (cUC8_Count01ms == TIME_COUNT01MS)	/* every 01ms */
	{
	   	SETBIT(fUC8_JOB01ms);				/* set 01msJOB Flag */
		cUC8_Count01ms = TIME_COUNT0MS;
	}

	if (cUC8_Count10ms == TIME_COUNT10MS)	/* every 10ms */
	{
	   	SETBIT(fUC8_JOB10ms);				/* set 10msJOB Flag */
		cUC8_Count10ms = TIME_COUNT0MS;
		flag_JOB10ms = 1;
	}

	if (cUC8_Count100ms == TIME_COUNT100MS) /* every 100ms */
	{
	   	SETBIT(fUC8_JOB100ms);				/* set 100msJOB Flag */
		cUC8_Count100ms = TIME_COUNT0MS;
	}

	if (cUC8_Count200ms == TIME_COUNT200MS) /* every 200ms */
	{
	   	SETBIT(fUC8_JOB200ms);				/* set 200msJOB Flag */
		cUC8_Count200ms = TIME_COUNT0MS;
	}
	execute_JOB();
}
