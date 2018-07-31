#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    50
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];

enum ScenarioState {
  Static_Movie = 1,
  Play_Explnation_Movie = 2,
  Unprotected_Start = 3,
  Unprotected_Stop = 4,
  Unprotected_End = 5,
  Protedted = 6
  
};

#define UPDATES_PER_SECOND 100
#define SOC_led_num 21
#define CarA_led_num 10
#define CarB_led_num 32
#define CarC_led_num 43
#define Antenna_2_CarA_Start 0
#define Antenna_2_CarA_End 9
#define CarA_2_SOC_Start 11
#define CarA_2_SOC_End 20
#define SOC_2_CarB_Start 22
#define SOC_2_CarB_End 31
#define SOC_2_CarC_Start 33
#define SOC_2_CarC_End 42

#define DEFAULT_ 100
#define PROBLEM 101
#define SOLVING_Prevent 102
#define SOLVING_Undestand 103
#define SOLVING_Response 104


int data;

void setup()   
{   
    delay(100); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(  BRIGHTNESS );
    Serial.begin(9600);
    Serial.println("START");
}   
void loop()   
{   
    data = Serial.parseInt(); //Read the serial buffer as a INT
      switch (data) {
      /////////////////////////////////////////////////////////////
      case DEFAULT_:
          default_mode(5000);
        break;
      /////////////////////////////////////////////////////////////
      case PROBLEM:
        Serial.println("data=100");
        break;
      /////////////////////////////////////////////////////////////
      case SOLVING_Prevent:
        Serial.println("data=101");
        solving_the_problem(5000);
        break;
      /////////////////////////////////////////////////////////////
      case SOLVING_Undestand:
        Serial.println("data=102");
        solving_the_understand(5000);
        break;
      /////////////////////////////////////////////////////////////
      case SOLVING_Response:
        solving_the_response(5000);
        break;
      /////////////////////////////////////////////////////////////
      case 5:
        for (int i=0; i<=NUM_LEDS;i++){
          leds[i].red   = 255;
          leds[i].green = 200;
          leds[i].blue  = 10;
           FastLED.show();
        }
        break;
      /////////////////////////////////////////////////////////////
      default:
      for (int i=0; i<=NUM_LEDS;i++){
          leds[i] = CRGB::Black; 
           FastLED.show();
      }
    }
}

void default_mode(unsigned long howLong )
{
    unsigned long startedAt = millis();
    while(millis() - startedAt < howLong)
    {
      leds[CarA_led_num] = CRGB::Green;
      FastLED.show(); 
    }
    leds[CarA_led_num] = CRGB::Black;
    FastLED.show();
}
////////////// lights travel from Antenna to carA and carA light is green /////////////
void solving_the_problem(unsigned long howLong )
{
  unsigned long startedAt = millis();
  leds[CarA_led_num] = CRGB::Green;
  while(millis() - startedAt < howLong)
  {
      delay(100);
      for (int i=Antenna_2_CarA_Start; i<=Antenna_2_CarA_End;i++){
           leds[i] = CRGB::Red; 
           delay(100);
           FastLED.show();
        }
        for (int i=Antenna_2_CarA_Start; i<=Antenna_2_CarA_End;i++)
          leds[i] = CRGB::Black;
        FastLED.show(); 
  }
      leds[CarA_led_num] = CRGB::Black;
      FastLED.show(); 
}
////////////// lights travel from carA to SOC and carA light is green /////////////
void solving_the_understand(unsigned long howLong )
{
  unsigned long startedAt = millis();
  leds[CarA_led_num] = CRGB::Green;
  while(millis() - startedAt < howLong)
  {
      delay(100);
      for (int i=CarA_2_SOC_Start; i<=CarA_2_SOC_End;i++){
           leds[i] = CRGB::Red; 
           delay(100);
           FastLED.show();
        }
        for (int i=CarA_2_SOC_Start; i<=CarA_2_SOC_End;i++)
          leds[i] = CRGB::Black;
        FastLED.show(); 
  }
      leds[CarA_led_num] = CRGB::Black;
      FastLED.show(); 
}

////////////// lights travel from carA to SOC and carA light is green /////////////
void solving_the_response(unsigned long howLong )
{
  unsigned long startedAt = millis();
  leds[SOC_led_num] = CRGB::Green;
  while(millis() - startedAt < howLong)
  {
      delay(100);
      for (int i=SOC_2_CarB_Start, j=SOC_2_CarC_Start; i<=SOC_2_CarB_End;i++,j++){
           leds[i] = CRGB::Green; 
           leds[j] = CRGB::Green; 
           delay(100);
           FastLED.show();
        }
        for (int i=SOC_2_CarB_Start, j=SOC_2_CarC_Start; i<=SOC_2_CarB_End;i++,j++){
          leds[i] = CRGB::Black;
          leds[j] = CRGB::Black;
        }
        FastLED.show(); 
  }
      leds[SOC_led_num] = CRGB::Black;
      FastLED.show(); 
}


