#define micPin A0

const int measurementDuration = 500; // 5 seconds
const int micBreakDelay = 10000; // starting measuring only after the mic 'heats up'

float averageLoudness; 
int sum = 0;
int amount_of_measurements = 0;
int loopStartTime;
boolean isMicReady = false;
//boolean heatingMic = true;

void setup() {
  Serial.begin(9600);
  Serial.println(F("\nPrepare silent or loud environment and write something when ready..."));
  while (Serial.available() && Serial.read()); // empty buffer
  while (!Serial.available());                 // wait for data
  while (Serial.available() && Serial.read()); // empty buffer again
  Serial.println("...Let's calibrate!");
  
  loopStartTime = millis();
}

void loop() {    
    int mic = analogRead(micPin);
    int t = millis();
    
    if(t % 100 == 0) {
      Serial.println(mic);
    }    
        
    if(t < loopStartTime + micBreakDelay && isMicReady == false) {
      static boolean heatingMic = true;
      if(heatingMic == true) {
         Serial.println("...preparing mic...");
         heatingMic = false;
      }
      if(t % 500 == 0) {
        Serial.print('.');
      }
    } else if (isMicReady == false){
      Serial.println(" ");
      Serial.println("...mic ready, calibrating...");
      isMicReady = true;
      loopStartTime = millis();      
    }
    
    if(isMicReady == true) {
      if(t < loopStartTime + measurementDuration) {
          sum = sum + mic;
          Serial.println(sum);
          amount_of_measurements++;
          Serial.println(" ");  
          Serial.println(amount_of_measurements);          
        
//          if(t % 500 == 0) {
//            Serial.print('.');
//          }    
        } else {
          averageLoudness = sum / amount_of_measurements;
          Serial.println("average loudness of your enviroment is: ");
          Serial.println(averageLoudness);
          delay(2000);
          exit(0);
        }
    }
            
  delay(1);
}
