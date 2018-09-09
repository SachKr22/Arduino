/*
  Using HC-12 (long range wireless) modules.
  To transmit and receive information from DHT-11 (temp, humidity sensor).
  Originally used Xbee's (but they are expensive).
  Had several issues with the coding. Forgot to identify "MySerial" in both send and receive code.
  Also connected RX and TX to wrong pins (2 and 3 on Arduino).
  Getting data now. But it seems a bit confusing, cannot distinguish between humidity and temperature.
  Solved by telling transmitter to Serial.write(variable).
  Tried adding an analog read to measure rain via ADC, this caused the data to be mixed.
  Removed the rain detector.
  Added a RTC clock to indicate time and date of temp/humidity.
  Having issues setting the date and time.
  The only thing that sets correctly is year, time follows but is wrong.
  Replaced clock onboard battery.
  This fixed the problem.
  Had issue with some digits in minutes not displaying.
  Tried doing some math fixes, this only caused more troubles.
  Also had issue with time error between RTC and PC.
  Removed all math fixes and enabled rtc.adjust line to run everytime in setup.
  This fixed both issues.
  Replaced DHT-11 sensor with BME280 (Barometer, temperature and humidity) sensor. This uses I2C serial communication.
  All its data outputs in meteric.
  Also replaced 5W solar panel with 20W panel.
  Daylight savings time messed up onboard clock time. Pulled battery and reloaded sketch.
  Added a OLED display and LiPo battery pack.
  After realizing Im not viewing or using my PC, I set the time at a specific time/date versus using the PC time/date.
*/

#include <SoftwareSerial.h>
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

SoftwareSerial MySerial(2, 3); // RX, TX
RTC_DS1307 rtc;

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

//#if (SSD1306_LCDHEIGHT != 64)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif

void setup() {
  MySerial.begin(9600); //begins HC-12 to receive serial data
  Serial.begin(9600); //begins PC serial data
  Wire.begin(); // begins RTC serial data

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Initialize with the I2C addr 0x3D (for the 128x64)
  display.setTextColor(WHITE);

  //rtc.adjust(DateTime(2016, 11, 20, 19,45,00)); // once this is SET, remark out this statement and reload.
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();
  delay (500);
}

void loop() {
  DateTime now = rtc.now(); // get time and date from RTC

  if (MySerial.available() > 1) { // checks to see if HC-12 is receiving data.
    for (int x = 0; x < 1; x ++) { // run once. Keep in mind transmitter actually sends twice.
      int h = MySerial.read(); // read humidity value
      int f = MySerial.read(); // read temperature value
      float p = MySerial.read(); // read barometeric value

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);

      display.print(now.year(), DEC);
      display.print('/');
      display.print(now.month(), DEC);
      display.print('/');
      display.print(now.day(), DEC);
      display.print(' ');
      display.print(now.hour(), DEC);
      display.print(':');
      display.print(now.minute (), DEC);
      display.println("");
      display.println("");
      display.print("Humidity:    ");
      display.print(h);
      display.print(" %");
      display.println("");
      display.print("Temperature: ");
      display.print(f);
      display.print(" ");
      display.print(char(248)); // degree symbol
      display.print("F");
      display.println("");
      display.print("Barometer:   ");
      display.print(p);
      display.print(" Hg");

      display.display();

    }
  }
  MySerial.flush(); // flush serial data
  delay(100);
}
