/*    Arduino Long Range Wireless Communication using HC-12
      Tx = Arduino Micro - Rx = Arduino Uno
*/

#include <SoftwareSerial.h>

SoftwareSerial HC12(10, 11); // HC-12 TX Pin 10, HC-12 RX Pin 11

void setup() {
  Serial.begin(115200);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12

}

void loop() {
  while (HC12.available()) {        // Si je recois sur via HC-12
    Serial.write(HC12.read());      // Envoi vers Serial monitor
  }

}
