#include <SoftwareSerial.h>

//28BYJ 5V or 12V stepper Motor Connection
const int STEPPER_PIN_1 = 8;
const int STEPPER_PIN_2 = 7;
const int STEPPER_PIN_3 = 6;
const int STEPPER_PIN_4 = 5;

#define sensorPin1 12
#define sensorPin2 11
#define RedLed 2
#define Buzzer 13

int sensorState1 = 0;
int sensorState2 = 0;
int count = 0;

int Sms = 0;

const int pushButtonPin = 10;
const int pushButtonPin_1 = 9;

const int STEPS_PER_REVOLUTION = 2048;
const float DEGREES_PER_STEP = 360.0 / STEPS_PER_REVOLUTION;
const int TARGET_DEGREES = 20;

const int longPressDuration = 3000;
bool buttonPressed = false;
unsigned long buttonStartTime = 0;

const int longPressDuration_1 = 3000;
bool buttonPressed_1 = false;
unsigned long buttonStartTime_1 = 0;

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(4, 3); //SIM800L Tx & Rx is connected to Arduino #3 & #2

void setup() {
  // put your setup code here, to run once:
  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);

  pinMode(pushButtonPin, INPUT_PULLUP);
  pinMode(pushButtonPin_1, INPUT_PULLUP);

  pinMode(sensorPin1, INPUT_PULLUP);
  pinMode(sensorPin2, INPUT_PULLUP);
  pinMode(RedLed, OUTPUT);
  pinMode(Buzzer, OUTPUT);

  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("AUTOMATED DOOR MAT");
  Serial.println("No Patient in Waiting Room");
  delay(200);

  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

}

void loop() {
  sensorState1 = digitalRead(sensorPin1);
  sensorState2 = digitalRead(sensorPin2);

  if (sensorState1 == LOW) {
    count++;
    delay(500);
  }

  if (sensorState2 == LOW) {
    count--;
    delay(500);
  }

  if (count <= 0) {
    Serial.println("No Patient in Waiting Room");
  } else {
    Serial.print("Patient In Waiting Room: ");
    Serial.println(count);
  }

  if(count < 5){
    Sms = 0;
    digitalWrite(RedLed, LOW);
    digitalWrite(Buzzer, LOW);
       // Check for button press and hold
    if (digitalRead(pushButtonPin) == LOW) {
      if (!buttonPressed) {
        buttonPressed = true;
        buttonStartTime = millis();
      }

      if (millis() - buttonStartTime >= longPressDuration ) {
        rotateAntiClockwise();
        delay(10000);
        rotateClockwise();
        
      }
    } else {
      buttonPressed = false;
    }

    if (digitalRead(pushButtonPin_1) == LOW) {
      if (!buttonPressed_1) {
        buttonPressed_1 = true;
        buttonStartTime_1 = millis();
      }

      if (millis() - buttonStartTime_1 >= longPressDuration_1 ) {
        rotateClockwise();
        delay(10000);
        rotateAntiClockwise();
      }
    } else {
      buttonPressed_1 = false;
    }
  }else if(count >= 5){
    digitalWrite(RedLed, HIGH);
    digitalWrite(Buzzer, HIGH);
    if(Sms == 0){
      mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
      updateSerial();
      mySerial.println("AT+CMGS=\"+254748613509\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
      updateSerial();
      mySerial.print("5 Patient Waiting in the Waiting Room"); //text content
      updateSerial();
      mySerial.write(26);
      delay(1000);
      Sms = 1;
    }
    if (digitalRead(pushButtonPin_1) == LOW) {
      if (!buttonPressed_1) {
        buttonPressed_1 = true;
        buttonStartTime_1 = millis();
      }

      if (millis() - buttonStartTime_1 >= longPressDuration_1 ) {
        rotateClockwise();
        delay(10000);
        rotateAntiClockwise();
      }
    } else {
      buttonPressed_1 = false;
    }
  }


}
//Motor Function To rotate clockwise
void rotateClockwise() {
  int targetSteps = TARGET_DEGREES / DEGREES_PER_STEP;
  const int stepSequence[8][4] = {
    {HIGH, LOW, LOW, LOW},
    {HIGH, HIGH, LOW, LOW},
    {LOW, HIGH, LOW, LOW},
    {LOW, HIGH, HIGH, LOW},
    {LOW, LOW, HIGH, LOW},
    {LOW, LOW, HIGH, HIGH},
    {LOW, LOW, LOW, HIGH},
    {HIGH, LOW, LOW, HIGH}
  };
  for (int i = 0; i < targetSteps; i++) {
    for (int j = 0; j < 8; j++) {
      digitalWrite(STEPPER_PIN_1, stepSequence[j][0]);
      digitalWrite(STEPPER_PIN_2, stepSequence[j][1]);
      digitalWrite(STEPPER_PIN_3, stepSequence[j][2]);
      digitalWrite(STEPPER_PIN_4, stepSequence[j][3]);
      delayMicroseconds(1000);
    }
  }
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, LOW);
}
//Motor Function to ratate anticlockwise
void rotateAntiClockwise() {
  int targetSteps = TARGET_DEGREES / DEGREES_PER_STEP;
  const int stepSequence[8][4] = {
    {HIGH, LOW, LOW, HIGH},
    {LOW, LOW, LOW, HIGH},
    {LOW, LOW, HIGH, HIGH},
    {LOW, LOW, HIGH, LOW},
    {LOW, HIGH, HIGH, LOW},
    {LOW, HIGH, LOW, LOW},
    {HIGH, HIGH, LOW, LOW},
    {HIGH, LOW, LOW, LOW}  
    
  };
  for (int i = 0; i < targetSteps; i++) {
    for (int j = 0; j < 8; j++) {
      digitalWrite(STEPPER_PIN_4, stepSequence[j][3]);
      digitalWrite(STEPPER_PIN_3, stepSequence[j][2]);
      digitalWrite(STEPPER_PIN_2, stepSequence[j][1]);
      digitalWrite(STEPPER_PIN_1, stepSequence[j][0]);
      delayMicroseconds(1000);
    }
  }

  digitalWrite(STEPPER_PIN_4, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_1, LOW);
}
void updateSerial(){
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}