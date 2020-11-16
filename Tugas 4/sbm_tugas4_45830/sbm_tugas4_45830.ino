volatile unsigned PulseTime = 0;

ISR(TIMER1_CAPT_vect){
  static unsigned RisingEdgeTime = 0;
  static unsigned FallingEdgeTime = 0;
  
  // Which edge is armed?
  if (TCCR1B & (1 << ICES1)){
    RisingEdgeTime = ICR1; // Rising Edge
    TCCR1B &= ~(1 << ICES1); // Switch to Falling Edge
  }
  else {
    FallingEdgeTime = ICR1; // Falling Edge
    TCCR1B |= (1 << ICES1); // Switch to Rising Edge
    PulseTime = FallingEdgeTime - RisingEdgeTime;
  }
}

void setup() {
 Serial.begin(9600);
 analogWrite(10,255/2);
 noInterrupts ();  // protected code
 // reset Timer 1
  //TCCR1A= 0;
  //TCCR1B= 0;
  TCNT1= 0;
  // clear flag
  TIFR1= 0;   
  // start Timer 1, no prescaler
  TCCR1B|=(1<<CS10);  
// plus Input Capture Edge Select (rising on D8)
  TCCR1B|=(1<<ICES1); 
// Enable Timer 1 Input Capture Interrupt Enable
  TIMSK1|=(1<<ICIE1); 
  interrupts ();
}

void loop() {
  unsigned pulseTime = 0;
  noInterrupts();
  pulseTime = PulseTime;
  PulseTime = 0;
  interrupts();

  if (pulseTime){
    Serial.println(pulseTime);
  }
}
