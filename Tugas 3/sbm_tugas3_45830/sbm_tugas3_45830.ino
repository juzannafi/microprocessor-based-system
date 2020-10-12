// Juz'an Nafi Haifa
// 17/410473/TK/45830

//OLED I2C Adafruit libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const byte rowKeypad[4] = {3, 4, 5, 6}; //connect to the Keypad Row
const byte columnKeypad[4] = {7, 8, 9, 10}; //connect to the Keypad Column

char keys[4][4] = // Keypad char list
{ 
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
}; 

String inputSerial; // input from Serial Monitor
String inputKeypad; // input from Keypad
bool isNumber;      // for checking whether an input is a valid number

String binString;   // output binary
String hexString;   // output hexadecimal

void DisplayTitle(char state){ // display "Input: ", "Binary", and "Hex: " to OLED
  switch (state){
    case '1':
      display.print(F("Input : "));
      break;
    case '2':
      display.print(F("Binary: "));
      break;
    case '3':
      display.print(F("Hex   : "));
      break;
    default:
      break;
  }
}

void DisplayOLED(int cursor_vertical, String &str, char flag){ // display conversion result to OLED
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,cursor_vertical);
  DisplayTitle(flag);
  display.println(str);
  display.display();
}

void ReadKeypad(){  // read Keypad input
  int i,j;
  for(i=0; i<4; i++){
    digitalWrite(rowKeypad[i], LOW);  //activate row i
    for(j=0; j<4; j++){
      if(digitalRead(columnKeypad[j])==LOW){   // read column j
        delay(10);                              // bouncing filter
        if(digitalRead(columnKeypad[j])==LOW){
          delay(10);                            // bouncing filter
          while(!(digitalRead(columnKeypad[j]))){};
            display.clearDisplay(); // reset display to blank
            
            //display input
            inputKeypad = String(keys[i][j]);
            Serial.print("Input: "); Serial.println(inputKeypad);
            DisplayOLED(10, inputKeypad, '1');
            
            //display binary conversion result
            binString = String(keys[i][j], BIN);
            Serial.print("Binary: "); Serial.println(binString);
            DisplayOLED(20, binString, '2');

            //display hexadecimal conversion result
            hexString = String(keys[i][j], HEX);
            Serial.print("Hex: "); Serial.println(hexString);
            DisplayOLED(30, hexString, '3');
            
            Serial.println("");
            Serial.println("==========================================================================");
            Serial.println("Enter any characters or numbers from keyboard or Press any key from keypad");
        }
      }
    }
    digitalWrite(rowKeypad[i], HIGH); //deactivate row i
  }
}

void SerialFlush() {
  while (Serial.available() > 0) {
    char t = Serial.read();
  }
}

bool IsValidNumber(String str){ // checking an input is whether a number or not
   for(int i=0; i<str.length() - 1; i++){
      if(isDigit(str.charAt(i)) == false){
        return false;
      }
   }
   return true;
}

void ReadSerial(String &inputSerial){ // Keypad input conversion
  display.clearDisplay(); // reset display to blank
  
  //display input
  Serial.print("Input: "); Serial.print(inputSerial);
  DisplayOLED(10, inputSerial, '1');

  // checking an input is whether a number or not
  isNumber = IsValidNumber(inputSerial);
  if (isNumber){  // if YES, convert directly the number
      int number = inputSerial.toInt();
      binString = String(number, BIN);
      hexString = String(number, HEX);
  }
  else{ // if NO, convert each char of input
      binString = "";
      hexString = "";
      String temp;
          
      for(int i=0; i<inputSerial.length() - 1; i++){        
          temp = String(inputSerial.charAt(i), BIN);
          // fault handling start from 2nd char and so on: make them to 8 digits by adding '0' from MSB of each char
          if(i != 0){
              for(int j=0; j<8-temp.length(); j++){
                  binString += "0";
              }
          }
          binString += temp;

          temp = String(inputSerial.charAt(i), HEX);
          // fault handling start from 2nd char and so on: make them to 2 digits by adding '0' from MSB of each char
          if(i != 0){
              for(int j=0; j<2-temp.length(); j++){
                  hexString += "0";
              }
          }
          hexString += temp;
      }
  }
  //display binary conversion result
  Serial.print("Binary: "); Serial.println(binString);
  DisplayOLED(20, binString, '2');

  //display hexadecimal conversion result
  Serial.print("Hex: "); Serial.println(hexString);
  DisplayOLED(30 + ((binString.length() + 7)/21)*8, hexString, '3');  // display fault handling to make the text display not overlap
}

void setup() {
  Serial.begin(9600); // baudrate 9600 bps

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  delay(1000); // Pause for 1 seconds

  // display welcome command
  display.clearDisplay(); // reset display to blank
  String command = "Enter any characters or numbers from keyboard or Press any key from keypad";
  DisplayOLED(10, command, '0');

  //setup Keypad row and column
  for (int i = 0; i < 4; i++) {
    pinMode(rowKeypad[i], OUTPUT);
    pinMode(columnKeypad[i], INPUT_PULLUP);
    digitalWrite(rowKeypad[i], HIGH);
  }
}

void loop() {
  Serial.println("Enter any characters or numbers from keyboard or Press any key from keypad");
  SerialFlush();
  while (!Serial.available()) { //checking for serial input from Keyboard, while checking, it's also checking for Keypad input
        ReadKeypad(); //read input from Keypad & process conversion
    } 
  inputSerial = Serial.readString(); //read input from Keypad
  ReadSerial(inputSerial); // process conversion
  Serial.println("");
  Serial.println("==========================================================================");
}
