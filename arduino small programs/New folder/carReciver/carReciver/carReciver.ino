#include <Thread.h>
#include <ThreadController.h>
#include <SoftwareSerial.h>


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

/// LED Blink number when input recived
#define BlinkNumber 3

/// RF Radio public variables
RF24 radio(9, 10);
const byte addresses[][6] = {"00001", "00002"};


/// Motor Vars
int MODE = 2;
int APHASE = 4;
int AENBL = 5;
// motor speed is any number between 0 and 255
int MotorSpeed = 50;


/// Leds public variables
int ledG = 6;
int ledB = 7;
int ledR = 3;
bool isGreenActive = false;
int redBlinkNumber = 0;
int blueBlinkNumber = 0;
int greenBlinkNumber = 0;
int purpleBlinkNumber = 0;

// ThreadController that will controll all threads with multi threhads
ThreadController controll = ThreadController();
Thread ledOutputThread = Thread();


void setup() {
  Serial.begin(9600);
  radio.begin();

  Serial.print("is Chip Connected = " );
  Serial.println(radio.isChipConnected());

  pinMode(ledG, OUTPUT);
  pinMode(ledR, OUTPUT);
  pinMode(ledB, OUTPUT);
  //digitalWrite(ledR, HIGH);

  pinMode(MODE, OUTPUT);
  pinMode(APHASE, OUTPUT);
  pinMode(AENBL, OUTPUT);
  digitalWrite(MODE, HIGH); //set mode to high

  ledOutputThread.onRun(blinkLeds);
  ledOutputThread.setInterval(0);
  
  controll.add(&ledOutputThread); // & to pass the pointer to it
}


void loop() {

  controll.run();

  /// Reading from pipe
  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();
  if (radio.available())
  {
    int text = 0;
    radio.read(&text, sizeof(text));
    Serial.println(text);
  //  writeToRadio(text);
    handleInput(text);
    driveForward(APHASE, AENBL);
  }
}

void handleInput(int msg) {
  switch (msg) {
    case 1: {
        redBlinkNumber = BlinkNumber;
        break;
      }
    case 2: {
        blueBlinkNumber = BlinkNumber;
        break;
      }
    case 3: {
        greenBlinkNumber = BlinkNumber;
        break;
      }
    case 4: {
        purpleBlinkNumber = BlinkNumber;
        break;
      }
    case 5: {
        redBlinkNumber = BlinkNumber;
        break;
      }
    default : {
        blueBlinkNumber = BlinkNumber;
        break;
      }
  }

}


// callback for hisThread
void blinkLeds() {
  /// Blink Leds if message was recived in a diffrent thread
  if (redBlinkNumber >= 1) {
    digitalWrite(ledR, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(200);                 // wait for a second
    digitalWrite(ledR, LOW);    // turn the LED off by making the voltage LOW
    delay(200);
    redBlinkNumber--;
  }
  else if (blueBlinkNumber >= 1) {
    digitalWrite(ledB, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(200);                 // wait for a second
    digitalWrite(ledB, LOW);    // turn the LED off by making the voltage LOW
    delay(200);
    blueBlinkNumber--;
  }
  else if (greenBlinkNumber >= 1) {
    digitalWrite(ledG, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(200);                 // wait for a second
    digitalWrite(ledG, LOW);    // turn the LED off by making the voltage LOW
    delay(200);
    greenBlinkNumber--;
  }
  else if (purpleBlinkNumber >= 1) {
    digitalWrite(ledR, HIGH);
    digitalWrite(ledG, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(200);                 // wait for a second
    digitalWrite(ledR, LOW);    // turn the LED off by making the voltage LOW
    digitalWrite(ledG, LOW);
    delay(200);
    purpleBlinkNumber--;
  }


}

void writeToRadio(int messege) {
  radio.openWritingPipe(addresses[0]);
  radio.stopListening();
  radio.write(&messege, sizeof(messege));
  Serial.println(messege);
}

void driveForward(int Phase, int Enable) {
  digitalWrite(Phase, LOW);
  analogWrite(Enable, MotorSpeed);
}

void driveReverse(int Phase, int Enable) {
  digitalWrite(Phase, HIGH);
  analogWrite(Enable, MotorSpeed);
}
void setLedColor(int redValue, int greenValue, int blueValue) {
  analogWrite(ledR, redValue);
  analogWrite(ledG, greenValue);
  analogWrite(ledB, blueValue);
}




