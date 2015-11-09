#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//sound
int buzzer = 13;
int score1 = 0;
int score2 = 0;

//timere
unsigned long currentFrame = 0;

// For joysticks
int lowDead = 412;
int highDead = 612;

// Joystick 1
int xPort = A0;
int yPort = A1;
int button1 = 9;
int buttonPress1 = 0;

int x = 7;
int y = 7;
int aX;
int aY;


// Joystick 2
int xPort2 = A2;
int yPort2 = A3;
int x2 = 7;
int y2 = 7;
int aX2;
int aY2;
int button2 = 10;
int buttonPress2 = 0;

// LED Matrix
int datapin = 6; 
int clockpin = 7;
int latchpin = 8;

byte data = 0;
byte data2 = 0;
byte data3 = 0;
byte data4 = 0;

void setup() {
  pinMode(latchpin, OUTPUT);
  pinMode(xPort, INPUT);
  pinMode(yPort, INPUT);
  pinMode(xPort2, INPUT);
  pinMode(yPort2, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(buzzer, OUTPUT);
  lcd.begin(16, 2);
  //LCDWrite(score1, score2); 
}

void loop() {
  aX = analogRead(xPort);
  aY = analogRead(yPort);
  aX2 = analogRead(xPort2);
  aY2 = analogRead(yPort2);
  buttonPress1 = digitalRead(button1);
  buttonPress2 = digitalRead(button2);
  /*if(currentFrame % 10 == 0) {
    if(buttonPress1 == 0){
      //tone(buzzer, 500,100);
      score1++;
      LCDWrite(score1, score2);
    }
    if(buttonPress2 == 0){
      //tone(buzzer, 500,100);
      score2++;
      LCDWrite(score1, score2);
    }
  }*/
  lcd.clear();  
  lcd.setCursor(0,0);
  lcd.print(currentFrame/(millis() / 1000));
  if (currentFrame%3 == 0){
    updateXAndY();
  }
  
  refreshMatrix();
}

void LCDWrite(int x, int y){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Player1: ");
  lcd.print(x);
  lcd.setCursor(0, 1);
  lcd.print("Player2: ");
  lcd.print(y);
}
void updateXAndY(){
  if(aX < lowDead && x != 15) {
    x++;
  } else if (aX > highDead && x != 0) {
    x--;
  }
  if(aY < lowDead && y != 0) {
    y--;
  } else if (aY > highDead && y != 15) {
    y++;
  }

  if(aX2 < lowDead && x2 != 15) {
    x2++;
  } else if (aX2 > highDead && x2 != 0) {
    x2--;
  }
  if(aY2 < lowDead && y2 != 0) {
    y2--;
  } else if (aY2 > highDead && y2 != 15) {
    y2++;
  }
}

void refreshMatrix() {
  // Sets LED ON
  digitalWrite(latchpin, 0);
  (x < 8)?colLeft(x, 1):colRight(x % 8, 1);
  (y < 8)?rowTop(y, 0):rowBot(y % 8 , 0);
  shiftAll();
  digitalWrite(latchpin, 1);
  delay(1);
  // Sets all LEDs OFF
  digitalWrite(latchpin, 0);
  for(int i = 0; i < 8; i++) {
    rowTop(i, 1);
    rowBot(i, 1);
    colLeft(i, 0);
    colRight(i, 0);
  }
  shiftAll();
  digitalWrite(latchpin, 1);
  
  digitalWrite(latchpin, 0);
  (x2 < 8)?colLeft(x2, 1):colRight(x2 % 8, 1);
  (y2 < 8)?rowTop(y2, 0):rowBot(y2 % 8 , 0);
  shiftAll();
  digitalWrite(latchpin, 1);
  delay(1);
  // Sets all LEDs OFF
  digitalWrite(latchpin, 0);
  for(int i = 0; i < 8; i++) {
    rowTop(i, 1);
    rowBot(i, 1);
    colLeft(i, 0);
    colRight(i, 0);
  }
  shiftAll();
  digitalWrite(latchpin, 1);
  currentFrame++;
}

void rowTop(int desiredPin, boolean desiredState) {
  bitWrite(data,desiredPin,desiredState);
  //shiftOut(datapin, clockpin, data);
}

void rowBot(int desiredPin, boolean desiredState) {
  bitWrite(data2,desiredPin,desiredState);
  //shiftOut(datapin, clockpin, data2);
}

void colLeft(int desiredPin, boolean desiredState) {
  bitWrite(data3,desiredPin,desiredState);
  //shiftOut(datapin, clockpin, data3);
}

void colRight(int desiredPin, boolean desiredState) {
  bitWrite(data4,desiredPin,desiredState);
  //shiftOut(datapin, clockpin, data4);
}

void shiftAll() {
  // Push in reverse because it is serial
  shiftOut(datapin, clockpin, data4);
  shiftOut(datapin, clockpin, data3);
  shiftOut(datapin, clockpin, data2);
  shiftOut(datapin, clockpin, data);
}


void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low

 //internal function setup
  int i=0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOut
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=7; i>=0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {  
      pinState= 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin  
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}
