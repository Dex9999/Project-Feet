//ultra sonic sensors on front/side to see xy of foot
//gyro on foot to allow pedal-like movement? (or light sensor)
#include <Arduino_JSON.h>
const int trigPin = 9;
const int echoPin = 10;

long duration;
int distance;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); // trigPin
  pinMode(echoPin, INPUT); // echoPin
}

const char input[] = "{\"type\":\"mouse\",\"button\":\"left/right/middle/doubleLeft/null\",\"drag\":false,\"newMovement\":{\"x\":5,\"y\":-5},\"scroll\":{\"x\":10,\"y\":15}}";
JSONVar data;
JSONVar moveJson;
void loop() {
 

  // Clear trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets trigPin to HIGH for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // multiply by speed of sound (340m/s 34cm/ms) and divide by two as the sensor
  // measures the time to travel to object and back
  distance = duration * 0.034 / 2;
  data["type"] = "mouse";
  moveJson["x"] = distance;
  moveJson["y"] = 0;
  data["newMovement"] = moveJson;
  Serial.print("*" + JSON.stringify(data) + "*");
}