
#include <SFE_BMP180.h>
#include <Wire.h>

SFE_BMP180 bmp180;



void setup() {
  Serial.begin(9600);


  if (bmp180.begin()) {
    Serial.println("BMP180 init success");
  } else {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
}

void loop() {
  char status;
  double T,P,p0,a;

  status = bmp180.startTemperature();

  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    status = bmp180.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.print(" deg C, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" deg F");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");

  delay(5000);
}
