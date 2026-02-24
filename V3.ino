int sigLed=8;
int swPin=9;

int relayPin=4; //temp

int frontHall=A0;
int rearHall=A1; 

float circRear=0.80; // metri, treba tocno izmjerit
float circFront=0.80; // metri, treba tocno izmjerit

int magNumRear=1; // vise magneta > veca preciznost
int magNumFront=1; // vise magneta > veca preciznost

volatile unsigned long pulseRearCount=0;
volatile unsigned long pulseFrontCount=0;

unsigned long lastMillis=0;

float speedRear=0;
float speedFront=0;
float speedDiffVal=0;

float tresh=5;

// pragovi za analog pulse detection (TREBA PODESIT)
int rearHigh=600;
int rearLow=500;
int frontHigh=600;
int frontLow=500;

// stanje za hysteresis (da ne broji vise puta)
bool rearWasHigh=false;
bool frontWasHigh=false;

float rearWheelSpeed(float dt){

  float revs = (float)pulseRearCount / magNumRear;
  float dist = revs * circRear;
  float mps = dist / dt;

  return mps * 3.6; // pretvori u kmh
}
float frontWheelSpeed(float dt){

  float revs = (float)pulseFrontCount / magNumFront;
  float dist = revs * circFront;
  float mps = dist / dt;

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

  pinMode(rearHall, INPUT);
  pinMode(frontHall, INPUT);

  lastMillis = millis();
  Serial.begin(9600);
}



void loop() {
  // put your main code here, to run repeatedly:
  
  //debug test za peak i idle
Serial.print("Front: ");
Serial.print(analogRead(A0));
Serial.print("  Rear: ");
Serial.println(analogRead(A1));
delay(500);

  // citanje analognog halla i pretvaranje u pulse
  int rearVal = analogRead(rearHall);
  int frontVal = analogRead(frontHall);

  // rear pulse detection
  if(!rearWasHigh && rearVal >= rearHigh){
    pulseRearCount++;
    rearWasHigh = true;
  }
  else if(rearWasHigh && rearVal <= rearLow){
    rearWasHigh = false;
  }

  // front pulse detection
  if(!frontWasHigh && frontVal >= frontHigh){
    pulseFrontCount++;
    frontWasHigh = true;
  }
  else if(frontWasHigh && frontVal <= frontLow){
    frontWasHigh = false;
  }


  if(digitalRead(swPin)==1){
    digitalWrite(sigLed, LOW);
    digitalWrite(relayPin, LOW); // drzi u NC
    return;
  }

  digitalWrite(sigLed, HIGH); 


  if(millis()-lastMillis >= 100){ // brze update za ABS

    float dt = (millis()-lastMillis)/1000.0;

    speedRear=rearWheelSpeed(dt);
    speedFront=frontWheelSpeed(dt);
    speedDiffVal=speedDiff();
  
    pulseFrontCount = 0;
    pulseRearCount = 0;

    lastMillis = millis();


    if((speedFront - speedRear)>tresh){
      digitalWrite(relayPin, HIGH); // stavi u NO
    }
    else{
      digitalWrite(relayPin, LOW); // ostani u NC
    }
  }
}