int sigLed=8;
int swPin=9;

int relayPin=4; //temp

int frontHall=2;
int rearHall=3; 

int circRear=0.80; // metri, treba tocno izmjerit
int circFront=0.80; // metri, treba tocno izmjerit

int magNumRear=1; // vise magneta > veca preciznost
int magNumFront=1; // vise magneta > veca preciznost

volatile unsigned long pulseRearCount=0;
volatile unsigned long pulseFrontCount=0;

unsigned long lastMillis=0;

float speedRear=0;
float speedFront=0;
float speedDiffVal=0;

float tresh=5;


void pulseRear(){
  pulseRearCount++;
}
void pulseFront(){
  pulseFrontCount++;
}

float rearWheelSpeed(){

  float revs = (float)pulseRearCount / magNumRear;
  float dist = revs * circRear;
  float mps = dist / 1.0;

  return mps * 3.6; // pretvori u kmh
}
float frontWheelSpeed(){

  float revs = (float)pulseFrontCount / magNumFront;
  float dist = revs * circFront;
  float mps = dist / 1.0;

  return mps * 3.6; // pretvori u kmh
}

float speedDiff(){
  return speedRear - speedFront;
}


void setup() {
  // put your setup code here, to run once:
  pinMode(swPin, INPUT_PULLUP);
  pinMode(sigLed, OUTPUT);
  
  pinMode(relayPin, OUTPUT);

  pinMode(rearHall, INPUT_PULLUP);
  pinMode(frontHall, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(rearHall), pulseRear, FALLING);
  attachInterrupt(digitalPinToInterrupt(frontHall), pulseFront, FALLING);
}



void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(swPin)==1){
  digitalWrite(sigLed, LOW);
  digitalWrite(relayPin, LOW); // drzi u NC
  return;
  }

    digitalWrite(sigLed, HIGH); 

    if(millis()-lastMillis >= 1000){
        detachInterrupt(digitalPinToInterrupt(rearHall));
        detachInterrupt(digitalPinToInterrupt(frontHall));

        speedRear=rearWheelSpeed();
        speedFront=frontWheelSpeed();
        speedDiffVal=speedDiff();
      
        pulseFrontCount = 0;
        pulseRearCount = 0;

        lastMillis = millis();


        attachInterrupt(digitalPinToInterrupt(rearHall), pulseRear, FALLING);
        attachInterrupt(digitalPinToInterrupt(rearHall), pulseFront, FALLING);

        if((speedFront - speedRear)>tresh){
          digitalWrite(relayPin, HIGH); // stavi u NO
        }
        else{
          digitalWrite(relayPin, LOW); // ostani u NC
        }
    }
}

