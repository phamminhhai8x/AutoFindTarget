// myOS_hard_init.h

#ifndef _MYOS_HARD_INIT_h
#define _MYOS_HARD_INIT_h
#if 1

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#endif
void hardware_init();
void variable_init();

#endif

