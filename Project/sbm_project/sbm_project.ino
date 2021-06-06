//OLED I2C Adafruit libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const byte ledRPin = 5;
const byte ledYPin = 4;
const byte ledGPin = 3;
const byte trigPin = 9;
const byte echoPin = 10;
const byte buzzPin = 8;

int countdown, state;
float duration=0, distance=0;

void OledDisplay(int state, int countdown){
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(WHITE);
  if(state == 0){
    if(String(countdown).length() == 2){
      display.setCursor(45,20);
    }
    else{
      display.setCursor(55,20);
    }
    display.println(String(countdown));
    display.display();
  }
  else if(state == 1){
    display.setCursor(45,20);
    display.println("!!");
    display.display();
  }
  else if(state == 2){
    if(String(countdown).length() == 2){
      display.setCursor(45,20);
    }
    else{
      display.setCursor(55,20);
    }
    display.println(String(countdown));
    display.display();
  }
}

void setup() {
  Serial.begin(9600);
  
  countdown = 10;
  state = 0;

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  delay(1000); // Pause for 1 seconds

  // display welcome command
  OledDisplay(state, countdown);

  // pin setup
  pinMode(trigPin, OUTPUT); // trig pin will have pulses output
  pinMode(echoPin, INPUT);  // echo pin should be input to get pulse width
  pinMode(buzzPin, OUTPUT); // buzz pin is output to control buzzering
  pinMode(ledRPin, OUTPUT); // red LED as output pin
  pinMode(ledYPin, OUTPUT); // yellow LED as output pin
  pinMode(ledGPin, OUTPUT); // green LED as output pin
}

void loop() {
  // countdown & state control
  countdown--;
  
  if(countdown == -1){
    if(state == 0){
      state = 2;
      countdown = 5;
    }
    else if(state == 1){
      state = 0;
      countdown = 10;
    }
    else if(state == 2){
      state=1;
      countdown = 2;
    }
  }

  // LED control
  if(state == 0){
    digitalWrite(ledRPin, HIGH);
    digitalWrite(ledYPin, LOW);
    digitalWrite(ledGPin, LOW);
  }
  else if (state == 1){
    digitalWrite(ledRPin, LOW);
  digitalWrite(ledYPin, HIGH);
  digitalWrite(ledGPin, LOW);
  }
  else if (state == 2){
    digitalWrite(ledRPin, LOW);
    digitalWrite(ledYPin, LOW);
    digitalWrite(ledGPin, HIGH);
  }

  // OLED Control
  OledDisplay(state, countdown);

//  Serial.print("state: "); Serial.println(state);
//  Serial.println(countdown);

  // Ultrasonic sensor acquisition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
//  Serial.print("Duration: "); Serial.println(duration);
  Serial.print("Distance: "); Serial.println(distance);

  // Piezo Buzzer control
  if ((distance <= 5.5 || distance >= 10) && distance >= 0 && state == 0) {
    // Buzz
    digitalWrite(buzzPin, HIGH);
    delay(100);
    digitalWrite(buzzPin, LOW);
    delay(100);
    digitalWrite(buzzPin, HIGH);
    delay(100);
    digitalWrite(buzzPin, LOW);
    delay(100);
    digitalWrite(buzzPin, HIGH);
    delay(100);
    digitalWrite(buzzPin, LOW);
    delay(100);
    digitalWrite(buzzPin, HIGH);
    delay(100);
    digitalWrite(buzzPin, LOW);
    delay(300);
  } 
  else {
    // Don't buzz
    digitalWrite(buzzPin, LOW);
    delay(1000);
  }

//  delay(1000);
}
