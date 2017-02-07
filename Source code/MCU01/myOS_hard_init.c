// 
// 
// 

#include "myOS_hard_init.h"

void hardware_init(){
	IO_init();
	timer_init();
}

void variable_init(){

}


void IO_init(){
	//DDRD |= _BV(2); 
	//DDRD |= _BV(4); 
 //   DDRD &= ~_BV(3); 
	//DDRD &= ~_BV(5); 
	pinMode(A0, OUTPUT);
	pinMode(A2, OUTPUT);
	pinMode(A4, OUTPUT);
	
	pinMode(A1, INPUT);
	pinMode(A3, INPUT);
	pinMode(A5, INPUT);
}

void timer_init(){
	
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;
  
  OCR2A = 249;
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 64 prescaler
  TCCR2B |= (1 << CS22);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
}
