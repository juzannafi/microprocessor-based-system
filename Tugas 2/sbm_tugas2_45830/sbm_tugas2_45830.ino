// Juz'an Nafi Haifa
// 17/410473/TK/45830

const byte rowKeypad[4] = {10, 9, 8, 7}; //connect to the Keypad Row
const byte columnKeypad[4] = {6, 5, 4, 3}; //connect to the Keypad Column

char keys[4][4] = // Keypad char list
{ 
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
}; 

byte state = 0; // keypad polling state
char Inbyte; // serialRead state

char PollingKeypad()  // read Keypad input
{
//  Serial.println("OK");
  int i,j;
  for(i=0; i<4; i++){
    digitalWrite(rowKeypad[i], LOW);  //activate row i
    for(j=0; j<4; j++){
      if(digitalRead(columnKeypad[j])==LOW){   // read column j
        delay(10);                              // bouncing filter
        if(digitalRead(columnKeypad[j])==LOW){
          delay(10);                            // bouncing filter
          while(!(digitalRead(columnKeypad[j]))){};
            Serial.println("");
            Serial.print(keys[i][j]);
            Serial.print(": ");
          return keys[i][j]; //return key value as pressed key
          }
      }
    }
    digitalWrite(rowKeypad[i], HIGH); //deactivate row i
  }
  return 'Z';  
}

void keypadActivate() { 
  char kp = 'Z';
  switch (state) {
    case 0:
      {
        kp = PollingKeypad(); // return keypad state
        switch (kp) {
          case '1':
            state = 1;
            break;
          case '2':
            state = 2;
            break;
          case '3':
            state = 3;
            break;
          case 'A':
            state = 4;
            break;
          case '4':
            state = 5;
            break;
          case '5':
            state = 6;
            break;
          case '6':
            state = 7;
            break;
          case 'B':
            state = 8;
            break;
          case '7':
            state = 9;
            break;
          case '8':
            state = 10;
            break;
          case '9':
            state = 11;
            break;
          case 'C':
            state = 12;
            break;
          case '*':
            state = 13;
            break;
          case '0':
            state = 14;
            break;
          case '#':
            state = 15;
            break;
          case 'D':
            state = 16;
            break;
          default:
            state = 0;
            break;
        }
        break;
      }
    case 1:
      {
        Serial.println("Tetap tersenyum hari ini ");
        state = 0;
        break;
      }
    case 2:
      {
        Serial.println("Sudahkah bersyukur hari ini?");
        state = 0;
        break;
      }
    case 3:
      {
        Serial.println("Jalani hidup dengan bahagia");
        state = 0;
        break;
      }
    case 4:
      {
        Serial.println("Patuhi protokol kesehatan ");
        state = 0;
        break;
      }
    case 5:
      {
        Serial.println("Tetaplah berada di jalan yang lurus");
        state = 0;
        break;
      }
    case 6:
      {
        Serial.println("Sabar, sabar, dan sabar dalam setiap ujian");
        state = 0;
        break;
      }
    case 7:
      {
        Serial.println("Hidup itu bagaikan roda yang berputar");
        state = 0;
        break;
      }
    case 8:
      {
        Serial.println("Pakai masker ketika di luar rumah");
        state = 0;
        break;
      }
    case 9:
      {
        Serial.println("Sisihkan harta untuk bersedekah");
        state = 0;
        break;
      }
    case 10:
      {
        Serial.println("Besok harus lebih baik daripada hari ini");
        state = 0;
        break;
      }
    case 11:
      {
        Serial.println("Jaga kesehatan, pola makan, dan pola tidur");
        state = 0;
        break;
      }
    case 12:
      {
        Serial.println("Cuci tangan dengan air bersih dan sabun");
        state = 0;
        break;
      }
    case 13:
      {
        Serial.println("Jangan menjadi perfeksionis dalam suatu jika memang tidak mampu");
        state = 0;
        break;
      }
    case 14:
      {
        Serial.println("Kendalikan stress");
        state = 0;
        break;
      }
    case 15:
      {
        Serial.println("Jika ada masalah, ungkapkan!");
        state = 0;
        break;
      }
    case 16:
      {
        Serial.println("Jaga jarak ketika kontak fisik");
        state = 0;
        break;
      }
    default:
      state = 0;
      break;
  }
}

void serialFlush() {
  while (Serial.available() > 0) {
    char t = Serial.read();
  }
}

void setup() {
  Serial.begin(9600);         // baudrate 9600 bps
  char i;
  for (i = 0; i < 4; i++) {   //setup Keypad row and column as input
    pinMode(rowKeypad[i], OUTPUT);
    pinMode(columnKeypad[i], INPUT_PULLUP);
    digitalWrite(rowKeypad[i], HIGH);
  }
}

void loop() {
  Serial.println("Enter any key to activate the keypad");
  serialFlush();
  while (!Serial.available()) {} //checking for serial input
  Inbyte = Serial.read();
  switch (Inbyte) {
    default: {   //activate keypad mode
        Serial.println("========================================");
        Serial.println("");
        Serial.println("Activated the keypad");
        Serial.println("Enter any key to deactivate the keypad");

        // each mode it will loop the mode until serial input is inputed
        serialFlush();
        while (!Serial.available()) {
          keypadActivate();
          delay(100);
          Inbyte = Serial.read();
        }
        Serial.println("");
        Serial.println("Deactivated the keypad");
        Serial.println("========================================");
      }
      break;
  }
}
