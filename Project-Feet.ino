//ultra sonic sensors on front/side to see xy of foot
//gyro on foot to allow pedal-like movement? (or light sensor)

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("\"mouse\": {\"button\": \"null\",\"drag\": false,\"newMovement\": {\"x\": 5,\"y\": -5},\"scroll\": {\"x\": 10,\"y\": 15}}");
  delay(2000);
}