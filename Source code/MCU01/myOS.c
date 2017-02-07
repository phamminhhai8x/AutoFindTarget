// 
// 
// 

// Include Dictionary
#include "Dictionary\Header\header.h"
#include "Dictionary\Flag\flag.h"
#include "Dictionary\Predefine\Predefine.h"
// Include OS
#include "myOS.h"
#include "JOBS\OS_JOBS.h"



#define TIME_COUNT10MS	10U
#define TIME_COUNT20MS	20U
#define TIME_COUNT50MS	50U
#define TIME_COUNT100MS	100U
#define TIME_COUNT0MS	0U

uchar8 cUC8_Count10ms = 0;
uchar8 cUC8_Count20ms = 0;
uchar8 cUC8_Count50ms = 0;
uchar8 cUC8_Count100ms = 0;

/********************************************************************/
/* Function			: OS_control									*/
/* Description		: set flag to run JOB in JOB mode 				*/
/* Argument			: void											*/
/* Return			: void										 	*/
/********************************************************************/
void OS_control(void)
{
	cUC8_Count10ms++;
	cUC8_Count20ms++;
	cUC8_Count50ms++;
	cUC8_Count100ms++;
	if (cUC8_Count10ms == TIME_COUNT10MS)	/* every 10ms */
	{
	   	SETBIT(fUC8_JOB10ms);				/* set 10msJOB Flag */
		cUC8_Count10ms = TIME_COUNT0MS;
	}
	if (cUC8_Count20ms == TIME_COUNT20MS)	/* every 20ms */
	{
	   	SETBIT(fUC8_JOB20ms);				/* set 20msJOB Flag */
		cUC8_Count20ms = TIME_COUNT0MS;
	}
	if (cUC8_Count50ms == TIME_COUNT50MS)	/* every 50ms */
	{
	   	SETBIT(fUC8_JOB50ms);				/* set 50msJOB Flag */
		cUC8_Count50ms = TIME_COUNT0MS;
	}
	if (cUC8_Count100ms == TIME_COUNT100MS) /* every 100ms */
	{
	   	SETBIT(fUC8_JOB100ms);				/* set 100msJOB Flag */
		cUC8_Count100ms = TIME_COUNT0MS;
	}
	execute_JOB();
}
