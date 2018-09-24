/* -------------------------------------------------------- //
   ===========================================================
   | File Tx07.ini
   |
   | HC11 Radio settings:
   | U3/B9600/C001/A002/P8
   |
   | Data is sent in byte packets. 
   | A Float signal is converted to an integer by multiplying with
   | either 10 or 100 depending on the number of decimals required.
   | Integer signals are then split into byteHigh and byteLow
   | The checksum error of these two bytes are then sent as a package
   | for the three bytes.
   | On the receiving node, the checksum is added to byte1 + byte2
   | and the result should be "0" (zero)
   | If result is good, the value is used, else the last good
   | value will be used until good values arrive again.
   | 
   | G0.2 - Tx after 500ms 
   |
   | HC11 Tranceiver
   | 1 - Vcc		Arduino Pin 5V
   | 2 - GND		Arduino Pin Gnd
   | 3 - RXD		Arduino Pin D3 SoftwareSerial(Tx)
   | 4 - TXD		Arduino Pin D2 SoftwareSerial(Rx)
   | 5 - SET		(To GND to enable changes)
   | 
   | I2C LCD display
   | 1 - Gnd		Arduino Pin Gnd
   | 2 - Vcc		Arduino Pin 5V
   | 3 - SDA		Arduino Pin A4
   | 4 - SCL		Arduino Pin A5
   |
   | I2C DS1307
   | 1 - Gnd		Arduino Pin Gnd
   | 2 - Vcc		Arduino Pin 5V
   | 3 - SDA		Arduino Pin A4
   | 4 - SCL		Arduino Pin A5
   |
   | MAX6675
   | 1 - Gnd		Arduino Pin Gnd
   | 2 - Vcc		Arduino Pin 5V
   | 3 - DO			Arduino Pin D5
   | 4 - CS			Arduino Pin D6
   | 5 - CLK		Arduino Pin D7
   |
   | DS18b20
   | 1 - Vdd		Arduino Pin 5V
   | 2 - Vcc		Arduino Pin D8 Connect 4k7 to Vdd
   | 3 - GND		Arduino Pin Gnd
   |
   |
   ===========================================================
 // -------------------------------------------------------- */

#include <SoftwareSerial.h>;								// HC11 Serial comms
#include <Wire.h>											// I2C LCD
#include <LiquidCrystal_I2C.h>								// I2C LCD
#include "Sodaq_DS3231.h"									// https://github.com/SodaqMoja/Sodaq_DS3231
#include "max6675.h"										// Thermocouple module
#include "Sodaq_DS3231.h"									// https://github.com/SodaqMoja/Sodaq_DS3231
#include <OneWire.h>										// DS18B20
#include <DallasTemperature.h>								// DS18B20

// ͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞
//  Hardware configuration and declarations
// ____________________________________________________________

#define SoftRX      2										// Tx on HC11
#define SoftTX      3										// Rx on HC11


#define DO          5										// Max667 DO (Data Out)
#define CS          6										// Max667 CS (Chip Select)
#define CK          7										// Max667 CLK

#define LDR         3										// AI[3]
#define OWB         8										// DS18B20 temp

#define TxL        10										// Tx LED
#define RxL        12										// Rx LED

LiquidCrystal_I2C lcd(0x3F,20,4);							// set the LCD address to 0x3f

OneWire oneWire(OWB);										// DS18B20
DallasTemperature sensors(&oneWire);						// DS18B20

/* -----------------------------------------------------------
   MAX6675 variables
----------------------------------------------------------- */
const int SampleNum = 3;									// Number of samples for average

int readIndex = 0;											// the index of the current reading

float fBMPTemp = 0.0;

float fTemp, fMax = -9.0;									// Temperature signal
float fMin = 99.0;											//
float fTCTemp;												// TC
float fRead[SampleNum];										// the fRead from the analog input

float total = 0.0;											// the running total


MAX6675 thermocouple(CK, CS, DO);							//

/* -----------------------------------------------------------
   Reserve bytes for Serial communication
----------------------------------------------------------- */
const byte numChars = 21;
char rcvChars[numChars];
char tempChars[numChars];
bool newData = false;
bool NowTx, bTx, bRx;
unsigned char dTx[14];
unsigned char dRx[12];
unsigned char check1, check2, check3;
int iCnt1 = 0;
int iCnt2 = 0;


SoftwareSerial HC11 (SoftRX, SoftTX);


/////////  ReadDS1820()  /////////////////////////////////////
float fTemp1, fTemp2;
float tIn, tOut;

/////////  RunTime()  ////////////////////////////////////////
static unsigned long NowMillis = 0;
static unsigned long LastMillis = 0;
static unsigned long LoopMillis = 0;
//byte LoopCount = 0;

/////////  GetRTCtime()  /////////////////////////////////////
uint32_t old_ts;
byte iHour, iMin, iSec;

int iWdtA, iWdtY, i3, iNodeSY, iNodeSYL;
int iLDR, iTCTemp, iBMPTemp, iBMPTempLast;

float fTT;
bool bDir, x1;

/////////  Replace Delay functions  //////////////////////////
static unsigned long previousMillis = 0;
static unsigned long prevMillis02 = 0;
static unsigned long prevMillis03 = 0;



/* ===========================================================
   -- SETUP -- SETUP -- SETUP -- SETUP -- SETUP -- SETUP --
=========================================================== */
void setup() {

	lcd.begin();											// initialize the lcd
	Serial.begin(115200);									// Open serial port to computer
	HC11.begin(9600);										// Open software serial port to HC11
	rtc.begin();											// RTC
	sensors.begin();										// DS18B20

	bDir = true;
	iWdtA = 0;


	pinMode(TxL, OUTPUT);
	pinMode(RxL, OUTPUT);


/* -----------------------------------------------------------
   Display software date and revision
----------------------------------------------------------- */
	RevInfo();												// Show software revision

//-------------------+
}   // End Setup()   |
//-------------------+






/* ===========================================================
   -- LOOP -- LOOP -- LOOP -- LOOP -- LOOP -- LOOP -- LOOP
=========================================================== */

void loop () 
{

	iLDR = analogRead(LDR);

	GetRTCtime();
	ReadMAX6675();

	DisplayLCD();

	dataToTx();


/* -----------------------------------------------------------
   If anything is received on the HC11 tranceiver ...
----------------------------------------------------------- */
if (!NowTx)
{
	if (HC11.available()){
		digitalWrite(RxL, HIGH);
			getData();
		digitalWrite(RxL, LOW);

		if (newData == true) {
			strcpy(tempChars, rcvChars);
			parseData();
			newData = false;
			}
	}
}

 
/* -----------------------------------------------------------
   Every 2nd loop
----------------------------------------------------------- */

	if (millis() - previousMillis > 500)
	{
	NowTx = true;
	digitalWrite(TxL, HIGH);
		TxData();
	digitalWrite(TxL, LOW);
		NowTx = false;
		previousMillis = millis();
	}


	ReadDS1820();
	RunTime();

//----------------+
}   // End LOOP   |
//----------------+







/* ===========================================================
   -- FUNCTIONS -- FUNCTIONS -- FUNCTIONS -- FUNCTIONS -- 
=========================================================== */

/* -----------------------------------------------------------
   RevInfo - Display software revision
----------------------------------------------------------- */

void RevInfo() {
	lcd.backlight();
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("File: Tx07.ino");
	lcd.setCursor(0, 1);
	lcd.print("Built Rev G0.2");
	lcd.setCursor(0, 2);
	lcd.print("2017-166-03G");
	lcd.setCursor(0, 3);
	lcd.print("2017-06-15");
	delay(1500);
	lcd.clear();

//-------------------+
}   // END RevInfo() |
//-------------------+


/* -----------------------------------------------------------
   Display on LCD (X, Y) Column, row
----------------------------------------------------------- */
/* +---------------------------------------+
   |        0          |        1          | <-X
   |0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|6|7|8|9| <-X
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 0 |H|H|:|M|M|:|S|S| | | | |L|D|R|:| |8|8|8|
 1 |T|e|m|p|:| |2|5|.|6| |*|2|6|.|8| |9|9|9|
 2 |T|e|m|p|:| |2|5|.|6| | |2|6|.|8| | | | |
 3 |L|T|:| |1|2|3| |1|0|0| |#|1|0|0| |9|9|9|
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
---------------------------------------------------------- */
void DisplayLCD() {
/* -----------------------------------------------------------
   Row 0
----------------------------------------------------------- */
	lcd.setCursor(12, 0);
	lcd.print(F("LDR:"));
	lcd.setCursor(17, 0);
	lcd.print(iLDR);
/* -----------------------------------------------------------
   Row 1
----------------------------------------------------------- */
	lcd.setCursor(0, 1);
	lcd.print(F("Temp:"));
	lcd.setCursor(6, 1);
	lcd.print(fTCTemp,1);
	lcd.print(" ");
	lcd.setCursor(12, 1);
	lcd.print(fBMPTemp,1);
	lcd.print(" ");
	lcd.setCursor(17, 1);
	lcd.print(iCnt1);
/* -----------------------------------------------------------
   Row 2
----------------------------------------------------------- */
	lcd.setCursor(0, 2);
	lcd.print(F("Temp:"));
	lcd.setCursor(6, 2);
	lcd.print(fTemp1,1);
	lcd.setCursor(12, 2);
	lcd.print(fTemp2,1);
/* -----------------------------------------------------------
   Row 3
----------------------------------------------------------- */
	lcd.setCursor(8, 3);
	lcd.print(iWdtA);
	lcd.print(" ");
	lcd.setCursor(13, 3);
	lcd.print(iNodeSY);
	lcd.print(" ");
	lcd.setCursor(17, 3);
	lcd.print(iCnt2);
//----------------------+
}   // END DisplayLCD() |
//----------------------+



/* ===========================================================
   Read Dallas DS1820 temperature sensors
=========================================================== */
void ReadDS1820()
{

	if (millis() - prevMillis03 > 500)
	{
//	sensors.setWaitForConversion(false);					// This causes Rx data to fail
	sensors.requestTemperatures();
//	sensors.setWaitForConversion(true);

	fTemp1 = (sensors.getTempCByIndex(0));					// Temperature Sensor 1
	fTemp2 = (sensors.getTempCByIndex(1));					// Temperature Sensor 2
    
	if (fTemp1 < -10.0) {									// Test if Roof temp < -100
		return;}											// if so, do not use that value
	else {tIn = fTemp1;}									// If Not, pass value into tR

	if (fTemp2 < -10.0) {									// Test if Outlet temp < -100
		return;}											// if so, do not use that value
	else {tOut = fTemp2;}									// If Not, pass value into tO

		prevMillis03 = millis();
	}

//----------------------+
}   // END ReadDS1820() |
//----------------------+









/* -----------------------------------------------------------
   dataToTx()
   Pack all the data you want to transmit into byte registers
----------------------------------------------------------- */
void dataToTx()
{
	dTx[1] = iWdtA & 0xFF;								// LSB
	dTx[2] = (iWdtA >> 8);								// MSB
	dTx[3] = ~(dTx[1]+dTx[2]) + 1;						// Checksum

	dTx[4] = iTCTemp & 0xFF;							// LSB
	dTx[5] = (iTCTemp >> 8);							// MSB
	dTx[6] = ~(dTx[4]+dTx[5]) + 1;						// Checksum

	dTx[7] = iHour;
	dTx[8] = ~(dTx[7]) + 1;

	dTx[9] = iMin;
	dTx[10] = ~(dTx[9]) + 1;

	dTx[11] = iSec;
	dTx[12] = ~(dTx[11]) + 1;				// Checksum

//-------------------+
}   // END dataToTx() |
//-------------------+



/* -----------------------------------------------------------
   TxData()
----------------------------------------------------------- */
void TxData()
{

	if (iWdtA > 100) 
		{bDir = false;}
	if (iWdtA < 0)
		{bDir = true;}

	if (bDir) {iWdtA ++;}
	if (!bDir) {iWdtA --;}

			HC11.print("<");
			HC11.print(",");
			HC11.print(dTx[1]);
			HC11.print(",");
			HC11.print(dTx[2]);
			HC11.print(",");
			HC11.print(dTx[3]);
			HC11.print(",");
			HC11.print(dTx[4]);
			HC11.print(",");
			HC11.print(dTx[5]);
			HC11.print(",");
			HC11.print(dTx[6]);
			HC11.print(",");
			HC11.print(dTx[7]);
			HC11.print(",");
			HC11.print(dTx[8]);
			HC11.print(",");
			HC11.print(dTx[9]);
			HC11.print(",");
			HC11.print(dTx[10]);
			HC11.print(",");
			HC11.print(dTx[11]);
			HC11.print(",");
			HC11.print(dTx[12]);
			HC11.print(",");
			HC11.print(">");
           

//			Serial.flush();
			bTx = false;

//-------------------+
}   // End dataTx()  |
//-------------------+



/* ===========================================================
   Calculate the average of the TC signal
   Average values are used taking a reading sample
   each cycle of the Arduino and then doing average
   calculation
=========================================================== */

void ReadMAX6675(void)
{
	if (millis() - prevMillis02 > 250) {					// MAX6675 needs about 250ms to read 

		float temp = (thermocouple.readCelsius());

		total = total - fRead[readIndex];					// subtract the last reading:
		fRead[readIndex] = temp;							// read from the sensor:
		total = total + fRead[readIndex];					// add the reading to the total:
		readIndex = readIndex + 1;							// advance to the next position in the array:
		if (readIndex >= SampleNum)							// if we're at the end of the array...
		{
			readIndex = 0;									// ...wrap around to the beginning:
		}
		fTCTemp = total / SampleNum;						// calculate the average:
		iTCTemp = (fTCTemp * 100);							// Chsnge float to int

	prevMillis02 = millis();
	}

//---------------------------+
}   // END ReadMAX6675(void) |
//---------------------------+



/* -----------------------------------------------------------
   getData()
   Original code by Robin2
   http://forum.arduino.cc/index.php?topic=225329
----------------------------------------------------------- */
void getData() {
	static boolean recvInProgress = false;
	static byte ndx = 0;
	char startMarker = '<';
	char endMarker = '>';
	char rc;

	while (HC11.available() > 0 && newData == false) {
		rc = HC11.read();

		if (recvInProgress == true) {
			if (rc != endMarker) {
				rcvChars[ndx] = rc;
				ndx++;
				if (ndx >= numChars) {
					ndx = numChars - 1;
				}
			}
			else {
//				Serial.print(F(" Size: ")); Serial.println(ndx); // To debug the size of the array
				rcvChars[ndx] = '\0';
				recvInProgress = false;
				ndx = 0;
				newData = true;
			}
		}

		else if (rc == startMarker) {
			recvInProgress = true;
		}
	}
//---------------------+
}   // END getData()   |
//---------------------+



/* -----------------------------------------------------------
   parseData()
----------------------------------------------------------- */

void parseData() {

	char * strtokIndx;

	strtokIndx = strtok(tempChars,",");
	dRx[1] = atoi(strtokIndx);
 
	strtokIndx = strtok(NULL, ",");
	dRx[2] = atoi(strtokIndx);

	strtokIndx = strtok(NULL, ",");
	dRx[3] = atoi(strtokIndx);

	strtokIndx = strtok(NULL, ",");
	dRx[4] = atoi(strtokIndx);

	strtokIndx = strtok(NULL, ",");
	dRx[5] = atoi(strtokIndx);

	strtokIndx = strtok(NULL, ",");
	dRx[6] = atoi(strtokIndx);

	strtokIndx = strtok(NULL, ",");
	dRx[7] = atoi(strtokIndx);

	strtokIndx = strtok(NULL, ",");
	dRx[8] = atoi(strtokIndx);

	strtokIndx = strtok(NULL, ",");
	dRx[9] = atoi(strtokIndx);


	check1 = dRx[1]+dRx[2]+dRx[3];
	if (check1 == 0)
		{
			iNodeSYL = ((dRx[2] << 8) | dRx[1]);
			iNodeSY = iNodeSYL;
			x1 = false;
			lcd.setCursor(12, 3);
			lcd.print(" ");
		} else
		{
			iNodeSY = iNodeSY;
			x1 = true; iCnt2 ++;
			lcd.setCursor(12, 3);
			lcd.print("#");
		}

	check2 = dRx[4]+dRx[5]+dRx[6];
	if (check2 == 0)
		{
			iBMPTempLast = ((dRx[5] << 8) | dRx[4]);
			iBMPTemp = iBMPTempLast;
			lcd.setCursor(11, 1);
			lcd.print(" ");
		} else 
		{
			iBMPTemp = iBMPTemp;
			iCnt1 ++;
			lcd.setCursor(11, 1);
			lcd.print("*");
		}

	fBMPTemp = (iBMPTemp / 10.0);

//-----------------------+
}   // END parseData()   |
//-----------------------+


/* -----------------------------------------------------------
   Get Time and Date from RTC
----------------------------------------------------------- */

void GetRTCtime() {

	DateTime now = rtc.now();								// get the current date-time
	uint32_t ts = now.getEpoch();

	if (old_ts == 0 || old_ts != ts) {
	old_ts = ts;

	lcd.setCursor(0, 0);
	iHour = (now.hour());
	iMin = (now.minute());
	iSec = (now.second());

	if (iHour < 10) {
		lcd.print("0");
	}
	lcd.print(iHour);
	lcd.print(':');
	if (iMin < 10) 
	{
		lcd.print("0");
	}
	lcd.print(iMin);
	lcd.print(':');
	if (iSec < 10) 
	{
		lcd.print("0");
	}
	lcd.print(iSec);
	lcd.print(" ");
	}

//----------------------+
}   // END GetRTCtime() |
//----------------------+


/* -----------------------------------------------------------
   RunTime routine
----------------------------------------------------------- */

void RunTime() {
	LastMillis = NowMillis;									//
	NowMillis = millis();									//
	LoopMillis = NowMillis - LastMillis;					//
	lcd.setCursor(0, 3);									//
	lcd.print("LT: ");
	lcd.print(LoopMillis);									//
	lcd.print(" ");											//
//-------------------+
}   // END RunTime() |
//-------------------+


