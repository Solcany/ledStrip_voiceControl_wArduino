#include <FastLED.h>
//#define PI 3.1415926535897932384626433832795
#define LED_PIN 7

#define NUM_LEDS 10//Number of leds
CRGB leds[NUM_LEDS];


int b = 0;
int desiredFadePeriod = 1500;
unsigned long previousMillis = 0;
int currentLPS;


void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(255);
  
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i].setRGB(255, 255, 255);
    }
    FastLED.show();
}

void loop() {
  int loopsPerSecond = measure_loopsPerSecond(5);
  int fadeStep = desiredFadePeriod / loopsPerSecond; 
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= fadeStep) {
      previousMillis = currentMillis;

    if(b < 256) {
      for(int i = 0; i< NUM_LEDS; i++ ) {
        leds[i].fadeToBlackBy(b);
      }
      b++;
    } else {
      Serial.println("fading finished");
    }
    FastLED.show();
  }


  delay(5);
}

static inline int measure_loopsPerSecond(const int seconds){
  // Create static variables so that the code and variables can
  // all be declared inside a function
  static unsigned long previousMillis;
  static unsigned long frameCount;
  static unsigned int loopsPerSecond;
  
  
  // It is best if we declare millis() only once
  unsigned long currentMillis = millis();
  frameCount ++;
  if (currentMillis - previousMillis >= seconds * 1000) {
    loopsPerSecond = frameCount / seconds;
    frameCount = 0;
    previousMillis = currentMillis; 
    return loopsPerSecond;
  }
}
