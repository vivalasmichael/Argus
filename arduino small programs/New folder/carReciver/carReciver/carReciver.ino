#include <Thread.h>
#include <ThreadController.h>
#include <SoftwareSerial.h>


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define BlinkNumber 3

/// RF Radio public variables
RF24 radio(9, 10);
const byte addresses[][6] = {"00001", "00002"};

/// Leds public variables
int ledG = 3;
int ledB = 2;
int ledR = 4;
bool isGreenActive = false;
int redBlinkNumber = 0;
int blueBlinkNumber = 0;
int greenBlinkNumber = 0;
int purpleBlinkNumber = 0;

/// buttons public variables
const int button1Pin = 7;
int button1State = 0;
int button1Timer = 0;
bool wasButton1Pressed = false;
const int button2Pin = 6;
int button2State = 0;
int button2Timer = 0;
bool wasButton2Pressed = false;
const int button3Pin = 5;
int button3State = 0;
int button3Timer = 0;
bool wasButton3Pressed = false;

// ThreadController that will controll all threads with multi threhads
ThreadController controll = ThreadController();
Thread inputButtonThread = Thread();
Thread outputButtonThread = Thread();

/// Moto Unit Variables
bool isMotoComander = true;
bool isTestMode = false;
int counter = 0;
int mode = 0;

bool wasActivated = false;
bool wasRequestIdSent = false;

void setup() {


  Serial.begin(9600);


  radio.begin();

  Serial.print("is Chip Connected = " );
  Serial.println(radio.isChipConnected());


  pinMode(ledG, OUTPUT);
  pinMode(ledR, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);


  digitalWrite(ledR, HIGH);


  // Configure Threads
  inputButtonThread.onRun(chackInputButtons);
  inputButtonThread.setInterval(0);

  outputButtonThread.onRun(blinkBlueLed);
  outputButtonThread.setInterval(0);

  // Adds both threads to the controller
  controll.add(&inputButtonThread);
  controll.add(&outputButtonThread); // & to pass the pointer to it


}


void loop() {

  controll.run();

  
  decraseButtonTimers();
  /// Writing to pipe
 // if (wasButton1Pressed && button1Timer <= 0) {
  //  uint32_t messege = 0;
  //  writeToRadio(messege);
  //  wasButton1Pressed =  false;
 // }
  

  /// Reading from pipe
  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();
  if (radio.available())
  {
    uint32_t text = {0};
    radio.read(&text, sizeof(text));
    Serial.println(text);
    writeToRadio(text);
    digitalWrite(ledB, HIGH);
  }
}


int startTimer  = 5000;
// callback for inputButtonThread
void chackInputButtons() {

  /// Normal Mode
  /// will only send messages when buttons are pressed
  button1State = digitalRead(button1Pin);
  if (button1State == HIGH) {
    if (!wasButton1Pressed) {
      wasButton1Pressed = true;
      button1Timer = startTimer;
    }
  }
  button2State = digitalRead(button2Pin);
  if (button2State == HIGH ) {
    if (!wasButton2Pressed) {
      wasButton2Pressed = true;
      button2Timer = startTimer;
    }
  }
  button3State = digitalRead(button3Pin);
  if (button3State == HIGH ) {
    if (!wasButton3Pressed) {
      wasButton3Pressed = true;
      button3Timer = startTimer;
    }
  }



}

// callback for hisThread
void blinkBlueLed() {
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




void writeToRadio(uint32_t messege) {
  radio.openWritingPipe(addresses[0]);
  radio.stopListening();
  radio.write(&messege, sizeof(messege));
  Serial.println(messege);
}






void decraseButtonTimers() {
  if (button1Timer > 0) {
    button1Timer--;
  }
  if (button2Timer > 0) {
    button2Timer--;
  }
  if (button3Timer > 0) {
    button3Timer--;
  }
}


