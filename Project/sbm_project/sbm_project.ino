const byte trigPin = 3;
const byte echoPin = 4;
const byte buzzPin = 5;

int duration, distance;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); // trig pin will have pulses output
  pinMode(echoPin, INPUT); // echo pin should be input to get pulse width
//  pinMode(buzzPin, OUTPUT); // buzz pin is output to control buzzering
}

void loop() {
  digitalWrite(trigPin, HIGH);
  delay(1);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  Serial.print("Distance: "); Serial.println(distance);
  delay(500);
}
