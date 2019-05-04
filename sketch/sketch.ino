#include <FastLED.h>
//#define PI 3.1415926535897932384626433832795
#define LED_PIN     7
#define micPin A0

#define NUM_LEDS    10//Number of leds
//int stepLength    = 30; //difference in brightness between neighboring leds. Can make it multiples of 30
//int stepSpeed     = 5;  //difference in speed/intensity of change of the individual leds max=360
//int brightness    = 50; //control brightness. values 0-127
//int movementSpeed = 30; //control delay of movement (in milliseconds)

// mic output settings
const int micRawMin = 335;
const int micRawMax = 342;

// rgb settings
const int minRgb = 0;
const int maxRgb = 254;

// pulse settings
const int pulseIntervalMin = 2;
const int pulseIntervalMax = 2500;
int interval = pulseIntervalMax;

int ledPointer;
int previousPointer;


// delay
unsigned long previousMillis = 0;
boolean areLedsToggled[NUM_LEDS];


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
CRGB leds[NUM_LEDS];
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

  sum = sum - samples[sampleIndex];
  samples[sampleIndex] = micRawOutput;
  sum = sum + samples[sampleIndex];
  sampleIndex = sampleIndex + 1;

  if (sampleIndex >= sampleSize) {
    sampleIndex = 0;
  }

  micSmoothed = sum / sampleSize;

  interval = (int) round(map(micSmoothed, micRawMin, micRawMax, pulseIntervalMax, pulseIntervalMin));
  noiseSpeed = (int) round(map(micSmoothed, micRawMin, micRawMax, noiseSpeed_min, noiseSpeed_max));

//  Serial.println(micSmoothed);

  //Serial.println("micRaw:");
  //Serial.println(micRaw);
  //Serial.println(" ");
  //Serial.println("micSmooth:");
  //Serial.println(micSmoothed);

  sensorspeed1 = map(micRawOutput, 0, 1023, 0, 5);
  //Serial.println("1:");
  //Serial.println(sensorspeed1);

  sensorlength1 = map(micRawOutput, 0, 1023, 0, 30);
  //Serial.println("2:");
  //Serial.println(sensorlength1);

  sensorbrightness1 = map(micRawOutput, 0, 1023, 0, 50);
  //Serial.println("3:");
  //Serial.println(sensorbrightness1);

  sensormovementspeed1 = map(micRawOutput, 0, 1023, 0, 30);
  //Serial.println("4:");
  //Serial.println(sensormovementspeed1);

   noiseRaw = inoise8(noiseX);
         // The range of the inoise8 function is roughly 16-238.
      // These two operations expand those values out to roughly 0..255
      // You can comment them out if you want the raw noise data.
   noiseScaled = qsub8(noiseRaw,16);
   noiseScaled = qadd8(noiseScaled,scale8(noiseScaled,39));

   noiseX += noiseSpeed;

   ledPointer = round(map(noiseScaled, 0, 255, 0, NUM_LEDS));
   Serial.println(ledPointer);

  if ( previousPointer == ledPointer) {
    ledPointer++;
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // turn on randomly selected led
    areLedsToggled[ledPointer] = true;

    leds[ledPointer].setRGB(255, 255, 255);
    FastLED.show();

    // turn off previous led
    areLedsToggled[previousPointer] = false;
    leds[previousPointer].setRGB(0, 0, 0);
    FastLED.show();

    // save current on led index
    previousPointer = ledPointer;
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
  //    FastLED.show();
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
