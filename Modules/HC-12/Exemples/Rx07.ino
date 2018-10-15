/* -------------------------------------------------------- //
   ===========================================================
   | File Rx06.ini
   |
   | Replace BMP180 with MAX6675
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
   | The master transmits every 200ms.
   | The slave replies 50ms after that last successful instruction
   | is applied to the received data.
   |
   | RevG.1 - Add pushbutton to allow for reading Min/Max reset temperature
   |
   ===========================================================
 // -------------------------------------------------------- */

#include <Wire.h>											// I2C LCD
#include <LiquidCrystal_I2C.h>								// I2C LCD
#include <SoftwareSerial.h>;								// HC11 Tranceiver
#include <Adafruit_GFX.h>									//
#include <Adafruit_SSD1306.h>								//
#include "max6675.h"										// Thermocouple module

// ͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞͞
//  Hardware configuration and declarations
// ____________________________________________________________

#define SoftRX      2										// Tx on HC11
#define SoftTX      3										// Rx on HC11

#define LDR         0										// AI[0]

#define SwMM        4										// Switch Min/Max select
#define DO          5										// Max667 DO (Data Out)
#define CS          6										// Max667 CS (Chip Select)
#define CK          7										// Max667 CLK
#define TxL        13										// Tx LED

#define b0          0
#define b1          1
#define b2          2
#define b3          3
#define b4          4
#define b5          5
#define b6          6
#define b7          7


LiquidCrystal_I2C lcd(0x3F,20,4);							// set the LCD address to 0x3B

SoftwareSerial HC11 (SoftRX, SoftTX);

//____________________________________________________________
/////////  ReadMAX6675()  ////////////////////////////////////
const int SampleNum = 10;									// Number of samples for average
int readIndex = 0;											// the index of the current reading
int iTCTemp;

float fTemp;												// Temperature signal
float fTCTemp;												// TC
float fRead[SampleNum];										// the fRead from the analog input
float total = 0.0;											// the running total
float fTempMin = 99.9;										// Initial value
float fTempMax = 0.0;										// Initial value
static unsigned long prevMillis01 = 0;
static unsigned long prevMillis02 = 0;
uint8_t Degr[8]  = {0xc,0x12,0x12,0xc,0x0,0x0,0x0}; 		// Contactor OFF indication

MAX6675 thermocouple(CK, CS, DO);							//

//____________________________________________________________
/////////  RunTime()  ////////////////////////////////////////
static unsigned long NowMillis = 0;
static unsigned long LastMillis = 0;
static unsigned long LoopMillis = 0;
static unsigned long previousMillis = 0;

//____________________________________________________________
/////////  getData()  ////////////////////////////////////////
const byte numChars = 42;
char rcvChars[numChars];
char tempChars[numChars];
bool newData = false;
bool NowTx;

int iTest, i1, i2, i3, i3Old;
int iLDR, iWdtA, iWdtY;
byte iHH, iMM, iSS;
byte iHHn, iMMn, iSSn;

unsigned char dTx[12];
unsigned char dRx[12];
unsigned char check1, check2, check3, check4, check5;
bool x1, x2, x3, x4;
int ifail = 0;
char buff[3];


/* -----------------------------------------------------------
   Switch Pressed bytes
----------------------------------------------------------- */
bool SwMM_Pressed, SwMM_Mem, bP1, bP2;
bool Okay2Measure = false;
static unsigned long prevMillis03 = 0;
static unsigned long prevMillis04 = 0;
byte CycleCount = 0;

/* -----------------------------------------------------------
   -- SETUP -- SETUP -- SETUP -- SETUP -- SETUP -- SETUP --
----------------------------------------------------------- */
void setup() {

	Serial.begin(9600);										// Open serial port to computer
	HC11.begin(9600);										// Open software serial port to HC11
	lcd.begin();											// initialize the lcd

	pinMode (TxL, OUTPUT);									// built-in LED
	pinMode (SwMM, INPUT_PULLUP);							// Min/Max select button

/* -----------------------------------------------------------
   LCD Special characters 
----------------------------------------------------------- */
	lcd.createChar(0, Degr);								// Degree sign

	iWdtA = 0;
	iWdtY = 0;

	RevInfo();
//

/* -----------------------------------------------------------
   When using PLX-DAQ Spreadsheet this will be the header on top
----------------------------------------------------------- */
//Serial.println("LABEL,Time,InTemp,#2,#3,#4");

//-------------------+
}   // End Setup()   |
//-------------------+



/* ===========================================================
   -- LOOP -- LOOP -- LOOP -- LOOP -- LOOP -- LOOP -- LOOP
=========================================================== */

void loop () {


	iLDR = analogRead(LDR);

	ReadMAX6675();


	dTx[1] = iWdtA & 0xFF;									// LSB
	dTx[2] = (iWdtA >> 8);									// MSB
	dTx[3] = ~(dTx[1]+dTx[2]) + 1;							// Checksum

	dTx[4] = iTCTemp & 0xFF;								// LSB
	dTx[5] = (iTCTemp >> 8);								// MSB
	dTx[6] = ~(dTx[4]+dTx[5]) + 1;							// Checksum



if (HC11.available()){
	getData();
	if (newData == true) {
		strcpy(tempChars, rcvChars);
		parseData();
		newData = false;
		NowTx = true;
		}
	}




/* -----------------------------------------------------------
   Wait for 10ms after successful receiving of data
   then transmit back to master.
   Do not make this time too long. Max 25ms else
   the Master node will start receiving bad data fragments
----------------------------------------------------------- */

if (NowTx)
{
	if (millis() - previousMillis > 10) {
		digitalWrite (TxL, HIGH);
		TxData();
		digitalWrite (TxL, LOW);
		NowTx = false;
		previousMillis = millis();
	}
}

	if (!SwMM_Mem)
	{
		DisplayLCD();
	}

	if (bP2)
	{
		DisplayMinMax();
	}
//



/* -----------------------------------------------------------
   Read the status of SwMM
   If SwMM pressed
----------------------------------------------------------- */
	SwMM_Pressed = !digitalRead(SwMM);						// Read switch status

	if (SwMM_Pressed && !SwMM_Mem)							// Switch pressed
	{
		lcd.clear();
		SwMM_Mem = true;
	}
//
	if (!SwMM_Pressed && SwMM_Mem)							// Switch pressed
	{
		bP2 = true;
	}

/* -----------------------------------------------------------
   While we are in the Min/Max window
   Timeout after 5000 millis
----------------------------------------------------------- */
	if (SwMM_Mem)
	{
		if (millis() - prevMillis03 > 7000)
		{
			SwMM_Mem = false;
			bP2 = false;
			lcd.clear();
			prevMillis03 = millis();
		}
	}

	RunTime();


//----------------+
}   // END LOOP   |
//----------------+



/* ###########################################################
   -- FUNCTIONS -- FUNCTIONS -- FUNCTIONS -- FUNCTIONS -- 
########################################################### */

/* ===========================================================
   RevInfo - Display software revision
=========================================================== */
void RevInfo()
{
	lcd.backlight();
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("File: Rx07.ino");
	lcd.setCursor(0, 1);
	lcd.print("Built Rev G.1");
	delay(1500);
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("2017-181-03G");
	lcd.setCursor(0, 1);
	lcd.print("2017-06-30");
	delay(1500);
	lcd.clear();
// ------------------+
}   // End RevInfo() |
// ------------------+


/* -----------------------------------------------------------
   RunTime()
----------------------------------------------------------- */
void RunTime() {
	LastMillis = NowMillis;
	NowMillis = millis();
	LoopMillis = NowMillis - LastMillis;

// ------------------+
}   // END RunTime() |
// ------------------+



/* -----------------------------------------------------------
   DisplayLCD()
----------------------------------------------------------- */
// Display on LCD (X, Y) Column, row
/* +---------------------------------------+
   |        0          |        1          | <-X
   |0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|  6|7|8|9| <-X
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  -+-+-+-+
 0 |H|H|:|M|M|:|S|S| | | | | | | | |   | | | |
 1 |T|e|m|p|:| |2|5|.|6| | | | |9|9|   |9| | |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  -+-+-+-+ */
void DisplayLCD() 
{
/* -----------------------------------------------------------
   Display 1st
----------------------------------------------------------- */
	lcd.setCursor(0, 0);
	if(iHH < 10){lcd.print(F("0"));}
	lcd.print(iHH);
	lcd.print(F(":"));

	if(iMM < 10){lcd.print(F("0"));}
	lcd.print(iMM);
	lcd.print(F(":"));

	if(iSS < 10){lcd.print(F("0"));}
	lcd.print(iSS);
//	lcd.print(F("  "));

	lcd.setCursor(9, 0);
	lcd.print(ifail);


	sprintf(buff, "%3d", iWdtA);							// Right justify text

	lcd.setCursor(13, 0);
	lcd.print(buff);
	lcd.print(F("  "));

/* -----------------------------------------------------------
   Display 2nd
----------------------------------------------------------- */
	lcd.setCursor(0, 1);
	lcd.print(F("Temp: "));
	lcd.print(fTCTemp,1);
	lcd.write(byte(0));										// Degree circle 
	lcd.print(F("C"));

	lcd.setCursor(14, 1);
	lcd.print(LoopMillis);

// ---------------------+
}   // END DisplayLCD() |
// ---------------------+




/* -----------------------------------------------------------
   DisplayMinMax()
----------------------------------------------------------- */
// Display on LCD (X, Y) Column, row
/* +---------------------------------------+
   |        0          |        1          | <-X
   |0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 0 |M|i|n|:| |1|2|.|5|°|C| | | | | |
 1 |M|a|x|:| |5|2|.|5|°|C| | | |9|9|
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
void DisplayMinMax() 
{
/* -----------------------------------------------------------
   Display 1st
----------------------------------------------------------- */
	lcd.setCursor(0, 0);

	lcd.setCursor(0, 0);
	lcd.print(F("Min: "));
	lcd.print(fTempMin,1);
	lcd.write(byte(0));										// Degree circle 
	lcd.print(F("C"));

	lcd.setCursor(0, 1);
	lcd.print(F("Min: "));
	lcd.print(fTempMax,1);
	lcd.write(byte(0));										// Degree circle 
	lcd.print(F("C"));

	lcd.setCursor(14, 1);
	lcd.print(LoopMillis);
//

/* -----------------------------------------------------------
   If Button is pressed and held down for > 2000 millis
   Reset Min/Max
----------------------------------------------------------- */

	if (SwMM_Pressed && SwMM_Mem)							// Switch pressed
	{
		if (millis() - prevMillis04 > 3000)
		{
			fTempMin = 99.9;								// Initial value
			fTempMax = 0.0;									// Initial value
			prevMillis04 = millis();
		}
	}


// ------------------------+
}   // END DisplayMinMax() |
// ------------------------+



/* -----------------------------------------------------------
   showData()
----------------------------------------------------------- */
void showData()
{

//----------------------+
}   // END showData()   |
//----------------------+




/* -----------------------------------------------------------
   TxData()
----------------------------------------------------------- */
void TxData()
{
//			Serial.println(" TX! ");
			iWdtY = iWdtA;
//



/*____________________________________________________________
   This is one method of sending the data over serial.
   Individual print commands for each byte and segmant
   of data to be transmitted
____________________________________________________________ */

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
			HC11.print(">");

//-------------------+
}   // End dataTx()  |
//-------------------+



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

	strtokIndx = strtok(NULL, ",");
	dRx[10] = atoi(strtokIndx);

	strtokIndx = strtok(NULL, ",");
	dRx[11] = atoi(strtokIndx);

	strtokIndx = strtok(NULL, ",");
	dRx[12] = atoi(strtokIndx);


	check1 = dRx[1]+dRx[2]+dRx[3];				// value + checksum
	if (check1 == 0)							// If checksum ok
		{
			iWdtA = ((dRx[2] << 8) | dRx[1]);	// Store value
			x4 = true;
		}
		else
		{
			x4 = false;
		}


	check2 = dRx[4]+dRx[5]+dRx[6];				// Value + Checksum
	if (check2 == 0)							// If Checksum okay
		{
			i2 = ((dRx[5] << 8) | dRx[4]);		// Store value
		}


	check3 = dRx[7]+dRx[8];						// Value + Checksum
	if (check3 == 0)							// If Checksum okay
		{
			iHHn = dRx[7];						// Store value
			iHH = iHHn;							// Save Hours
			x1 = true;
		}
		else
		{
			iHH = iHH; 
			x1 = false;
		}


	check4 = dRx[9]+dRx[10];					// Value + Checksum
	if (check4 == 0)							// If Checksum okay
		{
			iMMn = dRx[9];						// Store value
			iMM = iMMn;							// Save Minutes
			x2 = true;
		} else 
		{
			iMM = iMM;
			x2 = false;
		}



	check5 = dRx[11]+dRx[12];					// Value + Checksum
	if (check5 == 0)							// If Checksum okay
		{
			iSSn = dRx[11];						// Store value
			iSS = iSSn;							// Save Seconds
			x3 = true;
		} else 
		{
			iSS = iSS;
			x3 = false;
		}


if (!x1 || !x2 || !x3 || !x4)
	{
		ifail ++;
	}


//-----------------------+
}   // END parseData()   |
//-----------------------+


/* ===========================================================
   Calculate the average of the TC signal
   Average values are used taking a reading sample
   each cycle of the Arduino and then doing average
   calculation
=========================================================== */

void ReadMAX6675(void)
{
	if (millis() - prevMillis02 > 250) {					// MAX6675 needs about 250ms to read 

//

	if ((CycleCount > 10) && !Okay2Measure)
		{
			Okay2Measure = true;
		}

		if (!Okay2Measure)
		{
			CycleCount ++;
		}


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
		iTCTemp = (fTCTemp * 10);							// Chsnge float to int
//
		if (fTCTemp > fTempMax) 
		{
			fTempMax = fTCTemp;
		}

		if ((fTCTemp < fTempMin) && Okay2Measure) 
		{
			fTempMin = fTCTemp;
		}

	prevMillis02 = millis();
	}

//---------------------------+
}   // END ReadMAX6675(void) |
//---------------------------+
