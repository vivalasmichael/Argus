#include <FastLED.h>
#include <Thread.h>
#include <ThreadController.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


RF24 radio(9, 10);
const byte addresses[][6] = {"00001", "00002"};

// ThreadController that will controll all threads with multi threhads
ThreadController controll = ThreadController();
Thread radioListenerThread = Thread();
Thread buttonListenThread = Thread();
Thread rf24Sender = Thread();

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
int pressurePin = A1;
int touch_force;

///////////////// soft Toch Sensor //////////////////
int softpotPin = A0; //analog pin 0
int softpotReading ;

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
#define TIME_FOR_EXPLANATION_MOVIE 5000
#define TIME_BEFORE_CAR_TURNNING_RED_UNPROTECTED 1000
#define TIME_FOR_FIRST_MOVIE_UNPROTECTED 24000
#define TIME_LIMIT_FOR_TOUCH 25000
#define TIME_FOR_SECOND_MOVIE_UNPROTECTED 1000
#define TIME_PROTECTED_MOVIE 50000

///////////////// Scenarios //////////////////
enum ScenarioState {
  STATIC_MOVIE = 0,
  PLAY_EXPLANATION_MOVIE = 1,
  UNPROTECTED_START = 3,
  UNPROTECTED_END = 4,
  PROTECTED = 2
};




bool testMode = true;


void setup() {
  Serial.begin(9600);

  //Serial.println("START");

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  digitalWrite(softpotPin, HIGH); //enable pullup resistor


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
  leds[TREE_1] = CRGB::Green;
  leds[TREE_2] = CRGB::Green;
  leds[TREE_3] = CRGB::Green;

  leds[ANTENA_LED] = CRGB::Blue;
  leds[HACKER_TO_ANTENA_LED] = CRGB::Red;
  leds[TOUCH_LED] = CRGB::Brown;

  FastLED.show();

  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.stopListening();
  //radio.setAutoAck(false);
  //radio.setPALevel(RF24_PA_LOW);
  PrintDebug("is Chip Connected = " );
  PrintDebug(radio.isChipConnected());
  //radio.printDetails();
  // Configure Threads
  // radioListenerThread.onRun(chackRadioForInput);
  //radioListenerThread.setInterval(50);

  rf24Sender.onRun(SendRfWhenPossible);
  rf24Sender.setInterval(0);

  buttonListenThread.onRun(chackInputButtons);
  buttonListenThread.setInterval(0);



  // Adds both threads to the controller
  controll.add(&rf24Sender);
  controll.add(&buttonListenThread);
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


void sendMsgToRadio(int msg) {
  radio.write(&msg, sizeof(msg));
  //delay(100);
}
const int caMaxResend = 100;
int caResendLeft = 0;
uint32_t globalMessege = 0;
bool outMessageAvilable = false;
void writeToRadio(int messege) {
  Serial.println("--------------------------------------");
  Serial.println("Sending Message");
  radio.openWritingPipe(addresses[0]);
  radio.stopListening();
  bool writeRslt = radio.write(&messege, sizeof(messege));

  if (!writeRslt) {
    if (caResendLeft > 0) {
      Serial.print("Ack Not Recived -> resending for time : ");
      Serial.println(caResendLeft);
      Serial.print("Waiting For : ");
    //  Serial.println(randBackoff);
      //delay(randBackoff);
      outMessageAvilable = true;
      caResendLeft--;
    } else {
      outMessageAvilable = false;
      Serial.println("Ack Not Recived 20 times Msg Failed");
    //  ReturnToListen();
    }

  } else {
    outMessageAvilable = false;
    Serial.println("Ack Recived");
   // ReturnToListen();
    Serial.println(messege);
  }
}

void SendRfWhenPossible() {
  if (outMessageAvilable) {
    writeToRadio(globalMessege);
  }
}






void loop() {
  controll.run();
  //leds_off();
  // first button (PLAY_EXPLANATION_MOVIE)
  if (wasButton1Pressed) {
    writeToRadio(1);
    wasButton1Pressed = false;
    PrintDebug("Button 1 pressed");
    Serial.println(PLAY_EXPLANATION_MOVIE);
    unsigned long startedAt = millis();
    while (millis() - startedAt < TIME_FOR_EXPLANATION_MOVIE) {

    }
   // PrintDebug("Message Not Recived Fromn PC and we break from timeout");
  }// first button if

  // second button if (UNPROTECTED)
  if (wasButton2Pressed) {
    writeToRadio(2);
    wasButton2Pressed = false;
    PrintDebug("Button 2 pressed");
    Serial.println(UNPROTECTED_START);
    /////////////////// start servo ////////////////////////

    unsigned long startedAt = millis();

    while (millis() - startedAt < TIME_FOR_FIRST_MOVIE_UNPROTECTED) {
      UnprotectedLedLoop(millis() - startedAt);
    }
    unsigned long startedAtInternal = millis();
    ///////////// wait for touch sensor to be triggered or time limit אם end (while(touch==LOW || TIME_LIMIT_FOR_TOUCH)
   //while ( millis() - startedAtInternal < TIME_LIMIT_FOR_TOUCH) {
    
   while (analogRead(pressurePin) < 1000 && millis() - startedAtInternal < TIME_LIMIT_FOR_TOUCH) {
    //  PrintDebug("----------------------");
      UnprotectedKidnapLedLoop(millis() - startedAtInternal);
      if(touch_force < 100){
        //PrintDebug("analogRead(pressurePin) < 100");
        }
      }
    leds_off();
    writeToRadio(3);
    PrintDebug("UNPROTECTED_END");
    Serial.println(UNPROTECTED_END);

    unsigned long startedAtInternal2 = millis();
    while (millis() - startedAtInternal2 < TIME_FOR_SECOND_MOVIE_UNPROTECTED) {
      ////////////// start servo //////////////////
    }
    leds_off();



  } // second button if
  // third button if (PROTECTED)
  if (wasButton3Pressed) {
    wasButton3Pressed = false;
    writeToRadio(4);

    PrintDebug("Button 3 pressed");
    Serial.println(PROTECTED);
    unsigned long startedAt = millis();
    while (millis() - startedAt < TIME_PROTECTED_MOVIE) {
      /////////////////// start servo ////////////////////////
      ProtectedLedLoop(millis() - startedAt);
    }
    leds_off();
  }// third button if

  decraseButtonTimers();
}

int protectedStage = 0;
int protectedRunningLED = 0;
int protectedRunningLEDTime = 0;
int protectedRunningLEDMaxTime = 2000;

void ProtectedLedLoop(unsigned long secenarioTime) {
  //leds[Car_LED] = CRGB::Green;
  if (secenarioTime <= 1000) {
    protectedStage = 0;
  }
  if (protectedStage == 0 && secenarioTime >= 12000) {
    leds_off();
    protectedStage = 1;
    PrintDebug("Stage Haker");
    //Serial.println("Stage 1");
    for (int i = HACKER_LED_START; i <= HACKER_LED_END; i++) {
      leds[i] = CRGB::Red;
    }
    FastLED.show();
    protectedRunningLEDTime = 0;
    protectedRunningLED = 0;
  }
  if (protectedStage == 1 && secenarioTime >= 16000 ) {
    leds_off();
    protectedStage = 2;
    PrintDebug("Stage Haker To Antena");
    leds[HACKER_TO_ANTENA_LED] = CRGB::Red;
    FastLED.show();

  }
  if (protectedStage == 2 && secenarioTime >= 20000) {
    PrintDebug("Stage Antena");
    leds_off();
    protectedStage = 3;
    leds[ANTENA_LED] = CRGB::Red;
    FastLED.show();
    protectedRunningLEDTime = 0;
    // protectedRunningLED = 0;
    protectedRunningLED = ANTENA_TO_CAR_LED_END;
  }
  if ((protectedStage == 4 || protectedStage == 3) && secenarioTime >= 24000 && secenarioTime <= 28000) {
    //PrintDebug("Stage Antena To Car 4");
    protectedStage = 4;
    if (protectedRunningLEDTime >= protectedRunningLEDMaxTime) {
      leds_off();
      protectedRunningLEDTime = 0;
      protectedRunningLED--;
      if ( protectedRunningLED < ANTENA_TO_CAR_LED_START) {
        protectedRunningLED = ANTENA_TO_CAR_LED_END;
      }

      PrintDebug("Runing LED");
      PrintDebug( protectedRunningLED);
      leds[protectedRunningLED] = CRGB::Red;
      FastLED.show();

    }
    // FastLED.show();
    protectedRunningLEDTime++;



  }
  if ((protectedStage == 4 || protectedStage == 5) && secenarioTime >= 32000 && secenarioTime <= 36000) {
    protectedStage = 5;
    if (protectedRunningLEDTime >= protectedRunningLEDMaxTime) {
      leds_off();
      protectedRunningLEDTime = 0;
      protectedRunningLED--;
      if ( protectedRunningLED < CAR_TO_TECHNICIAN_LED_START) {
        protectedRunningLED = CAR_TO_TECHNICIAN_LED_END;
      }
      PrintDebug("Runing LED");
      PrintDebug( protectedRunningLED);
      leds[protectedRunningLED] = CRGB::Red;
      FastLED.show();

    }

    protectedRunningLEDTime++;

  }
  if (protectedStage == 5 && secenarioTime >= 36000) {
    leds_off();
    protectedStage = 6;
    PrintDebug("Stage Technision");
    //Serial.println("Stage 1");
    for (int i = TECHNICIAN_LED_START; i <= TECHNICIAN_LED_END; i++) {
      leds[i] = CRGB::Green;
    }
    FastLED.show();
    protectedRunningLEDTime = 0;
    protectedRunningLED = TECHNICIAN_TO_PARKING_LED_END;

  }
  if ((protectedStage == 6 || protectedStage == 7) && secenarioTime >= 40000 && secenarioTime <= 44000) {
    protectedStage = 7;
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
      FastLED.show();

    }

    protectedRunningLEDTime++;

  }
  if (protectedStage == 7 && secenarioTime >= 44000) {
    leds_off();
    protectedStage = 8;
    PrintDebug("Stage Parkin Lot");
    for (int i = PARKING_LEDS_START; i <= PARKING_LEDS_END; i++) {
      leds[i] = CRGB::Green;
    }
    FastLED.show();

  }
  if (protectedStage == 8 && secenarioTime >= 49000) {
    leds_off();
    protectedStage = 9;
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
  if (secenarioTime <= 1000) {
    unprotectedStage = 0;
  }
  if (unprotectedStage == 0 && secenarioTime >= 4000) {
    unprotectedStage = 1;
    PrintDebug("Stage 1");
    /// Car Moving
  }

  if (unprotectedStage == 1 && secenarioTime >= 8000) {
    leds_off();
    unprotectedStage = 2;
    PrintDebug("Stage 2");
    for (int i = HACKER_LED_START; i <= HACKER_LED_END; i++) {
      leds[i] = CRGB::Red;
    }
    FastLED.show();
  }
  if (unprotectedStage == 2  && secenarioTime >= 12000) {
    leds_off();
    unprotectedStage = 3;
    leds[HACKER_TO_ANTENA_LED] = CRGB::Red;
    FastLED.show();
  }
  if (unprotectedStage == 3 && secenarioTime >= 16000) {
     leds_off();
    unprotectedStage = 4;
       
    leds[ANTENA_LED] = CRGB::Red;
    FastLED.show();
  }
  if ((unprotectedStage == 4 || unprotectedStage == 5) && secenarioTime >= 20000 && secenarioTime <= 24000) {
    //PrintDebug("Stage Antena To Car 4");
    unprotectedStage = 4;
    if (unprotectedRunningLEDTime >= unprotectedRunningLEDMaxTime) {
      leds_off();
      unprotectedRunningLEDTime = 0;
      unprotectedRunningLED--;
      if ( unprotectedRunningLED < ANTENA_TO_CAR_LED_START) {
        unprotectedRunningLED = ANTENA_TO_CAR_LED_END;
      }

      PrintDebug("Runing LED");
      PrintDebug( protectedRunningLED);
      leds[unprotectedRunningLED] = CRGB::Red;
      FastLED.show();

    }
    // FastLED.show();
    unprotectedRunningLEDTime++;
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
   
   
  }
  if (unprotectedKidnapStage == 2 && secenarioTime >= 49000) {
    //leds_off();
    unprotectedStage = 7;  

  }


}


////////// shut down all leds //////////
void leds_off() {
  for (int i = 0; i <= NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    FastLED.show();
  }
}


int startTimer  = 5000;
// callback for inputButtonThread
void chackInputButtons() {
  touch_force = analogRead(pressurePin);
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

/*
  void chackRadioForInput() {

  /// Reading from pipe
  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();
  if (radio.available())
  {
    int text = 0;
    radio.read(&text, sizeof(text));
    Serial.println("Messge Recived From Radio");
    Serial.println(text);
  }
  }*/

