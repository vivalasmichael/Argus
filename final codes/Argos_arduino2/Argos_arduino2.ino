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


///////////////// LEDS  //////////////////
#define LED_PIN     5
#define NUM_LEDS    50
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];
///////////////// program consts  //////////////////
#define Car_LED 4
#define HACKER_LED 5
#define TECHNICIAN_LED 3  // check with Ram
#define PARKING_LEDS_START 6
#define PARKING_LEDS_END 10
#define IN_SECURITY_CENTER_START 11
#define IN_SECURITY_CENTER_END 15
#define OUT_SECURITY_CENTER_START 16
#define OUT_SECURITY_CENTER_END 21
#define TOUCH_LEDS_START 22
#define TOUCH_LEDS_END 26
#define TIME_FOR_EXPLANATION_MOVIE 5000
#define TIME_BEFORE_CAR_TURNNING_RED_UNPROTECTED 1000
#define TIME_FOR_FIRST_MOVIE_UNPROTECTED 1000
#define TIME_LIMIT_FOR_TOUCH 15000
#define TIME_FOR_SECOND_MOVIE_UNPROTECTED 1000
#define TIME_PROTECTED_MOVIE 1000
///////////////// Old Buttons Oren //////////////////
//const int buttonPin1 = 2;
//const int buttonPin2 = 6;
//const int buttonPin3 = 7;
///////////////// Toch Sensor //////////////////
int pressurePin = A0;
int touch_force;
///////////////// Scenarios //////////////////
enum ScenarioState {
  STATIC_MOVIE = 1,
  PLAY_EXPLANATION_MOVIE = 2,
  UNPROTECTED_START = 3,
  UNPROTECTED_STOP = 4,
  UNPROTECTED_END = 5,
  PROTECTED = 6
};

void setup() {
  Serial.begin(9600);

  //Serial.println("START");

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);

  


  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.stopListening();
  Serial.print("is Chip Connected = " );
  Serial.println(radio.isChipConnected());
  radio.printDetails();
  // Configure Threads
 // radioListenerThread.onRun(chackRadioForInput);
  //radioListenerThread.setInterval(50);

  buttonListenThread.onRun(chackInputButtons);
  buttonListenThread.setInterval(0);



  // Adds both threads to the controller
  //controll.add(&radioListenerThread);
  controll.add(&buttonListenThread);
}

int getMsgFromSerial() {
  int msg =  -1;
  if (Serial.available()) {
    msg = Serial.read();
    Serial.println(msg);
  }
  return msg;
}


void sendMsgToComputer(int msg) {
  Serial.write((byte*)&msg, sizeof(msg));
}


void sendMsgToRadio(int msg) {
  radio.write(&msg, sizeof(msg));
  delay(100);
}




void loop() {
  controll.run();
  leds_off();



    // first button (PLAY_EXPLANATION_MOVIE)
    if (wasButton1Pressed) {
      sendMsgToRadio(1);
      wasButton1Pressed = false;
      Serial.println("Button 1 pressed");
      Serial.println(PLAY_EXPLANATION_MOVIE);
      unsigned long startedAt = millis();
      while (millis() - startedAt < TIME_FOR_EXPLANATION_MOVIE) {
        int msg = 0;
        if ( getMsgFromSerial() == 49) {
          Serial.println("Message Recived Fromn PC and we break");
          break;
        }

      }
      Serial.println("Message Not Recived Fromn PC and we break from timeout");
    }// first button if

    // second button if (UNPROTECTED)
    if (wasButton2Pressed) {
      sendMsgToRadio(2);
      wasButton2Pressed = false;
      Serial.println("Button 2 pressed");
      Serial.println(UNPROTECTED_START);
      /////////////////// start servo ////////////////////////
      UnprotectedLedLoop();
      unsigned long startedAt = millis();
      while (millis() - startedAt < TIME_FOR_FIRST_MOVIE_UNPROTECTED) {
        int msg = 0;
        if ( getMsgFromSerial() == UNPROTECTED_START) {
          Serial.println("Message Recived Fromn PC and we send Unprotected Stop");
          Serial.println(UNPROTECTED_STOP);
          continue;
        }

        unsigned long startedAt = millis();
        ///////////// wait for touch sensor to be triggered or time limit אם end (while(touch==LOW || TIME_LIMIT_FOR_TOUCH)
        while (analogRead(pressurePin) < 100 && millis() - startedAt < TIME_LIMIT_FOR_TOUCH) {}

        sendMsgToRadio(3);
        Serial.println("UNPROTECTED_END");
        Serial.println(UNPROTECTED_END);

        startedAt = millis();
        while (millis() - startedAt < TIME_FOR_SECOND_MOVIE_UNPROTECTED) {
          ////////////// start servo //////////////////
        }
        leds_off();
      }
    } // second button if
    // third button if (PROTECTED)
    if (wasButton3Pressed) {
      wasButton3Pressed = false;
      sendMsgToRadio(4);

      Serial.println("Button 3 pressed");
      Serial.println(PROTECTED);
      unsigned long startedAt = millis();
      while (millis() - startedAt < TIME_PROTECTED_MOVIE) {
        /////////////////// start servo ////////////////////////
        ProtectedLedLoop();
      }
      leds_off();
    }// third button if

    decraseButtonTimers();
  }

  void ProtectedLedLoop() {
    leds[Car_LED] = CRGB::Green;
    leds[HACKER_LED] = CRGB::Red;
    for (int i = PARKING_LEDS_START; i <= PARKING_LEDS_END; i++) {
      leds[i] = CRGB::Green;
    }
    for (int i = IN_SECURITY_CENTER_START; i <= IN_SECURITY_CENTER_END; i++) {
      leds[i] = CRGB::Red;
    }
    for (int i = OUT_SECURITY_CENTER_START; i <= OUT_SECURITY_CENTER_END; i++) {
      leds[i] = CRGB::Green;
    }
    FastLED.show();
  }


  void UnprotectedLedLoop() {
    leds[Car_LED] = CRGB::Green;
    FastLED.show();
    delay(TIME_BEFORE_CAR_TURNNING_RED_UNPROTECTED);
    leds[Car_LED] = CRGB::Red;
    leds[HACKER_LED] = CRGB::Red;
    for (int i = PARKING_LEDS_START; i <= PARKING_LEDS_END; i++) {
      leds[i] = CRGB::Red;
    }
    FastLED.show();
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

