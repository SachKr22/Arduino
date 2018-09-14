#define IR_output 2

void setup() {
pinMode(IR_output,INPUT);

Serial.begin(9600);
}

void loop() {
  if(digitalRead(IR_output)==HIGH){
   Serial.println("mouvement detecte");
  }
  if(digitalRead(IR_output)==LOW){
   Serial.println("pas de mouvement detecte");
  }

delay(2000);
}
