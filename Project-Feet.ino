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


float xduration;
int xdistance;
float yduration;
int ydistance;

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
  servoX.write(130);
  servoY.write(60);
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
  xdistance = xduration * 0.034 / 2.000000;
  ydistance = yduration * 0.034 / 2.000000;
  
  data["type"] = "mouse";
  moveJson["x"] = xdistance;
  // servoY.write(map(ydistance, 0, 120, 0, 180)); // not workiongs
  moveJson["y"] = ydistance;
  data["move"] = moveJson;
  Serial.print("*" + JSON.stringify(data) + "*");

  // if(abs(xdistance-14) > abs(ydistance-14)){
    // y is 0-120 l-r
    servoY.write(map(xdistance,3,25,0, 120));
  // } else{
    // x is 50-180 d-u
    servoX.write(map(ydistance,3,25,180, 50));
  // }
  
}