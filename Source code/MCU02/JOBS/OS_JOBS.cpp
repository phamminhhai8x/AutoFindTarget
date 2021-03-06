// 
// 
// 
#include <avr/io.h>
#include <util/delay.h>

#include "OS_JOBS.h"

#if 1
const int SPI_CS_PIN = 10;
MCP_CAN _CAN(SPI_CS_PIN);

void CANReceive() {
	uchar8 len = 0;
	uchar8 buf[8];
	uchar8 stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	if (CAN_MSGAVAIL == _CAN.checkReceive()) {
		_CAN.readMsgBuf(&len, buf);
		uchar8 canId = _CAN.getCanId();
		d[0] = buf[0];
		d[1] = buf[1];
		d[2] = buf[2];
		d[3] = buf[3];
		d[4] = canId;
	}
	_CAN.sendMsgBuf(0x00, 0, 8, stmp);
}
#endif

#if 1
void calVel() {
	gloPo = count;
	if (gloPo != _gloPo)
		velocity = (gloPo - _gloPo) * 200 * 3.14 * 2 * 3.2/200;
	else 
		velocity = 0;
	_gloPo = gloPo;
}
#endif

#if 1
void counter() {
	val = digitalRead(5);
	if (!val && val != _val)
		count++;
	_val = val;
}
#endif

/*********JOBS 01ms**********/
#if NO_JOB_01MS != 0
static void (*const JOBS01ms[])() =
{
	&counter
};
#endif

/*********JOBS 10ms**********/
#if NO_JOB_10MS != 0
static void (*const JOBS10ms[])() =
{
	&CANReceive
};
#endif

/*********JOBS 100ms**********/
#if NO_JOB_100MS != 0
static void (*const JOBS100ms[])() =
{

};
#endif

/*********JOBS 200ms**********/
#if NO_JOB_200MS != 0
static void (*const JOBS200ms[])() =
{
	&calVel
};
#endif

/*********JOBS APP**********/
#if NO_JOB_APP != 0
static void (*const JOBSAPP[])() =
{

};
#endif

/********************************************************************/
/* Function			: execute_JOB									*/
/* Description		: Call all functions based on JOB mode			*/
/* Argument			: void											*/
/* Return			: void											*/
/********************************************************************/
void execute_JOB(void)
{
	uchar8 cUC8_indexArr; /* loop counter */

	if (TSTBIT(fUC8_JOBRunning)) /* IO JOB is running now! */
	{
		/* nothing if have JOB running */
	}
	else
	{
		SETBIT(fUC8_JOBRunning);
		do
		{
			/* Check flag fJOB01ms = 1 */
			if ((fUC8_JOB01ms != OFF))	
			{
#if NO_JOB_01MS != 0
				for (cUC8_indexArr = 0U; cUC8_indexArr < NO_JOB_01MS; cUC8_indexArr++)
				{
					(*JOBS01ms[cUC8_indexArr])();			/* Call all JOB 01ms*/
				}
#endif
				CLRBIT(fUC8_JOB01ms);						/* Clear flag fJOB01ms */
			}
			else if ((fUC8_JOB10ms != OFF))					/* Check flag fJOB10ms = 1 */
			{
#if NO_JOB_10MS != 0
				for (cUC8_indexArr = 0U; cUC8_indexArr < NO_JOB_10MS; cUC8_indexArr++)
				{
					(*JOBS10ms[cUC8_indexArr])();			/* Call all JOB 10ms*/
				}
#endif
				CLRBIT(fUC8_JOB10ms);						/* Clear flag fJOB10ms */
			}
			else if (fUC8_JOB100ms != OFF)					/* Check flag fJOB100ms = 1 */
			{
#if NO_JOB_100MS != 0
				for (cUC8_indexArr = 0U; cUC8_indexArr < NO_JOB_100MS; cUC8_indexArr++)
				{
					(*JOBS100ms[cUC8_indexArr])();			/* Call all JOB 100ms*/
				}
#endif
				CLRBIT(fUC8_JOB100ms);						/* Clear flag fJOB100ms */
			}
			else if (fUC8_JOB200ms != OFF)					/* Check flag fJOB200ms = 1 */
			{
#if NO_JOB_200MS != 0
				for (cUC8_indexArr = 0U; cUC8_indexArr < NO_JOB_200MS; cUC8_indexArr++)
				{
					(*JOBS200ms[cUC8_indexArr])();			/* Call all JOB 200ms*/
				}
#endif
				CLRBIT(fUC8_JOB200ms);						/* Clear flag fJOB200ms */
			}
			else if (fUC8_JOBApp != OFF)					/* Check flag fUC8_JOBApp = 1 */
			{
#if NO_JOB_APP != 0
				for (cUC8_indexArr = 0U; cUC8_indexArr < NO_JOB_APP; cUC8_indexArr++)
				{
					(*JOBSAPP[cUC8_indexArr])();			/* Call all JOB App*/
				}
#endif
				CLRBIT(fUC8_JOBApp);						/* Clear flag fUC8_JOBApp */
			}
			else
			{
				CLRBIT(fUC8_JOBRunning);					/* Clear flag fJOBRunning */
			}
		}while(TSTBIT(fUC8_JOBRunning));
	}
} /* End of execute_JOB() */
