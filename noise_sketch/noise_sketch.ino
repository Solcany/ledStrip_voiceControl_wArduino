#include <FastLED.h>

const int num_cells = 10;
uint8_t noiseRaw;
uint8_t noiseScaled;
int ledPointer;

uint16_t noiseSpeed = 5;


static uint16_t noiseX;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  random16_set_seed(7500);
  noiseX = random16();


}

void loop() {
  // put your main code here, to run repeatedly:
   
   noiseRaw = inoise8(noiseX);
         // The range of the inoise8 function is roughly 16-238.
      // These two operations expand those values out to roughly 0..255
      // You can comment them out if you want the raw noise data.
   noiseScaled = qsub8(noiseRaw,16);
   noiseScaled = qadd8(noiseScaled,scale8(noiseScaled,39));

   ledPointer = round(map(noiseScaled, 0, 255, 0, num_cells));

   noiseX += noiseSpeed;

   Serial.println(ledPointer);
   delay(500);
    

}
