/*
Name:		myOS.ino
Created:	3/3/2016 8:50:44 AM
Author:	Fresher104
*/
// Include Dictionary
#include <mcp_can_dfs.h>
#include <Wire.h>
#include <Servo.h>

//#include "Dictionary\Header\header.h"
//#include "Dictionary\Flag\flag.h"
//#include "Dictionary\Predefine\Predefine.h"

#include "myOS_hard_init.h"
#include "myOS.h"
#include "JOBS\MPU\MPU6050.h"

#if CANMODE
#include <mcp_can.h>
#include <SPI.h>

const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin
#endif

void counter();
void variables_init();
void MPU6050_init();
void servoEsc_init();
void CAN_Receive();
void tempBreak();
int  svMode_One(float angles);
int  controlEsc(int vel, int target, int tolenVal, int escVal);
float angleDifferential(float angle, float orgAngle);

Servo		sv,esc;
MPU6050		mpu;

// ================================================================
// ===                     VARIABLE SETUP                       ===
// ================================================================

uint16 i		= 0;
uint16 queue	= 0;
uchar8 csdelay  = 0;
uchar8 _csdelay = 0;
uint32 cTime	= 0;
uint32 xTime	= 0;
uint32 tTime	= 0;
uint32 _tTime	= 0;
uchar8 avoid	= 0;
uchar8 _avoid   = 0;
uint32 debugTimer = 0;

uchar8 aft_wc[8] = {3, 2, 1, 0, 0, 0, 0, 0};

uint16			carStatus, vel, turnDone;
float32			angle, _angle, exAngle, orgAngle;
uint32			distance, _distance, aodistance;
uint16			tempVar, staStatus;
static uint32	posi;

// MPU control/status vars
bool      dmpReady = false;		// set true if DMP init was successful
uint8_t   mpuIntStatus;			// holds actual interrupt status byte from MPU
uint8_t   devStatus;			// return status after each device operation (0 = success, !0 = error)
uint16_t  packetSize;			// expected DMP packet size (default is 42 bytes)
uint16_t  fifoCount;			// count of all bytes currently in FIFO
uint8_t   fifoBuffer[64];		// FIFO storage buffer
bool      stab = false;

// orientation/motion vars
Quaternion		q;				// [w, x, y, z]         quaternion container
VectorFloat		gravity;		// [x, y, z]            gravity vector
float			euler[3];       // [psi, theta, phi]    Euler angle container
static float	oldeuler[3];    // last values [yaw, pitch, roll]

// the setup function runs once when you press reset or power the board
// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================
void setup() {	
	servoEsc_init();

	#if DEBUGMODE
	Serial.begin(9600);
	#endif	
	Serial.println("Servo_Esc_Succesfull!");

	MPU6050_init();     // MPU6050_init()
	delay(500);
	cli();				// Disable Interrupts
	#if CANMODE
	//START_INIT:
		if (CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
		{
			//Serial.println("CAN BUS Shield init ok!");
		}
		else
		{
			Serial.println("CAN BUS Shield init fail");
			Serial.println("Init CAN BUS Shield again");
			delay(100);
			//goto START_INIT;
		}
	#endif
	delay(1000);
	// Init OS, IO, Timer, Variable
	variables_init();
	hardware_init();
	variable_init();
	sei();				// Enable Interrupts
	digitalWrite(7, LOW);
	Serial.println("Start");
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================
// the loop function runs over and over again until power down or reset
void loop() {
	posi = count;
	vel  = velocity;

	int sumX = 0;
	sumX = d[1] + d[2] + d[3];

	if (sumX == 3)
		digitalWrite(7, HIGH);

// ================================================================
// ===                     SYSTEMS SETUP                        ===
// ================================================================
	if (millis() - cTime > 18000) {
		if (staStatus == 0)
			staStatus = 1;
	}

	if (staStatus == 1 && orgAngle == 0) {
		orgAngle = angle;
	}
	
	if (staStatus == 1 && carStatus == 0) {
		carStatus = 1;
		servoMode = 1;
	}

// ================================================================
// ===						AVOID OBS					        ===
#if AVOID_OBS
	if ((escMode == 2) && ((d[0] <= 60) && (d[0] > 5)) && (avoid == 0)) {
		servoMode = 4;
		escMode   = 1;
		avoid     = 1;
		xTime     = millis();
		_distance = distance;
	}
	if ((servoMode == 4) && (avoid == 1) && (((distance - _distance) > 45) && ((distance - _distance) < 50))) {
		servoMode  = 1;
		escMode    = 4;
		aodistance = distance - _distance;
		_distance  = distance;
		xTime      = millis();
	}
	if ((servoMode == 1) && (avoid == 1) && (((distance - _distance) > 60) && ((distance - _distance) < 65))) {
		servoMode  = 5;		
		escMode    = 1;
		_distance  = distance;
		xTime      = millis();
	}
	if ((servoMode == 5) && (avoid == 1) && (((distance - _distance) > (aodistance + 10)) && ((distance - _distance) < (aodistance + 15)))) {
		servoMode  = 1;
		escMode    = 2;
		avoid      = 0;
		xTime      = 0;
	}
#endif
// ================================================================

// ================================================================
// ===						COLOR SENSOR				        ===
#if COLOR_SENSOR
	if ((carStatus == 1) && (servoMode == 1) && (sumX == 3)) {
		csdelay   = 1;
		//servoMode = 3;
		_distance = distance;
	}

	if (((distance - _distance) > 4) && (csdelay == 1) && (servoMode == 1)) {
		servoMode = 3;
		csdelay   = 0;
	}

#endif
// ================================================================

// ================================================================
// ===					AUTO FIND TARGET				        ===
#if AUTOFIND_TARGET
	if ((carStatus == 1) && (servoMode == 1) && (distance > TARGET) && ((distance - TARGET) < 10)) {
		servoMode = 3;
	}

	if ((carStatus == 1) && (servoMode == 1) && (distance > TARGET_2) && ((distance - TARGET_2) < 10)) {
		servoMode = 2;
	}

	if ((carStatus == 1) && (servoMode == 1) && (distance > TARGET_3) && ((distance - TARGET_3) < 10)) {
		servoMode = 2;
	}

	if ((carStatus == 1) && (servoMode == 1)  && (distance > TARGET_4) && ((distance - TARGET_4) < 10)) {
		servoMode = 2;
	}
#endif
// ================================================================

// ================================================================
// ===			  AUTO FIND TARGET WITH COLOR SENSOR			===
#if AUTOFIND_WCOLOR
	if ((escMode == 1) && (servoMode == 1) && (sumX == 3) && (aft_wc[queue] != 0)) {
		servoMode = aft_wc[queue];
		queue++;
	}
#endif
// ================================================================

	if (escMode == 0 && servoMode == 1) {
		escMode = 1;
		_tTime  = millis();
		xTime   = 0;
	}

	if (escMode == 1 && distance > 10) {
		escMode = 2;
		_tTime  = millis();
	}
#if 1
	if (d[0] < 15 && d[0] > 05) {
		escMode = 5;
		tempBreak();
	}
	else 
#endif // 0		
	if (distance > _TARGET) {
		escMode   = 5;
		tempBreak();
	}

	if (servoMode == 3 && turnDone == 1) {
		servoMode = 1;
		turnDone  = 0;
		orgAngle  = angle;
	}

	if (servoMode == 2 && turnDone == 1) {
		servoMode = 1;
		turnDone  = 0;
		orgAngle  = angle;
	}
// ================================================================
// ===					      MPU6050		                    ===
// ================================================================
#if 1	
if (flag_JOB10ms) {
	if (!dmpReady) return;                  // if programming failed, don't try to do anything
	mpuIntStatus = mpu.getIntStatus();  
	fifoCount = mpu.getFIFOCount();
// ================================================================
// ===					      MPU6050		                    ===
// ================================================================
	if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
		mpu.resetFIFO();
	} else if (mpuIntStatus & 0x02) 	{
		while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
		mpu.getFIFOBytes(fifoBuffer, packetSize);
		fifoCount -= packetSize;       
		// display Euler angles in degrees
		//
		mpu.dmpGetQuaternion(&q, fifoBuffer);
		mpu.dmpGetEuler(euler, &q);        

		angle = euler[0] * 180/M_PI;
		if (angle < 0)
			angle = 360 + angle;
	}
	flag_JOB10ms = 0;
// ================================================================
// ===	 				   MPU6050 END	 					    ===
// ================================================================
}
#endif
// ================================================================
// ===	 				 SERVO CONTROLLER	 				    ===
// === Mode 0: Waitting											===
// === Mode 1: Move straight									===
// === Mode 2: Turn left										===	
// === Mode 3: Turn right										===
// === Mode 4: Avoid obstacles									===
// === Mode 5: Avoid obstacles_tt								===
// ================================================================
	// so sanh goc lech vs goc orgi
	//
	_angle = angleDifferential(angle, orgAngle);
	
	switch(servoMode) {
		case 0 :
			svVal = 1500;
			break;
		case 1 :
			svVal = svMode_One(_angle);
			break;
		case 2 :
			if (millis() - xTime > 100) {
				xTime = millis();
				if (svVal > 1050)
					svVal = svVal - 75;
			}
			if (svVal < 1000)
				svVal = 1000;
			if ((_angle > 90) || (_angle < -90))
				turnDone  = 1;
			else if (((_angle > 60) || (_angle < -60)) && (svVal < 1300))
				svVal = svVal + 75;
			break;
		case 3 :
			if (millis() - xTime > 100) {
				xTime = millis();
				if (svVal < 1950)
					svVal = svVal + 75;
			}
			if (svVal > 2000)
				svVal = 2000;
			if ((_angle > 90) || (_angle < -90))
				turnDone = 1;
			else if (((_angle > 60) || (_angle < -60)) && (svVal > 1700))
				svVal = svVal - 75;
			break;
		case 4 :
			if (millis() - xTime > 50) {
				xTime = millis();
				if (svVal < 1950)
					svVal = svVal + 85;
			}
			if ((_angle > 65) || (_angle < -65))
				svVal = 1500;
			break;
		case 5 :
			if (millis() - xTime > 50) {
				xTime = millis();
				if (svVal > 1050)
					svVal = svVal - 85;
			}
			if ((_angle > 65) || (_angle < -65))
				svVal = 1500;
			break;
		default :
			svVal = 1500;
	}

// ================================================================
// ===	 				SERVO CONTROLLER END	 			    ===
// ================================================================

// ================================================================
// ===	 				  ESC CONTROLLER	 				    ===
// === Mode 0: Standby											===
// === Mode 1: Depa												===
// === Mode 2: Moving											===	
// === Mode 3: Turn												===
// === Mode 4: Avoid											===
// === Mode 5: Stop												===
// ================================================================
	switch(escMode) {
		case 0 :
			escVal = 1500;
			break;
		case 1 :
			if (millis() - tTime > 200) {
				tTime = millis();
				escVal = escVal + 10;
			}
			break;
		case 2 :
			if(millis() - _tTime >= 100) {
				_tTime = millis();
				escVal = controlEsc(vel, velTarget, toleranceVal, escVal);
				if ((servoMode == 2) || (servoMode == 3))
					escVal = escVal + 10;
			}
			break;
		case 3 :
			break;
		case 4 :
			escVal = 1599;
			if (vel < 35)
				escVal + 5;
			break;
		case 5 :
			escVal = 1500;
			break;
		default :
			escVal = 1500;
	}
// ================================================================
// ===	 			 	  ESC CONTROLLER END	 		 	    ===
// ================================================================
	if (posi > 0)
		distance = (posi + 1) * 3.14 * 2 * 3.2/5;
	
	// bao ve servo
	if (svVal > 2000)
		svVal = 2000;
	if (svVal < 1000)
		svVal = 1000;

	if (escVal > 1620)
		escVal = 1619;
	else if (escVal < 1380)
		escVal = 1380;

#if 1
	if ((millis() - debugTimer) > 50) {
		debugTimer = millis();
		Serial.print(orgAngle);
		Serial.print("\t");
		Serial.print(angle);
		Serial.print("\t");
		Serial.print(_angle);
		Serial.print("\t");	
		Serial.print(servoMode);
		Serial.print("\t");
		Serial.print(escMode);
		Serial.print("\t");
		Serial.print(d[0]);
		Serial.print("\t");
		Serial.print(svVal);
		Serial.print("\t");
		Serial.print(escVal);
		Serial.print("\t");
		Serial.print(distance);
		Serial.print("\t");
		Serial.print(vel);
		Serial.println();
	}
#endif

#if 0
	Serial.print(d[0]);
	Serial.print("\t");
	Serial.print(d[1]);
	Serial.print("\t");
	Serial.print(d[2]);
	Serial.print("\t");	
	Serial.print(d[3]);
	Serial.println();
#endif

#if 1
	esc.writeMicroseconds(escVal);
	sv.writeMicroseconds(svVal);
#endif
}

/* Interrupt Vector for OS */
ISR (TIMER2_COMPA_vect) 
{
	OS_control();
}

void servoEsc_init() {
	pinMode(SVPIN,  OUTPUT);
	pinMode(ESCPIN, OUTPUT);
	delay(500);
	sv.attach(SVPIN);
	esc.attach(ESCPIN);
	delay(500);
	sv.writeMicroseconds(1500);
	esc.writeMicroseconds(1500);
	delay(500);
}

void variables_init() {
	xTime     = 0;
	tTime     = 0;
	cTime     = 0;
	orgAngle  = 0;
	angle     = 0;
	_angle    = 0;
	exAngle   = 0;
	escVal    = 1500;
	_escVal   = 0;
	svVal     = 1500;
	_svVal    = 0;
	tempVar   = 0;
	staStatus = 0;
	posi      = 0;
	count     = 0;
	distance  = 0;
	_distance = 0;
	vel       = 0;
	carStatus = 0;
	servoMode = 0;
	turnDone  = 0;
	aodistance = 0;
}

float angleDifferential(float angle, float orgAngle) {
  float difAngle = angle - orgAngle;
  float _difAngle = 0;
  if ((difAngle > 0) && (difAngle < 180))
    _difAngle = -difAngle;
  else if ((difAngle > 0) && (difAngle > 180))
    _difAngle = 360 - difAngle;
  else if ((difAngle < 0) && (difAngle > -180))
    _difAngle = -difAngle;
  else if ((difAngle < 0) && (difAngle < -180))
    _difAngle = -(360 + difAngle);
  else _difAngle = 180;

  return _difAngle; 
}

int svMode_One(float angles) {
	int _svVal = 0;
	if (angles > 0.00) {
		if (angles < 2)         _svVal = 1500;   // 50
		else if (angles < 5)    _svVal = 1460;   // 50
		else if (angles < 7)    _svVal = 1420;   // 50
		else if (angles < 9)    _svVal = 1380;   // 50
		else if (angles < 11)   _svVal = 1340;   // 50
		else if (angles < 20)   _svVal = 1300;   // 50
		else if (angles >= 20)  _svVal = 1280;   // 50
	}
			  
	if (angles < 0.00) {
		if (angles > -2)        _svVal = 1500;
		else if (angles > -5)   _svVal = 1540;
		else if (angles > -7)   _svVal = 1580;
		else if (angles > -9)   _svVal = 1620;
		else if (angles > -11)  _svVal = 1660;
		else if (angles > -20)  _svVal = 1700;
		else if (angles <= -20) _svVal = 1720;
	}
	return _svVal;
}

#if 1
void MPU6050_init() {  
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties

  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();  

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

	// supply your own gyro offsets here, scaled for min sensitivity
	mpu.setXGyroOffset(220);
	mpu.setYGyroOffset(76);
	mpu.setZGyroOffset(-85);
	mpu.setZAccelOffset(1788); // 1688 factory default for my test chip


  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);
    
    mpuIntStatus = mpu.getIntStatus();
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      Serial.print(F("DMP Initialization failed (code "));
      Serial.print(devStatus);
      Serial.println(F(")"));
  }
}
#endif

int controlEsc(int vel, int target, int tolenVal, int escVal) {
	/*
	if ((vel - target) > 150)
		return escVal - 5;

	if (escVal < 1574)
		return 1575;
	if (vel > target && ((vel - target) < tolenVal)) {
		esc.writeMicroseconds(1380);
		return escVal + 0; 
	}
	else if (vel > target)
		return escVal - 1;

	if (target > vel && ((target - vel) < (tolenVal + 20)))
		return escVal + 0;
	else if (target > vel)
		return escVal + 5;
	*/

#if AVOID_OBS
	if (vel < 35)
		return escVal + 8;
	return 1594;
#endif

/*
#if AUTOFIND_TARGET	
	if (vel < 35)
		return escVal + 5;
	return 1588;			
#endif

#if COLOR_SENSOR
	if (vel < 35)
		return escVal + 5;
	return 1593;
#endif
*/
}

void tempBreak() {
	esc.writeMicroseconds(1500);
	delay(2);
	esc.writeMicroseconds(1380);
	delay(2);
	esc.writeMicroseconds(1500);
}