#define LED4 D4     // Buildin Blue LED
#define LED_blue D1
#define LED_red D0


void setup() {
  pinMode(LED4, OUTPUT);    // LED pin as output.
  pinMode(LED_blue, OUTPUT);    // LED pin as output.
  pinMode(LED_red, OUTPUT);    // LED pin as output.
//  digitalWrite(LED4, HIGH);   // LED D4 HIGH = turn-off

}

void loop() {
  digitalWrite(LED_blue, HIGH);
  delay(3000);
  digitalWrite(LED_blue, LOW);
  delay(500);

  digitalWrite(LED_red, HIGH);
  delay(500);
  digitalWrite(LED_red, LOW);
  delay(500);

}
