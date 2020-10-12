// Juz'an Nafi Haifa
// 17/410473/TK/45830

const byte pinLed[4] = {6, 5, 4, 3}; // connect to the LED
char input; // input read

void LedRunning(){ // Run the LEDs from pinLed[0] to pinLed[3]
  char i,j;

  bool LedRun[4] = {HIGH, HIGH, HIGH, LOW};
  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      digitalWrite(pinLed[j], LedRun[j]);
    }
    bool temp = LedRun[0];
    for(j=0; j<3; j++){
      LedRun[j] = LedRun[j+1];
    }
    LedRun[3] = temp;
    delay(500);
  }
}

void setup() {
  Serial.begin(9600); // baud rate 9600 bps
  char i;
  for (i = 0; i < 4; i++){    // initiate the LEDs
    pinMode(pinLed[i],OUTPUT);
    digitalWrite(pinLed[i], HIGH);
  }
}

void loop() {
  Serial.println("Press any key");
  while (!Serial.available()) {} // hold the commands
  input = Serial.read();  // read the input
  
  while (!Serial.available()) {
      Serial.println("Hello World!"); // display "Hello World"! to Serial Monitor
      LedRunning(); // run LEDs
      input = Serial.read();
    }
}
