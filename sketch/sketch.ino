#include <FastLED.h>
//#define PI 3.1415926535897932384626433832795
#define LED_PIN     7
#define SOUND_MIN 333
#define SOUND_MAX 400

#define micPin A0

#define NUM_LEDS    10//Number of leds
//int stepLength    = 30; //difference in brightness between neighboring leds. Can make it multiples of 30
//int stepSpeed     = 5;  //difference in speed/intensity of change of the individual leds max=360
//int brightness    = 50; //control brightness. values 0-127
//int movementSpeed = 30; //control delay of movement (in milliseconds)


// rgb settings
const int minRgb = 0;
const int maxRgb = 255;

// pulse settings
const int pulseIntervalMin = 2;
const int pulseIntervalMax = 2500;
int interval = pulseIntervalMax;

// delay
unsigned long previousMillis = 0;
boolean areLedsToggled[NUM_LEDS];
int previousToggle;

// sound input smoothing & sampling
const int sound_minmax_diff = SOUND_MAX - SOUND_MIN;
const int avgSmoothing = 10;
const int envSmoothing = 2;
const int numSamples = 1000;
int micSmoothed = 0;


int sensorlength = 0;
int sensorlength1 = 0;
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

}


void loop()
{
  // read output from microphone

  micSmoothed = getSound();
  interval = (int) round(map(micSmoothed, 0, sound_minmax_diff, pulseIntervalMax, pulseIntervalMin));
  Serial.println(micSmoothed);

  //Serial.println("micRaw:");
  //Serial.println(micRaw);
  //Serial.println(" ");
  //Serial.println("micSmooth:");
  //Serial.println(micSmoothed);

  sensorspeed1 = map(micSmoothed, 0, 1023, 0, 5);
  //Serial.println("1:");
  //Serial.println(sensorspeed1);

  sensorlength1 = map(micSmoothed, 0, 1023, 0, 30);
  //Serial.println("2:");
  //Serial.println(sensorlength1);

  sensorbrightness1 = map(micSmoothed, 0, 1023, 0, 50);
  //Serial.println("3:");
  //Serial.println(sensorbrightness1);

  sensormovementspeed1 = map(micSmoothed, 0, 1023, 0, 30);
  //Serial.println("4:");
  //Serial.println(sensormovementspeed1);

  int r = (int) round(random(0, NUM_LEDS));
  if ( previousToggle == r) {
    r++;
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // turn on randomly selected led
    areLedsToggled[r] = true;

    leds[r].setRGB(255, 255, 255);
    FastLED.show();

    // turn off previous led
    areLedsToggled[previousToggle] = false;
    leds[previousToggle].setRGB(0, 0, 0);
    FastLED.show();

    // save current on led index
    previousToggle = r;
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
  delay(5);
}
int getSound() {
  static int average = SOUND_MIN;
  static int avgEnvelope = 0;
  int envelope = 0;
  
  for ( int i = 0; i< numSamples; i++ ) {
    int sound = analogRead(micPin);
    int sampleEnvelope = abs(sound - average);
    envelope = (sampleEnvelope+envelope)/2;
    avgEnvelope = (envSmoothing * avgEnvelope + sampleEnvelope) / (envSmoothing + 1);
    average = (avgSmoothing * average + sound) / (avgSmoothing + 1);
  }
  return envelope;
}
