#include <FastLED.h>
#define PI_ 3.1415

#define LED_PIN 7
#define micPin A0

#define NUM_LEDS  10

/* ––––––––––––––––  ENVIROMENT LOUDNESS –––––––––––––––– 

enviromentSilence: measure mic value when the environment.
enviromentSoundMax: measure mic value while a loud sound is present.

*/

const int enviromentSilence = 335;
const int enviromentLoudness = 342;


/* ––––––––––––––––  SETTINGS –––––––––––––––– 

change these to the affect behavior of the program

*/

// how often will a new led pixel be selected
// the bigger the max number, the 'slower' the whole led strip will be 
const int periodDurationMin = 100;
const int periodDurationMax = 1000;

// how fast will a single led finish fadein/out pulse
// bigger the max number, the longer the pulse will last
const int pulseDurationMin = 25;
const int pulseDurationMax = 100;

const float pulseStepMin = 3.0;
const float pulseStepMax = 0.05;


// how distant will a selected led pixel be from the previous one
// bigger the max number, the more 'random' will the whole led strip look
const int noiseSpeedMin = 1;
const int noiseSpeedMax = 50;

// how many samples will be collected before moving average is calculated
// bigger the number the smoother the sound wave will be
const int sampleSize = 35;


/* ––––––––––––––––  VARIABLES ––––––––––––––––  */

CRGB leds[NUM_LEDS];

float micSmoothed;

unsigned long period_previousMillis;
unsigned long pulse_previousMillis;

int ledPointer;
int previousPointer;
boolean areLedsToggled[NUM_LEDS];
float ledsBrightness_inRadians[NUM_LEDS];

int desiredFadePeriod = 1500;

uint8_t noiseRaw;
uint8_t noiseScaled;
uint16_t noiseX;
int noiseSpeed;

int samples[sampleSize];
int sampleIndex = 0;
int sum = 0;


void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  random16_set_seed(7500);
  noiseX = random16();
}


void loop()
{
    
  /* ––––––––––––––––  SOUND INPUT SMOOTHING ––––––––––––––––  */

  // read output from microphone
  int micRawOutput = analogRead(micPin);

  // get moving average of raw mic input
  sum = sum - samples[sampleIndex];
  samples[sampleIndex] = micRawOutput;
  sum = sum + samples[sampleIndex];
  sampleIndex = sampleIndex + 1;

  if (sampleIndex >= sampleSize) {
    sampleIndex = 0;
  }

  micSmoothed = round(sum / sampleSize);
  
  /* ––––––––––––––––  SMOOTHED SOUND INPUT RERANGING ––––––––––––––––  */

  // how often will a new led pixel be selected
  int period = (int) abs(round(map(micSmoothed, enviromentSilence, enviromentLoudness, periodDurationMax, periodDurationMin)));

  // how long will a fade in/out cycle of led pixel take

  float pulseStep =  abs(map(micSmoothed, enviromentSilence, enviromentLoudness, pulseStepMax, pulseStepMin)) / 100.0;
  
  // how distant will the next selected pixel be from the previous
  int noiseSpeed = (int) abs(round(map(micSmoothed, enviromentSilence, enviromentLoudness, noiseSpeedMin, noiseSpeedMax)));

  /* ––––––––––––––––  LED PIXEL SELECTION ––––––––––––––––  */

  unsigned long period_currentMillis = millis();  
  if (period_currentMillis - period_previousMillis >= period) {
    
    // default noise value is between 30 – 230
    noiseRaw = inoise8(noiseX);
    // rerange to 0 – 255
    noiseScaled = qsub8(noiseRaw,16);
    noiseScaled = qadd8(noiseScaled,scale8(noiseScaled,39));
    
    // advance noise
    noiseX += noiseSpeed;
   
    // choose a led pixel to light up
    ledPointer = round(map(noiseScaled, 0, 255, 0, NUM_LEDS));
    if ( previousPointer == ledPointer) {
      // if the pixel is the same as the previous, select the next one
      ledPointer++;
    }
    // toggle the selected pixel
    areLedsToggled[ledPointer] = true;

    // save current time
    period_previousMillis = period_currentMillis;
    // save current on led index
    previousPointer = ledPointer;
  }

  /* ––––––––––––––––  LED PIXELS FADE IN/OUT PULSE (or cycle) ––––––––––––––––  */

    for(int i = 0; i < NUM_LEDS; i++) {
        if(areLedsToggled[i] && ledsBrightness_inRadians[i] < PI_) {
          // brighten a led pixel to maximum brightness, then back to black
          float sin_ = sin(ledsBrightness_inRadians[i]);
          float brightness = sin_ * 255;
          leds[i] = CRGB(brightness, brightness, brightness);
          ledsBrightness_inRadians[i] += pulseStep;
        } else {
          // when pixel cycle is finished, reset the pixel and turn it off
//          leds[i].fadeLightBy(128);
          leds[i] = CRGB(15, 15, 15);
          areLedsToggled[i] = false;
          ledsBrightness_inRadians[i] = 0.0;
        }
    }

  FastLED.show();
  delay(2);
}
