#define SILENT_VALUE 333 // starting neutral mic value (self-correcting)1

int micPin = A0;
int micVal = 0;

const int avgSmoothing = 10;
const int envSmoothing = 2;
const int numSamples = 500;

void setup() {
    Serial.begin(9600);         // open the serial connection to your PC
}

void loop() {

  micVal = getSound();
  Serial.print( "MIC = " );
  Serial.println( micVal );

  delay(250);

}

int getSound() {
  static int average = SILENT_VALUE;
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
