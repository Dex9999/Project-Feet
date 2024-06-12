//ultra sonic sensors on front/side to see xy of foot
//gyro on foot to allow pedal-like movement? (or light sensor)
#include <Arduino_JSON.h>
#include <Servo.h>

const int xTrigPin = 3;
const int xEchoPin = 5;
const int yTrigPin = 11;
const int yEchoPin = 10;
const int xServoPin = 6;
const int yServoPin = 9;

const int xMidpoint = 130;
const int yMidpoint = 60;

float xduration;
float xdistance;
float yduration;
float ydistance;

float cxDistance;
float cyDistance;

Servo servoX;
Servo servoY;


void setup() {
  Serial.begin(9600);
  pinMode(yTrigPin, OUTPUT); // trigPin for y axis
  pinMode(yEchoPin, INPUT); // echoPin for y axis
  pinMode(xTrigPin, OUTPUT); // trigPin for x axis
  pinMode(xEchoPin, INPUT); // echoPin for x axis
  servoX.attach(xServoPin, 900, 2100);
  servoY.attach(yServoPin, 900, 2100);
  servoX.write(xMidpoint);
  servoY.write(yMidpoint);
}

JSONVar data;
JSONVar moveJson;

void loop() {
  // clear trigPin
  digitalWrite(xTrigPin, LOW);
  digitalWrite(yTrigPin, LOW);
  delayMicroseconds(2);
  // set trigPin to HIGH for 10 micro seconds
  digitalWrite(xTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(xTrigPin, LOW);
  // read echoPin, returns the soundwave travel time in microseconds
  xduration = pulseIn(xEchoPin, HIGH);
  // repeat with y
  digitalWrite(yTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(yTrigPin, LOW);
  yduration = pulseIn(yEchoPin, HIGH);

  // multiply by speed of sound (340m/s 34cm/ms) and divide by two as the sensor
  // measures the time to travel to object and back
  
  cxDistance = xduration * 0.034 / 2.000000;
  cyDistance = yduration * 0.034 / 2.000000;

  data["type"] = "mouse";

  if(cxDistance > 0 && cxDistance <= 25){
    xdistance = cos(radians(xMidpoint-servoX.read()))*cxDistance;
  } else {
    // if nothing seen reset to midpoint distance
    xdistance = 14;
  }
  moveJson["x"] = xdistance;
  // y is 0-120 l-r
  servoY.write(map(xdistance,3,25,0, 120));

  if(cyDistance > 0 && cyDistance <= 25){
    ydistance = cos(radians(yMidpoint-servoY.read()))*cyDistance;
  } else {
    // if nothing seen reset to midpoint distance
    ydistance = 14;
  }
  moveJson["y"] = ydistance;
  // x is 50-180 d-u
  servoX.write(map(ydistance,3,25,180, 60));
  
  
  data["move"] = moveJson;
  Serial.print("*" + JSON.stringify(data) + "*");
  
}