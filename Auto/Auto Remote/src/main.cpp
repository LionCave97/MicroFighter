#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Radio Setup
RF24 radio(7, 8);  // CE, CSN
byte sAddresses[][6] = {"BasT","BasR"};

const bool autoPair = true;

//Radio Pair
typedef struct{
  int id = 1; // Each student will receive a id number
  bool paired = false;
}
pair;
pair pairData;


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
  int leftSpeed = 90;   /*These values will be used as the initial values*/
  int rightSpeed = 90;  
  bool leftTrigger = 0;
  bool rightTrigger = 0;
}
ctrl;
ctrl ctrlData;

void RGB_color(int red_light_value, int green_light_value, int blue_light_value){
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}

void pairNow(){
  radio.begin();
  radio.setChannel(1);
  radio.stopListening();
  radio.openReadingPipe(1, sAddresses[0]);
  radio.startListening();
  radio.write( &pairData, sizeof(pairData) );  
  if (radio.available())
    {
      Serial.println("Received");
      radio.read(&pairData, sizeof(pairData));
      Serial.println(pairData.id);
      pairData.paired = true;
      radio.setChannel(pairData.id);
      radio.stopListening();
      radio.openWritingPipe(sAddresses[0]);      
    }
}

void getController(){
  //Serial.println("Get controls");
  pairbuttonState = !digitalRead(pairButton);
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
  ctrlData.leftSpeed = map(leftValue, 4, 708, 0, 180);
  ctrlData.rightSpeed = map(rightValue, 4, 708, 0, 180);
  //Serial.println(ctrlData.rightTrigger);
  radio.write( &ctrlData, sizeof(ctrlData) );

}

void setup() {
  Serial.begin(9600);
  Serial.println("Remote Init");

  if (!autoPair)
  {
    radio.begin();
    radio.setChannel(pairData.id);
    radio.openWritingPipe(sAddresses[0]); 
    radio.stopListening();
    Serial.println("Manual Pair");
    pairData.paired = true;
  } else{
    Serial.println("Auto Pair");
  } 

  pinMode(rightButton,INPUT_PULLUP);
  pinMode(leftButton,INPUT_PULLUP);
  pinMode(pairButton,INPUT_PULLUP);

  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);

  //radio.setPALevel(RF24_PA_LOW); //Default Max Power

}

void loop() {  
  getController();
  if (pairbuttonState)
  {
    RGB_color(0, 0, 255); // Blue
    pairNow();
  }
  
  if (!pairData.paired)
  { 
    RGB_color(255, 0, 0); // Red
  }else {
    RGB_color(255, 255, 255); // White
    sendData();  
  }
  
}