#include <FastLED.h>
#define PI_ 3.1415
#define LED_PIN 7
#define micPin A0
#define micRawMin 335
#define micRawMax 342
#define NUM_LEDS  10//Number of leds

//int stepLength    = 30; //difference in brightness between neighboring leds. Can make it multiples of 30
//int stepSpeed     = 5;  //difference in speed/intensity of change of the individual leds max=360
//int brightness    = 50; //control brightness. values 0-127
//int movementSpeed = 30; //control delay of movement (in milliseconds)

CRGB leds[NUM_LEDS];

/* ––––––––––––––––  CONSTANTS –––––––––––––––– 

change these to the affect behavior of the program

*/

// how often will a new led be selected
// the bigger the max number, the 'slower' the led strip will be 

const int periodDurationMin = 200;
const int periodDurationMax = 1000;

// how fast will a single led finish fadein/out cycle
// bigger the max number, the longer a pulse cycle will be

const int pulseDurationMin = 100;
const int pulseDurationMax = 500;

// how distant will a selected led be from previous one
// bigger the max number, the more 'random' will the led strip look

const int noiseSpeed_min = 1;
const int noiseSpeed_max = 50;

// how many samples will be collected before moving average is calculated
// bigger the number the smoother the sound wave will be
const int sampleSize = 35;


/* ––––––––––––––––  VARIABLES ––––––––––––––––  */

float ledsBrightness_inRadians[NUM_LEDS];
float pulseStep;

int ledPointer;
int previousPointer;

unsigned long period_previousMillis;
unsigned long pulse_previousMillis;

boolean areLedsToggled[NUM_LEDS];

int desiredFadePeriod = 1500;

uint8_t noiseRaw;
uint8_t noiseScaled;
uint16_t noiseX;
int noiseSpeed;

int samples[sampleSize];
int sampleIndex = 0;
int sum = 0;

int micRawOutput = 0;
float micSmoothed = 0;


void setup() {

  Serial.begin(9600);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  random16_set_seed(7500);
  noiseX = random16();

  for (int i = 0; i < sampleSize; i++) {
    samples[i] = 0;
  }

}


void loop()
{
  int loopsPerSecond = measure_loopsPerSecond(5);
  int pulseDurationPerLoop = desiredPulsePeriod / loopsPerSecond;  
  
  // read output from microphone
  micRawOutput = analogRead(micPin);

  // get moving average of raw mic input
  sum = sum - samples[sampleIndex];
  samples[sampleIndex] = micRawOutput;
  sum = sum + samples[sampleIndex];
  sampleIndex = sampleIndex + 1;

  if (sampleIndex >= sampleSize) {
    sampleIndex = 0;
  }
  
  micSmoothed = sum / sampleSize;

  // how often will a new led pixel be randomly picked
  int period = (int) round(map(micSmoothed, micRawMin, micRawMax, pulseIntervalMax, pulseIntervalMin));
      //DEBUG
      period = 250;
      
  // how much will next value differ from the previous
  int noiseSpeed = (int) round(map(micSmoothed, micRawMin, micRawMax, noiseSpeed_min, noiseSpeed_max));


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
      // if the pixel is same as the previous, advance by 1
      ledPointer++;
    }

    // toggle the selected pixel
    areLedsToggled[ledPointer] = true;

    // save current time
    period_previousMillis = period_currentMillis;
    // save current on led index
    previousPointer = ledPointer;
  }


  // fade in the selected pixel
  unsigned long currentMillis_ = millis();
  if (currentMillis_ - pulse_previousMillis >= pulseDurationPerLoop) {
    for(int i = 0; i < NUM_LEDS; i++) {
      if(areLedsToggled[i]) {
        if(ledsBrightness_inRadians[i] < PI_) {
          float sinA = sin(ledsBrightness_inRadians[i]);
          ledsBrightness_inRadians[i] += pulseStep;
          float brightness = sinA * 255;
          leds[i] = CRGB(brightness, brightness, brightness);
        } else {
          Serial.println("a led finished cycle");
          leds[i].fadeToBlackBy(255);
          areLedsToggled[i] = false;
          ledsBrightness_inRadians[i] = 0;
        }
      }
    }
    previousMillis_ = currentMillis_;
  }

 


  //  for(int i=0; i<360; i = i+sensorspeed1){
  //    iRad = i * PI / 180;
  //    for(int j = 0; j<NUM_LEDS; j++){
  //      degArray[j] = (i - sensorlength1*j);
  //      radArray[j] = (degArray[j]*PI/180);
  //      sinArray[j] = sin(radArray[j]);
  //      ledArray[j] = (1 + sinArray[j]) * sensorbrightness1;
  //    }
  //
  //    delay(sensormovementspeed1);
  //    for(int z=0; z<NUM_LEDS; z++){
  //      leds[z] = CRGB(ledArray[z], ledArray[z], ledArray[z]);
  //    }
  FastLED.show();
  delay(2);
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
