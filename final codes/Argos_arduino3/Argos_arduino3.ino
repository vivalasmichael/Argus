#include <FastLED.h>
#include <Thread.h>
#include <ThreadController.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

/// Motor Vars
int MODE = 8;
int APHASE = 9;
int AENBL = 10;
// motor speed is any number between 0 and 255
int MotorSpeed = 254;

/// Leds public variables
int carLedG = 12;
int carLedR = 11;

// ThreadController that will controll all threads with multi threhads
ThreadController controll = ThreadController();
Thread radioListenerThread = Thread();
Thread buttonListenThread = Thread();
Thread rf24Sender = Thread();
// 762
/// buttons public variables
const int button1Pin = 2;
int button1State = 0;
int button1Timer = 0;
bool wasButton1Pressed = false;
const int button2Pin = 6;
int button2State = 0;
int button2Timer = 0;
bool wasButton2Pressed = false;
const int button3Pin = 7;
int button3State = 0;
int button3Timer = 0;
bool wasButton3Pressed = false;

///////////////// Toch Sensor //////////////////
int pressurePin = A1;
int touch_force;

///////////////// soft Toch Sensor //////////////////
//int softpotPin = A0; //analog pin 0
//int softpotReading ;

///////////////// LEDS  //////////////////
#define LED_PIN     5
#define NUM_LEDS    50
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];

///////////////// Static Light LED Strip Location Definisions  //////////////////
#define HACKER_LED_START 9
#define HACKER_LED_END 13
#define TECHNICIAN_LED_START 44
#define TECHNICIAN_LED_END 48
#define ANTENA_LED 7
#define PARKING_LEDS_START 18
#define PARKING_LEDS_END 23
#define TOUCH_LED 0
#define TOUCH_LEDS_END 0

#define TREE_1 49
#define TREE_2 17
#define TREE_3 14

///////////////// Moving Light LED Strip Location Definisions  //////////////////
#define HACKER_TO_ANTENA_LED 8
#define ANTENA_TO_CAR_LED_START 1
#define ANTENA_TO_CAR_LED_END 6
#define CAR_TO_TECHNICIAN_LED_START 37
#define CAR_TO_TECHNICIAN_LED_END 43
#define TECHNICIAN_TO_PARKING_LED_START 24
#define TECHNICIAN_TO_PARKING_LED_END 35

///////////////// Movie Times  //////////////////
#define TIME_FOR_EXPLANATION_MOVIE 100000
#define TIME_BEFORE_CAR_TURNNING_RED_UNPROTECTED 1000
#define TIME_FOR_FIRST_MOVIE_UNPROTECTED 24000
#define TIME_LIMIT_FOR_TOUCH 25000
#define TIME_FOR_SECOND_MOVIE_UNPROTECTED 8000
#define TIME_PROTECTED_MOVIE 50000

///////////////// Scenarios //////////////////
enum ScenarioState {
  STATIC_MOVIE = 0,
  PLAY_EXPLANATION_MOVIE = 1,
  UNPROTECTED_START = 3,
  UNPROTECTED_END = 4,
  PROTECTED = 2
};

void driveForward(int Phase, int Enable) {
  digitalWrite(Phase, LOW);
  analogWrite(Enable, MotorSpeed);
}

void driveReverse(int Phase, int Enable) {
  digitalWrite(Phase, HIGH);
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


bool testMode = false;

void setup() {
  Serial.begin(9600);
  //Serial.println("START");

  /// LEDs
  pinMode(carLedG, OUTPUT);
  pinMode(carLedR, OUTPUT);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );

  /// Buttons
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
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

  TestLeds();
  // driveForward(APHASE, AENBL);
  //driveReverse(APHASE, AENBL);
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
    PrintDebug("Message Not Recived Fromn PC and we break from timeout");
  }// first button if

  // second button if (UNPROTECTED)
  if (wasButton2Pressed) {

    // writeToRadio(2);
    wasButton2Pressed = false;
    PrintDebug("Button 2 pressed");
    PrintDebug("Unprotected Stage");
    Serial.println(UNPROTECTED_START);
    /////////////////// start servo ////////////////////////

    unsigned long startedAt = millis();

    while (millis() - startedAt < TIME_FOR_FIRST_MOVIE_UNPROTECTED) {
      UnprotectedLedLoop(millis() - startedAt);
    }
    unsigned long startedAtInternal = millis();
    driveStop(APHASE, AENBL);
    ///////////// wait for touch sensor to be triggered or time limit אם end (while(touch==LOW || TIME_LIMIT_FOR_TOUCH)
    //while ( millis() - startedAtInternal < TIME_LIMIT_FOR_TOUCH) {
    PrintDebug("Unprotected Ask Ransom");
    startedAtInternal = millis();
    int prsureRes = analogRead(pressurePin);
    while (prsureRes < 1000 && millis() - startedAtInternal < TIME_LIMIT_FOR_TOUCH) {
      //  PrintDebug("----------------------");
      UnprotectedKidnapLedLoop(millis() - startedAtInternal);
      prsureRes = analogRead(pressurePin);

    }
    PrintDebug("analogRead(pressurePin)");

    PrintDebug(prsureRes);

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


  } // second button if
  // third button if (PROTECTED)
  if (wasButton3Pressed) {
    wasButton3Pressed = false;
    //  writeToRadio(4);

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
  }// third button if

  decraseButtonTimers();
}

int protectedStage = 0;
int protectedRunningLED = 0;
int protectedRunningLEDTime = 0;
int protectedRunningLEDMaxTime = 3000;
bool protectedWasLightOn = false;
bool wasCarAllradyThere;

void ProtectedLedLoop(unsigned long secenarioTime) {
  //leds[Car_LED] = CRGB::Green;
  if (secenarioTime <= 1000) {
    protectedStage = 0;
    wasCarAllradyThere = false;
  }
  if (protectedStage == 0 && secenarioTime >= 8000) {
    PrintDebug("Car starts");
    digitalWrite(carLedG, HIGH);
    driveReverse(APHASE, AENBL);
    protectedStage = 1;
  }
  /// Stop Car At Spot
  if (protectedStage <= 5 && protectedStage >= 1 && secenarioTime <= 20000) {
    if (wasCarThere() && !wasCarAllradyThere) {
      delay(400);
      driveStop(APHASE, AENBL);
      wasCarAllradyThere = true;
      // delay(400);

    }
  }
  if (protectedStage == 1 && secenarioTime >= 12000) {

    leds_off();
    protectedStage = 2;
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
  if (protectedStage == 1 && secenarioTime >= 12000) {


  }
  if ((protectedStage == 2 || protectedStage == 3) && secenarioTime >= 16000 ) {
    //  PrintDebug("Haker To Antena");
    protectedStage = 3;
    protectedRunningLEDTime++;
    if ( protectedRunningLEDTime >= 10) {
      if (protectedWasLightOn) {
        //   PrintDebug("Light On ");
        // leds[HACKER_TO_ANTENA_LED] = CRGB::Red;
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

  if (protectedStage == 3 && secenarioTime >= 20000) {

    PrintDebug("Stage Antena");
    leds_off();
    protectedStage = 4;
    leds[ANTENA_LED] = CRGB::Red;
    FastLED.show();
    protectedRunningLEDTime = 0;
    // protectedRunningLED = 0;
    protectedRunningLED = ANTENA_TO_CAR_LED_END;
  }
  if ((protectedStage == 5 || protectedStage == 4) && secenarioTime >= 24000 && secenarioTime <= 28000) {
    //  PrintDebug("Stage Antena To Car 4");
    protectedStage = 5;
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

    if (secenarioTime >= 27800) {
      protectedRunningLED = CAR_TO_TECHNICIAN_LED_START;
      leds_off();
    }
    if (secenarioTime >= 27600 && secenarioTime <= 27750) {
      driveReverse(APHASE, AENBL);
    }
  }
  if ((protectedStage == 6 || protectedStage == 5) && secenarioTime >= 32500 && secenarioTime <= 36000) {
    protectedStage = 6;
    if (protectedRunningLEDTime >= protectedRunningLEDMaxTime) {
      leds_off();
      protectedRunningLEDTime = 0;
      protectedRunningLED++;
      if ( protectedRunningLED > CAR_TO_TECHNICIAN_LED_END) {
        protectedRunningLED = CAR_TO_TECHNICIAN_LED_START;
      }
      // PrintDebug("Runing LED");
      //  PrintDebug( protectedRunningLED);
      leds[protectedRunningLED] = CRGB::Red;
      if (!(protectedRunningLED + 1 <= CAR_TO_TECHNICIAN_LED_START || protectedRunningLED + 1 >= CAR_TO_TECHNICIAN_LED_END)) {
        leds[protectedRunningLED + 1] = CRGB::Red;
      }

      FastLED.show();

    }
    if (secenarioTime >= 35800) {
      leds_off();
    }
    protectedRunningLEDTime++;
    if ( secenarioTime >= 32500 && secenarioTime <= 32700) {

      driveStop(APHASE, AENBL);
    }


  }
  if (protectedStage == 6 && secenarioTime >= 36000) {
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
  if ((protectedStage == 8 || protectedStage == 7) && secenarioTime >= 40000 && secenarioTime <= 44000) {

    protectedStage = 8;
    // PrintDebug("Technision to parking");
    if (protectedRunningLEDTime >= protectedRunningLEDMaxTime) {
      leds_off();
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
    if (secenarioTime >= 43800) {
      leds_off();
    }

  }
  if (protectedStage == 8 && secenarioTime >= 44000) {
    digitalWrite(carLedG, LOW);
    leds_off();
    protectedStage = 9;

    PrintDebug("Stage Parkin Lot");
    for (int i = PARKING_LEDS_START; i <= PARKING_LEDS_END; i++) {
      leds[i] = CRGB::Green;
    }
    FastLED.show();

  }
  if (protectedStage == 9 && secenarioTime >= 49000) {

    // digitalWrite(carLedG, LOW);
    leds_off();
    protectedStage = 10;
    PrintDebug("Stage 6");
    protectedRunningLEDTime = 0;
    protectedRunningLED = 0;

  }

}

int unprotectedStage = 0;
int unprotectedRunningLED = 0;
int unprotectedRunningLEDTime = 0;
int unprotectedRunningLEDMaxTime = 2000;
void UnprotectedLedLoop(unsigned long secenarioTime) {


  if (secenarioTime <= 700 && secenarioTime >= 500) {
    leds_off();
    unprotectedStage = 0;
    wasCarAllradyThere = false;
  }
  if (unprotectedStage == 0 && secenarioTime >= 4000) {
    unprotectedStage = 1;
    PrintDebug("unprotected Stage 1");
  }
  if (unprotectedStage == 1 && secenarioTime >= 8000) {
    unprotectedStage = 2;
    //PrintDebug("Stage 2");
    /// Car Moving
    PrintDebug("Car starts");
    digitalWrite(carLedG, HIGH);
    driveReverse(APHASE, AENBL);
    for (int i = HACKER_LED_START; i <= HACKER_LED_END; i++) {
      leds[i] = CRGB::Red;
    }
    FastLED.show();
  }
  /// Stop Car At Spot
  if (unprotectedStage <= 5 && unprotectedStage >= 1 && secenarioTime <= 20000) {
    if (wasCarThere() && !wasCarAllradyThere) {
      delay(400);
      digitalWrite(carLedR, HIGH);
      digitalWrite(carLedG, LOW);
      driveStop(APHASE, AENBL);
      wasCarAllradyThere = true;
    }
  }
  if ((unprotectedStage == 2 || unprotectedStage == 3)
      && secenarioTime >= 12000 && secenarioTime <= 16000 ) {
    unprotectedStage = 3;
    // PrintDebug("Haker To Antena");
    unprotectedRunningLEDTime++;
    if ( unprotectedRunningLEDTime >= 25) {
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
  if (unprotectedStage == 3 && secenarioTime >= 16000) {
    PrintDebug("Antena");
    leds_off();
    unprotectedStage = 4;
    leds[ANTENA_LED] = CRGB::Red;
    leds[HACKER_TO_ANTENA_LED] = CRGB::Black;
    FastLED.show();
    unprotectedRunningLED = ANTENA_TO_CAR_LED_END;
  }
  if ((unprotectedStage == 4 || unprotectedStage == 5) && secenarioTime >= 20000 && secenarioTime <= 24000) {
    /// Change car lights to red
    unprotectedRunningLEDTime++;
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


    if (secenarioTime >= 23800) {
      leds_off();
    }
    // FastLED.show();

  }
  if (unprotectedStage == 5 && secenarioTime >= 24000) {
    leds_off();
    unprotectedStage = 6;

  }
  if (unprotectedStage == 6 && secenarioTime >= 25000) {
    //leds_off();
    unprotectedStage = 7;

  }


}

int unprotectedKidnapStage = 0;
int unprotectedKidnapRunningLED = 0;
int unprotectedKidnapRunningLEDTime = 0;
int unprotectedKidnapRunningLEDMaxTime = 2000;
void UnprotectedKidnapLedLoop(unsigned long secenarioTime) {

  if (secenarioTime <= 1000) {
    unprotectedKidnapStage = 0;
    leds[TOUCH_LED] = CRGB::Red;
    FastLED.show();
  }
  if (unprotectedKidnapStage == 0 && secenarioTime >= 4000) {
    unprotectedKidnapStage = 1;
    PrintDebug("Kidnap Stage 1");
  }

  if (unprotectedKidnapStage == 1 && secenarioTime >= 8000) {
    leds_off();
    unprotectedKidnapStage = 2;
  }
  if (unprotectedKidnapStage == 2 && secenarioTime >= 49000) {
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
    driveReverse(APHASE, AENBL);
    digitalWrite(carLedR, LOW);
  }
  if (unprotectedEndStage == 0 && secenarioTime >= 5000
      && secenarioTime <= 5200) {
    driveStop(APHASE, AENBL);
    unprotectedEndStage = 1;
  }
  if (unprotectedEndStage == 1 && secenarioTime >= 49000) {
    unprotectedEndStage = 3;
  }


}


////////// shut down all leds //////////
void leds_off() {
  for (int i = 0; i <= NUM_LEDS; i++) {
    if (i == TREE_1 || i == TREE_2 || i == TREE_3) {

    } else {
      leds[i] = CRGB::Black;
    }


  }
  FastLED.show();
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
      delay(20);
      //return true;
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



void TestLeds() {

  for (int i = TECHNICIAN_TO_PARKING_LED_START; i <= TECHNICIAN_TO_PARKING_LED_END; i++) {
    leds[i] = CRGB::Red;
  }
  FastLED.show();

  for (int i = HACKER_LED_START; i <= HACKER_LED_END; i++) {
    leds[i] = CRGB::White;
  }

  for (int i = CAR_TO_TECHNICIAN_LED_START; i <= CAR_TO_TECHNICIAN_LED_END; i++) {
    leds[i] = CRGB::Blue;
  }

  for (int i = PARKING_LEDS_START; i <= PARKING_LEDS_END; i++) {
    leds[i] = CRGB::Magenta;
  }

  for (int i = TECHNICIAN_LED_START; i <= TECHNICIAN_LED_END; i++) {
    leds[i] = CRGB::Orange;
  }

  for (int i = ANTENA_TO_CAR_LED_START; i <= ANTENA_TO_CAR_LED_END; i++) {
    leds[i] = CRGB::Purple;
  }
  leds[TREE_1] = CRGB::White;
  leds[TREE_2] = CRGB::White;
  leds[TREE_3] = CRGB::White;

  leds[ANTENA_LED] = CRGB::Blue;
  leds[HACKER_TO_ANTENA_LED] = CRGB::Red;
  leds[TOUCH_LED] = CRGB::Brown;

  FastLED.show();


}

