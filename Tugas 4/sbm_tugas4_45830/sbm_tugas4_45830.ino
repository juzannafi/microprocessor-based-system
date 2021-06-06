#define TOut 11 //Pin 11 (OC2A/PB3) as output pin
#define F_CPU 16000000UL

bool opt = false; //checking whether have setting or not
char inputMode, inputPrescaler; //serial input for mode and prescaler option (from keyboard)
String inputPreload, inputCompareValue; //serial input for preload value and compare value (from keyboard)
char inputReload = '0'; //serial input for reload value option (from keyboard)
int mode;
volatile unsigned pulseTime = 0, offPulseTime=0; //pulse width in clock cycles

//clear serial input
void SerialFlush() {
  while (Serial.available() > 0) {
    char t = Serial.read();
  }
}

// input preload and reload value
void PreloadReload(){
  Serial.println("");
  Serial.println("Enter preload value (0 - 254):                          ");
  SerialFlush();
  while (!Serial.available()) {} //checking for serial input
  inputPreload = Serial.readString(); //read input from Keypad
  Serial.print(">> Preload value: ");
  Serial.println(inputPreload);

  //skip input reload if preload is 0 (reload value is auto 0)
  if(inputPreload != "0\n"){
    Serial.println("Enter reload value (1 or 2):");
    Serial.println("1. Reload to 0");
    Serial.print("2. Reload to preload value: ");
    Serial.print(inputPreload);
    SerialFlush();
    while (!Serial.available()) {} //checking for serial input
    inputReload = Serial.read(); //read input from Keypad

    Serial.print(">> ");
    switch(inputReload){
      case '1':
        Serial.println("Reload to 0");
        break;
      case '2':
        Serial.print("Reload to ");
        Serial.println(inputPreload);
        break;
     default:
        break;
    }
  }
}

//input compare value
void CompareValue(){
  Serial.println("");
  Serial.println("Enter compare value (0 - 255): ");
  SerialFlush();
  while (!Serial.available()) {} //checking for serial input
  inputCompareValue = Serial.readString(); //read input from Keypad
  Serial.print(">> Compare value: ");
  Serial.println(inputCompareValue);
}

ISR(TIMER2_OVF_vect){
  digitalWrite(TOut, digitalRead(TOut) ^ 1); //toggle

  //Reload setup
  switch(inputReload){
    case '1':
      TCNT2 = 0;
      break;
    case '2':
      TCNT2 = inputPreload.toInt();
      break;
    default:
      break;
  }
}

ISR(TIMER2_COMPA_vect){
  digitalWrite(TOut, digitalRead(TOut) ^ 1); //toggle
  if(mode == 2){
    TCNT2 = 0; //reload to 0
  }
}

ISR(TIMER1_CAPT_vect){
  static unsigned risingEdgeTime = 0;
  static unsigned fallingEdgeTime = 0;
  
  // Which edge is armed?
  if (TCCR1B & (1 << ICES1)){
    // Rising Edge
    risingEdgeTime = ICR1;
//    Serial.print("Rising: "); Serial.println(risingEdgeTime);
    TCCR1B &= ~(1 << ICES1); // Switch to Falling Edge
    offPulseTime = risingEdgeTime - fallingEdgeTime;
//    Serial.print("Off: "); Serial.println(offPulseTime);
  }
  else {
    // Falling Edge
    fallingEdgeTime = ICR1;
//    Serial.print("Falling: "); Serial.println(fallingEdgeTime);
    TCCR1B |= (1 << ICES1); // Switch to Rising Edge
    pulseTime = fallingEdgeTime - risingEdgeTime;
//    Serial.print("On: "); Serial.println(pulseTime);
  }
}

void ModeOVF(){
  Serial.println("Mode: Overflow");

  //Setup Register
  //OVF (Mode 0) -> bit WGM22=0, WGM21=0, WGM20=0 | COM2A1=0, COM2A0=0
  //Register TCCR2A=0b00xxxx00, Register TCCR2B=0bxxxx0xxxx
  TCCR2A = TCCR2A & 0b00111100;
  TCCR2B = TCCR2B & 0b11110111;

  //Setup Preload Counter
  PreloadReload();
  TCNT2 = inputPreload.toInt();

  //Turn on timer overflow interrupt flag -> timsk2=0bxxxxxxx1
  TIMSK2 = 0;
  TIMSK2 = TIMSK2 | 0b00000001;
  
  //Turn on global interrupts
  sei();
}

void ModeCTC(){
  Serial.println("Mode: Clear Timer on Compare (CTC)");

  //Setup Register
  //CTC (Mode 2) -> bit WGM22=0, WGM21=1, WGM20=0 | COM2A1=0, COM2A0=0
  //Register TCCR2A=0b00xxxx10, Register TCCR2B=0bxxxx0xxxx
  TCCR2A = TCCR2A & 0b00111110;
  TCCR2A = TCCR2A | 0b00000010;
  TCCR2B = TCCR2B & 0b11110111;

  //Setup Counter
  TCNT2 = 0;
  
  //Setup Compare Value
  CompareValue();
  OCR2A = inputCompareValue.toInt();

  //Turn on timer compare with OCRA interrupt flag -> timsk2=0bxxxxxx1x
  TIMSK2 = 0;
  TIMSK2 = TIMSK2 | 0b00000010;
  
  //Turn on global interrupts
  sei();
}

void ModeFastPWM(){
  Serial.println("Mode: Fast PWM");
  
  //Setup Register
  //Fast PWM (Mode 3 / non inverting) -> bit WGM22=0, WGM21=1, WGM20=1 | COM2A1=1, COM2A0=0
  //Register TCCR2A=0b10xxxx11, Register TCCR2B=0bxxxx0xxxx
  TCCR2A = TCCR2A & 0b10111111;
  TCCR2A = TCCR2A | 0b10000011;
  TCCR2B = TCCR2B & 0b11110111;

  //Setup Counter
  TCNT2 = 0;

  //Setup Duty Cycle
  float duty_cycle = 50.0;
  OCR2A = (duty_cycle*256)/100;

  //Turn on timer compare with OCRA interrupt flag -> timsk2=0bxxxxxx1x
  TIMSK2 = 0;
  TIMSK2 = TIMSK2 | 0b00000010;
  
  //Turn on global interrupts
  sei();
}

void ModePWMPC(){
  Serial.println("Mode: PWM Phase Correct");

  //Setup Register
  //PWM Phase Correct (Mode 1 / non inverting) -> bit WGM22=0, WGM21=0, WGM20=1 | COM2A1=1, COM2A0=0
  //Register TCCR2A=0b10xxxx01, Register TCCR2B=0bxxxx0xxxx
  TCCR2A = TCCR2A & 0b10111101;
  TCCR2A = TCCR2A | 0b10000001;
  TCCR2B = TCCR2B & 0b11110111;

  //Setup Counter
  TCNT2 = 0;

  //Setup Duty Cycle
  float duty_cycle = 50.0;
  OCR2A = (duty_cycle*256)/100;

  //Turn on timer compare with OCRA interrupt flag -> timsk2=0bxxxxxx1x
  TIMSK2 = 0;
  TIMSK2 = TIMSK2 | 0b00000010;
  
  //Turn on global interrupts
  sei();
}

void ChoosePrescaler(){
  Serial.println("Choose Prescaler (1 - 7) from keyboard");
  Serial.println("1. 1");
  Serial.println("2. 8");
  Serial.println("3. 32");
  Serial.println("4. 64");
  Serial.println("5. 128");
  Serial.println("6. 256");
  Serial.println("7. 1024");
  SerialFlush();
  while (!Serial.available()) {} //checking for serial input
  inputPrescaler = Serial.read(); //read input from Keypad

  Serial.print(">> ");
  switch(inputPrescaler){
    case '1':
            //1 -> TCCR2B=0bxxxxx001
            Serial.println("Prescaler: 1");
            TCCR2B = TCCR2B & 0b11111001;
            TCCR2B = TCCR2B | 0b00000001;
            break;
    case '2':
            //8 -> TCCR2B=0bxxxxx010
            Serial.println("Prescaler: 8");
            TCCR2B = TCCR2B & 0b11111010;
            TCCR2B = TCCR2B | 0b00000010;
            break;
    case '3':
            //32 -> TCCR2B=0bxxxxx011
            Serial.println("Prescaler: 32");
            TCCR2B = TCCR2B & 0b11111011;
            TCCR2B = TCCR2B | 0b00000011;
            break;
    case '4':
            //64 -> TCCR2B=0bxxxxx100
            Serial.println("Prescaler: 64");
            TCCR2B = TCCR2B & 0b11111100;
            TCCR2B = TCCR2B | 0b00000100;
            break;
    case '5':
            //128 -> TCCR2B=0bxxxxx101
            Serial.println("Prescaler: 128");
            TCCR2B = TCCR2B & 0b11111101;
            TCCR2B = TCCR2B | 0b00000101;
            break;
    case '6':
            //256 -> TCCR2B=0bxxxxx110
            Serial.println("Prescaler: 256");
            TCCR2B = TCCR2B & 0b11111110;
            TCCR2B = TCCR2B | 0b00000110;
            break;
    case '7':
            //1024 -> TCCR2B=0bxxxxx111
            Serial.println("Prescaler: 1024");
            TCCR2B = TCCR2B | 0b00000111;
    default:
            break;
  }
}

void setup() {
  Serial.begin(9600); // baudrate 9600 bps
  pinMode(TOut, OUTPUT);
  analogWrite(TOut, 255/2);
  noInterrupts();

  //reset Timer 1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TIFR1 |= (1 << ICF1);  // clear input capture flag so we don't get a bogus interrupt

  // start Timer 1
  TCCR1B |= (1 << CS10);  // no prescaler
  TCCR1B |= (1 << ICES1); // plus Input Capture Edge Select (rising on D11)
  TIMSK1 |= (1 << ICIE1);  // Enable Timer 1 Input Capture Interrupt Enable
  interrupts();
}

void loop() {
  if(opt == false){
    opt = true;
    Serial.println("==========================================================================");
    Serial.println("Enter mode (1 - 4) from keyboard:");
    Serial.println("1. Overflow");
    Serial.println("2. Clear Timer on Compare (CTC)");
    Serial.println("3. Fast PWM");
    Serial.println("4. PWM Phase Correct");
  
    SerialFlush();
    while (!Serial.available()) {} //checking for serial input 
    inputMode = Serial.read(); //read input from Keypad
    Serial.print(">> ");
    switch(inputMode){
      case '1':
              mode = 1;
              ModeOVF();
              break;
      case '2':
              mode = 2;
              ModeCTC();
              break;
      case '3':
              mode = 3;
              ModeFastPWM();
              break;
      case '4':
              mode = 4;
              ModePWMPC();
              break;
      default:
              break;
    }
    ChoosePrescaler();
  }

  Serial.println("");
  Serial.println("  ==================================================");
  Serial.println("| Press Enter or input any key to choose mode again |");
  Serial.println("  ==================================================");
  delay(10);

  SerialFlush();
  delay(10);

  while (!Serial.available()) { //checking for serial input    
    unsigned onClock = 0, offClock=0;
    noInterrupts();
    onClock = pulseTime;
    pulseTime = 0;

    offClock = offPulseTime;
    offPulseTime = 0;
    interrupts();

    if (onClock) {
//      Serial.print("On: "); Serial.println(onClock);
//      Serial.print("Off: "); Serial.println(offClock);

      float onTime = (float) onClock;
      float offTime = (float) offClock;
      float period = onTime + offTime;

      float frequency = (float) (F_CPU / period);
      Serial.print("Frequency: ");
      Serial.print(frequency);
      Serial.println(" Hz");
      
      float dutyCycle = ((float) onTime/period)*100;
      Serial.print("Duty Cycle: ");
      Serial.print(dutyCycle);
      Serial.println("%");
      Serial.println("");
    }
    else{
//      Serial.println("!!!");
    }
    delay(1000);
  } 
  inputMode = Serial.read(); //read input from Keypad

  if(Serial.available() || inputMode == '\n'){
    opt = false;
  }
  
  Serial.println("");
}
