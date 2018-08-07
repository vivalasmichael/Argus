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
Thread computerListenerThread = Thread();

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
#define TIME_LIMIT_FOR_TOUCH 5000
#define TIME_FOR_SECOND_MOVIE_UNPROTECTED 1000
#define TIME_PROTECTED_MOVIE 1000
///////////////// Buttons //////////////////
const int buttonPin1 = 2;
const int buttonPin2 = 6;
const int buttonPin3 = 7;
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
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);

  radio.begin();
  // Configure Threads
  radioListenerThread.onRun(chackRadioForInput);
  radioListenerThread.setInterval(50);

  // Adds both threads to the controller
  controll.add(&radioListenerThread);
}

int getMsgFromSerial() {
  int msg =  -1;
  if (Serial.available()) {
    msg = Serial.read();
      Serial.print(msg);
  }
  return msg;
}


void sendMsgToComputer(int msg) {
  Serial.write((byte*)&msg, sizeof(msg));
}


void sendMsgToRadio(uint32_t msg) {
  radio.openWritingPipe(addresses[0]);
  radio.stopListening();
  radio.write(&msg, sizeof(msg));
}

void chackRadioForInput() {

  /// Reading from pipe
  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();
  if (radio.available())
  {
    int text = 0;
    radio.read(&text, sizeof(text));
    Serial.print(text);
  }
}


void loop() {
  controll.run();
  // first button if
  if (digitalRead(buttonPin1) == HIGH) {
    //Serial.println("Button 1 pressed");
    Serial.print(PLAY_EXPLANATION_MOVIE);
    unsigned long startedAt = millis();
    while (millis() - startedAt < TIME_FOR_EXPLANATION_MOVIE) {
      int msg = 0;
      if ( getMsgFromSerial() == PLAY_EXPLANATION_MOVIE) {
        break;
      }
      // String data = Serial.readString();
      //  if (data == "done") {
      //Serial.println("DONE IS HERE");
      //    data = "";
      //    break;
      //  }
    }
  }// first button if

// second button if
  if (digitalRead(buttonPin2) == HIGH) {
    //Serial.println("Button 2 pressed");
    Serial.print(UNPROTECTED_START);
    /////////////////// start servo ////////////////////////
    leds[Car_LED] = CRGB::Green;
    FastLED.show();
    delay(TIME_BEFORE_CAR_TURNNING_RED_UNPROTECTED);
    leds[Car_LED] = CRGB::Red;
    leds[HACKER_LED] = CRGB::Red;
    for (int i = PARKING_LEDS_START; i <= PARKING_LEDS_END; i++) {
      leds[i] = CRGB::Red;
    }
    FastLED.show();
    unsigned long startedAt = millis();
    while (millis() - startedAt < TIME_FOR_FIRST_MOVIE_UNPROTECTED) {
      int msg = 0;
      if ( getMsgFromSerial() == UNPROTECTED_START) {
        Serial.print(UNPROTECTED_STOP);
        continue;
      }
     // String data = Serial.readString();
     // if (data == "stop_car") {
    //    Serial.println(UNPROTECTED_STOP);
        ////////////// stop servo //////////////////
     //   data = "";
   //     continue;
   //   }
      unsigned long startedAt = millis();
      while (analogRead(pressurePin) < 500 && millis() - startedAt < TIME_LIMIT_FOR_TOUCH) {} ///////////// wait for touch sensor to be triggered or time limit אם end (while(touch==LOW || TIME_LIMIT_FOR_TOUCH)
      Serial.println(UNPROTECTED_END);
      startedAt = millis();
      while (millis() - startedAt < TIME_FOR_SECOND_MOVIE_UNPROTECTED) {
        ////////////// start servo //////////////////
      }
      leds_off();
    }
  } // second button if
// third button if
  if (digitalRead(buttonPin3) == HIGH) {
    //Serial.println("Button 3 pressed");
    Serial.println(PROTECTED);
    unsigned long startedAt = millis();
    while (millis() - startedAt < TIME_PROTECTED_MOVIE) {
      /////////////////// start servo ////////////////////////
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
    leds_off();
  }// third button if
}

////////// shut down all leds //////////
void leds_off() {
  for (int i = 0; i <= NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    FastLED.show();
  }
}


