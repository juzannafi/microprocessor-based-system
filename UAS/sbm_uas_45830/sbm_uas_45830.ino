#define TOut 11 //Pin 11 (OC2A/PB3) as output pin
#define F_CPU 16000000UL
const byte InPins[4] = {2, 3, 4, 5}; //connect to the pushbuttons
const byte adc = 7; // connect to the potentio
byte state = 0, state2 = 0; mode, inputPrescaler;
bool selectModeFlag = false, selectPrescalerFlag = false, modeFreq = false;
int potVal, pwmOut, N;
volatile unsigned pulseTime = 0, offPulseTime=0; //pulse width in clock cycles

// Access push button
unsigned char PollingPushButton() {
  if (digitalRead(InPins[0]) == LOW){
    delay(10);
    if (digitalRead(InPins[0]) == LOW){
      delay(10);
      while (!(digitalRead(InPins[0]))) {};
      return 0;
    }
  }
  else if (digitalRead(InPins[1]) == LOW){
    delay(10);
    if (digitalRead(InPins[1]) == LOW){
      delay(10);
      while (!(digitalRead(InPins[1]))) {};
      return 1;
    }

  }
  else if (digitalRead(InPins[2]) == LOW){
    delay(10);
    if (digitalRead(InPins[2]) == LOW){
      delay(10);
      while (!(digitalRead(InPins[2]))) {};
      return 2;
    }

  }
  else if (digitalRead(InPins[3]) == LOW){
    delay(10);
    if (digitalRead(InPins[3]) == LOW){
      delay(10);
      while (!(digitalRead(InPins[3]))) {};
      return 3;
    }
  }
  return (4);
}

// ISR Timer 2 if overflow
ISR(TIMER2_OVF_vect){
  digitalWrite(TOut, digitalRead(TOut) ^ 1); //toggle
  TCNT = 0; // reload to 0 if overflow
}

// ISR Timer 2 if compared
ISR(TIMER2_COMPA_vect){
  digitalWrite(TOut, digitalRead(TOut) ^ 1); //toggle
  //reload to 0 after compare if mode CTC (not reload if mode PWM)
  if(mode == 3){
    TCNT2 = 0; 
  }
}

//ISR Timer 1 input capture
ISR(TIMER1_CAPT_vect){
  static unsigned risingEdgeTime = 0;
  static unsigned fallingEdgeTime = 0;
  
  //Which edge is armed?
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

void ChoosePrescaler(){
  if(selectPrescalerFlag == false){
    switch (state) {
      case 0:
       {
        //read pushbutton
        pb = PollingPushButton();
        switch (pb) {
          case 0:
            state = 1;
            break;
          case 1:
            state = 2;
            break;
          case 2:
            state = 3;
            break;
          case 3:
            state = 4;
            break;
          default:
            state = 0;
            break;
        }
      }
        break;
      case 1: {
          inputPrescaler = 1;
          state = 0;
          selectPrescalerFlag = true;
//          Serial.println("PB0");
          break;
        }
      case 2:
        {
          inputPrescaler = 2;
          state = 0;
          selectModeFlag = true;
//          Serial.println("PB1");
          break;
        }
      case 3:
        {
          inputPrescaler = 3;
          state = 0;
          selectModeFlag = true;
//          Serial.println("PB2");
          break;
        }
      case 4:
        {
          switch(state2){
            case 0:
              pb = PollingPushButton();
              switch(pb){
                case 0:
                  state2 = 1;
                  break;
                case 1:
                  state2 = 2;
                  break;
                case 2:
                  state2 = 3;
                  break;
                case 3:
                  state2 = 4;
                  break;
                default:
                  state2 = 0;
                  break;
              }
              break;
           case 1:
            inputPrescaler = 4;
            state2 = 0;
            selectPrescalerFlag = true;
            break;
           case 2:
            inputPrescaler = 5;
            state2 = 0;
            selectPrescalerFlag = true;
            break;
           case 3:
            inputPrescaler = 6;
            state2 = 0;
            selectPrescalerFlag = true;
            break;
           case 4:
            inputPrescaler = 7;
            state2 = 0;
            selectPrescalerFlag = true;
            break;
           default:
            state2 = 0;
            break;
          }

          state = 0;
          selectPrescalerFlag = true;
          break;
        }
      default:
        state = 0;
        break;
      }
    }
  }
  
  switch(inputPrescaler){
    case '1':
            //1 -> TCCR2B=0bxxxxx001
            Serial.println("Prescaler: 1");
            N = 1;
            TCCR2B = TCCR2B & 0b11111001;
            TCCR2B = TCCR2B | 0b00000001;
            break;
    case '2':
            //8 -> TCCR2B=0bxxxxx010
            Serial.println("Prescaler: 8");
            N = 8;
            TCCR2B = TCCR2B & 0b11111010;
            TCCR2B = TCCR2B | 0b00000010;
            break;
    case '3':
            //32 -> TCCR2B=0bxxxxx011
            Serial.println("Prescaler: 32");
            N = 32;
            TCCR2B = TCCR2B & 0b11111011;
            TCCR2B = TCCR2B | 0b00000011;
            break;
    case '4':
            //64 -> TCCR2B=0bxxxxx100
            Serial.println("Prescaler: 64");
            N = 64;
            TCCR2B = TCCR2B & 0b11111100;
            TCCR2B = TCCR2B | 0b00000100;
            break;
    case '5':
            //128 -> TCCR2B=0bxxxxx101
            Serial.println("Prescaler: 128");
            N = 128;
            TCCR2B = TCCR2B & 0b11111101;
            TCCR2B = TCCR2B | 0b00000101;
            break;
    case '6':
            //256 -> TCCR2B=0bxxxxx110
            Serial.println("Prescaler: 256");
            N = 256;
            TCCR2B = TCCR2B & 0b11111110;
            TCCR2B = TCCR2B | 0b00000110;
            break;
    case '7':
            //1024 -> TCCR2B=0bxxxxx111
            Serial.println("Prescaler: 1024");
            N = 1024;
            TCCR2B = TCCR2B | 0b00000111;
    default:
            break;
  }
}

void ModeFreq(){
  // activate or deactivate Mode Showing Frequency & Duty Cycle
  modeFreq = !modeFreq;
}

void ModePWM(){
  Serial.println("Mode: Fast PWM");
  ChoosePrescaler();
  
  //Setup Register
  //Fast PWM (Mode 3 / non inverting) -> bit WGM22=0, WGM21=1, WGM20=1 | COM2A1=1, COM2A0=0
  //Register TCCR2A=0b10xxxx11, Register TCCR2B=0bxxxx0xxxx
  TCCR2A = TCCR2A & 0b10111111;
  TCCR2A = TCCR2A | 0b10000011;
  TCCR2B = TCCR2B & 0b11110111;

  //Setup Counter
  TCNT2 = 0;

  //Setup Duty Cycle
  OCR2A = pmwOut;

  //Turn on timer compare with OCRA interrupt flag -> timsk2=0bxxxxxx1x
  TIMSK2 = 0;
  TIMSK2 = TIMSK2 | 0b00000010;
  
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
  byte freq = pmwOut;
  OCR2A = freq/(F_CPU*2*N) - 1;

  //Turn on timer compare with OCRA interrupt flag -> timsk2=0bxxxxxx1x
  TIMSK2 = 0;
  TIMSK2 = TIMSK2 | 0b00000010;
  
  //Turn on global interrupts
  sei();
}

void setup() {
  Serial.begin(9600);
  
  char i;
  for (i = 0; i < 4; i++) {
    pinMode(InPins[i], INPUT_PULLUP);
  }
  pinMode(adc, INPUT);
}

void loop() {
  unsigned char pb;

  // select mode (access Push Button)
  if(selectModeFlag == false){
    switch (state) {
      case 0:
       {
        //read pushbutton
        pb = PollingPushButton();
        switch (pb) {
          case 0:
            state = 1;
            break;
          case 1:
            state = 2;
            break;
          case 2:
            state = 3;
            break;
          default:
            state = 0;
            break;
        }
        break;
       }
      case 1: {
          mode = 1;
          ModeFreq();
          state = 0;
          selectModeFlag = true;
          selectPrescalerFlag = false;
//          Serial.println("PB0");
          break;
        }
      case 2:
        {
          mode = 2;
          ModePWM();
          state = 0;
          selectModeFlag = true;
          selectPrescalerFlag = false;
//          Serial.println("PB1");
          break;
        }
      case 3:
        {
          mode = 3;
          ModeCTC();
          state = 0;
          selectModeFlag = true;
          selectPrescalerFlag = false;
//          Serial.println("PB2");
          break;
        }
      default:
        state = 0;
        break;
    }
  }

  // Read Potensio
  potVal = analogRead(adc);
  pmwOut = map(potVal, 0, 4095, 0, 255);

  //Show frequency and duty cycle
  while(state == 0 && modeFreq == true){
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
}
