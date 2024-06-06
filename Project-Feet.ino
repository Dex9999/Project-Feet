//ultra sonic sensors on front/side to see xy of foot
//gyro on foot to allow pedal-like movement? (or light sensor)
#include <Arduino_JSON.h>
const int xTrigPin = 5;
const int xEchoPin = 6;
const int yTrigPin = 9;
const int yEchoPin = 10;

float xduration;
int xdistance;
float yduration;
int ydistance;

void setup() {
  Serial.begin(9600);
  pinMode(yTrigPin, OUTPUT); // trigPin for y axis
  pinMode(yEchoPin, INPUT); // echoPin for y axis
  pinMode(xTrigPin, OUTPUT); // trigPin for x axis
  pinMode(xEchoPin, INPUT); // echoPin for x axis
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
  moveJson["y"] = ydistance;
  data["move"] = moveJson;
  Serial.print("*" + JSON.stringify(data) + "*");
}