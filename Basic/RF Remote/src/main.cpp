#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Radio Setup
RF24 radio(7, 8);  // CE, CSN
byte sAddresses[][6] = {"BasT","BasR"};

//Radio Data
typedef struct{
  bool pair = false;
  int channel = 0;
}
remote;
remote remoteData;

// Controller Setup
#define rightButton 2
boolean rightbuttonState;
#define leftButton 4
boolean leftbuttonState;

// Controller Ports
int leftSlider = A1;
int leftValue = 0;
int rightSlider = A0;
int rightValue = 0;

int red_light_pin= 6;
int green_light_pin = 5;
int blue_light_pin = 3;

#define pairButton 10
boolean pairbuttonState;


//Controller Data
typedef struct{
  int leftSpeed = 90;   // debug value
  int rightSpeed = 90;
  bool leftTrigger = 0;
  bool rightTrigger = 0;

}
ctrl;
ctrl ctrlData;

const int maxSpeed = 105;
const int minSpeed = 80;

void RGB_color(int red_light_value, int green_light_value, int blue_light_value){
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}


void getController(){
  //Serial.println("Get controls");
  rightbuttonState = !digitalRead(rightButton);
  leftbuttonState = !digitalRead(leftButton);
  leftValue = analogRead(leftSlider);
  rightValue = analogRead(rightSlider);  
  //Serial.println(rightbuttonState);
  // Serial.println(leftValue);
}

void sendData(){
  ctrlData.leftTrigger = leftbuttonState;
  ctrlData.rightTrigger = rightbuttonState;

  //Left Control
  if (leftValue < 300)
  {
    ctrlData.leftSpeed = map(leftValue, 1, 290, minSpeed, 88);
    if (ctrlData.leftTrigger)
    {
      ctrlData.leftSpeed = 1;
    }
    //ctrlData.leftSpeed = 88;
  }else if (leftValue > 550)
  {
    ctrlData.leftSpeed = map(leftValue, 560, 708, 98, maxSpeed);
    //ctrlData.leftSpeed = 98;
    if (ctrlData.leftTrigger)
    {
      ctrlData.leftSpeed = 180;
    }
  }
  else{
    ctrlData.leftSpeed = 92;
    if (ctrlData.leftTrigger)
    {
      ctrlData.leftSpeed = 180;
    }
  }
  
  // rightValue = map(rightValue, 0, 708, 708, 0);
  //Right Control
  if (rightValue < 300)
  {
    ctrlData.rightSpeed = map(rightValue, 1, 290, minSpeed, 88);
    if (ctrlData.leftTrigger)
    {
      ctrlData.rightSpeed = 1;
    }
    //ctrlData.rightSpeed = 88;
  }else if (rightValue > 550)
  {
    ctrlData.rightSpeed = map(rightValue, 560, 708, 98, maxSpeed);
    //ctrlData.rightSpeed = 98;
    if (ctrlData.leftTrigger)
    {
      ctrlData.rightSpeed = 180;
    }
  }
  else{
    ctrlData.rightSpeed = 92;
    if (ctrlData.leftTrigger)
    {
      ctrlData.rightSpeed = 1;
    }
  }
  
  //ctrlData.leftSpeed = map(leftValue, 4, 708, 0, 180);
  //ctrlData.rightSpeed = map(rightValue, 4, 708, 0, 180);
  //Serial.println(ctrlData.rightTrigger);
  radio.write( &ctrlData, sizeof(ctrlData) );

}

void setup() {
  Serial.begin(9600);
  Serial.println("Start Remote Basic");

  radio.begin();
  radio.openWritingPipe(sAddresses[0]);
  radio.setAutoAck(1);
  radio.stopListening();

  pinMode(rightButton,INPUT_PULLUP);
  pinMode(leftButton,INPUT_PULLUP);
  pinMode(pairButton,INPUT_PULLUP);

  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);

  // put your setup code here, to run once:
}

void loop() {
  //Serial.println("Transmit");
  RGB_color(255, 255, 255); // Red
  getController();
  sendData();  
  delay(1000);
}