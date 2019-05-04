#include <FastLED.h>
#define PI_ 3.1415
#define LED_PIN 7
#define micPin A0
#define micRawMin 335
#define micRawMax 342

#define NUM_LEDS    10//Number of leds
CRGB leds[NUM_LEDS];

//int stepLength    = 30; //difference in brightness between neighboring leds. Can make it multiples of 30
//int stepSpeed     = 5;  //difference in speed/intensity of change of the individual leds max=360
//int brightness    = 50; //control brightness. values 0-127
//int movementSpeed = 30; //control delay of movement (in milliseconds)

// mic output settings


// rgb settings
const int minRgb = 0;
const int maxRgb = 254;
int ledsBrightness[NUM_LEDS];
float ledsFadeState_inRadians[NUM_LEDS];
float radStep = PI_ / 60.0;

// pulse settings
const int pulseIntervalMin = 2;
const int pulseIntervalMax = 2500;
int interval = pulseIntervalMax;
int fadeStep = 25;



int ledPointer;
int previousPointer;


// delay
unsigned long previousMillis = 0;
unsigned long previousMillis_ = 0;

boolean areLedsToggled[NUM_LEDS];

int desiredFadePeriod = 1500;


// noise settings
const int noiseSpeed_min = 1;
const int noiseSpeed_max = 50;

uint8_t noiseRaw;
uint8_t noiseScaled;
int noiseSpeed;


uint16_t noiseX;


// sound sampling & smoothing
const int sampleSize = 35;
int samples[sampleSize];
int sampleIndex = 0;
int sum = 0;



int sensorlength = 0;
int sensorlength1 = 0;
int micRawOutput = 0;
float micSmoothed = 0;
int smoothedMicVal = 0;
int sensorspeed1 = 0;
int sensorbrightness = 0;
int sensorbrightness1 = 0;
int sensormovementspeed = 0;
int sensormovementspeed1 = 0;


int value = 0;
int degArray[10];
float radArray[10];
float sinArray[10];
int ledArray[10];
float iRad = 0;
float sinVal = 0;
int ledVal = 0;

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
      period = 2000;
      
  // how much will next value differ from the previous
  int noiseSpeed = (int) round(map(micSmoothed, micRawMin, micRawMax, noiseSpeed_min, noiseSpeed_max));


  unsigned long currentMillis = millis();  
  if (currentMillis - previousMillis >= period) {
    
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
    previousMillis = currentMillis;
    // save current on led index
    previousPointer = ledPointer;
  }


  // fade in the selected pixel
  unsigned long currentMillis_ = millis();
  if (currentMillis_ - previousMillis_ >= fadeStep) {
    for(int i = 0; i < NUM_LEDS; i++) {
      if(areLedsToggled[i]) {
        if(ledsFadeState_inRadians[i] < PI_) {
          float sinA = sin(ledsFadeState_inRadians[i]);
          ledsFadeState_inRadians[i] += radStep;
          float brightness = sinA * 255;
          leds[i] = CRGB(brightness, brightness, brightness);
        } else {
          Serial.println("a led finished cycle");
          leds[i].fadeToBlackBy(255);
          areLedsToggled[i] = false;
          ledsFadeState_inRadians[i] = 0;
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


/*
  leds[0] = CRGB(value, value, value);
  FastLED.show();
  delay(500);
  leds[1] = CRGB(200, 200 , 200);
  FastLED.show();
  delay(500);
  leds[2] = CRGB(150, 150, 150);
  FastLED.show();
  delay(500);
  leds[3] = CRGB(100, 100, 100);
  FastLED.show();
  delay(500);
  leds[4] = CRGB(50, 50, 50);
  FastLED.show();
  delay(500);
  leds[5] = CRGB(10, 10, 10);
  FastLED.show();
  delay(500);
  leds[6] = CRGB(1, 1, 1);
  FastLED.show();
  delay(500);
*/
//}
