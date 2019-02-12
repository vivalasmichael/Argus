#include <FastLED.h>
#include <Thread.h>
#include <ThreadController.h>

#include <SPI.h>
//#include <nRF24L01.h>
//#include <RF24.h>

/// Motor Vars
int MODE = 8;
int APHASE = 9;
int AENBL = 10;
// motor speed is any number between 0 and 255
int MotorSpeed = 254;

/// Leds public variables
int carLedG = 12;
int carLedR = 11;
int coinLed = 13;


// ThreadController that will controll all threads with multi threhads
ThreadController controll = ThreadController();
//Thread radioListenerThread = Thread();
Thread buttonListenThread = Thread();
//Thread rf24Sender = Thread();
// 762
/// buttons public variables
const int button1Pin = 7;
int button1State = 0;
int button1Timer = 0;
bool wasButton1Pressed = false;
const int button2Pin = 6;
int button2State = 0;
int button2Timer = 0;
bool wasButton2Pressed = false;
const int button3Pin = 2;
int button3State = 0;
int button3Timer = 0;
bool wasButton3Pressed = false;

///////////////// Toch Sensor //////////////////
//int pressurePin = A1;
//int touch_force;
const int buttonPayPin = 13;     // the number of the pushbutton pin

///////////////// soft Toch Sensor //////////////////
//int softpotPin = A0; //analog pin 0
//int softpotReading ;
bool wasCarAllradyThere;

///////////////// LEDS  //////////////////
#define LED_PIN     5
#define NUM_LEDS    50
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];

///////////////// Static Light LED Strip Location Definisions  //////////////////
#define HACKER_LED_START 8
#define HACKER_LED_END 12
#define TECHNICIAN_LED_START 43
#define TECHNICIAN_LED_END 47
#define ANTENA_LED 6
#define PARKING_LEDS_START 17
#define PARKING_LEDS_END 22
//#define TOUCH_LED 0
//#define TOUCH_LEDS_END 0

#define TREE_1 48
#define TREE_2 16
#define TREE_3 13

///////////////// Moving Light LED Strip Location Definisions  //////////////////
#define HACKER_TO_ANTENA_LED 7
#define ANTENA_TO_CAR_LED_START 0
#define ANTENA_TO_CAR_LED_END 5
#define CAR_TO_TECHNICIAN_LED_START 36
#define CAR_TO_TECHNICIAN_LED_END 42
#define TECHNICIAN_TO_PARKING_LED_START 23
#define TECHNICIAN_TO_PARKING_LED_END 34

///////////////// Movie Times  //////////////////
#define TIME_FOR_EXPLANATION_MOVIE 116000
#define TIME_BEFORE_CAR_TURNNING_RED_UNPROTECTED 1000
#define TIME_FOR_FIRST_MOVIE_UNPROTECTED 24000
#define TIME_LIMIT_FOR_TOUCH 25000
#define TIME_FOR_SECOND_MOVIE_UNPROTECTED 19000
#define TIME_PROTECTED_MOVIE 58000

///////////////// Scenarios //////////////////
enum ScenarioState {
  STATIC_MOVIE = 0,
  PLAY_EXPLANATION_MOVIE = 1,
  UNPROTECTED_START = 3,
  UNPROTECTED_RANSOM = 4,
  UNPROTECTED_END = 5,
  PROTECTED = 2
};

void driveForward(int Phase, int Enable) {
  digitalWrite(Phase, LOW);
  analogWrite(Enable, MotorSpeed);
}

void driveReverse(int Phase, int Enable) {
  digitalWrite(Phase, LOW);
  analogWrite(Enable, MotorSpeed);
}

void driveStop(int Phase, int Enable) {
  digitalWrite(Phase, HIGH);
  analogWrite(Enable, 0);
}

// defines pins numbers
const int trigPin = 3;
const int echoPin = 4;

// defines variables
long duration;
int distance;

int sensorPin = A0;    
int sensorValue = 0;

bool testMode = false;

void setup() {
  Serial.begin(9600);
  //Serial.println("START");

  /// LEDs
  pinMode(carLedG, OUTPUT);
  pinMode(carLedR, OUTPUT);
    pinMode(coinLed, OUTPUT);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );
  for (int i = 0; i <= NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  TestLeds();
  FastLED.show();
  //

  /// Buttons
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
 // pinMode(buttonPayPin, INPUT);
  //digitalWrite(pressurePin, HIGH); //enable pullup resistor

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  /// Motor
  pinMode(MODE, OUTPUT);
  pinMode(APHASE, OUTPUT);
  pinMode(AENBL, OUTPUT);
  digitalWrite(MODE, HIGH); //set mode to high



  //rf24Sender.onRun(SendRfWhenPossible);
  //rf24Sender.setInterval(0);

  buttonListenThread.onRun(chackInputButtons);
  buttonListenThread.setInterval(0);

  // Adds both threads to the controller
  //controll.add(&rf24Sender);
  controll.add(&buttonListenThread);


  // driveForward(APHASE, AENBL);
  //driveReverse(APHASE, AENBL);
  TestLeds();
  FastLED.show();
 //  driveStop(APHASE, AENBL);
  //  driveReverse(APHASE, AENBL);
}


int getMsgFromSerial() {
  int msg =  -1;
  if (Serial.available()) {
    msg = Serial.read();
    PrintDebug(msg);
  }
  return msg;
}


void sendMsgToComputer(int msg) {
  Serial.write((byte*)&msg, sizeof(msg));
}
bool wasCarDetected = false;
int delayUntilCarHome = 400;
void loop() {
  //driveReverse(APHASE, AENBL);
  // if (wasCarThere()) {
  //  delay(400);
  //  driveStop(APHASE, AENBL);
  // delay(400);

  // }

  controll.run();
  //leds_off();
  // first button (PLAY_EXPLANATION_MOVIE)
  if (wasButton1Pressed) {
    // writeToRadio(1);
    wasButton1Pressed = false;
    PrintDebug("Button 1 pressed");
    PrintDebug("About Argus Movie");

    Serial.println(PLAY_EXPLANATION_MOVIE);

    unsigned long startedAt = millis();

    while (millis() - startedAt < TIME_FOR_EXPLANATION_MOVIE) {
  

    }
 
 
  }// first button if

  // second button if (UNPROTECTED)
  if (wasButton2Pressed) {

    // writeToRadio(2);
    wasButton2Pressed = false;
    wasCarDetected = false;
    PrintDebug("Button 2 pressed");
    PrintDebug("Unprotected Stage");
    driveReverse(APHASE, AENBL);
    digitalWrite(carLedR, LOW);
    digitalWrite(carLedG, HIGH);
    delay(1500);
    
    Serial.println(UNPROTECTED_START);
    delay(2000);
    /////////////////// start servo ////////////////////////

    unsigned long startedAt = millis();

    while (millis() - startedAt < TIME_FOR_FIRST_MOVIE_UNPROTECTED) {
      UnprotectedLedLoop(millis() - startedAt);
    }
    unsigned long startedAtInternal = millis();
    digitalWrite(carLedG, LOW);
    digitalWrite(carLedR, HIGH);
    driveStop(APHASE, AENBL);
    ///////////// wait for touch sensor to be triggered or time limit אם end (while(touch==LOW || TIME_LIMIT_FOR_TOUCH)
    //while ( millis() - startedAtInternal < TIME_LIMIT_FOR_TOUCH) {
    PrintDebug("Unprotected Ask Ransom");
    startedAtInternal = millis();
    //  int prsureRes = analogRead(pressurePin);
    bool buttonState = digitalRead(buttonPayPin);

    Serial.println(UNPROTECTED_RANSOM);
    //Serial.print("buttonState=");
    // Serial.println(buttonState);
    int prsureRes = analogRead(sensorPin); 
    while (prsureRes > 300 && millis() - startedAtInternal < TIME_LIMIT_FOR_TOUCH) {
      // Serial.print("buttonState=");
      // Serial.println(buttonState);
      //  PrintDebug("----------------------");
      UnprotectedKidnapLedLoop(millis() - startedAtInternal);
      prsureRes = analogRead(sensorPin); 
    }
    PrintDebug("analogRead(pressurePin)");

    // PrintDebug(prsureRes);

    PrintDebug("Unprotected Ransom Given");
    leds_off();
    //  writeToRadio(3);

    Serial.println(UNPROTECTED_END);
    PrintDebug("Unprotected senerio End Movie");
    unsigned long startedAtInternal2 = millis();
    while (millis() - startedAtInternal2 < TIME_FOR_SECOND_MOVIE_UNPROTECTED) {
      ////////////// start servo //////////////////
      UnprotectedEndLedLoop(millis() - startedAtInternal2);
    }
    leds_off();
    PrintDebug("Unprotected End ");
 driveStop(APHASE, AENBL);

  } // second button if
  // third button if (PROTECTED)
  if (wasButton3Pressed) {
    wasButton3Pressed = false;
    //  writeToRadio(4);
wasCarDetected = false;
    PrintDebug("Button 3 pressed");
    PrintDebug("Protected senerio Start");
    Serial.println(PROTECTED);
    unsigned long startedAt = millis();
    while (millis() - startedAt < TIME_PROTECTED_MOVIE) {
      /////////////////// start servo ////////////////////////
      ProtectedLedLoop(millis() - startedAt);
    }
    
    PrintDebug("Protected senerio Start");
    leds_off();
     driveStop(APHASE, AENBL);
  }// third button if

  decraseButtonTimers();
}

int protectedStage = 0;
int protectedRunningLED = 0;
int protectedRunningLEDTime = 0;
int protectedRunningLEDMaxTime = 3000;
bool protectedWasLightOn = false;


unsigned long timeCarDetected;
void ProtectedLedLoop(unsigned long secenarioTime) {
  //leds[Car_LED] = CRGB::Green;
  if (secenarioTime <= 1000) {
    wasCarDetected = false;
    //MotorSpeed = 238;
    digitalWrite(carLedR, LOW);
    digitalWrite(carLedG, HIGH);
    driveReverse(APHASE, AENBL);
    protectedStage = 0;
    wasCarAllradyThere = false;
  }
  if (protectedStage == 0 && secenarioTime >= 2000) {
    //  PrintDebug("Car starts");
    protectedStage = 1;
    leds_off();
    PrintDebug("Stage Haker");
    //Serial.println("Stage 1");
    for (int i = HACKER_LED_START; i <= HACKER_LED_END; i++) {
      leds[i] = CRGB::Red;
    }
    FastLED.show();
    protectedRunningLEDTime = 0;
    protectedRunningLED = 0;
    PrintDebug("Stage Haker To Antena");
  }

  if ((protectedStage == 2 || protectedStage == 1) && secenarioTime >= 6000) {
    protectedStage = 2;
    //leds_off();
    protectedRunningLEDTime++;
    if ( protectedRunningLEDTime >= 5000) {
      if (protectedWasLightOn) {
        //   PrintDebug("Light On ");
        leds[HACKER_TO_ANTENA_LED] = CRGB::Red;
        FastLED.show();
      }
      else {
        //  PrintDebug("Light Off ");
        leds[HACKER_TO_ANTENA_LED] = CRGB::Black;
        FastLED.show();
      }
      protectedRunningLEDTime = 0;
      protectedWasLightOn = !protectedWasLightOn;
    }
  }

  if (protectedStage == 2 && secenarioTime >= 10000 ) {
    protectedStage = 3;
    PrintDebug("Stage Antena");
    leds[HACKER_TO_ANTENA_LED] = CRGB::Black;
    protectedStage = 4;
    leds[ANTENA_LED] = CRGB::Red;
    FastLED.show();
    protectedRunningLEDTime = 0;
    // protectedRunningLED = 0;
    protectedRunningLED = ANTENA_TO_CAR_LED_END;


  }

  if ((protectedStage == 3 || protectedStage == 4) && secenarioTime >= 14000) {
    protectedStage = 4;
    if (protectedRunningLEDTime >= 5000) {
      ///leds_off();
      protectedRunningLEDTime = 0;
      protectedRunningLED--;
      if ( protectedRunningLED < ANTENA_TO_CAR_LED_START) {
        protectedRunningLED = ANTENA_TO_CAR_LED_END ;
      }
      PrintDebug("Runing LED");
      PrintDebug( protectedRunningLED);
      for (int i = ANTENA_TO_CAR_LED_START; i <= ANTENA_TO_CAR_LED_END; i++) {
        leds[i] = CRGB::Black;
      }
      leds[protectedRunningLED] = CRGB::Red;
      if (!(protectedRunningLED + 1 <= ANTENA_TO_CAR_LED_START || protectedRunningLED + 1 >= ANTENA_TO_CAR_LED_END)) {
        leds[protectedRunningLED + 1] = CRGB::Red;
      }

      FastLED.show();

    }
    protectedRunningLEDTime++;

  }
  if (protectedStage == 4 && secenarioTime >= 17900) {
    protectedRunningLED = CAR_TO_TECHNICIAN_LED_START;
    leds_off();
    //  PrintDebug("Stage Antena To Car 4");
    protectedStage = 5;


  }
  if ((protectedStage == 6 || protectedStage == 5) && secenarioTime >= 21000 && secenarioTime <= 26000) {
    protectedStage = 6;
    if (protectedRunningLEDTime >= protectedRunningLEDMaxTime) {
      for (int i = CAR_TO_TECHNICIAN_LED_START; i <= CAR_TO_TECHNICIAN_LED_END; i++) {
        leds[i] = CRGB::Black;
      }
      protectedRunningLEDTime = 0;
      protectedRunningLED++;
      if ( protectedRunningLED > CAR_TO_TECHNICIAN_LED_END) {
        protectedRunningLED = CAR_TO_TECHNICIAN_LED_START;
      }
      leds[protectedRunningLED] = CRGB::Yellow;
      if (!(protectedRunningLED + 1 <= CAR_TO_TECHNICIAN_LED_START || protectedRunningLED + 1 >= CAR_TO_TECHNICIAN_LED_END)) {
        leds[protectedRunningLED + 1] = CRGB::Yellow;
      }

      FastLED.show();

    }
    if (secenarioTime >= 25970) {
      leds_off();
    }
    protectedRunningLEDTime++;
  }
  if (protectedStage == 6 && secenarioTime >= 26000) {
    leds_off();
    protectedStage = 7;
    PrintDebug("Stage Technision");
    //Serial.println("Stage 1");
    for (int i = TECHNICIAN_LED_START; i <= TECHNICIAN_LED_END; i++) {
      leds[i] = CRGB::Green;
    }
    FastLED.show();
    protectedRunningLEDTime = 0;
    protectedRunningLED = TECHNICIAN_TO_PARKING_LED_END;

  }
  if ((protectedStage == 8 || protectedStage == 7) && secenarioTime >= 38000 && secenarioTime <= 44000) {
    protectedStage = 8;
    // PrintDebug("Technision to parking");
    if (protectedRunningLEDTime >= protectedRunningLEDMaxTime) {
      for (int i = TECHNICIAN_TO_PARKING_LED_START; i <= TECHNICIAN_TO_PARKING_LED_END; i++) {
        leds[i] = CRGB::Black;
      }
      // leds_off();
      protectedRunningLEDTime = 0;
      protectedRunningLED--;
      if ( protectedRunningLED < TECHNICIAN_TO_PARKING_LED_START) {
        protectedRunningLED = TECHNICIAN_TO_PARKING_LED_END;
      }
      PrintDebug("Runing LED");
      PrintDebug( protectedRunningLED);
      leds[protectedRunningLED] = CRGB::Green;
      leds[protectedRunningLED + 1] = CRGB::Green;
      FastLED.show();

    }

    protectedRunningLEDTime++;

  }
  if (protectedStage == 8 && secenarioTime >= 44000) {

    //leds_off();
    for (int i = TECHNICIAN_TO_PARKING_LED_START; i <= TECHNICIAN_TO_PARKING_LED_END; i++) {
      leds[i] = CRGB::Black;
    }
    for (int i = TECHNICIAN_LED_START; i <= TECHNICIAN_LED_END; i++) {
      leds[i] = CRGB::Green;
    }
    protectedStage = 9;

    PrintDebug("Stage Parkin Lot");
    for (int i = PARKING_LEDS_START; i <= PARKING_LEDS_END; i++) {
      leds[i] = CRGB::Green;
    }
    FastLED.show();

  }
  if (protectedStage == 9 && secenarioTime >= 55000) {
   
     digitalWrite(carLedG, LOW);
    leds_off();
    protectedStage = 10;
    PrintDebug("Stage 6");
    protectedRunningLEDTime = 0;
    protectedRunningLED = 0;

  }

   if (protectedStage == 10 && secenarioTime >= TIME_PROTECTED_MOVIE - 2000 ) {
   
    digitalWrite(carLedG, LOW);
    leds_off();
    protectedStage = 11;
    PrintDebug("Stage 6");
    protectedRunningLEDTime = 0;
    protectedRunningLED = 0;

  }

  /// Stop Car At Spot
  if (secenarioTime >= TIME_PROTECTED_MOVIE - 13000 && !wasCarAllradyThere){ //&&
       // !(protectedStage == 8 || protectedStage == 7) ) {
    if (wasCarThere() ) {
      // delay(4400);

      wasCarAllradyThere = true;
      wasCarDetected = true;
      timeCarDetected = secenarioTime;
      // delay(400);

    }
  }
  if (wasCarDetected == true && secenarioTime - timeCarDetected >= 5050) {
    wasCarDetected = false;
    driveStop(APHASE, AENBL);
    //MotorSpeed = 254;
  }

}

int unprotectedStage = 0;
int unprotectedRunningLED = 0;
int unprotectedRunningLEDTime = 0;
int unprotectedRunningLEDMaxTime = 2000;
long lastTimeInProtected;
void UnprotectedLedLoop(unsigned long secenarioTime) {


  if (secenarioTime <= 550 && secenarioTime >= 500) {
    leds_off();
    unprotectedStage = 0;
    wasCarAllradyThere = false;
    
    wasCarDetected = false;
    PrintDebug("Car starts");
    digitalWrite(carLedR, LOW);
    digitalWrite(carLedG, HIGH);
    driveReverse(APHASE, AENBL);
  }
  if (unprotectedStage == 0 && secenarioTime >= 4000) {
    unprotectedStage = 1;
    PrintDebug("unprotected Stage 1");
  }
  if (unprotectedStage == 1 && secenarioTime >= 8000) {
    unprotectedStage = 2;
    //PrintDebug("Stage 2");
    /// Car Moving

    for (int i = HACKER_LED_START; i <= HACKER_LED_END; i++) {
      leds[i] = CRGB::Red;
    }
    FastLED.show();
  }
  /// Stop Car At Spot

 
  if ((unprotectedStage == 2 || unprotectedStage == 3)
      && secenarioTime >= 12000 && secenarioTime <= 16000 ) {
        
    unprotectedStage = 3;
    // PrintDebug("Haker To Antena");
    unprotectedRunningLEDTime++;
    if ( unprotectedRunningLEDTime >= 4000) {
      if (protectedWasLightOn) {
        PrintDebug("Light On ");
        // leds[HACKER_TO_ANTENA_LED] = CRGB::Red;
        leds[HACKER_TO_ANTENA_LED] = CRGB::Red;
        FastLED.show();
      }
      else {
        PrintDebug("Light Off ");
        leds[HACKER_TO_ANTENA_LED] = CRGB::Black;
        FastLED.show();
      }
      unprotectedRunningLEDTime = 0;
      protectedWasLightOn = !protectedWasLightOn;
    }
  }
  if (unprotectedStage == 3 && secenarioTime >= 15700) {
    PrintDebug("Antena");
    // leds_off();
    unprotectedStage = 4;
    leds[ANTENA_LED] = CRGB::Red;
    leds[HACKER_TO_ANTENA_LED] = CRGB::Black;
    FastLED.show();
    unprotectedRunningLED = ANTENA_TO_CAR_LED_END;
    lastTimeInProtected = secenarioTime;
  }
  if ((unprotectedStage == 4 || unprotectedStage == 5) &&
      secenarioTime >= 19000 && secenarioTime <= 23000) {
        long diff = lastTimeInProtected - secenarioTime;
        lastTimeInProtected = secenarioTime;
    /// Change car lights to red
    unprotectedRunningLEDTime ++;
    //  PrintDebug("Stage Antena To Car");
    unprotectedStage = 5;
    if (unprotectedRunningLEDTime >= 4000) {
      unprotectedRunningLEDTime = 0;

      if ( unprotectedRunningLED < ANTENA_TO_CAR_LED_START) {
        unprotectedRunningLED = ANTENA_TO_CAR_LED_END ;
      }
      PrintDebug("Runing LED");
      PrintDebug( unprotectedRunningLED);
      for (int i = ANTENA_TO_CAR_LED_START; i <= ANTENA_TO_CAR_LED_END; i++) {
        leds[i] = CRGB::Black;
      }
      leds[unprotectedRunningLED] = CRGB::Red;
      if (!(unprotectedRunningLED + 1 <= ANTENA_TO_CAR_LED_START || unprotectedRunningLED + 1 >= ANTENA_TO_CAR_LED_END)) {
        leds[unprotectedRunningLED + 1] = CRGB::Red;
      }
      unprotectedRunningLED--;
      FastLED.show();

    }



    // FastLED.show();

  }
  if (unprotectedStage == 5 && secenarioTime >= 23000) {
    leds_off();
  //  leds[TOUCH_LED] = CRGB::Red;
    FastLED.show();
    //
    unprotectedStage = 6;

  }

   if ( secenarioTime <= 220000 && secenarioTime >= 16000 && !wasCarAllradyThere) {
    if (wasCarThere() ) {
      wasCarAllradyThere = true;
      wasCarDetected = true;
      timeCarDetected = secenarioTime;
    }
  }
  if (wasCarDetected == true && secenarioTime - timeCarDetected >= 690) {
    digitalWrite(carLedG, LOW);
    digitalWrite(carLedR, HIGH);
    wasCarDetected = false;
    driveStop(APHASE, AENBL);
  }
 


}

int unprotectedKidnapStage = 0;
int unprotectedKidnapRunningLED = 0;
int unprotectedKidnapRunningLEDTime = 0;
int unprotectedKidnapRunningLEDMaxTime = 2000;
void UnprotectedKidnapLedLoop(unsigned long secenarioTime) {

  if (secenarioTime <= 1000) {
    unprotectedKidnapStage = 0;
  digitalWrite(coinLed, HIGH);
       
   

  }
  if ((unprotectedKidnapStage == 0 || unprotectedKidnapStage == 1)) {
    unprotectedKidnapStage = 1;
  }
  if (unprotectedKidnapStage == 1 && secenarioTime >= 49000) {
    unprotectedKidnapStage = 3;
  }


}

int unprotectedEndStage = 0;
int unprotectedEndRunningLED = 0;
int unprotectedEndRunningLEDTime = 0;
int unprotectedEndRunningLEDMaxTime = 2000;
void UnprotectedEndLedLoop(unsigned long secenarioTime) {


  if (secenarioTime <= 1000) {
    unprotectedEndStage = 0;
    leds_off();
    PrintDebug("Unprotected End Stage 1");
      digitalWrite(coinLed, LOW);
    driveReverse(APHASE, AENBL);
    
    digitalWrite(carLedR, LOW);
    digitalWrite(carLedG, HIGH);
  }
  if (unprotectedEndStage == 0 && secenarioTime >= 4300
      && secenarioTime <= 4400) {
    driveStop(APHASE, AENBL);
    digitalWrite(carLedG, LOW);
    unprotectedEndStage = 1;
  }
  if (unprotectedEndStage == 1 && secenarioTime >= 49000) {
    unprotectedEndStage = 2;
  }
   if (unprotectedEndStage == 2 && secenarioTime >= TIME_FOR_SECOND_MOVIE_UNPROTECTED - 5000) {
    unprotectedEndStage = 3;
  }


}



int globalDistance;

bool wasCarThere() {
  int distance;
  distance = chackDistance();
  // Prints the distance on the Serial Monitor
  if (distance >= 3 && distance <= 7) {
   delay(20);
    distance = chackDistance();
    if (distance >= 3 && distance <= 7) {
      //delay(20);
      return true;
      distance = chackDistance();
      if (distance >= 3 && distance <= 7) {
        return true;
      }
    }

  }
  return false;



}


int chackDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;

  // Prints the distance on the Serial Monitor
  // Serial.print("Distance: ");
  //Serial.println(distance);
  globalDistance = distance;
  return distance;

}

int startTimer  = 5000;
// callback for inputButtonThread
void chackInputButtons() {

  //chackDistance();
  //touch_force = analogRead(pressurePin);
  //Serial.println(touch_force);
  //delay(300);
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

void PrintDebug(const char * text) {
  if (testMode) {
    Serial.print(text );
    Serial.println("");

  }


}

void PrintDebug(int num) {
  if (testMode) {
    Serial.print(num );
    Serial.println("");

  }


}
////////// shut down all leds //////////
void leds_off() {
  for (int i = TECHNICIAN_TO_PARKING_LED_START; i <= TECHNICIAN_TO_PARKING_LED_END; i++) {
    leds[i] = CRGB::Black;
  }
  for (int i = CAR_TO_TECHNICIAN_LED_START; i <= CAR_TO_TECHNICIAN_LED_END; i++) {
    leds[i] = CRGB::Black;
  }
  for (int i = ANTENA_TO_CAR_LED_START; i <= ANTENA_TO_CAR_LED_END; i++) {
    leds[i] = CRGB::Black;
  }
  leds[ANTENA_LED] = CRGB::Black;
  leds[HACKER_TO_ANTENA_LED] = CRGB::Black;
//  leds[TOUCH_LED] = CRGB::Black;
  leds[TREE_1] = CRGB::White;
  leds[TREE_2] = CRGB::White;
  leds[TREE_3] = CRGB::White;
  TestLeds();
  //FastLED.show();
}


void TestLeds() {

  for (int i = TECHNICIAN_TO_PARKING_LED_START; i <= TECHNICIAN_TO_PARKING_LED_END; i++) {
    //leds[i] = CRGB::Red;
  }
  // FastLED.show();

  for (int i = HACKER_LED_START; i <= HACKER_LED_END; i++) {
    leds[i] = CRGB::White;
  }

  for (int i = CAR_TO_TECHNICIAN_LED_START; i <= CAR_TO_TECHNICIAN_LED_END; i++) {
    //leds[i] = CRGB::Blue;
  }

  for (int i = PARKING_LEDS_START; i <= PARKING_LEDS_END; i++) {
    leds[i] = CRGB::White;
  }

  for (int i = TECHNICIAN_LED_START; i <= TECHNICIAN_LED_END; i++) {
    leds[i] = CRGB::White;
  }

  for (int i = ANTENA_TO_CAR_LED_START; i <= ANTENA_TO_CAR_LED_END; i++) {
    // leds[i] = CRGB::Purple;
  }
 // leds[TREE_1] = CRGB::White;
  leds[TREE_2] = CRGB::White;
  leds[TREE_3] = CRGB::White;

  // leds[ANTENA_LED] = CRGB::Blue;
  // leds[HACKER_TO_ANTENA_LED] = CRGB::Red;
  // leds[TOUCH_LED] = CRGB::Brown;

  FastLED.show();


}

