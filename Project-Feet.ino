#include <Arduino_JSON.h>
#include <Servo.h>

const int xTrigPin = 3;
const int xEchoPin = 5;
const int yTrigPin = 11;
const int yEchoPin = 10;
const int xServoPin = 6;
const int yServoPin = 9;
const int buttonPin = 2; 

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

// center (x,y) left (x,y) right (x,y) up (x,y) down (x,y)
float calibrationValues[5][2]; // my best attempt
float servoPositions[5][2] = {
    {130, 60},   // Center: xy
    {130, 0}, // Left: xy
    {130, 120}, // Right: xy
    {170, 60},   // Up: xy
    {80, 60}   // Down: xy
};
int calibrationStep = 0; 

void setup() {
  Serial.begin(9600);
  pinMode(yTrigPin, OUTPUT); // trigPin for y axis
  pinMode(yEchoPin, INPUT); // echoPin for y axis
  pinMode(xTrigPin, OUTPUT); // trigPin for x axis
  pinMode(xEchoPin, INPUT); // echoPin for x axis
  pinMode(buttonPin, INPUT_PULLUP); 

  servoX.attach(xServoPin, 900, 2100);
  servoY.attach(yServoPin, 900, 2100);
}

JSONVar data;
JSONVar moveJson;
int currentMode = 0;
void loop() {
  
  if (calibrationStep < 5) {
    servoX.write(servoPositions[calibrationStep][0]);
    servoY.write(servoPositions[calibrationStep][1]);
    if (digitalRead(buttonPin) == LOW) { 
      delay(200);
      calibrate();
      calibrationStep++;
      if (calibrationStep == 5) {
        Serial.println("Calibration complete");
        servoX.attach(xServoPin, 900, 2100); 
        servoY.attach(yServoPin, 900, 2100); 
      } else {
        Serial.print("Move to position ");
        Serial.print(calibrationStep);
      }
    }
  } else {

    if (digitalRead(buttonPin) == LOW) {
      delay(200);  
      currentMode = (currentMode + 1) % 3;  // cycle  modes (0 = servo, 1 = no servo, 2 = pedal)
    }

    if (currentMode == 0) {
      servoTrack();
    }
    else if (currentMode == 1) {
      readSensorsOnly();  
    }
    else if (currentMode == 2) {
      pedalMode();  
    }
    
  }
}

void pedalMode(){
  digitalWrite(yTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(yTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(yTrigPin, LOW);
  yduration = pulseIn(yEchoPin, HIGH);
  ydistance = yduration * 0.034 / 2.000000;
  data["type"] = "pedal";
  moveJson["y"] = ydistance;
  data["move"] = moveJson;
  Serial.print("*" + JSON.stringify(data) + "*");
  
}

void readSensorsOnly(){
  digitalWrite(xTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(xTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(xTrigPin, LOW);
  xduration = pulseIn(xEchoPin, HIGH);
  xdistance = xduration * 0.034 / 2.000000;

  digitalWrite(yTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(yTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(yTrigPin, LOW);
  yduration = pulseIn(yEchoPin, HIGH);
  ydistance = yduration * 0.034 / 2.000000;

  if(xdistance > calibrationValues[2][0]){
    xdistance = (calibrationValues[2][0]+calibrationValues[1][0])/2;
  }
  if(ydistance > calibrationValues[4][1]){
    ydistance = (calibrationValues[3][1]+calibrationValues[4][1])/2;
  }
  if(xdistance > calibrationValues[2][0] && ydistance > calibrationValues[4][1]){
    return;
  }
  data["type"] = "mouse";
  moveJson["x"] = 0;
  moveJson["y"] = 0;
  if(abs((calibrationValues[2][0]+calibrationValues[1][0])/2-xdistance) > abs((calibrationValues[3][1]+calibrationValues[4][1])/2-ydistance)){
    // Serial.print("x");
    // Serial.println(xdistance);
    moveJson["x"] = map(xdistance, calibrationValues[1][0],calibrationValues[2][0], -20, 20);
  } else{
    // Serial.print("y");
    // Serial.println(ydistance);
    moveJson["y"] = map(ydistance, calibrationValues[3][1],calibrationValues[4][1], -20, 20);;
  }
  data["move"] = moveJson;
  Serial.print("*" + JSON.stringify(data) + "*");
}

void servoTrack(){
  
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

    if(cxDistance > calibrationValues[1][0] && cxDistance <= calibrationValues[2][0]){
      xdistance = cos(radians(servoPositions[0][0]-xMidpoint))*cxDistance;
    } else {
      // if nothing seen reset to midpoint distance
      xdistance = (calibrationValues[2][0]+calibrationValues[1][0])/2;
    }
    moveJson["x"] = map(xdistance, calibrationValues[1][0],calibrationValues[2][0], -20, 20);
    // y is 0-120 l-r
    servoY.write(map(xdistance,calibrationValues[1][0],calibrationValues[2][0],servoPositions[1][1],servoPositions[2][1]));

    if(cyDistance > calibrationValues[3][1] && cyDistance <= calibrationValues[4][1]){
      ydistance = cos(radians(servoPositions[0][1]-yMidpoint))*cyDistance;
    } else {
      // if nothing seen reset to midpoint distance
      ydistance = (calibrationValues[3][1]+calibrationValues[4][1])/2;
    }
    moveJson["y"] = map(ydistance, calibrationValues[3][1],calibrationValues[4][1], -20, 20);;
    // x is 40-180 d-u
    servoX.write(map(ydistance,calibrationValues[3][1],calibrationValues[4][1],servoPositions[3][0], servoPositions[4][0]));

    data["move"] = moveJson;
    Serial.print("*" + JSON.stringify(data) + "*");
}

void calibrate() {
  
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

  delay(200);
  calibrationValues[calibrationStep][0] = cxDistance;
  // calibrationValues[calibrationStep][0][1] = servoX.read();
  calibrationValues[calibrationStep][1] = cyDistance;
  // calibrationValues[calibrationStep][1][1] = servoY.read();

  Serial.print("Calibration step ");
  Serial.print(calibrationStep);
  Serial.print(": x = ");
  Serial.print(cxDistance);
  Serial.print(" cm, y = ");
  Serial.println(cyDistance);
}
