//pins declaration
int MODE = 2;
int APHASE = 4;
int AENBL = 5;
int redPin= 9;
int greenPin = 10;
int bluePin = 11;
// motor speed is any number between 0 and 255
int MotorSpeed = 50;

void setup() {
  pinMode(MODE, OUTPUT);
  pinMode(APHASE, OUTPUT);
  pinMode(AENBL, OUTPUT);
  Serial.begin(9600);
  digitalWrite(MODE, HIGH); //set mode to high
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void loop() {
  forward(APHASE, AENBL); //move M1 forward
  setColor(255, 0, 0); // Red Color
  Serial.println("Motor M1 is moving forward");
  delay(2000); //delay for 2 seconds
  reverse(APHASE, AENBL); //move M1 reverse
  setColor(0, 255, 0); // Green Color
  Serial.println("Motor M1 is moving reverse"); //display message in serial monitor
  delay(2000); //delay for 2 seconds
}

void forward(int Phase, int Enable) {
  digitalWrite(Phase, LOW);
  analogWrite(Enable, MotorSpeed);
}

void reverse(int Phase, int Enable) {
  digitalWrite(Phase, HIGH);
  analogWrite(Enable, MotorSpeed);
}
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

