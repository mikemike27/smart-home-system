#include "Servo.h"

#define WHITE_LED 7
#define GREEN_LED 8
#define RED_LED 9
#define PIR_PIN 2
#define LDR_PIN A0
#define BUZZER_PIN 10
#define SERVO_PIN 3
#define BUTTON_PIN 4

Servo myServo;

int sensorState = 0;
int LDRValue = 0;
int systemState = 0; //the whole smart home security system
int nightState = 0;
int lockState = 0;
int unlocking = 0;

String password = "12345";

void setup() {
  // put your setup code here, to run once:
  pinMode(WHITE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  myServo.attach(SERVO_PIN);

  Serial.begin(9600);

  digitalWrite(GREEN_LED, HIGH);

}

void loop() {

  while(unlocking == 1){
    while(Serial.available() == 0){
      //Serial.println("waiting for pass");
    }
    
    String pass = Serial.readString();
    pass.trim();
    if(pass == password){
      //unlock the system
      lockState = 0;
      myServo.write(0);
      noTone(BUZZER_PIN);
      unlocking = 0;

      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);

    }
    else{
      Serial.println("Password incorrect");
    }
  }

  Serial.println("What would you like to do?");
  Serial.println("1. System Off");
  Serial.println("2. System On");
  Serial.println("3. Lock");
  Serial.println("4. Unlock");

  while(Serial.available() == 0){

    if(digitalRead(BUTTON_PIN) == HIGH){
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
    }

    //system is on
    if(systemState == 1){

      //LDR
      LDRValue = analogRead(LDR_PIN);
      //Serial.println(LDRValue);

      if(LDRValue < 500){
        //night is coming, turn on the spotlight
        nightState = 1;

      }else{
        //night is ending, turn off the spotlight to save power
        //only turn off the spotlight not the movement sensor
        //because there are thieves in the day time and you need to continue detecting in day time
        nightState = 0;

        //always set to low
        digitalWrite(WHITE_LED, LOW);

      }


      // put your main code here, to run repeatedly:
      sensorState = digitalRead(PIR_PIN);

      if(sensorState == HIGH){

        if(nightState == 1){
          digitalWrite(WHITE_LED, HIGH);
        }
      }
      else{
        digitalWrite(WHITE_LED, LOW);
      }

    }

  }
  
  int input = Serial.parseInt();

  switch(input){
    case 1:
      if(systemState == 0){

        //system already down, no need set again
        Serial.println("System is already down.");
        //return;

      }
      else{

        //system is on, turn off system and sensors
        systemState = 0;
        lockState = 0; 

        digitalWrite(WHITE_LED, LOW);
        myServo.write(0);
        noTone(BUZZER_PIN);

      }
      break;
    case 2:
      if(systemState == 1){

        //system already down, no need set again
        Serial.println("System is already up.");
        //return;

      }
      else{

        //system is on, turn off system
        systemState = 1;

      }
      break;
    case 3:
      if(systemState == 0){
        Serial.println("The system is down. Please turn on the system first");
      }
      else{

        if(lockState == 0){

          //lock the thieves inside the house, and turn on the siren(buzzer) to alert the security
          lockState = 1;
          myServo.write(180);
          tone(BUZZER_PIN, 1000);

        }
        else{
          Serial.println("The system is already locked.");
        }

      }
      break;
    case 4:

      if(systemState == 0){
        Serial.println("The system is down. Please turn on the system first");
      }
      else{

        if(lockState == 1){
          
          Serial.println("Please input the password: ");
          unlocking = 1;

        }
        else{
          Serial.println("The system is already unlocked.");
        }

      }

      break;
  }

  delay(1000);

}

