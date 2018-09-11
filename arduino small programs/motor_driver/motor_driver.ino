//pins declaration
int MODE = 8;
int APHASE = 9;
int AENBL = 10;
int redPin= 3;
int greenPin = 6;
int bluePin = 7;
// motor speed is any number between 0 and 255
int MotorSpeed = 254;

// defines pins numbers
const int trigPin = 3;
const int echoPin = 4;

// defines variables
long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(MODE, OUTPUT);
  pinMode(APHASE, OUTPUT);
  pinMode(AENBL, OUTPUT);
  Serial.begin(9600);
  digitalWrite(MODE, HIGH); //set mode to high
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
   forward(APHASE, AENBL); //move M1 forward
}

void loop() {
 
 // setColor(255, 0, 0); // Red Color
  //Serial.println("Motor M1 is moving forward");
 // delay(2000); //delay for 2 seconds
  //reverse(APHASE, AENBL); //move M1 reverse
 // setColor(0, 255, 0); // Green Color
 // Serial.println("Motor M1 is moving reverse"); //display message in serial monitor
  //delay(2000); //delay for 2 seconds

  // Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;

// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);
delay(100);
}

void forward(int Phase, int Enable) {
  digitalWrite(Phase, HIGH);
  analogWrite(Enable, MotorSpeed);
}

void reverse(int Phase, int Enable) {
  digitalWrite(Phase, LOW);
  analogWrite(Enable, MotorSpeed);
}
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

