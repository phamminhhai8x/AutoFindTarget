// myOS.h

#ifndef _MYOS_h
#define _MYOS_h
#if 0
#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#endif
// Include Dictionary
//#include "Dictionary\Header\header.h"
//#include "Dictionary\Flag\flag.h"
//#include "Dictionary\Predefine\Predefine.h"

#include "JOBS\OS_JOBS.h"
#include <Wire.h>

void OS_control(void);

#endif

