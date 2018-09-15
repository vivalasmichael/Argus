int pressurePin = A0;
int force;
int LEDpin = 13;
void setup() {
  Serial.begin(9600);
  pinMode(LEDpin, OUTPUT);
}
void loop() {
  force = analogRead(pressurePin);
  Serial.println(force);
  if (force > 100)
  {
    digitalWrite(LEDpin, HIGH);
    Serial.print(force);
    Serial.println("----bigger than 100");
  }
  else
  {
    digitalWrite(LEDpin, LOW);
    Serial.print(force);
    Serial.println("----less than 100");
  }
  delay(100 );
}


