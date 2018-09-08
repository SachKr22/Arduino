Example 01 – Arduino Code (même code pour les 2 arduinos)
https://howtomechatronics.com/tutorials/arduino/arduino-and-hc-12-long-range-wireless-communication-module/
Here’s the Arduino code for the first example, a basic communication between the two modules using the Serial Monitor.
-----------------------------------------------------------------------------
/*    Arduino Long Range Wireless Communication using HC-12
                      Example 01
   by Dejan Nedelkovski, www.HowToMechatronics.com
*/

#include <SoftwareSerial.h>

SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin

void setup() {
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12

}

void loop() {
  while (HC12.available()) {        // If HC-12 has data
    Serial.write(HC12.read());      // Send the data to Serial monitor
  }
  while (Serial.available()) {      // If Serial monitor has data
    HC12.write(Serial.read());      // Send that data to HC-12
  }
}
-----------------------------------------------------------------------------

How the code works: So once we type something in the serial monitor and click the Send button, at the first Arduino, the while loop with the Serial.available() function will become true and using the HC12.write() function we will send the data from the serial monitor to the HC-12 module. This module will transfer the data wirelessly to the second HC-12 module, so at the second Arduino the while loop with the HC12.available() function will become true and using the Serial.write() function the data will be sent to the serial monitor.

We can use the same code for sending AT Commands and configuring the module parameters. All we have to do is connect the “Set” pin of the module to Ground or any digital pin of the Arduino and set the pin to low logic level.

To test whether we have successfully enter the mode, in the serial monitor we can type “AT” and we should get a response message “OK”. There are total of 12 AT Commands, and they are used for changing various parameters like the baud rate, the channel, the transmitting power etc.  For example, if we type “AT+B38400” the baud rate of the module will be set to 38400.

AT Commands:
1. AT – Test command.
Example: Send “AT” to module, and the module returns “OK”.
2. AT+Bxxxx – Change the serial port baud rate.
Available baud rates: 1200 bps, 2400 bps, 4800 bps, 9600 bps, 19200 bps, 38400 bps, 57600 bps, and 115200 bps. Default: 9600 bps.
Example: Send “AT+B38400” to module, and the module returns “OK+B19200”.
3. AT+Cxxxx – Change wireless communication channel, from 001 to 100.
Default: Channel 001, with working frequency of 433.4MHz. Each next channel is 400KHz higher.
Example: If we want to set the module to channel 006, we need to send “AT+C006” command to the module, and the module will return “OK+C006”. The new working frequency will be 435.4MHz.

