/* flag_000.c */
/*************************************************************************************************/
/* 10-29-2015*/ /* 000 */ /* @  */ /*  DaoHVN : Create new */
/*************************************************************************************************/
#include "flag.h"

/* OS Flag */
struct _job_rsv JOBRsv;
uchar8 d[5];
uint32 gloPo	= 0;
uint32 _gloPo	= 0;
uint32 velocity = 0;

uchar8 flag_JOB10ms  = 0;
uchar8 val      = 0;
uchar8 _val     = 0;

uint16 servoMode = 0;
uint16 escMode  = 0;
uint16 escVal	= 0;
uint16 svVal	= 0;
uint16 _escVal  = 0;
uint16 _svVal   = 0;

volatile uint32 count = 0;

///* define common flag */
//struct _flag_list flag_list = {0U};
//
///* define common flag z */
//struct _flag_list_bk_z flag_list_bk, flag_list_bk_z = {0U};
