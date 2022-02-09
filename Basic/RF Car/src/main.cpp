#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

//Radio Setup
RF24 radio(7, 8);  // CE, CSN
byte sAddresses[][6] = {"BasT","BasR"};

//Radio Pair
typedef struct{
  int id = 1; // Each student will receive a id number
  bool paired = false;
}
pair;
pair pairData;

//Servo Data
typedef struct{
  int leftSpeed = 90;   // debug value
  int rightSpeed = 90;
  bool leftTrigger = 0;
  bool rightTrigger = 0;
}
ctrl;
ctrl ctrlData;


//Servo Setup
Servo leftServo;
Servo rightServo;

int motor = 2;

void setup() {
  Serial.begin(9600);
  Serial.println("Start Car");

  leftServo.attach(5);
  rightServo.attach(3);
  pinMode(motor, OUTPUT);
  digitalWrite(motor, LOW);

  radio.begin();
  radio.openReadingPipe(0, sAddresses[0]);
  radio.setAutoAck(1);
  radio.startListening();
}

void loop() {

  if (radio.available())
    {
      Serial.println("Received");
      radio.read(&ctrlData, sizeof(ctrlData));
      // Serial.println(ctrlData.rightSpeed);
      // Serial.println(ctrlData.leftSpeed);
      Serial.println(ctrlData.rightTrigger);
      Serial.println(ctrlData.leftTrigger);
      rightServo.write(ctrlData.rightSpeed);
      leftServo.write(ctrlData.leftSpeed);
      digitalWrite(motor, ctrlData.rightTrigger);
    }
}