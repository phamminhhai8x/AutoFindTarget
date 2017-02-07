/*
 Name:		myOS.ino
 Created:	3/3/2016 8:50:44 AM
 Author:	Fresher11
*/
// Include Dictionary

#include "Dictionary\Header\header.h"
#include "Dictionary\Flag\flag.h"
#include "Dictionary\Predefine\Predefine.h"
extern "C" {
// Include OS
#include "myOS.h"
#include "myOS_hard_init.h"
}

#if CANMODE!=0
#include <mcp_can.h>
#include <SPI.h>

const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin
#endif

uint16 detectColor(uint16 taosOutPin,uint16 *array);
uint16 detectColor2(uint16 taosOutPin,uint16 *array);
uint16 colorRead(int taosOutPin, int color, bool LEDstate);
uint16 smooth(uint16 data, float32 filterVal, float32 smoothedVal);
void   var_init();

uint32 duration  = 0;
uint32 duration1 = 0;
uint32 duration2 = 0;
uint32 duration3 = 0;

uint32 dFR_S = 0;
uint32 dFL_S = 0;
uint32 dFF_S = 0;

uchar8 dFF = 0;
uchar8 dFR = 0;
uchar8 dFL = 0;

uchar8 _dFF = 0;
uchar8 _dFR = 0;
uchar8 _dFL = 0;

uint16 i = 0;
uint32 cTime = 0;
uint32 aTime = 0;
uchar8 avoid = 0;

uint16 S0 = 3;
uint16 S1 = 4;
uint16 S2 = 5;
uint16 S3 = 6;
uint16 taosOutPin_left   = 7;
uint16 taosOutPin_front  = 8;
uint16 taosOutPin_right  = 9;

uint16 white_array_front[4];
uint16 white_array_right[4];
uint16 white_array_left[4];

// the setup function runs once when you press reset or power the board
void setup() {
	#if DEBUGMODE
	Serial.begin(9600);
	#endif
	delay(1000);
	cli();					// Disable Interrupts
	#if CANMODE != 0
	START_INIT:

	while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
    }
    Serial.println("CAN BUS Shield init ok!");

	/*
	int i;
	for (i = 0; i < 8;i++) {
		stmp[i] = 0;
	}*/
	#endif

	// Init OS, IO, Timer, Variable
	hardware_init();
	variable_init();
	TCS3200setup();
	var_init();
	sei();					// Enable Interrupts
	Serial.println("Ready!!!");
}


// the loop function runs over and over again until power down or reset
void loop() {
	dFF = distance1;

	uint16 a,b,c;
	a = 0;
	b = 0;
	c = 0;

#if DEFAULT_COLOR	
	a = detectColor(taosOutPin_left,  white_array_left);
	b = detectColor(taosOutPin_front, white_array_front);
	c = detectColor(taosOutPin_right, white_array_right);
#endif

#if AUTOFIND_WCOLOR
	a = detectColor2(taosOutPin_left,  white_array_left);
	b = detectColor2(taosOutPin_front, white_array_front);
	c = detectColor2(taosOutPin_right, white_array_right);
#endif

	//dFF_S = smooth(dFF, 0.6, dFF_S);

	if (dFF > 70) 
		stmp[0] = 0;
	else if (dFF > 0)
		stmp[0] = dFF;


	stmp[1] = a;
	stmp[2] = b;
	stmp[3] = c;

	if (millis() - cTime >= 10) {
		// send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
		cTime = millis();
		CAN.sendMsgBuf(0x70,0, 8, stmp);
	}

	#if 1
		Serial.print(stmp[0]);
		Serial.print("\t");
		Serial.print(stmp[1]);
		Serial.print("\t");
		Serial.print(stmp[2]);
		Serial.print("\t");
		Serial.print(stmp[3]);
		Serial.print("\t");
		Serial.println();
	#endif
}

/* Interrupt Vector for OS */
ISR (TIMER2_COMPA_vect) 
{
	OS_control();
}

void TCS3200setup(){
	//color mode selection
	pinMode(S2,OUTPUT); 
	pinMode(S3,OUTPUT); 
	pinMode(S0,OUTPUT); 
	pinMode(S1,OUTPUT); 

	//color response pin (only actual input from taos)
	pinMode(taosOutPin_left,  INPUT); 
	pinMode(taosOutPin_front, INPUT); 
	pinMode(taosOutPin_right, INPUT); 
}

/*****************************************************
**
** detectColor function
** Input:   taosOurPin, range of color to compare
** Output:  1: Match
            2: Not Match
**
*****************************************************/
uint16 detectColor(uint16 taosOutPin,uint16 *array){

  uint16 get_C[4];
  uint16 white  = colorRead(taosOutPin,0,1);
  uint16 red    = colorRead(taosOutPin,1,1);
  uint16 blue   = colorRead(taosOutPin,2,1);
  uint16 green  = colorRead(taosOutPin,3,1);

  get_C[0] = white;
  get_C[1] = red;
  get_C[2] = blue;
  get_C[3] = green;

  /*
  Serial.print(get_C[0]);
  Serial.print("\t");
  Serial.print(get_C[1]);
  Serial.print("\t");
  Serial.print(get_C[2]);
  Serial.print("\t");
  Serial.print(get_C[3]);
  Serial.println();
  */

  uint16 count;
  count = 0;

  for (int n = 0;n<3;n++)
  {
    if (get_C[n] < array[n])
      count++;
  }
  /*
  Serial.print(count);
  */
  if(count == 3)
    return 1;
  else
    return 0;
}
/*==================================================*/

/*****************************************************
**
** detectColor function
** Input:   taosOurPin, range of color to compare
** Output:  1: Match
            2: Not Match
**
*****************************************************/
uint16 detectColor2(uint16 taosOutPin,uint16 *array){

  uint16 get_C[4];
  uint16 white  = colorRead(taosOutPin,0,1);
  uint16 red    = colorRead(taosOutPin,1,1);
  uint16 blue   = colorRead(taosOutPin,2,1);
  uint16 green  = colorRead(taosOutPin,3,1);

  get_C[0] = white;
  get_C[1] = red;
  get_C[2] = blue;
  get_C[3] = green;

  uint16 count;
  count = 0;

  for (int n = 0;n<3;n++)
  {
    if (get_C[n] > array[n])
      count++;
  }
  /*
  Serial.print(count);
  */
  if(count == 3)
    return 1;
  else
    return 0;
}
/*==================================================*/

uint16 colorRead(int taosOutPin, int color, bool LEDstate){   
  //turn on sensor and use highest frequency/sensitivity setting
  taosMode(1);
  //setting for a delay to let the sensor sit for a moment before taking a reading.
  int sensorDelay = 5;

  //set the S2 and S3 pins to select the color to be sensed 
  if(color == 0){//white
    digitalWrite(S3, LOW);  //S3
    digitalWrite(S2, HIGH); //S2
//    delayMicroseconds(5);
//    digitalWrite(S2, LOW);
//    Serial.print(" w");
  }
  else if(color == 1){//red
    digitalWrite(S3, LOW); //S3
    digitalWrite(S2, LOW); //S2
    // Serial.print(" r");
  }
  else if(color == 2){//blue
    digitalWrite(S3, HIGH); //S3
//  delayMicroseconds(5);
//  digitalWrite(S3, LOW);
    digitalWrite(S2, LOW); //S2 
//  Serial.print(" b");
  }
  else if(color == 3){//green
    digitalWrite(S3, HIGH); //S3
    digitalWrite(S2, HIGH); //S2 
    // Serial.print(" g");
  }
  
  uint16 readPulse;

  // wait a bit for LEDs to actually turn on, as directed by sensorDelay var
  delay(sensorDelay);
  
  // now take a measurement from the sensor, timing a low pulse on the sensor's "out" pin
  readPulse = pulseIn(taosOutPin, LOW, 20000);
  
  //if the pulseIn times out, it returns 0 and that throws off numbers. just cap it at 80k if it happens
  if(readPulse < .1){
    readPulse = 80000;
  }

  // return the pulse value back to whatever called for it... 
  return readPulse;
}

// Operation modes area, controlled by hi/lo settings on S0 and S1 pins.
//setting mode to zero will put taos into low power mode. taosMode(0);

/*====================================================
====================================================*/

void taosMode(int mode){    
	
  if(mode == 1){
  //this will put in 1:1, highest sensitivity
  digitalWrite(S0, HIGH); //S0
  digitalWrite(S1, HIGH); //S1
  // Serial.println("m1:1m");
  
  }else if(mode == 2){
  //this will put in 1:5
  digitalWrite(S0, HIGH); //S0
  digitalWrite(S1, LOW); //S1
  //Serial.println("m1:5m");
  
  }else if(mode == 3){
  //this will put in 1:50
  digitalWrite(S0, LOW); //S0
  digitalWrite(S1, HIGH); //S1 
  //Serial.println("m1:50m");
  }    
  return;
}

void var_init() {
#if DEFAULT_COLOR
  white_array_front[0] = 10;
  white_array_front[1] = 30;
  white_array_front[2] = 20;
  white_array_front[3] = 30;

  white_array_right[0] = 10;
  white_array_right[1] = 30;
  white_array_right[2] = 20;
  white_array_right[3] = 30;

  white_array_left[0]  = 10;
  white_array_left[1]  = 30;
  white_array_left[2]  = 20;
  white_array_left[3]  = 30;
#endif

#if AUTOFIND_WCOLOR	
  white_array_front[0] = 10;
  white_array_front[1] = 30;
  white_array_front[2] = 20;
  white_array_front[3] = 30;

  white_array_right[0] = 10;
  white_array_right[1] = 30;
  white_array_right[2] = 20;
  white_array_right[3] = 20;

  white_array_left[0]  = 10;
  white_array_left[1]  = 30;
  white_array_left[2]  = 20;
  white_array_left[3]  = 30;
#endif
}

uint16 smooth(uint16 data,float32 filterVal,float32 smoothedVal) {

	if (filterVal > 1) {      // check to make sure param's are within range
		filterVal = .99;
	}
	else if (filterVal <= 0) {
		filterVal = 0;
	}

	smoothedVal = (data * (1 - filterVal)) + (smoothedVal  *  filterVal);

	return (uint16)smoothedVal;
}